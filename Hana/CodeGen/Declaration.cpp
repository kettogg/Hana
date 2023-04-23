#include <typeinfo>

#if defined(_MSC_VER)
#pragma warning( push , 0 )
#endif
#include "llvm/Transforms/Utils/Cloning.h"
#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#include "Declaration.h"
#include "Assignment.h"
#include "CodeGenContext.h"
#include "parser.hpp"


using namespace std;
using namespace llvm;

namespace hana {
    
Value* VariableDeclaration::codeGen(CodeGenContext& context)
{
    Value* val = nullptr;
    if( context.findVariable(id->getName()) ) {
        Node::printError(location, " Variable '" + id->getName()  + "' already exists!\n");
        context.addError();
        return nullptr;
    }

    Type* ty = context.typeOf(*type);
    if( ty->isStructTy() && ty->getStructName() == "var" ) {
       // It is a var declaration, postpone type until assignment.
       context.locals()[id->getName()] = nullptr;
    } else if( ty->isStructTy() && context.getScopeType() != ScopeType::FunctionDeclaration ) {
        // It is really a declaration of a class type which we put always onto the heap.
        AllocaInst* alloc = new AllocaInst(ty, 0, id->getName().c_str(), context.currentBlock());
        context.locals()[id->getName()] = alloc;
        val = alloc;
        context.varStruct = val; // Indicates that a variable of a class is declared
    }
    else
    {
        if( ty->isStructTy() ) {
            // It is a declaration of a class type in a function declaration as a formal parameter.
            // Therefor a pointer reference is needed.
            ty = PointerType::get(ty,0);
        }
        AllocaInst* alloc = new AllocaInst(ty, 0, id->getName().c_str(), context.currentBlock());
        context.locals()[id->getName()] = alloc;
        val = alloc;
    }
    context.setVarType(type->getName(), id->getName());
    
    if (assignmentExpr != nullptr) {
        Assignment assn(id, assignmentExpr, location);
        assn.codeGen(context);
        // they are already deleted by assn.
        id = nullptr;
        assignmentExpr = nullptr;
    }
    else if ( context.varStruct )
    {
        // The variable gets nothing assigned so 
        // auto assign defaults (member assignments) on classes ctor call.
        auto stmts = context.getKlassInitCode(type->getName());
        for ( auto assign : stmts )
        {
            assign->codeGen( context );
        }

        // Generate function call to the ctor, if exists.
        Function *fn = context.getModule()->getFunction( "__init__%" + type->getName() );
        if ( fn != nullptr )
        {
            std::vector<Value*> args;
            args.push_back( context.varStruct );
            (void) CallInst::Create(fn, args, "", context.currentBlock());
        }
        context.varStruct = nullptr;
    }
    return val;
}

}

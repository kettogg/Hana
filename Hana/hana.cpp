#include "hana.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cassert>
#include <stack>
#include "config.h"
#include "CodeGenContext.h"
#include "AstNode.h"
#include "GetOpt.h"

extern int yyparse();
extern int yylex_destroy();
extern FILE* yyin;
extern hana::Block* programBlock;
extern std::stack<std::string> fileNames;
extern std::vector<std::string> libPaths;
extern int parsing_error;

void usage();

int main(int argc, char** argv)
{
   std::string fileName;
   if( argc == 1 ) {
      // No argument passed Haha!
      // fileName = "./Idk.hana";
      std::cout << "Hana version " << MAJOR_VER << "." << MINOR_VER << "." << REVISION_VER << "\n";
      std::cout << "Use 'hana -h' for more information, Noob.\n";
   }

   libPaths.push_back("./"); // Current path
   bool verbose = false;
   bool quiet = false;
   bool debug = false;

   // Use Getopt for getting the command arguments.
   GetOpt getopt(argc, argv, "hr:vqd");
   for( auto opt : getopt ) {
      switch( opt ) {
         case 'r': {
            std::stringstream ss(getopt.get());
            std::string item;
            while( std::getline(ss, item, '|') ) {
               std::replace(std::begin(item), std::end(item), '\\', '/');
               if( item[item.size()] != '/' ) {
                  item += '/';
               }
               libPaths.push_back(item);
            }
         } break;
         case 'v':
            verbose = true;
            break;
         case 'q':
            quiet = true;
            break;
         case 'd':
            debug = true;
            break;
         case 'h':
            usage();
            return 1;
         case EOF:
            break;
         default:
            std::cout << getopt.error() << "\n";
            usage();
            return 1;
      }
   }

   if( !quiet ) {
      std::cout << "========================\n";
      std::cout << "   Hana version " << MAJOR_VER << "." << MINOR_VER << "." << REVISION_VER << "\n";
      std::cout << "========================\n";
   }
   auto files = getopt.getRemainingArguments();
   assert(files.size() == 1);
   fileName = files[0]; // Currently only one file is supported.

   yyin = fopen(fileName.c_str(), "r+");
   if( yyin == nullptr ) {
      std::cout << "File " << fileName << "not found. Abort" << std::endl;
      return -1;
   }

   fileNames.push("");       // Add the empty file name after last EOF.
   fileNames.push(fileName); // Add the top level file name.
   if( yyparse() || parsing_error ) {
      yylex_destroy();
      return 1;
   }

   if( programBlock == nullptr ) {
      std::cout << "Parsing " << fileName << "failed. Abort" << std::endl;
   } else {
      std::ostringstream devNull;
      hana::CodeGenContext context(quiet ? devNull : std::cout);
      context.verbose = verbose;
      context.debug = debug;
      if( verbose )
         context.printCodeGeneration(*programBlock, std::cout);
      if( context.preProcessing(*programBlock) ) {
         if( context.generateCode(*programBlock) ) {
            context.runCode();
         }
      }
   }

   if( yyin != nullptr )
      fclose(yyin);
   delete programBlock;
   yylex_destroy();
   return 0;
}

void usage()
{
   std::cout << "Hana version " << MAJOR_VER << "." << MINOR_VER << "." << REVISION_VER << "\n";
   std::cout << "Usage:\n";
   std::cout << "hana <filename> -h -d -v -q -r importPath1|importPath2|...\n";
   std::cout << "\t-h => You are smart enough to know what it is for.\n";
   std::cout << "\t-d => Debug mode, basically it disables the Code Optimizer.\n";
   std::cout << "\t-v => Verbose mode, displays more information.\n";
   std::cout << "\t-q => Quiet Mode, displays no output.\n";
   std::cout << "\t-r => Require other Hana files, seperated by the pipe '|'.\n";
}

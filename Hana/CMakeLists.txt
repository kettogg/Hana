cmake_minimum_required(VERSION 3.12)

project(Hana)

message(STATUS "====================")
message(STATUS "  CONFIGURING Hana")
message(STATUS "====================")

macro(replace_flags flag newflag)
    FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        IF(${flag_var} MATCHES ${flag})
            STRING(REGEX REPLACE "${flag}" "${newflag}" ${flag_var} "${${flag_var}}")
        ENDIF()
    ENDFOREACH(flag_var)
endmacro()

# Setup the make environment. #
# ###############################
# Activate very verbose makefiles.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Allow else() and endif() constructs (with empty parentheses)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Default to debug" FORCE)

# Setup the cmake directory containing numerous cmake scripts and macros.
set(CMAKE_MODULE_PATH "${Hana_SOURCE_DIR}/cmake")

# Search for the different needed libs. #
# ########################################
FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

# We incorporate the CMake features provided by LLVM:
if($ENV{LLVM_ROOT})
    set(LLVM_ROOT $ENV{LLVM_ROOT})
endif()

find_package(LLVM PATHS "${LLVM_ROOT}/lib/cmake/llvm" "$ENV{ProgramFiles}/lib/cmake/llvm")

if(LLVM_FOUND)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
else()
    message(FATAL_ERROR "LLVM installation not found. Set LLVM_ROOT to the installation root path.")
endif()

include(LLVMConfig)
message(STATUS "Found LLVM Package Version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Built-type: ${LLVM_BUILD_TYPE}")
string(REGEX MATCH "[0-9]+\\.[0-9]+" LLVM_VER ${LLVM_PACKAGE_VERSION})

if(NOT ${LLVM_VER} VERSION_EQUAL "14.0")
    message(WARNING "LLVM version ${LLVM_VER} is not tested.")
endif()

if(MSVC)
    if(NOT CMAKE_CL_64)
        message(FATAL_ERROR "Currently only 64bit compilation is supported!")
    endif()
endif()

# Let's suppose we want to build a JIT compiler with support for
# binary code :
llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES mcjit interpreter native ipo)

# Put all source files into one variable. #
# #########################################
set(SOURCES
    hana.cpp
        Builtins/buildins.cpp
        AST/AstNode.cpp
        CodeGen/Array.cpp
        CodeGen/Declaration.cpp
        CodeGen/FunctionDeclaration.cpp
        CodeGen/ClassDeclaration.cpp
        CodeGen/CodeGenContext.cpp
        Visitor/VisitorSyntaxCheck.cpp
        Visitor/VisitorPrettyPrint.cpp
        Lexer/tokens.l
        Parser/parser.y
        CodeGen/GetOpt.cpp
        CodeGen/Range.cpp
        CodeGen/BinaryOperator.cpp
        CodeGen/UnaryOperator.cpp
        CodeGen/CompareOperator.cpp
        CodeGen/Return.cpp
        CodeGen/WhileLoop.cpp
        CodeGen/Conditional.cpp
        CodeGen/Assignment.cpp
        CodeGen/MethodCall.cpp
)

set(HEADERS
        Builtins/buildins.h
        AST/AstNode.h
        CodeGen/Array.h
        CodeGen/Declaration.h
        CodeGen/FunctionDeclaration.h
        CodeGen/ClassDeclaration.h
        CodeGen/CodeGenContext.h
        Visitor/Visitor.h
        Visitor/VisitorSyntaxCheck.h
        Visitor/VisitorPrettyPrint.h
        CodeGen/GetOpt.h
        CodeGen/BinaryOperator.h
        CodeGen/UnaryOperator.h
        CodeGen/CompareOperator.h
        CodeGen/Range.h
        CodeGen/Return.h
        CodeGen/WhileLoop.h
        CodeGen/Conditional.h
        CodeGen/Assignment.h
        CodeGen/MethodCall.h
)

set(VER_MAJ 0)
set(VER_MIN 2)
set(VER_REV 0)

configure_file(config.in ${Hana_SOURCE_DIR}/config.h)

BISON_TARGET(HanaParser Parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS --report=solved COMPILE_FLAGS --feature=fixit)
FLEX_TARGET(HanaScanner Lexer/tokens.l ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp)
ADD_FLEX_BISON_DEPENDENCY(HanaScanner HanaParser)

add_executable(hana ${SOURCES} ${HEADERS} ${BISON_HanaParser_OUTPUTS} ${FLEX_HanaScanner_OUTPUTS})

# Compiler-dependent and build-depended flags:
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(hana PRIVATE -Wall)
elseif(MSVC)
    replace_flags("/MDd" "/MTd")
    replace_flags("/MD" "/MT")
    target_compile_options(hana PRIVATE /W4 /permissive-)
    target_compile_definitions(hana PRIVATE YY_NO_UNISTD_H)
    target_compile_definitions(hana PRIVATE _SCL_SECURE_NO_WARNINGS)
    target_compile_definitions(hana PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(hana PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp PROPERTIES COMPILE_DEFINITIONS __STDC_VERSION__=199901L)
endif()

# Debug or release ?
target_compile_definitions(hana PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

# Add the compiler definitions of LLVM
if(MSVC)
    # In order to use it with target_compile_definitions we have to replace the -D
    # with a semicolon otherwise they are not correctly populated to MSVC and the
    # IntelliSense is broken (command line error).
    string(REPLACE "-D" ";" LLVM_DEFINITIONS ${LLVM_DEFINITIONS})
endif()

target_compile_definitions(hana PRIVATE ${LLVM_DEFINITIONS})

if("${LLVM_BUILD_TYPE}" MATCHES "Release|RelWithDebInfo|MinSizeRel")
    target_compile_definitions(hana PRIVATE LLVM_NO_DUMP)
endif()

if(NOT LLVM_ENABLE_RTTI)
    target_compile_definitions(hana PRIVATE HANA_NO_RTTI)

    if(MSVC)
        target_compile_options(hana PRIVATE /GR-)
    else()
        target_compile_options(hana PRIVATE -fno-rtti)
    endif()
endif()

if(LLVM_ENABLE_EH AND CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(hana PRIVATE -fexceptions)
endif()

# Add additional include search directories.
target_include_directories(hana PRIVATE ${Hana_SOURCE_DIR} ${LLVM_INCLUDE_DIRS})
target_include_directories(hana PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_compile_features(hana PRIVATE cxx_std_17)

# Finally, we link the LLVM libraries to our executable:
target_link_libraries(hana ${REQ_LLVM_LIBRARIES})

if(MSVC)
    source_group(Header\ Files FILES ${HEADERS})
    source_group(Source\ Files FILES ${SOURCES})
endif()

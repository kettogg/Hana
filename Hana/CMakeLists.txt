cmake_minimum_required(VERSION 3.12)

project(Hana)

message(STATUS "======================")
message(STATUS "[*] BUILDING Hana ...")
message(STATUS "======================")

macro(replace_flags flag newflag)
	FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
		IF(${flag_var} MATCHES ${flag})
			STRING(REGEX REPLACE "${flag}" "${newflag}" ${flag_var} "${${flag_var}}")
		ENDIF()
	ENDFOREACH(flag_var)
endmacro()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
set(CMAKE_MODULE_PATH "${Hana_SOURCE_DIR}/cmake")

FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

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
message(STATUS "Found LLVM Package Version:${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Built type : ${LLVM_BUILD_TYPE}")
string(REGEX MATCH "[0-9]+\\.[0-9]+" LLVM_VER ${LLVM_PACKAGE_VERSION})

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES mcjit interpreter native ipo) # JIT Support

set(HANA_SOURCES
	hana.cpp
	Lib/Stlib.cpp
	Ast/AstNode.cpp
	CodeGen/Array.cpp
	CodeGen/Declaration.cpp
	CodeGen/FunctionDeclaration.cpp
	CodeGen/ClassDeclaration.cpp
	CodeGen/CodeGenContext.cpp
	Visitor/VisitorSyntaxCheck.cpp
	Visitor/VisitorPrettyPrint.cpp
	tokens.l
	parser.y
	Utils/GetOpt.cpp
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

set(HANA_HEADERS
	Lib/Stlib.h
	Ast/AstNode.h
	CodeGen/Array.h
	CodeGen/Declaration.h
	CodeGen/FunctionDeclaration.h
	CodeGen/ClassDeclaration.h
	CodeGen/CodeGenContext.h
	Visitor/Visitor.h
	Visitor/VisitorSyntaxCheck.h
	Visitor/VisitorPrettyPrint.h
	Utils/GetOpt.h
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

if(MSVC)
	source_group(Header\ Files FILES ${HANA_HEADERS})
	source_group(Source\ Files FILES ${HANA_SOURCES})
endif()

set(VER_MAJ 1)
set(VER_MIN 1)
set(VER_REV 2)

configure_file(config.in ${Hana_SOURCE_DIR}/config.h)

BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS --report=solved COMPILE_FLAGS --feature=fixit)
FLEX_TARGET(Scanner tokens.l ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

add_executable(hana ${HANA_SOURCES} ${HANA_HEADERS} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

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

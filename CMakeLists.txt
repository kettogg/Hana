cmake_minimum_required(VERSION 3.12)

project(Hana)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH "${Hana_SOURCE_DIR}/cmake")

add_subdirectory(Hana)
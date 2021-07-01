cmake_minimum_required(VERSION 3.16)
project(avgdist CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/cage/cmake")
include(cage_all)
cage_default_init()

add_subdirectory(externals/cage EXCLUDE_FROM_ALL)

file(GLOB_RECURSE avgdist-sources "sources/*")
add_executable(avgdist ${avgdist-sources})
target_link_libraries(avgdist cage-core)
cage_ide_category(avgdist avgdist)
cage_ide_sort_files(avgdist)
cage_ide_working_dir_in_place(avgdist)

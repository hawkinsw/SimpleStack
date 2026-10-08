cmake_minimum_required(VERSION 3.25)

project(StackStroll DESCRIPTION "A naive stack walking library.")

# stroll library

find_package(LLVM CONFIG REQUIRED)

add_library(stroll lib/stackstroll.cpp)
target_include_directories(stroll PUBLIC include)

# Link in necessary DWARF libraries from LLVM.
set(LLVM_LINK_COMPONENTS
  BinaryFormat
  DebugInfoDWARF
  X86
  AllTargetsDescs
  AllTargetsInfos
  Core
  IRReader
  MC
  Object
  Support
  TargetParser
  )

llvm_map_components_to_libnames(llvm_libraries ${LLVM_LINK_COMPONENTS})
target_link_libraries(stroll ${llvm_libraries})

# stroll use examples.
add_executable(test test/example.cpp)
target_link_libraries(test stroll)

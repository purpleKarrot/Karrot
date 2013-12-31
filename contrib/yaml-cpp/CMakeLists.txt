
cmake_minimum_required(VERSION 2.8)
project(YAML_CPP CXX)

find_package(Boost REQUIRED)

add_definitions(-Wno-deprecated-declarations)

file(GLOB sources "src/[a-zA-Z]*.cpp")
file(GLOB_RECURSE public_headers "include/yaml-cpp/[a-zA-Z]*.h")
file(GLOB private_headers "src/[a-zA-Z]*.h")

file(GLOB contrib_sources "src/contrib/[a-zA-Z]*.cpp")
file(GLOB contrib_public_headers "include/yaml-cpp/contrib/[a-zA-Z]*.h")
file(GLOB contrib_private_headers "src/contrib/[a-zA-Z]*.h")

add_library(yaml-cpp STATIC
  ${sources}
  ${public_headers}
  ${private_headers}
  ${contrib_sources}
  ${contrib_public_headers}
  ${contrib_private_headers}
  )

target_include_directories(yaml-cpp
  PUBLIC
    include
  PRIVATE
    ${Boost_INCLUDE_DIRS}
    src
  )

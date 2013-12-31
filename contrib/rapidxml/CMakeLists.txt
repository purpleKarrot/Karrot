
cmake_minimum_required(VERSION 2.8)
project(RapidXML NONE)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE include)

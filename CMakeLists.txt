cmake_minimum_required(VERSION 3.26)
project(
  Mineclone
  VERSION 0.0.1
  DESCRIPTION "A Minecraft clone written in C++"
  LANGUAGES CXX
)

message(STATUS "CMake version: ${CMAKE_VERSION}!")

add_subdirectory(client)
add_subdirectory(server)

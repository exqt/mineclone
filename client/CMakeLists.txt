# Find packages
INCLUDE(FindPkgConfig)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

message(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
message("Parent directory: ${PARENT_DIR}")
set(THIRD_PARTY_DIR "${PARENT_DIR}/thirdparty")


file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS
  ${PARENT_DIR}/common/*.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${COMMON_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)


# libraries
# FastNoise
set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)
add_subdirectory("${THIRD_PARTY_DIR}/FastNoise2" thirdparty/FastNoise2)
target_compile_options(FastNoise PRIVATE "-Wno-overflow")

# imgui
add_library(imgui STATIC
  ${THIRD_PARTY_DIR}/imgui/imgui.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
  ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl2.cpp
  ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${THIRD_PARTY_DIR}
  ${THIRD_PARTY_DIR}/imgui
  ${THIRD_PARTY_DIR}/imgui/backends
  ${SDL2_INCLUDE_DIRS}
)

# ENet
if (NOT TARGET enet)
  add_subdirectory(${THIRD_PARTY_DIR}/enet thirdparty/enet)
endif()

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  ${THIRD_PARTY_DIR}/FastNoise2
  ${THIRD_PARTY_DIR}/imgui
  ${THIRD_PARTY_DIR}/enet/include
  ${THIRD_PARTY_DIR}/glm
)

# if not debug build then link statically
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_link_options(${PROJECT_NAME} PRIVATE
    -static-libgcc
    -static-libstdc++
  )

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)

elseif(CMAKE_BUILD_TYPE STREQUAL "Gprof")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -no-pie")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

target_link_libraries(${PROJECT_NAME}
  GLEW::GLEW
  OpenGL::GL
  SDL2::SDL2-static

  FastNoise
  imgui

  enet
)

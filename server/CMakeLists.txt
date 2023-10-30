set(TARGET_NAME ${PROJECT_NAME}Server)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# files
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(THIRD_PARTY_DIR "${PARENT_DIR}/thirdparty")

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS ${PARENT_DIR}/common/*.cpp)

# executable
add_executable(${TARGET_NAME} ${SRC_FILES} ${COMMON_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# if not debug build then link statically
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_link_options(${PROJECT_NAME}Server PRIVATE
    -static-libgcc
    -static-libstdc++
    -static
  )
endif()

# libraries
# enet
if (NOT TARGET enet)
  add_subdirectory(${THIRD_PARTY_DIR}/enet thirdparty/enet)
endif()

target_include_directories(${TARGET_NAME} PRIVATE
  ${THIRD_PARTY_DIR}/enet/include
)

# FastNoise
target_include_directories(${TARGET_NAME} PRIVATE
  ${THIRD_PARTY_DIR}/FastNoise2
)

# link
target_link_libraries(${TARGET_NAME}
  enet
  FastNoise
)

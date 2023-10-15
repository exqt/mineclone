set(TARGET_NAME ${PROJECT_NAME}Server)

# Mineclone
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
message("Parent directory: ${PARENT_DIR}")
set(THIRD_PARTY_DIR "${PARENT_DIR}/thirdparty")

file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS
  ${PARENT_DIR}/common/*.cpp
)

add_executable(${TARGET_NAME} ${SRC_FILES} ${COMMON_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

if (NOT TARGET enet)
  add_subdirectory(${THIRD_PARTY_DIR}/enet thirdparty/enet)
endif()

# include directories
target_include_directories(${TARGET_NAME} PRIVATE
  ${THIRD_PARTY_DIR}/enet/include
  ${THIRD_PARTY_DIR}/FastNoise2
)

target_link_libraries(${TARGET_NAME}
  enet
  FastNoise
)

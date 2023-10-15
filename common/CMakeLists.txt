set(TARGET_NAME ${PROJECT_NAME}Common)

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

add_library(${TARGET_NAME} STATIC ${SRC_FILES})

# include directories
target_include_directories(${TARGET_NAME} PRIVATE
  ${THIRD_PARTY_DIR}/glm
)

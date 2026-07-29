cmake_minimum_required(VERSION 3.15)

project(STTCHeatmapGenerator)
set(CMAKE_CXX_STANDARD 17)


# collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# puts the executable in build/bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
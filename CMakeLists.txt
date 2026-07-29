cmake_minimum_required(VERSION 3.15)

project(MEAScriptRunner)
set(CMAKE_CXX_STANDARD 17)

# collect source files
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Specify output directory for all configurations
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIR}"
)

# Copy matlab script from src to build/bin
set(COPY_FILE "${SOURCE_DIR}/Matlab Scripts/cmd_MagSpectrumBandpass.m")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${COPY_FILE}" "${OUTPUT_DIR}/cmd_MagSpectrumBandpass.m"
)
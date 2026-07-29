cmake_minimum_required(VERSION 3.15)

project(SpikeCorrelationValueExtraction)
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    # CHANGE THIS DIRECTORIES TO WHERE YOUR SDL LIBRARIES ARE STORED
    set(CMAKE_PREFIX_PATH "C:/msys64/Libraries")
    set(SDL3_DIR "${CMAKE_PREFIX_PATH}/SDL3/lib/cmake/SDL3")
    set(SDL3_image_DIR "${CMAKE_PREFIX_PATH}/SDL3_Image/lib/cmake/SDL3_Image")
    
    find_package(SDL3 REQUIRED CONFIG)
    find_package(SDL3_image REQUIRED CONFIG)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# puts the executable in build/bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# include SDL directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE "${CMAKE_PREFIX_PATH}/SDL3/include/" 
    PRIVATE "${CMAKE_PREFIX_PATH}/SDL3_Image/include/"
)

# link to SDL libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)
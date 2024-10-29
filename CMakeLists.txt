cmake_minimum_required(VERSION 3.10)

# Set the project name
project(SCiPnet)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the path to your SFML installation
set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6.1 COMPONENTS graphics window system REQUIRED)

# Check if SFML was found
if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML not found! Please check the SFML_DIR path.")
endif()

# Display the SFML include directories and libraries
message(STATUS "SFML_INCLUDE_DIRS: ${SFML_INCLUDE_DIRS}")
message(STATUS "SFML_LIBRARIES: ${SFML_LIBRARIES}")

# Set the build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "CMAKE_BUILD_TYPE not set. Defaulting to Release.")
endif()

# Handle Windows resource file
if(WIN32)
    set(RESOURCE_COMPILER "rc")
    set(RESOURCE_FILE "src/resource.rc")
    message(STATUS "Resource file path: ${RESOURCE_FILE}")
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${RESOURCE_FILE})
        message(WARNING "Resource file ${RESOURCE_FILE} not found. Skipping resource compilation.")
        set(RESOURCE_FILE "")  # Clear if not found
    endif()
endif()

# Add the executable with WIN32 subsystem
add_executable(${PROJECT_NAME} WIN32  # Add WIN32 here
    src/main.cpp
    ${RESOURCE_FILE}  # Include only if it exists
)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIRS})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Set output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
)

# Copy SFML DLLs to the output directory after building
if(WIN32)
    file(GLOB SFML_DLLS "${CMAKE_SOURCE_DIR}/SFML/SFML-2.6.1/lib/*.dll")
    foreach(DLL ${SFML_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach()
endif()

# Compiler options based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)  # Enable warnings
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)  # Optimize for release
endif()

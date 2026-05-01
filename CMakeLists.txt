cmake_minimum_required(VERSION 3.28)
project(Metal_Engine_cpp VERSION 1.0.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Metal_Engine_cpp
    Metal_Engine_cpp/src/main.cpp
    Metal_Engine_cpp/src/backend/glfw_adapter.mm
    Metal_Engine_cpp/src/backend/mtlm.cpp
    Metal_Engine_cpp/src/control/app.cpp
    Metal_Engine_cpp/src/model/camera.cpp
    Metal_Engine_cpp/src/view/gen/battlefield.cpp
    Metal_Engine_cpp/src/view/vertex_formats.cpp
    Metal_Engine_cpp/src/view/mesh.cpp
    Metal_Engine_cpp/src/view/image.cpp
    Metal_Engine_cpp/src/view/mesh_factory.cpp
    Metal_Engine_cpp/src/view/pipeline_factory.cpp
    Metal_Engine_cpp/src/view/renderer.cpp
)

# metal-cpp headers and project sources.
target_include_directories(Metal_Engine_cpp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Metal_Engine_cpp/src
)

# Keep Objective-C++ files non-ARC to avoid metal-cpp bridge-cast issues.
target_compile_options(Metal_Engine_cpp
    PRIVATE
        $<$<COMPILE_LANGUAGE:OBJCXX>:-fno-objc-arc>
)

find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
find_library(METALKIT_FRAMEWORK MetalKit REQUIRED)

# Prefer CMake package for GLFW, then fallback to direct library lookup.
find_package(glfw3 QUIET)
if (TARGET glfw)
    set(GLFW_TARGET glfw)
elseif (TARGET glfw3)
    set(GLFW_TARGET glfw3)
else()
    find_library(GLFW_LIB glfw REQUIRED)
    set(GLFW_TARGET ${GLFW_LIB})
endif()

target_link_libraries(Metal_Engine_cpp
    PRIVATE
        ${GLFW_TARGET}
        ${METAL_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${QUARTZCORE_FRAMEWORK}
        ${APPKIT_FRAMEWORK}
        ${METALKIT_FRAMEWORK}
        objc
)

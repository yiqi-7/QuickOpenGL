cmake_minimum_required(VERSION 3.20)
project(QuickOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(FetchContent)

# Dependencies are fetched at configure time. First configure requires network access.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git GIT_TAG v2.0.8 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glad)
# Glad 2 provides its CMake integration from the cmake subdirectory.
set(GLAD_CMAKE_DIR "${glad_SOURCE_DIR}/cmake" CACHE STRING "" FORCE)
set(GLAD_SOURCES_DIR "${glad_SOURCE_DIR}" CACHE STRING "" FORCE)
include(${GLAD_CMAKE_DIR}/GladConfig.cmake)
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

find_package(OpenGL REQUIRED)

function(add_gl_example target source)
    add_executable(${target} ${source})
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${target} PRIVATE glfw glm::glm glad_gl_core_33 OpenGL::GL)
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()
endfunction()

file(GLOB GL_EXAMPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
foreach(gl_source IN LISTS GL_EXAMPLE_SOURCES)
    get_filename_component(gl_target "${gl_source}" NAME_WE)
    add_gl_example(${gl_target} ${gl_source})
endforeach()

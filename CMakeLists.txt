
cmake_minimum_required(VERSION 3.12)
project(OpenGLStarter)

add_subdirectory(external/glfw-3.5.1)
add_subdirectory(external/glm)

set(FT_DISABLE_ZLIB     ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   ON CACHE BOOL "" FORCE)
add_subdirectory(external/freetype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(OpenGLStarter
    main.cpp
    ${GAME_SOURCES}
)

target_include_directories(OpenGLStarter PRIVATE external/glad/include src)
target_link_libraries(OpenGLStarter PRIVATE glfw OpenGL::GL glm::glm freetype)

target_compile_definitions(OpenGLStarter PRIVATE ASSET_DIR="${CMAKE_SOURCE_DIR}/assets")

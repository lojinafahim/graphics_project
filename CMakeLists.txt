
cmake_minimum_required(VERSION 3.12)
project(OpenGLStarter)

add_subdirectory(external/glfw-3.5.1)
add_subdirectory(external/glm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# Every .cpp inside src/ is compiled automatically.
# Add new files in your own folder - no need to edit this file.
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(OpenGLStarter
    main.cpp
    ${GAME_SOURCES}
)

target_include_directories(OpenGLStarter PRIVATE external/glad/include src)
target_link_libraries(OpenGLStarter PRIVATE glfw OpenGL::GL glm::glm)

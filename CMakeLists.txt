cmake_minimum_required(VERSION 3.5)

project(Project)
set(TARGET ${CMAKE_PROJECT_NAME})
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES
    "src/main.cpp"
    "src/utils.cpp"
    "src/bsptree.cpp"
    "src/Planer.cpp"
    "depends/imgui/imgui_impl_glfw.cpp"
    "depends/imgui/imgui_impl_opengl3.cpp"
    "depends/imgui/imgui.cpp"
    "depends/imgui/imgui_demo.cpp"
    "depends/imgui/imgui_draw.cpp"
    "depends/imgui/imgui_widgets.cpp"
)

set(HEADERS 
    "stb/stb_image.h"
    "stb/stb_image_write.h"
)

add_executable(${TARGET} ${SOURCES} ${HEADERS})  # Include headers in the target

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/depends/imgui
    ${CMAKE_SOURCE_DIR}/stb
    ${GLFW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}/../include  # Adjust the glm include directory
)

target_link_libraries(${TARGET} ${OPENGL_LIBRARIES} glfw GLEW::GLEW)

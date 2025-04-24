#Template for INFOH502
#Heavily inspired from https://github.com/Polytonic/Glitter/blob/master/CMakeLists.txt

#project setup
project(3D_Game)

#set the minimal version of cmake and the minimal version of the c++ language
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)

#for glad library
add_library( glad STATIC import/glad/src/glad.c)
set(GLAD_INCLUDE "import/glad/include")
target_include_directories(glad PUBLIC ${GLAD_INCLUDE})

#for GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

#Search for glfw in case it is installed
find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    #if glfw is not found, we will use the sources from the submodules
    add_subdirectory(import/glfw)
    include_directories(import/glfw/include/)
endif()


include_directories(import/glad/include/
                    import/glm/
                    import/stb/)


#set(SOURCES_EX_08_SOL "sol/ex08/sol08.cpp" "camera.h" "shader.h" "object.h")
#set(SOURCES_EX_09_SOL "sol/ex09/sol09.cpp" "camera.h" "shader.h" "object.h")
#set(SOURCES_EX_10_SOL "sol/ex10/sol10.cpp" "camera.h" "shader.h" "object.h")
set(SOURCES_main     
    main.cpp 
    src/EBO.h src/EBO.cpp
    src/VAO.h src/VAO.cpp
    src/VBO.h src/VBO.cpp
    src/shaderClass.h src/shaderClass.cpp
    src/Texture.h src/Texture.cpp
    src/Camera.h src/Camera.cpp
    src/Mesh.h src/Mesh.cpp)
set(SOURCES_TEST 
    test.cpp 
    src/EBO.h src/EBO.cpp
    src/VAO.h src/VAO.cpp
    src/VBO.h src/VBO.cpp
    src/shaderClass.h src/shaderClass.cpp
    src/Texture.h src/Texture.cpp
    src/Camera.h src/Camera.cpp
    src/Mesh.h src/Mesh.cpp
)

add_compile_definitions(PATH_TO_OBJECTS="${CMAKE_CURRENT_SOURCE_DIR}/assets/objects")
add_compile_definitions(PATH_TO_TEXTURE="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")


if (NOT glfw3_FOUND)
    add_executable(${PROJECT_NAME}_main ${SOURCES_main})
    target_include_directories(${PROJECT_NAME}_main PUBLIC ${GLAD_INCLUDE} import/glfw/include/ src)
    target_link_libraries(${PROJECT_NAME}_main PUBLIC OpenGL::GL glfw glad)

    add_executable(${PROJECT_NAME}_test ${SOURCES_TEST})
    target_include_directories(${PROJECT_NAME}_test PUBLIC ${GLAD_INCLUDE} import/glfw/include/ src)
    target_link_libraries(${PROJECT_NAME}_test PUBLIC OpenGL::GL glfw glad)
else()
    add_executable(${PROJECT_NAME}_main ${SOURCES_main})
    target_include_directories(${PROJECT_NAME}_main PUBLIC ${GLAD_INCLUDE} src)
    target_link_libraries(${PROJECT_NAME}_main PUBLIC OpenGL::GL glfw glad)

    add_executable(${PROJECT_NAME}_test ${SOURCES_TEST})
    target_include_directories(${PROJECT_NAME}_test PUBLIC ${GLAD_INCLUDE} src)
    target_link_libraries(${PROJECT_NAME}_test PUBLIC OpenGL::GL glfw glad)
endif()
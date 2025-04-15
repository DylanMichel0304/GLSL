cmake_minimum_required(VERSION 3.10)
project(3DGLSLGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Add source files
set(SOURCES
    main.cpp
)

# Add header files
set(HEADERS
    shader.h
    camera.h
)

# Add shader files
set(SHADERS
    vertex.glsl
    fragment.glsl
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} glfw)

# Copy shader files to build directory
foreach(SHADER ${SHADERS})
    configure_file(${SHADER} ${CMAKE_CURRENT_BINARY_DIR}/${SHADER} COPYONLY)
endforeach()

# Additional compiler flags
if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
    
    # On macOS, we need to link against the framework
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
endif() 
# 3D GLSL Game

A simple 3D environment with WASD movement controls and jumping using OpenGL and GLSL shaders. This project creates a 3D world with a floor that you can navigate around.

## Prerequisites

- CMake (version 3.10 or higher)
- GLFW3
- GLM

### Installing Dependencies

#### macOS (using Homebrew):
```bash
brew install cmake glfw glm
```

#### Ubuntu/Debian:
```bash
sudo apt-get install cmake libglfw3-dev libglm-dev
```

#### Windows (using vcpkg):
```bash
vcpkg install glfw3 glm
```

## Building the Project

1. Clone the repository
2. Navigate to the project directory
3. Create and navigate to a build directory:
```bash
mkdir build && cd build
```
4. Generate the build files:
```bash
cmake ..
```
5. Build the project:
```bash
cmake --build .
```

## Running the Game

After building, run the executable from the build directory:

```bash
./3DGLSLGame
```

## Controls

- **W**: Move forward
- **A**: Move left
- **S**: Move backward
- **D**: Move right
- **Space**: Jump
- **Mouse**: Look around
- **Escape**: Exit the game

## Project Structure

- `main.cpp`: Main application code with game loop and OpenGL setup
- `shader.h`: Shader handling class
- `camera.h`: Camera movement and perspective handling
- `vertex.glsl`: Vertex shader for transformations
- `fragment.glsl`: Fragment shader for coloring
- `CMakeLists.txt`: CMake build configuration 
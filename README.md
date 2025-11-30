# 🚤 Boat Renderer - Computer Graphics Project

**Student ID:** 47933  
**Course:** Computer Graphics (14350)  
**Assignment:** Assignment 2 - Loading, Illuminating and Rendering Meshes in R³  

---

## Project Overview

A modern OpenGL 4.3 application that loads, illuminates, and renders 3D boat models with advanced graphics features including Phong shading, multiple light sources, interactive camera controls, and a professional HUD overlay.

![Boat Renderer Screenshot]

---

##  Features

### Core Requirements 
-  **OBJ Model Loading** - Full support for Wavefront .obj files with material (MTL) support
-  **Modern OpenGL Rendering** - OpenGL 4.3 Core Profile with GLSL shaders
-  **Phong Illumination Model** - Complete implementation (Ambient + Diffuse + Specular)
-  **Point Light Source** - Dynamic lighting with attenuation
-  **Interactive Camera** - Full 6DOF camera control (translation + rotation)
-  **Mouse & Keyboard Input** - GLFW-based input handling

### Advanced Features 
-  **Multiple Light Sources** - 3 configurable lights (sun, fill, camera flashlight)
-  **Realistic Water Plane** - Animated water surface with reflections
-  **Dynamic Sky Gradient** - Procedural sky background
-  **Sun Rendering** - Visual sun object in the scene
-  **Real-time HUD** - Professional on-screen display with FPS counter
-  **Material System** - Full MTL material support with multiple materials per model
-  **Smooth Shading** - Area-weighted normal calculation
-  **Gamma Correction** - Proper color space conversion (linear → sRGB)
-  **Anti-aliasing** - MSAA 4x enabled

---

## Controls

| Input | Action |
|-------|--------|
| **W / S** | Move camera forward / backward |
| **A / D** | Move camera left / right |
| **Q / E** | Move camera down / up |
| **Mouse + Left Click** | Rotate camera (hold and drag) |
| **Mouse Scroll** | Zoom in / out |
| **L** | Toggle camera flashlight on/off |
| **ESC** | Exit application |

---

## Technologies & Libraries

### Core Technologies
- **C++17** - Modern C++ standard
- **OpenGL 4.3** - Graphics API (Core Profile)
- **GLSL 4.30** - Shader language

### External Libraries
- **GLFW 3.x** - Window management and input handling
- **GLAD** - OpenGL function loader
- **GLM** - Mathematics library for graphics (vectors, matrices, transforms)

### Build System
- **CMake 3.10+** - Cross-platform build configuration
- **MinGW-w64** - GCC compiler for Windows

---

## Project Structure

```
CG_Boat_Project/
├── src/                      # Source code
│   ├── main.cpp             # Main application entry point
│   ├── shader.h             # Shader loading and management
│   ├── camera.h             # Camera system with FPS controls
│   ├── mesh.h               # OBJ loader with MTL material support
│   ├── background.h         # Sky gradient and water plane
│   ├── hud.h                # On-screen HUD system
│   ├── sun.h                # Sun object rendering
│   └── text_renderer.h      # Text rendering utilities
│
├── shaders/                  # GLSL shader programs
│   ├── vertex.glsl          # Main vertex shader
│   ├── fragment.glsl        # Main fragment shader (Phong)
│   ├── water_vertex.glsl    # Water vertex shader
│   ├── water_fragment.glsl  # Water fragment shader (animated)
│   ├── background_vertex.glsl    # Sky gradient vertex shader
│   ├── background_fragment.glsl  # Sky gradient fragment shader
│   ├── sun_vertex.glsl      # Sun vertex shader
│   └── sun_fragment.glsl    # Sun fragment shader
│
├── models/                   # 3D models
│   ├── Boat.obj             # Boat 3D model
│   └── Boat.mtl             # Boat materials
│
├── libs/                     # External libraries
│   ├── glfw/                # GLFW library
│   ├── glad/                # GLAD OpenGL loader
│   └── glm/                 # GLM mathematics library
│
├── build/                    # Build output directory
├── CMakeLists.txt           # CMake configuration
├── build.bat                # Windows build script
└── README.md                # This file
```

---

## Building the Project

### Prerequisites

#### Windows 10/11
1. **CMake** (3.10 or higher)
   - Add to system PATH during installation

#### Verify Installation
```bash
cmake --version    # Should show version 3.10+
gcc --version      # Should show GCC version
git --version      # Should show Git version
```

---

### Installation Steps

#### 1. Clone or Download External Libraries

```bash
# Navigate to project root
cd CG_Boat_Project

# Install GLFW
cd libs
git clone https://github.com/glfw/glfw.git
cd ..

# Install GLM
cd libs
git clone https://github.com/g-truc/glm.git
cd ..
```

#### Option A: Using the Build Script (Recommended)

```bash
# In project root directory
build.bat
```

#### Option B: Manual Compilation

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles"

# Compile
mingw32-make

# Run
BoatRenderer.exe
```

---

## Technical Implementation

### Rendering Pipeline

1. **Sky Background** - Full-screen quad with vertical gradient
2. **Water Surface** - Large horizontal plane with animated waves (vertex displacement)
3. **Sun Object** - Billboard sphere with glow effect
4. **Boat Model** - OBJ mesh with Phong-shaded materials
5. **HUD Overlay** - 2D elements rendered last with depth testing disabled

### Lighting System

The application implements a sophisticated lighting system with three light sources:

#### Light 1: Sun (Primary Light)
- **Position**: (30, 25, -20)
- **Color**: Warm white (1.0, 0.95, 0.8)
- **Intensity**: 100%
- **Type**: Directional with distance attenuation

#### Light 2: Fill Light
- **Position**: (-5, 4, 3)
- **Color**: White (1.0, 1.0, 1.0)
- **Intensity**: 50%
- **Purpose**: Softens shadows, adds depth

#### Light 3: Camera Flashlight (Toggleable)
- **Position**: Follows camera
- **Color**: Warm white (1.0, 0.95, 0.9)
- **Intensity**: 80%
- **Control**: Toggle with 'L' key

### Phong Illumination Formula

```glsl
// Ambient component
vec3 ambient = lightColor * material.ambient * 0.35;

// Diffuse component
float diff = max(dot(normal, lightDir), 0.0);
vec3 diffuse = diff * lightColor * material.diffuse;

// Specular component (Phong)
vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
vec3 specular = spec * lightColor * material.specular;

// Distance attenuation
float distance = length(lightPos - fragPos);
float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance²);

// Final color
vec3 result = (ambient + diffuse + specular) * attenuation;
```

### Camera System

- **Type**: Free-flying FPS-style camera
- **Movement**: WASDQE keys with configurable speed (2.5 units/second)
- **Rotation**: Mouse look with adjustable sensitivity (0.1)
- **Zoom**: Mouse scroll (FOV 1° - 45°)
- **Initial Position**: (0, 3, 10)
- **Up Vector**: (0, 1, 0)

---

## Performance Metrics

### Typical Performance (on mid-range hardware)
- **FPS**: 120-200 fps (depending on GPU)
- **Vertices**: ~50,000 (boat model)
- **Triangles**: ~25,000
- **Draw Calls**: 5 (sky, water, sun, boat, HUD)
- **Shader Programs**: 4 active

### Optimization Techniques
- Vertex Array Objects (VAO) for efficient geometry
- Index Buffer Objects (EBO) for reduced data
- Frustum culling (implicit via projection matrix)
- Face culling disabled (to show boat interior)
- MSAA 4x for anti-aliasing
- Efficient HUD rendering (minimal draw calls)

---

## Troubleshooting

### Build Errors

**Problem**: `cannot find -lglfw3`  
**Solution**: Ensure GLFW is properly installed in `libs/glfw/`

**Problem**: `glad.h: No such file or directory`  
**Solution**: Verify GLAD is extracted correctly to `libs/glad/include/glad/`

**Problem**: CMake can't find compiler  
**Solution**: Add MinGW bin directory to system PATH

### Runtime Errors

**Problem**: Black screen / no model visible  
**Solution**: 
- Check that `Boat.obj` exists in `models/`
- Verify model scale (try adjusting camera distance with Q/E)
- Check console for "Loaded OBJ" confirmation message

**Problem**: Very low FPS  
**Solution**:
- Update graphics drivers
- Reduce MSAA samples in main.cpp: `glfwWindowHint(GLFW_SAMPLES, 2)`
- Disable water animation or sun rendering if needed

**Problem**: Shader compilation errors  
**Solution**:
- Verify shader files are in `build/shaders/` after compilation
- Check OpenGL version support: GPU must support OpenGL 4.3+

---


**End of Documentation** 
# OpenGL Rendering Engine

A C++ application for visualizing and manipulating 3D models with a deferred shading pipeline, real-time lighting, and a user-friendly IMGUI interface for scene customization.

## Features

### Editor Available options
![image](https://github.com/user-attachments/assets/e87a2633-8b33-4325-b543-dec7427bc29f)

### Example predefined scene

![gk3](https://github.com/user-attachments/assets/831055a3-3161-4fec-9d4a-1a5b564a809e)

### List of features

- **Automatic Dependency Download & Compilation**
  - Simple installation with minimal setup required
- **Model Loading**
  - *Supports a wide range of 3D model formats*
- ***Predefined Test Scenes***
  - *Quickly test and evaluate rendering features*
- ***Deferred Shading Pipeline***
  - *Efficient lighting calculations with high-quality rendering*
- ***Real-Time Lighting***
  - *Blinn-Phong lighting model*
  - *Arbitrary number of global, directional, and point lights*
- ***IMGUI-Based UI***
  - *Modify the scene interactively*
- ***Scene Management***
  - *Save and load scene configurations*
- ***Dynamic World Settings***
  - *Adjustable environment parameters*
  - *Simulated world time progression*
- ***Fog Support***
  - *Configurable atmospheric effects*

## *Technologies*

- ***C++***
- ***OpenGL***
- ***IMGUI***
- ***CMake***

## *Building & Installation*

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
cmake --build .
```

## *License*

*MIT*

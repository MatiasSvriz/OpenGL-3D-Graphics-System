# OpenGL 3D Graphics System

An interactive 3D graphics application developed in **C++ using OpenGL and GLSL** as part of a Computer Graphics university project.

The application features a solar system scene where users can explore a 3D environment, interact with celestial objects, and shoot projectiles with collision detection.

## Features

- **3D Rendering:** Real-time rendering using OpenGL, vertex buffers, textures, and depth testing.
- **Custom Shaders:** GLSL vertex and fragment shaders for lighting, materials, and transformations.
- **Camera System:** First-person camera with keyboard movement and mouse-controlled orientation.
- **Lighting:** Ambient, diffuse, and specular lighting.
- **Collision Detection:** Sphere-based collision detection between objects, projectiles, and the camera.
- **Interactive Gameplay:** Projectile shooting, object hit points, and collision-based interactions.
- **3D Transformations:** Object positioning, rotations, scaling, and perspective projection.

## Technologies

- C++20
- OpenGL
- GLSL
- GLFW and GLAD
- Visual Studio

## Controls

| Key | Action |
|---|---|
| W / A / S / D | Move camera |
| Mouse | Rotate camera |
| Left mouse button + movement | Control camera orientation |
| F | Shoot |
| Esc | Exit |

## Project Structure

```text
OpenGL-3D-Graphics-System/
├── ProgGrafica_2024/
│   ├── Camera.cpp / Camera.h
│   ├── Colliders.cpp / Colliders.h
│   ├── Object3D.cpp / Object3D.h
│   ├── Render.cpp / Render.h
│   ├── Shader.cpp / Shader.h
│   ├── Planet.cpp / Planet.h
│   ├── Bullet.cpp / Bullet.h
│   └── data/
│       ├── shader.vertex
│       ├── shader.fragment
│       ├── 3D models
│       └── textures
├── externalLibs/
├── ProgGrafica_2024.sln
└── README.md
```

## Getting Started

1. Clone the repository:

   ```bash
   git clone https://github.com/MatiasSvriz/OpenGL-3D-Graphics-System.git
   ```

2. Open `ProgGrafica_2024.sln` in Visual Studio.
3. Select the `x64` configuration and ensure the required C++ toolset is installed.
4. Build and run the project. The application uses the `ProgGrafica_2024/` directory as its working directory to load scene assets.

## About

This project helped me explore the fundamentals of real-time computer graphics, including the OpenGL rendering pipeline, shader programming, 3D mathematics, and collision detection.

It combines graphics programming in C++ with interactive mechanics to create a small 3D environment.

# OpenGL 3D Graphics System — Solar Shooter

A C++20/OpenGL project exploring real-time 3D rendering, custom GLSL shaders, camera transformations, textured materials, lighting, and simple collision-driven gameplay.

The current demo is an interactive **solar-system shooting scene**: navigate a first-person camera, shoot projectiles toward celestial objects, and detect sphere-based collisions. This project was developed as a computer graphics coursework exercise and extended with interactive features.

> **Repository status:** Source and selected demo assets are included, but a reproducible build has **not** yet been verified on a clean machine. The Visual Studio project expects separately configured GLAD and GLFW dependencies. See [Build](#build-on-windows) and [Publication checks](#publication-checks).

## Features

- **Rendering:** OpenGL vertex buffers, index buffers, vertex array objects, depth testing and alpha blending.
- **GLSL:** Vertex/fragment shader pipeline, model and model-view-projection transformations, transformed normals, textures and ambient/diffuse/specular illumination.
- **Camera:** First-person navigation with perspective projection and mouse-based orientation.
- **3D objects:** Custom `.frs` mesh/scene data loader, object transformations, materials and texture loading with `stb_image`.
- **Collisions:** Bounding-sphere overlap tests; camera movement rollback on contact; projectile/planet and projectile/sun impacts.
- **Interactive scene:** Projectiles with a firing cooldown, per-planet hit points and removal of inactive objects.

**Scope:** The collisions are approximate bounding-sphere tests, not a full rigid-body physics engine. Simulation and input updates use frame delta time; no deterministic fixed-timestep simulation loop is claimed. Planet orbital motion is implemented in code but its speed is set to zero in the current demo scene.

## Controls

| Input | Action |
|---|---|
| W / A / S / D or arrow keys | Move the camera |
| Left mouse button + mouse movement | Rotate camera view |
| F | Fire a projectile |
| Esc | Close the application |

## Technology

- C++20; Microsoft Visual C++ / Visual Studio project
- GLFW for windowing and input; GLAD for OpenGL function loading
- OpenGL 4.1 core context requested by the application; GLSL shaders declare `#version 330`
- `stb_image.h` for image loading
- Custom math routines (`libMath.h`)

## Project structure

```text
.
├── ProgGrafica_2024/
│   ├── MainPRGR_2024.cpp     # Demo scene entry point
│   ├── Render.{h,cpp}        # OpenGL setup, rendering, update loop and collisions
│   ├── Object3D.{h,cpp}      # Mesh loading, object transforms, materials
│   ├── Camera.{h,cpp}        # Camera and view/projection matrices
│   ├── Colliders.{h,cpp}     # Sphere collider
│   ├── Shader.{h,cpp} / Program.{h,cpp}
│   ├── Planet.{h,cpp} / Sun.{h,cpp} / Bullet.{h,cpp}
│   ├── data/                 # GLSL shaders, demo geometry and texture assets
│   └── ProgGrafica_2024.vcxproj
└── README.md
```

## Build on Windows

1. Open `ProgGrafica_2024/ProgGrafica_2024.vcxproj` in Visual Studio (or add the project to a `.sln`). Install the **Desktop development with C++** workload.
2. The provided `.vcxproj` references two MSVC toolsets: **v143** for Debug x64 and **v145** for Release x64. Install the relevant toolset or change the project property **Platform Toolset** to a version installed on your machine.
3. Provide the expected dependencies in a folder next to `ProgGrafica_2024/`:

   ```text
   externalLibs/
   ├── glad/inc/glad/gl.h
   ├── glfw/inc/GLFW/glfw3.h
   ├── glfw/lib/glfw3.lib
   └── glm/inc/        # Configured in the project; no direct glm includes identified
   ```

   The matching GLAD implementation and other dependency-specific files may also be needed. Follow the upstream packages' instructions. **Dependencies are not included in this archive.**
4. Select `x64` and a configured build mode in Visual Studio. Resolve any dependency/toolset issues reported by the compiler and linker.
5. Run with the working directory set to **`$(ProjectDir)`** (`ProgGrafica_2024/`); the executable expects paths such as `data/earth.frs` relative to that directory. In Visual Studio, configure **Project Properties → Debugging → Working Directory** if necessary.

This is a source review, **not a tested installation recipe**; the application has not been compiled in this environment.

## Asset format

`.frs` files store mesh geometry, texture coordinates and trailing references to texture and shader files. For example, the planet assets refer to `data/shader.vertex` and `data/shader.fragment`. Selected assets required by the current demo are staged under `data/`.

## Publication checks

- **Confirm reuse rights** for the JPEG textures and `.frs` models before committing them to a public repository. This preparation archive includes them for local review, not as evidence of a redistribution license. If unsure, replace the assets with original or appropriately licensed versions.
- Do not commit build outputs, user-specific Visual Studio files, local dependencies or unreviewed binary archives; see `.gitignore`.
- The original ZIP does not contain the `.sln` file visible in the local folder screenshots. You may copy your local `.sln` to the repo root after verifying its project paths, but opening the `.vcxproj` directly also works in Visual Studio.
- One unused source file, `Explosion.cpp`, references a missing `Explosion.h` and is not listed in the Visual Studio project's compile list. It has been deliberately omitted from this curated copy; the supplied original ZIP is unchanged.
- Add a real demo screenshot or video, verify a clean build, and document resource credits before sharing with recruiters.

## Attribution and license

This archive **does not assign a license to the entire project**. Review third-party source and media licenses and choose an appropriate license for code you own. `stb_image.h` carries its upstream licensing information; its notice has been preserved.

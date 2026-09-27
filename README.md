# Astronomical Simulator (C++ / GLUT / OpenGL)

> *Placeholder for the final project description: goal, motivation, context (coursework, personal project, etc.), and build/run instructions.*

## Architecture overview

The project follows a separation of responsibilities very close to what's used in real game engines and physics engines (Unity, Godot, Box2D, Bullet): each class takes care of **one** thing, and "composite" objects (like a planet) are assembled by putting these pieces together through composition, not inheritance.

```
DataStructures   -> pure math (Vector, Matrix, HomogeneousMatrix) + OpenGL wrappers + colors
Transform        -> stateless point/vector transformation utilities (translate, rotate, scale, matrix builders)
LightSource      -> a light's position + ambient/diffuse/specular color, applied to an OpenGL light unit
RigidBody        -> mass, velocity, position, force accumulation and motion integration
Mesh             -> geometry, lighting material (ambient/diffuse/specular/emission), visual transforms and rendering
CelestialBody    -> combines RigidBody + Mesh into one astronomical body (Sun, Earth, Moon...)
PhysicsWorld     -> manages every CelestialBody, computes N-body gravity and steps the simulation forward
Camera           -> camera position/orientation and projection
Simulation       -> keyboard/mouse input and the time loop (deltaTime)
```

## Classes

### `DataStructures` (namespace)
The math foundation everything else builds on. Defines `Vector`, `Matrix`, `HomogeneousVector` and `HomogeneousMatrix` as fixed-size `std::array` aliases, plus `ColorVector` for RGB colors. Provides the core vector/matrix operations (`normalize`, `addVectors`, `magnitude`, `distance`, `crossProduct`, `multiplyHMatrices`, `multiplyHMatrixVector`) and debug printers (`printVector`, `printMatrix`, `printHMatrix`).

Also bundles an OpenGL layer: the `gl` struct wraps calls like `glVertex3dv`/`glNormal3dv`/`glColor3fv` so custom `Vector`/`ColorVector` types can be passed straight into OpenGL without manual conversion, and a set of predefined `ColorVector` constants (`WHITE`, `RED`, `BLUE`, `YELLOW`, etc.) are used throughout the other classes for consistent coloring.

### `Transform` (namespace)
A stateless collection of point-transformation utilities - a namespace rather than a class, since (like `DataStructures`) it holds no state of its own: every function just operates on the `Vector`(s) or matrix it's given, mirroring the `crossProduct`/`multiplyHMatrices`-style free functions already used elsewhere in the project.

Translation is the only transformation that doesn't depend on a reference point; rotation and scale both need a **center** they're performed around (built internally by sandwiching the raw rotation/scale matrix between a translation to the center and back: `T(center) * R * T(-center)`). Two families of functions are provided:

- **In-place mutators** - `translate`, `rotate`, `scale`, each overloaded for a single `Vector&` or a `std::vector<Vector>&` (since the list can be of variable length). They mutate their argument directly and return nothing.
- **Matrix builders + application** - `getTranslationMatrix`, `getRotationMatrix(rotationCenter, pitch, yaw, roll)` and `getScaleMatrix(scaleCenter, scaleFactor)` return the corresponding `HomogeneousMatrix`, and `applyMatrix` applies a (possibly pre-composed) matrix to a single `Vector` or a `std::vector<Vector>`. Useful when you want to combine several transformations into one matrix before applying it once, instead of mutating the same points repeatedly.

### `LightSource`
Stores a light's `position` and its `ambientLight`/`diffuseLight`/`specularLight` colors, and knows how to push itself into OpenGL's fixed-function lighting pipeline as one of the `GL_LIGHTx` units via `apply(lightUnit)` (which also enables `GL_LIGHTING` and that unit). `scaleIntensity(factor)` multiplies all three color components by the same factor, so a value below 1.0 dims the light and above 1.0 brightens it - covering both "multiply" and "divide" with a single method, as division is just multiplication by `1/factor`.

One OpenGL subtlety worth calling out: a light's position is transformed by whatever modelview matrix is active *at the moment `apply()` is called* - so it must be called every frame, after the camera transform (`gluLookAt`) and before drawing any lit geometry, or the light will appear to drift as the camera moves. See `draw()` in `main.cpp` for where that happens.

### `RigidBody`
The physical representation of a body: mass, velocity, accumulated force, and **position** - it lives here rather than in a separate transform component, since position is a physical quantity that the simulation (gravity, integration) is directly responsible for. `integrate(deltaTime)` advances velocity and position using **semi-implicit (symplectic) Euler integration** (more energy-stable than explicit Euler - important for orbits, which would otherwise show noticeable energy drift over time), applies the resulting displacement to its own position via `Transform::translate`, clears the accumulated force, and **returns the change in position** so the caller can move the visual mesh by the same amount.

### `Mesh`
Holds vertices, faces (lists of indices - supports triangles, quads or larger polygons) and per-face normals, and keeps its vertices in **world space** directly rather than relying on a separate model matrix applied at render time.

It also owns the body's **lighting material**: `ambient`/`diffuse`/`specular` (how it reacts to external `LightSource`s) and `shininess`, set together via `setMaterial(...)`, plus `emission` (light it emits on its own, regardless of any external light) via `setEmission(...)`. A freshly-constructed `Mesh` derives a sensible default material from its `color` (dim ambient, full diffuse, modest specular) precisely so that, unless told otherwise, it **reacts normally to light** - never silently "transparent" to it. `render()` sends the material to OpenGL via `glMaterialfv` before drawing the filled faces; `renderWireframe()` temporarily disables `GL_LIGHTING` so its `color` is drawn flat, as expected for orbit rings/debug lines.

`translate`/`rotate`/`scale` are thin wrappers that hand the vertex list to the corresponding `Transform` function - rotation and scale need a reference point (typically the body's current position, for spinning or resizing in place) and recompute face normals afterwards, since those change face orientation; translation leaves normals unchanged. It also provides:
- `generateSphere(radius, stacks, slices, color)` - generates a UV sphere centered at the origin, ideal for planets/moons/the sun;
- `generateOrbitRing(radius, segments, color)` - generates a ring of points on the XZ plane, for drawing an orbital path.

### `CelestialBody`
A composition of `RigidBody` + `Mesh`, plus a name and a physical radius (used for gravity/collision calculations, independent of the render scale). Its constructor moves the (origin-centered) mesh to the body's initial position once; from then on, `advance(deltaTime)` integrates the `RigidBody` and translates the `Mesh` by the resulting delta, keeping the physical and visual positions in sync, and `spin(pitch, yaw, roll)` rotates the mesh around the body's current position for axial rotation. `render()` simply draws the mesh, since it already lives in world space.

Whether a given body reacts to a `LightSource` or emits its own light is purely a `Mesh` material choice (see above) - `CelestialBody` doesn't need to know or care which kind it composes; `main.cpp` demonstrates using this to turn the Sun's mesh into a self-illuminated, unlit-by-others object (see below).

### `PhysicsWorld`
Keeps a list of pointers to `CelestialBody` (it does not own the memory - whoever creates the bodies is responsible for them) and, on every `step(deltaTime)`:
1. Computes the gravitational force between **every pair** of bodies (O(n²), fine for a handful of bodies - Sun/Earth/Moon, etc.), using each body's `getPosition()`;
2. Applies the law of universal gravitation (`F = G·m₁·m₂/r²`);
3. Calls `advance(deltaTime)` on every body, which integrates its `RigidBody` and keeps its `Mesh` in sync.

It also exposes `renderAll()` to draw every body at once.

### `Camera`
Represents the viewer's position and orientation in the scene. Stores `position`, a `direction` vector derived from `pitch`/`yaw` (spherical-to-Cartesian conversion), an `up` vector, and the projection parameters (`fov`, `aspectRatio`, `nearPlane`, `farPlane`). Also snapshots its construction-time parameters into `initialParameters`, so the camera can be reset on demand.

`getCameraParameters()` packs eye/target/up into a `Matrix` ready to feed straight into `gluLookAt`. Movement is split into `move` (free translation), `moveForwardBackward`/`moveLeftRight` (movement relative to where the camera is facing, ignoring the Y axis - i.e. it walks rather than flies), and rotation via `updateRotation` (mouse-look style, accumulating pitch/yaw deltas) or `lookAt` (points the camera directly at a target).

### `Simulation`
Owns the GLUT-facing side of the program: keyboard/mouse input and the frame timer. Because GLUT callbacks are plain C function pointers (not member functions), `Simulation` keeps a single static `instance` pointer and a set of static wrapper functions (`glutKeyboardCallback`, `glutMouseCallback`, etc.) that forward each call to the real instance methods.

Input state is tracked continuously: `keyStates[256]` records which keys are currently held (so movement stays smooth instead of firing once per keypress), Shift toggles a speed boost, and pressing `r` resets the camera to its `initialParameters`. Dragging the mouse rotates the camera based on cursor movement since the last frame.

`updateSimulation()` is called roughly every 16 ms via `glutTimerFunc`. It computes `deltaTime` (clamped to avoid huge jumps if the window was dragged/paused), applies WASD-style camera movement scaled by `deltaTime`, steps the `PhysicsWorld` forward by `deltaTime` if one is attached, and requests a redraw with `glutPostRedisplay()`.

## How lighting fits into the scene (see `main.cpp`)

- A single `LightSource` is created at the Sun's position, with a modest ambient term and full-strength diffuse/specular - a fairly standard "sunlight" setup.
- Earth and Moon are left with `Mesh`'s default material, so they respond normally to the light and are shaded like any lit object should be.
- The Sun's `Mesh` is explicitly reconfigured in `initOpenGL()`: `setMaterial({0,0,0}, {0,0,0}, {0,0,0}, 0.0f)` zeroes out its reaction to external light, and `setEmission(YELLOW)` makes it glow with its own color regardless. This isn't just cosmetic - a sphere sitting exactly at its own light's position has, for most of its faces, a direction-to-light roughly opposite its outward normal, so the ordinary ambient/diffuse/specular equation would render much of it almost black. Emission sidesteps that entirely.
- OpenGL's fixed-function lighting has no shadows or occlusion built in, so the Sun's mesh existing in the scene never "blocks" light from reaching Earth or the Moon - nothing extra was needed for that part.
- In `draw()`, `sunLight->apply(GL_LIGHT0)` is called every frame, right after `gluLookAt` and before `physicsWorld->renderAll()` - light position is modelview-relative at the moment it's set, so this ordering keeps the light fixed in world space as the camera moves.

## How the physics world fits into the simulation loop

`Simulation` holds an optional `PhysicsWorld*` member (analogous to its `Camera*`), passed in through the constructor. Inside `updateSimulation()`, right after computing `deltaTime`:

```cpp
if (physicsWorld != nullptr) {
    physicsWorld->step(deltaTime);
}
```

This keeps `Simulation` usable even without any physics bodies attached (e.g. while testing the camera alone).

## Suggested next steps

- Add simple collision detection between bodies using `radius` (useful for detecting "impacts" or merging bodies);
- Replace the O(n²) gravity computation with a Barnes-Hut tree if the number of bodies grows large;
- Add texture support to `Mesh` (UV coordinates) to map planet textures;
- Add per-vertex (rather than per-face) normals and enable `GL_SMOOTH`/`GL_NORMALIZE` for smoother-looking spheres, especially if non-uniform scaling is introduced later;
- Give `PhysicsWorld` (or `CelestialBody`) a way to snapshot and restore each body's initial position/velocity (the way `Camera` already does with `initialParameters`) to allow "resetting" the simulation.

# Astronomical Simulator (C++ / GLUT / OpenGL)

> *Placeholder for the final project description: goal, motivation, context (coursework, personal project, etc.), and build/run instructions.*

## Architecture overview

The project follows a separation of responsibilities very close to what's used in real game engines and physics engines (Unity, Godot, Box2D, Bullet): each class takes care of **one** thing, and "composite" objects (like a planet) are assembled by putting these pieces together through composition, not inheritance.

```
DataStructures   -> pure math (Vector, Matrix, HomogeneousMatrix) + OpenGL wrappers + colors
Transform        -> stateless point/vector transformation utilities (translate, rotate, scale, matrix builders)
RigidBody        -> mass, velocity, position, force accumulation and motion integration
Mesh             -> geometry (vertices, faces, normals), visual rotation/scale and rendering
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

### `RigidBody`
The physical representation of a body: mass, velocity, accumulated force, and now also **position** - it lives here rather than in a separate transform component, since position is a physical quantity that the simulation (gravity, integration) is directly responsible for. `integrate(deltaTime)` advances velocity and position using **semi-implicit (symplectic) Euler integration** (more energy-stable than explicit Euler - important for orbits, which would otherwise show noticeable energy drift over time), applies the resulting displacement to its own position via `Transform::translate`, clears the accumulated force, and **returns the change in position** so the caller can move the visual mesh by the same amount.

### `Mesh`
Holds vertices, faces (lists of indices - supports triangles, quads or larger polygons) and per-face normals, and keeps its vertices in **world space** directly rather than relying on a separate model matrix applied at render time. `translate`/`rotate`/`scale` are thin wrappers that hand the vertex list to the corresponding `Transform` function - rotation and scale need a reference point (typically the body's current position, for spinning or resizing in place) and recompute face normals afterwards, since those change face orientation; translation leaves normals unchanged. It also provides:
- `render()` - draws the filled faces using the `DataStructures::gl` wrappers;
- `renderWireframe()` - draws them as `GL_LINE_LOOP`, useful for orbits or debugging;
- `generateSphere(radius, stacks, slices, color)` - generates a UV sphere centered at the origin, ideal for planets/moons/the sun;
- `generateOrbitRing(radius, segments, color)` - generates a ring of points on the XZ plane, for drawing an orbital path.

### `CelestialBody`
A composition of `RigidBody` + `Mesh`, plus a name and a physical radius (used for gravity/collision calculations, independent of the render scale). Its constructor moves the (origin-centered) mesh to the body's initial position once; from then on, `advance(deltaTime)` integrates the `RigidBody` and translates the `Mesh` by the resulting delta, keeping the physical and visual positions in sync, and `spin(pitch, yaw, roll)` rotates the mesh around the body's current position for axial rotation. `render()` simply draws the mesh, since it already lives in world space.

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
- Give `PhysicsWorld` (or `CelestialBody`) a way to snapshot and restore each body's initial position/velocity (the way `Camera` already does with `initialParameters`) to allow "resetting" the simulation.

# Astronomical Simulator (C++ / GLUT / OpenGL)

> *Placeholder for the final project description: goal, motivation, context (coursework, personal project, etc.), and build/run instructions.*

## Architecture overview

The project follows a separation of responsibilities very close to what's used in real game engines and physics engines (Unity, Godot, Box2D, Bullet): each class takes care of **one** thing, and "composite" objects (like a planet) are assembled by putting these pieces together through composition, not inheritance.

```
DataStructures   -> pure math (Vector, Matrix, HomogeneousMatrix, LightVector) + OpenGL wrappers + colors
Transform        -> stateless point/vector transformation utilities (translate, rotate, scale, matrix builders)
LightSource      -> a light's position + ambient/diffuse/specular LightVector, applied to an OpenGL light unit
RigidBody        -> mass, velocity, position, force accumulation and motion integration
Mesh             -> geometry, lighting material + alpha, visual transforms, rendering, and shape factories (sphere/ring/tube)
CelestialBody    -> combines RigidBody + Mesh into one astronomical body (Sun, Earth, Moon...)
PetrovaLine      -> a Bezier-curve stream of astrophages (+ a translucent central tube) between a star and a CO2-rich planet
SkySphere        -> a field of tiny emissive "stars" scattered on a large backdrop sphere of a given radius
PhysicsWorld     -> manages every CelestialBody (+ optionally a PetrovaLine/SkySphere/pulsing Sun), steps and renders the scene
Camera           -> camera position/orientation and projection
Simulation       -> keyboard/mouse input, the time loop (deltaTime), and the simulation speed multiplier
```

## Classes

### `DataStructures` (namespace)
The math foundation everything else builds on. Defines `Vector`, `Matrix`, `HomogeneousVector` and `HomogeneousMatrix` as fixed-size `std::array` aliases, `ColorVector` for RGB colors, and `LightVector` for RGBA light/material values (used by `LightSource` and `Mesh`'s material properties). Provides the core vector/matrix operations (`normalize`, `addVectors`, `subVectors`, `multiplyVectorScalar`, `magnitude`, `distance`, `dotProduct`, `crossProduct`, `findAngle3Points`, `multiplyHMatrices`, `multiplyHMatrixVector`, `hadamardProduct` for element-wise `LightVector` multiplication) and debug printers (`printVector`, `printMatrix`, `printHMatrix`). `findAngle3Points(A, vertex, B)` returns the angle at `vertex` between rays to `A` and `B` - used by `PetrovaLine` to track how far a planet has swept around its star between frames.

Also bundles an OpenGL layer: the `gl` struct wraps calls like `glVertex3dv`/`glNormal3dv`/`glColor3fv` so custom `Vector`/`ColorVector` types can be passed straight into OpenGL without manual conversion; a set of predefined `ColorVector` constants (`WHITE`, `RED`, `BLUE`, `YELLOW`, etc.) and default `LightVector` lighting presets (`DEFAULT_AMBIENT_LIGHT`, `DEFAULT_DIFFUSE_LIGHT`, `DEFAULT_SPECULAR_LIGHT`, `VOID_LIGHT`) are used throughout the other classes.

### `Transform` (namespace)
A stateless collection of point-transformation utilities - a namespace rather than a class, since (like `DataStructures`) it holds no state of its own: every function just operates on the `Vector`(s) or matrix it's given, mirroring the `crossProduct`/`multiplyHMatrices`-style free functions already used elsewhere in the project.

Translation is the only transformation that doesn't depend on a reference point; rotation and scale both need a **center** they're performed around (built internally by sandwiching the raw rotation/scale matrix between a translation to the center and back: `T(center) * R * T(-center)`). Two families of functions are provided:

- **In-place mutators** - `translate`, `rotate`, `scale`, each overloaded for a single `Vector&` or a `std::vector<Vector>&` (since the list can be of variable length). They mutate their argument directly and return nothing.
- **Matrix builders + application** - `getTranslationMatrix`, `getRotationMatrix(rotationCenter, pitch, yaw, roll)` and `getScaleMatrix(scaleCenter, scaleFactor)` return the corresponding `HomogeneousMatrix`, and `applyMatrix` applies a (possibly pre-composed) matrix to a single `Vector` or a `std::vector<Vector>`. Useful when you want to combine several transformations into one matrix before applying it once, instead of mutating the same points repeatedly.

Note that `Mesh::scale` (and `rotate`) is **relative/compounding**, not absolute - each call multiplies the current geometry by the given factor (or rotates it further), rather than setting an absolute size or orientation. `PhysicsWorld`'s Sun-pulsing animation (see below) takes this into account.

### `LightSource`
Stores a light's `position` and its `ambientLight`/`diffuseLight`/`specularLight` `LightVector`s (RGBA), and knows how to push itself into OpenGL's fixed-function lighting pipeline as one of the `GL_LIGHTx` units via `apply(lightUnit)` (which also enables `GL_LIGHTING` and that unit). `scaleIntensity(factor)` multiplies all three light components by the same factor, so a value below 1.0 dims the light and above 1.0 brightens it - covering both "multiply" and "divide" with a single method, as division is just multiplication by `1/factor`.

One OpenGL subtlety worth calling out: a light's position is transformed by whatever modelview matrix is active *at the moment `apply()` is called* - so it must be called every frame, after the camera transform (`gluLookAt`) and before drawing any lit geometry, or the light will appear to drift as the camera moves. See `draw()` in `main.cpp` for where that happens.

### `RigidBody`
The physical representation of a body: mass, velocity, accumulated force, and **position** - it lives here rather than in a separate transform component, since position is a physical quantity that the simulation (gravity, integration) is directly responsible for. `integrate(deltaTime)` advances velocity and position using **semi-implicit (symplectic) Euler integration** (more energy-stable than explicit Euler - important for orbits, which would otherwise show noticeable energy drift over time), applies the resulting displacement to its own position via `Transform::translate`, clears the accumulated force, and **returns the change in position** so the caller can move the visual mesh by the same amount.

### `Mesh`
Holds vertices, faces (lists of indices - supports triangles, quads or larger polygons), and keeps its vertices in **world space** directly rather than relying on a separate model matrix applied at render time.

It computes both **per-face** normals (`computeFaceNormals`) and **per-vertex** normals (`computeVertexNormals`, which averages the normals of every face touching a vertex). `render()` uses the per-vertex ones - that's what makes `glShadeModel(GL_SMOOTH)` (set once in `main.cpp`'s `initOpenGL()`) actually produce smooth-looking spheres instead of a faceted look.

It also owns the body's **lighting material**: `ambient`/`diffuse`/`specular` are `LightVector`s built with equal R/G/B components (homogeneous by construction) via `setMaterial(ambientIntensity, diffuseIntensity, specularIntensity, shininess)`. The mesh's actual color always comes from `color`/`extendedColor`: `render()` combines them with `hadamardProduct(material, extendedColor)` to build the final `glMaterial` arrays.

`emission` (a full `LightVector`) is the light the mesh emits on its own, via `setEmission(...)`. A mesh is simply **`emissive`** or not - passed as a `bool` to the constructor (and every shape factory), or toggled later with `setEmissive(bool)`:
- `false` (default): normal material (ambient/diffuse/specular > 0, no emission) - the mesh **reacts to light**;
- `true`: material intensities at 0 and `emission = color` - the mesh **glows with its own color**, regardless of any external light (used for the Sun, astrophages, the central Petrova line, and the sky's stars).

**Alpha / translucency**: every constructor and shape factory now also takes an `alpha` (`0.0` fully transparent, `1.0` fully opaque, default `1.0`), and `setAlpha(float)`/`getAlpha()` change it afterwards. Under OpenGL's fixed-function lighting, blending is actually driven by the **diffuse material's alpha channel** (not `glColor`'s alpha, unless `GL_COLOR_MATERIAL` is active - which this project doesn't use), so `setAlpha` writes straight into `diffuse`'s (and, for emissive meshes, `emission`'s) alpha component rather than some separate field. `main.cpp`'s `initOpenGL()` enables `GL_BLEND`/`glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` once, globally - alpha blending is otherwise a no-op with the default `alpha = 1.0`, so this doesn't change how any existing opaque mesh looks.

`renderWireframe()` temporarily disables `GL_LIGHTING` so its `color` is drawn flat, as expected for orbit rings/debug lines.

`translate`/`rotate`/`scale` are thin wrappers that hand the vertex list to the corresponding `Transform` function - rotation and scale need a reference point and recompute normals afterwards; translation leaves normals unchanged. Three shape factories are provided, all generated already in world space or centered at the origin:
- `generateSphere(radius, stacks, slices, color, emissive, alpha)` - a UV sphere centered at the origin, ideal for planets/moons/the sun/astrophages/stars;
- `generateOrbitRing(radius, segments, color, emissive, alpha)` - a ring of points on the XZ plane, for drawing an orbital path (call `renderWireframe()` on the result);
- `generateTube(centerPoints, radius, sides, color, emissive, alpha)` - builds a tube around an arbitrary polyline: a ring of `sides` vertices is placed around each point in `centerPoints`, in the plane perpendicular to the curve's local tangent there (approximated by a forward/backward difference between neighboring points, with an arbitrary-but-consistent perpendicular basis built via two cross products), and consecutive rings are connected into quad faces. Needs at least 2 center points and 3 sides; already in world space, unlike the other two factories. This is what `PetrovaLine`'s central line is built from (see below).

### `CelestialBody`
A composition of `RigidBody` + `Mesh`, plus a name and a physical radius (used for gravity/collision calculations, independent of the render scale). Its constructor moves the (origin-centered) mesh to the body's initial position once; from then on, `advance(deltaTime)` integrates the `RigidBody` and translates the `Mesh` by the resulting delta, keeping the physical and visual positions in sync, and `spin(pitch, yaw, roll)` rotates the mesh around the body's current position for axial rotation. `render()` simply draws the mesh, since it already lives in world space.

### `PetrovaLine`
A purely visual (no physics of its own) take on the Petrova line from *Project Hail Mary*: a stream of astrophages strung along a **cubic Bezier curve** between a star (`sun`) and a CO2-rich planet (`CO2planet`), built once in the constructor from `calculateBezierPoints()`.

`createAstrophages()` walks the curve in `resolution` steps, and at each step calls `calculatePoint(t, true)` to get a point on the curve with a small random offset added (scaled by `distribution`) - the "natural" scatter the astrophage cloud reads with. Each accepted point becomes a small emissive `Mesh::generateSphere(astrophageSize, 3, 3, RED, true)`, translated into place.

A `centralLine` member - a single thick, translucent tube `Mesh` - traces the curve's *exact* general direction (no random offset). `createCentralLine()` samples `LINE_SEGMENTS` points along the curve and hands them straight to `Mesh::generateTube(centers, astrophageSize * 1.5, LINE_SIDES, ORANGE, /* emissive = */ true, /* alpha = */ 0.35f)` - emissive so it reads clearly regardless of lighting, and translucent (`alpha = 0.35`) so it marks the general direction without hiding the astrophages scattered around it.

Rather than recomputing the full Bezier curve every frame, `advance()` takes a shortcut: it measures the angle the planet has swept around the Sun since the last call (`findAngle3Points`) and rotates every astrophage - and `centralLine` too - around the Sun by that same angle, via `Mesh::rotate`. This keeps the whole cloud "glued" to the Sun-planet line as the planet orbits, without rebuilding any geometry.

Getters/setters are provided for every constructor parameter and internal piece: `getSun`/`getCO2Planet`, `getResolution`/`setResolution`, `getAstrophageSize`/`setAstrophageSize`, `getDistribution`/`setDistribution`, `getAstrophages`, `getCentralLine`, and `getBezierPoints`. Each setter regenerates whatever it affects (the astrophage cloud and, for `astrophageSize`, the central line too).

### `SkySphere`
A purely visual backdrop, "in the same spirit as `PetrovaLine`" - just meshes and random scattering, no physics. Its constructor takes `center`, `starCount`, `starSize`, and `radius` (the size of the backdrop sphere itself). `createStars()` builds `starCount` tiny, low-poly, emissive `Mesh::generateSphere(starSize, 3, 3, WHITE, true)` spheres, each placed via `randomPointOnSphere()` - a uniform point on the surface of that sphere. Sampling picks `z` uniformly in `[-1, 1]` and an angle uniformly in `[0, 2π]` rather than sampling two angles uniformly, which would otherwise cluster stars near the poles.

`setCenter`/`setStarCount`/`setStarSize`/`setRadius` (with matching getters) each regenerate the whole star field, since all four directly control how it's built.

### `PhysicsWorld`
Keeps a list of pointers to `CelestialBody` (it does not own the memory), plus an optional `PetrovaLine*`, `SkySphere*`, and a `sun` pointer used only for the pulsing animation below (`setPetrovaLine`/`getPetrovaLine`, `setSkySphere`/`getSkySphere`, `setSun`/`getSun`), and an adjustable gravitational constant (`setGravitationalConstant`/`getGravitationalConstant`). On every `step(deltaTime)`:
1. Computes the gravitational force between **every pair** of bodies (`F = G·m₁·m₂/r²`, O(n²));
2. Calls `advance(deltaTime)` on every body, integrating its `RigidBody` and keeping its `Mesh` in sync;
3. Advances the `PetrovaLine`, if one is attached;
4. Calls the private `animateSun(deltaTime)`.

**Sun pulsing**: `animateSun` is deliberately kept out of `CelestialBody` - it only ever calls `Mesh::scale` through the attached Sun's `CelestialBody`, so it needs no new capability there, just a private helper here. Since `Mesh::scale` is *relative* (each call multiplies the current size, it doesn't set an absolute one - see the `Transform` note above), the animation tracks an absolute sine-wave "envelope" (`1.0 + amplitude * sin(2π·time/period)`, `period = 5s`) and applies the **ratio** between this step's envelope value and the previous one each frame. That reproduces the exact absolute envelope over time regardless of frame rate, without ever needing to know or store the Sun's actual current size.

`renderAll()` draws the `SkySphere` first (as a distant backdrop), then every `CelestialBody`, then the `PetrovaLine` - so nearer, physically-simulated objects are never hidden behind the background stars.

### `Camera`
Represents the viewer's position and orientation in the scene. Stores `position`, a `direction` vector derived from `pitch`/`yaw` (spherical-to-Cartesian conversion), an `up` vector, and the projection parameters (`fov`, `aspectRatio`, `nearPlane`, `farPlane`). Also snapshots its construction-time parameters into `initialParameters`, so the camera can be reset on demand.

`getCameraParameters()` packs eye/target/up into a `Matrix` ready to feed straight into `gluLookAt`. Movement is split into `move` (free translation), `moveForwardBackward`/`moveLeftRight` (movement relative to where the camera is facing, ignoring the Y axis), and rotation via `updateRotation` (mouse-look style) or `lookAt` (points the camera directly at a target).

### `Simulation`
Owns the GLUT-facing side of the program: keyboard/mouse input and the frame timer. Because GLUT callbacks are plain C function pointers, `Simulation` keeps a single static `instance` pointer and a set of static wrapper functions that forward each call to the real instance methods.

Input state is tracked continuously: `keyStates[256]` records which keys are currently held, Shift toggles a camera speed boost (`BASE_PLAYER_SPEED * 4`), and pressing `r` resets the camera to its `initialParameters`. Dragging the mouse rotates the camera based on cursor movement since the last frame.

`+`/`-` (handled as both `'+'`/`'='` and `'-'`/`'_'`, since keyboard layouts differ on whether `+` needs Shift) adjust **`simulationSpeed`** - a multiplier, clamped to `[MIN_SIMULATION_SPEED, MAX_SIMULATION_SPEED]` = `[0.0, 5.0]` in steps of `SIMULATION_SPEED_STEP = 0.1` - applied **only** to the `PhysicsWorld`'s time step, never to camera movement:

```cpp
if (physicsWorld != nullptr) {
    physicsWorld->step(deltaTime/4.0 * simulationSpeed);
}
```

`0.0` effectively pauses the whole `PhysicsWorld` (gravity, orbits, the Sun's pulse, the Petrova line) while the camera keeps moving normally. `updateSimulation()` itself is called roughly every 16 ms via `glutTimerFunc`, computing a clamped `deltaTime` and requesting a redraw with `glutPostRedisplay()`.

## How lighting fits into the scene (see `main.cpp`)

- `initOpenGL()` calls `glShadeModel(GL_SMOOTH)` (works thanks to `Mesh`'s per-vertex normals) and enables `GL_BLEND`/`glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` once, globally, for any translucent mesh (like the Petrova line's central tube) to render correctly.
- A single `LightSource` is created at the Sun's position, using `DataStructures`'s default `LightVector` presets.
- Earth is built with `Mesh::generateSphere(..., emissive = false)` (the default), so it responds normally to the light.
- The Sun, the astrophages, the Petrova line's central tube, and the sky's stars are all built `emissive = true`, so they glow with their own color regardless of the lighting math - important for the Sun in particular, since it sits at its own light's position.
- In `draw()`, `sunLight->apply(GL_LIGHT0)` is called every frame, right after `gluLookAt` and before `physicsWorld->renderAll()`.

## How the physics world fits into the simulation loop

`Simulation` holds an optional `PhysicsWorld*` member, passed in through the constructor, and steps it every frame scaled by `simulationSpeed` (see above). This keeps `Simulation` usable even without any physics bodies attached (e.g. while testing the camera alone), and `PhysicsWorld::step` in turn advances any attached `PetrovaLine` and the Sun's pulse automatically, so `main.cpp` never calls those directly.

## Suggested next steps

- Add simple collision detection between bodies using `radius` (useful for detecting "impacts" or merging bodies);
- Replace the O(n²) gravity computation with a Barnes-Hut tree if the number of bodies grows large;
- Add texture support to `Mesh` (UV coordinates) to map planet textures;
- Give `PhysicsWorld` (or `CelestialBody`) a way to snapshot and restore each body's initial position/velocity (the way `Camera` already does with `initialParameters`) to allow "resetting" the simulation;
- `SkySphere` regenerates its whole star field on every setter call - fine at typical star counts, but worth revisiting if `starCount` grows very large;
- Translucent meshes (the Petrova line's tube) are drawn in the same pass and order as opaque ones; for scenes with more overlapping translucent geometry, sorting translucent draws back-to-front (and/or disabling depth writes for them) would avoid the usual alpha-blending ordering artifacts.

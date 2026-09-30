# Astronomical Simulator (C++ / GLUT / OpenGL)

> *Placeholder for the final project description: goal, motivation, context (coursework, personal project, etc.), and build/run instructions.*

## Architecture overview

The project follows a separation of responsibilities very close to what's used in real game engines and physics engines (Unity, Godot, Box2D, Bullet): each class takes care of **one** thing, and "composite" objects (like a planet) are assembled by putting these pieces together through composition, not inheritance.

```
DataStructures   -> pure math (Vector, Matrix, HomogeneousMatrix, LightVector) + OpenGL wrappers + colors
Transform        -> stateless point/vector transformation utilities (translate, rotate, scale, matrix builders)
LightSource      -> a light's position + ambient/diffuse/specular LightVector, applied to an OpenGL light unit
RigidBody        -> mass, velocity, position, force accumulation and motion integration
Mesh             -> geometry, lighting material (ambient/diffuse/specular/emission), visual transforms and rendering
CelestialBody    -> combines RigidBody + Mesh into one astronomical body (Sun, Earth, Moon...)
PetrovaLine      -> a Bezier-curve stream of astrophages (+ a central tube) between a star and a CO2-rich planet
SkySphere        -> a field of tiny emissive "stars" scattered on a large, fixed-radius backdrop sphere
PhysicsWorld     -> manages every CelestialBody (+ optionally a PetrovaLine/SkySphere), steps and renders the scene
Camera           -> camera position/orientation and projection
Simulation       -> keyboard/mouse input and the time loop (deltaTime)
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

### `LightSource`
Stores a light's `position` and its `ambientLight`/`diffuseLight`/`specularLight` `LightVector`s (RGBA), and knows how to push itself into OpenGL's fixed-function lighting pipeline as one of the `GL_LIGHTx` units via `apply(lightUnit)` (which also enables `GL_LIGHTING` and that unit). `scaleIntensity(factor)` multiplies all three light components by the same factor, so a value below 1.0 dims the light and above 1.0 brightens it - covering both "multiply" and "divide" with a single method, as division is just multiplication by `1/factor`.

One OpenGL subtlety worth calling out: a light's position is transformed by whatever modelview matrix is active *at the moment `apply()` is called* - so it must be called every frame, after the camera transform (`gluLookAt`) and before drawing any lit geometry, or the light will appear to drift as the camera moves. See `draw()` in `main.cpp` for where that happens.

### `RigidBody`
The physical representation of a body: mass, velocity, accumulated force, and **position** - it lives here rather than in a separate transform component, since position is a physical quantity that the simulation (gravity, integration) is directly responsible for. `integrate(deltaTime)` advances velocity and position using **semi-implicit (symplectic) Euler integration** (more energy-stable than explicit Euler - important for orbits, which would otherwise show noticeable energy drift over time), applies the resulting displacement to its own position via `Transform::translate`, clears the accumulated force, and **returns the change in position** so the caller can move the visual mesh by the same amount.

### `Mesh`
Holds vertices, faces (lists of indices - supports triangles, quads or larger polygons), and keeps its vertices in **world space** directly rather than relying on a separate model matrix applied at render time.

It computes both **per-face** normals (`computeFaceNormals`) and **per-vertex** normals (`computeVertexNormals`, which averages the normals of every face touching a vertex). `render()` uses the per-vertex ones - that's what makes `glShadeModel(GL_SMOOTH)` (set once in `main.cpp`'s `initOpenGL()`) actually produce smooth-looking spheres instead of a faceted look: with one normal per face shared by all its vertices, every vertex would receive identical lighting and there'd be nothing for `GL_SMOOTH` to interpolate between neighboring faces.

It also owns the body's **lighting material**: `ambient`/`diffuse`/`specular` are `LightVector`s built with equal R/G/B components (homogeneous by construction - a single intensity scalar repeated three times, rather than carrying their own hue) via `setMaterial(ambientIntensity, diffuseIntensity, specularIntensity, shininess)`. The mesh's actual color always comes from `color`/`extendedColor`: `render()` combines them with `hadamardProduct(material, extendedColor)` to build the final `glMaterial` arrays, and also calls `gl::color(color)` directly (harmless while lit, but keeps the mesh looking right if lighting is ever disabled).

`emission` (a full `LightVector`, since a glow can have its own hue) is the light the mesh emits on its own, via `setEmission(...)`. Rather than juggling material and emission by hand, a mesh is simply **`emissive`** or not - passed as a `bool` to the constructor (and `generateSphere`/`generateOrbitRing`), or toggled later with `setEmissive(bool)`:
- `false` (default): normal material (ambient/diffuse/specular > 0, no emission) - the mesh **reacts to light**, never silently "transparent" to it;
- `true`: material intensities at 0 and `emission = color` - the mesh **glows with its own color**, regardless of any external light. This is exactly what a body sitting at its own light source's position needs (the Sun), or a small object that should read clearly regardless of where the light is (an astrophage, a star) - see `PetrovaLine` and `SkySphere` below.

`renderWireframe()` temporarily disables `GL_LIGHTING` so its `color` is drawn flat, as expected for orbit rings/debug lines.

`translate`/`rotate`/`scale` are thin wrappers that hand the vertex list to the corresponding `Transform` function - rotation and scale need a reference point (typically the body's current position, for spinning or resizing in place) and recompute normals afterwards, since those change face orientation; translation leaves normals unchanged. It also provides:
- `generateSphere(radius, stacks, slices, color, emissive)` - generates a UV sphere centered at the origin, ideal for planets/moons/the sun/astrophages/stars;
- `generateOrbitRing(radius, segments, color, emissive)` - generates a ring of points on the XZ plane, for drawing an orbital path.

### `CelestialBody`
A composition of `RigidBody` + `Mesh`, plus a name and a physical radius (used for gravity/collision calculations, independent of the render scale). Its constructor moves the (origin-centered) mesh to the body's initial position once; from then on, `advance(deltaTime)` integrates the `RigidBody` and translates the `Mesh` by the resulting delta, keeping the physical and visual positions in sync, and `spin(pitch, yaw, roll)` rotates the mesh around the body's current position for axial rotation. `render()` simply draws the mesh, since it already lives in world space.

Whether a given body reacts to a `LightSource` or emits its own light is purely a `Mesh` construction choice (`emissive`, see above) - `CelestialBody` doesn't need to know or care which kind it composes; `main.cpp` creates the Sun's mesh with `emissive = true` directly.

### `PetrovaLine`
A purely visual (no physics of its own) take on the Petrova line from *Project Hail Mary*: a stream of astrophages strung along a **cubic Bezier curve** between a star (`sun`) and a CO2-rich planet (`CO2planet`), built once in the constructor from `calculateBezierPoints()` - the curve's two control points are offset from the Sun in the XZ plane, giving it a gentle arc rather than a straight line.

`createAstrophages()` walks the curve in `resolution` steps, and at each step calls `calculatePoint(t, true)` to get a point on the curve with a small random offset added (scaled by `distribution`) - that's the "natural" scatter the astrophage cloud reads with, rather than a perfectly straight line of spheres. Each accepted point (only kept if it falls strictly between the Sun's surface and the planet) becomes a small emissive `Mesh::generateSphere(astrophageSize, 3, 3, RED, true)`, translated into place.

A new **`centralLine`** member - a single thick, low-poly tube `Mesh` - traces the curve's *exact* general direction (no random offset: `calculatePoint(t, false)`), so the astrophage cloud reads as following one coherent line rather than just a diffuse scatter. `createCentralLine()` samples `LINE_SEGMENTS` points along the curve, and at each one builds a `LINE_SIDES`-vertex ring (radius `astrophageSize * 1.5`) in the plane perpendicular to the curve's local tangent (approximated by a forward/backward difference between neighboring samples), with an arbitrary-but-consistent perpendicular basis (`right`/`trueUp`) built from that tangent via two cross products. Consecutive rings are connected into quad faces to form a continuous tube, built as one emissive orange `Mesh`.

Rather than recomputing the full Bezier curve every frame (the planet's actual position only defines the curve's *endpoint*, not a live parametrization), `advance()` takes a shortcut: it measures the angle the planet has swept around the Sun since the last call (`findAngle3Points`) and rotates every astrophage - and now `centralLine` too - around the Sun by that same angle, via `Mesh::rotate`. This keeps the whole cloud "glued" to the Sun-planet line as the planet orbits, without rebuilding any geometry.

Getters/setters were added for every constructor parameter and internal piece the class didn't previously expose: `getSun`/`getCO2Planet`, `getResolution`/`setResolution`, `getAstrophageSize`/`setAstrophageSize`, `getDistribution`/`setDistribution`, `getAstrophages`, `getCentralLine`, and `getBezierPoints`. Each setter regenerates whatever it affects (the astrophage cloud and, for `astrophageSize`, the central line too), so the visual always matches the current parameters.

### `SkySphere`
A purely visual backdrop, "in the same spirit as `PetrovaLine`" - just meshes and random scattering, no physics. Its constructor takes exactly three parameters: `center`, `starCount` and `starSize`. `createStars()` builds `starCount` tiny, low-poly, emissive `Mesh::generateSphere(starSize, 3, 3, WHITE, true)` spheres (emissive so they read as points of light regardless of where any `LightSource` is), each placed via `randomPointOnSphere()` - a uniform point on the surface of one large, fixed-radius sphere (`SKY_RADIUS = 5000.0`, chosen to sit far beyond any orbiting body) centered on `center`. Sampling picks `z` uniformly in `[-1, 1]` and an angle uniformly in `[0, 2π]` rather than sampling two angles uniformly, which would otherwise cluster stars near the poles.

`setCenter`/`setStarCount`/`setStarSize` (with matching getters) each regenerate the whole star field, since all three directly control how it's built.

### `PhysicsWorld`
Keeps a list of pointers to `CelestialBody` (it does not own the memory - whoever creates the bodies is responsible for them), plus an optional `PetrovaLine*` and `SkySphere*` (`setPetrovaLine`/`getPetrovaLine`, `setSkySphere`/`getSkySphere`), and an adjustable gravitational constant (`setGravitationalConstant`/`getGravitationalConstant`). On every `step(deltaTime)`:
1. Computes the gravitational force between **every pair** of bodies (O(n²), fine for a handful of bodies - Sun/Earth/Moon, etc.), using each body's `getPosition()`;
2. Applies the law of universal gravitation (`F = G·m₁·m₂/r²`);
3. Calls `advance(deltaTime)` on every body, which integrates its `RigidBody` and keeps its `Mesh` in sync;
4. If a `PetrovaLine` is attached, calls its `advance()` too, so it stays glued to whichever two bodies it connects.

`renderAll()` draws the `SkySphere` first (as a distant backdrop), then every `CelestialBody`, then the `PetrovaLine` - so nearer, physically-simulated objects are never hidden behind the background stars.

### `Camera`
Represents the viewer's position and orientation in the scene. Stores `position`, a `direction` vector derived from `pitch`/`yaw` (spherical-to-Cartesian conversion), an `up` vector, and the projection parameters (`fov`, `aspectRatio`, `nearPlane`, `farPlane`). Also snapshots its construction-time parameters into `initialParameters`, so the camera can be reset on demand.

`getCameraParameters()` packs eye/target/up into a `Matrix` ready to feed straight into `gluLookAt`. Movement is split into `move` (free translation), `moveForwardBackward`/`moveLeftRight` (movement relative to where the camera is facing, ignoring the Y axis - i.e. it walks rather than flies), and rotation via `updateRotation` (mouse-look style, accumulating pitch/yaw deltas) or `lookAt` (points the camera directly at a target).

### `Simulation`
Owns the GLUT-facing side of the program: keyboard/mouse input and the frame timer. Because GLUT callbacks are plain C function pointers (not member functions), `Simulation` keeps a single static `instance` pointer and a set of static wrapper functions (`glutKeyboardCallback`, `glutMouseCallback`, etc.) that forward each call to the real instance methods.

Input state is tracked continuously: `keyStates[256]` records which keys are currently held (so movement stays smooth instead of firing once per keypress), Shift toggles a speed boost, and pressing `r` resets the camera to its `initialParameters`. Dragging the mouse rotates the camera based on cursor movement since the last frame.

`updateSimulation()` is called roughly every 16 ms via `glutTimerFunc`. It computes `deltaTime` (clamped to avoid huge jumps if the window was dragged/paused), applies WASD-style camera movement scaled by `deltaTime` (using the `BASE_PLAYER_SPEED` constant), steps the `PhysicsWorld` forward by `deltaTime` if one is attached, and requests a redraw with `glutPostRedisplay()`.

## How lighting fits into the scene (see `main.cpp`)

- `initOpenGL()` calls `glShadeModel(GL_SMOOTH)`, which (thanks to `Mesh`'s per-vertex normals) actually produces smoothly-shaded spheres rather than a faceted look.
- A single `LightSource` is created at the Sun's position, using `DataStructures`'s default ambient/diffuse/specular `LightVector` presets - a fairly standard "sunlight" setup.
- Earth is built with `Mesh::generateSphere(..., emissive = false)` (the default), so it responds normally to the light and is shaded like any lit object should be.
- The Sun is built with `Mesh::generateSphere(..., emissive = true)`, so it glows with its own color regardless of the lighting math - no separate setup call needed in `initOpenGL()`. The astrophages, the `PetrovaLine`'s central tube, and the `SkySphere`'s stars are all emissive for the same reason: they should read clearly no matter where they sit relative to the Sun.
- OpenGL's fixed-function lighting has no shadows or occlusion built in, so the Sun's mesh existing in the scene never "blocks" light from reaching Earth - nothing extra was needed for that part.
- In `draw()`, `sunLight->apply(GL_LIGHT0)` is called every frame, right after `gluLookAt` and before `physicsWorld->renderAll()` - light position is modelview-relative at the moment it's set, so this ordering keeps the light fixed in world space as the camera moves.

## How the physics world fits into the simulation loop

`Simulation` holds an optional `PhysicsWorld*` member (analogous to its `Camera*`), passed in through the constructor. Inside `updateSimulation()`, right after computing `deltaTime`:

```cpp
if (physicsWorld != nullptr) {
    physicsWorld->step(deltaTime);
}
```

This keeps `Simulation` usable even without any physics bodies attached (e.g. while testing the camera alone). `PhysicsWorld::step` in turn advances any attached `PetrovaLine` automatically (see above), so `main.cpp` never needs to call `petrovaLine->advance()` itself.

## Suggested next steps

- Add simple collision detection between bodies using `radius` (useful for detecting "impacts" or merging bodies);
- Replace the O(n²) gravity computation with a Barnes-Hut tree if the number of bodies grows large;
- Add texture support to `Mesh` (UV coordinates) to map planet textures;
- Give `PhysicsWorld` (or `CelestialBody`) a way to snapshot and restore each body's initial position/velocity (the way `Camera` already does with `initialParameters`) to allow "resetting" the simulation;
- `SkySphere` regenerates its whole star field on every setter call - fine at typical star counts, but worth revisiting (e.g. only repositioning changed stars) if `starCount` grows very large.

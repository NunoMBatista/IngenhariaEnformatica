
---

# 📚 CGI – Class 3 Summary

## 1. Graphics Rendering Pipeline

Rendering follows a **pipeline of stages** that transform vertices into pixels on the screen:
```
Vertices →  [ Vertex Shader ]  →  ?  →  [ Fragment Shader ]  →  ?
```

### Vertex Shader

* **Input:** vertices (position, normal, texture coordinates, etc.)
* **What it does:** applies the full transform pipeline to each vertex — model matrix (places the object in the world), view matrix (positions the camera), and projection matrix (maps 3D to 2D). Every vertex is processed **independently and in parallel** on the GPU.
* **Output:** transformed vertex positions in clip space

### Fragment Shader

* **Input:** fragments — these are **potential pixels** generated after the GPU rasterizes the triangles (i.e. figures out which pixels each triangle covers)
* **What it does:** computes the final color of each fragment by evaluating lighting models, sampling textures, and applying material properties. Like the vertex shader, each fragment is processed **independently**.
* **Output:** a final RGBA color value for each pixel

---

### Putting it together
```
Vertices →  [ Vertex Shader ]  →  Clip space vertices  →  [ Fragment Shader ]  →  Final pixel colors
```

---

## 2. Recall two rendering approaches

Before diving into the pipeline details, it is worth recalling that there are two fundamentally different approaches to solving the core problem of rendering: **how do we calculate the color of each pixel on the screen?**
Each approach answers this question in a completely different way.


### Rasterization

* Convert geometry → pixels
* Fast and real-time
* Used in most games
* Think: “paint triangles onto the screen”

### Ray Tracing

* Simulates physical light rays
* More realistic lighting/reflections/shadows
* Slower, physically accurate
* Think: “follow light rays from camera”

---

## 3. Projections

When rendering a 3D scene, we need to decide **how** the 3D world is flattened onto the 2D screen. This choice is called the **projection type**, and it fundamentally changes how the scene is perceived by the viewer.


### Parallel (Orthographic)

* Lines remain parallel
* No depth perspective
* Used in CAD/technical drawings

### Perspective

* Lines converge to a point
* Objects farther away look smaller
* Mimics human vision
* Used in most 3D scenes

![Perspectives](./images/projections_parallel_perspective.png =600x)

*Figure: Parallel  (a) and perspective (b) projections.*

---
### Small comparison: Rendering VS Projection 

Projection is concerned with geometry — how do we transform and flatten 3D positions onto a 2D screen?

Rendering is concerned with color — how do we calculate the final color of each pixel on that screen?


---

## 4. Homogeneous Coordinates

Recall how we represent points - using **homogeneous coordinates**:

$$
\begin{bmatrix}
x & y & z & w 
\end{bmatrix}
$$

Typically: w = 1

### Why?

Because it allows **translation + rotation + scaling** using **one matrix multiplication**.

---

## 5. Transformation Matrices
Some transformation examples follow.

In these examples, we are considering a **2D space**!

### Translation

$$
\begin{bmatrix}
1 & 0 & 3 \\
0 & 1 & 5 \\
0 & 0 & 1
\end{bmatrix}
$$
Moves an object by 3 units along x and 5 units along y.

### Scaling

$$
\begin{bmatrix}
2 & 0 & 0 \\
0 & 2 & 0 \\
0 & 0 & 2
\end{bmatrix}
$$
Multiplies every coordinate by **2**, uniformly scaling the object 
to double its size in all directions.

### Rotation

$$
\begin{bmatrix}
\cos\theta & -\sin\theta & 0 \\
\sin\theta & \cos\theta & 0 \\
0 & 0 & 1
\end{bmatrix}
$$
Rotates an object by an angle **θ** around the origin. As θ increases,
the object rotates counterclockwise.

---

## 6. 3D Rotations

In 3D we rotate around an aditional axis, besides *x* and *y*:

* Roll → X axis
* Pitch → Y axis
* Yaw → Z axis

Each has its own rotation matrix.

---

## 7. Model, Projection and View Matrices | World, Camera and Clip Spaces  

In previous lessons we briefly mentioned the *model* matrix, the *view* matrix and the *projection* matrix. They transform points from one coordinate space to the next, following a strict order. Let's review what each one actually means.


### The full pipeline
```
Object → [Model] → World Space → [View] → Camera Space → [Projection] → Clip Space → Screen
```

---

### Model Matrix (M)

**Answers:** "Where is this object in the world?"

Takes the object's raw vertices and places them in the **world space** — applying position, rotation and scale. Every object has **its own** model matrix, since each one can move independently.

$$
v_{world} = M \cdot v_{object}
$$

---

### View Matrix (V)

**Answers:** "Where is the camera looking from?"

Transforms vertices from **world space → camera space**.



#### 💡 Why do we move the world instead of the camera?

Intuitively, when a camera moves to the right, you might expect to simply
move the camera. But in practice, we do the opposite — we move the
**entire world to the left**.

These two operations are **mathematically equivalent**:

$$
\text{move camera right} = \text{move world left}
$$

But computationally, moving the world is much cheaper. Here is why:

- Moving the camera would require **redefining the camera's position** and then
  recalculating how every single vertex relates to it — touching every single
  vertex in the scene.
- Instead, we apply the **opposite transformation to the world** — if the camera
  moves right, we move the world left. This opposite transformation is exactly
  what the **View matrix** encodes, and it slots naturally into our existing
  matrix multiplication pipeline without any extra cost.

In other words: instead of asking *"where is the camera relative to the world?"*,
we ask *"where is the world relative to the camera?"* — and the answer is just
one matrix multiplication away.

*Once more*: this is what the **View matrix** encodes: the inverse of the camera's position and rotation, applied to all vertices.

$$
v_{cam} = View \cdot v_{world}
$$

> 💡 **"It is easier to move the world than the camera."**

All objects share the **same** view matrix, since there is only one camera.

---

### Projection Matrix (P)

**Answers:** "How does the camera see the world?"

Maps vertices from **camera space → clip space**, encoding either perspective or orthographic projection.

$$
v_{clip} = P \cdot v_{cam}
$$

The **clip space** is an intermediate coordinate system produced by the projection matrix — its details will be covered in the next class.

This matrice/transformation determines how the scene is projected onto the screen: using perspective, orthographic, or other projection. With perspective projection, abjects farther away appear smaller. In orthographc projection all object keep the same size (no perspective).

After this, the GPU performs the **perspective divide** (dividing by w) to obtain **Normalized Device Coordinates (NDC)**, which are then mapped to pixels. (*This will subject of next class*)

All objects share the **same** projection matrix, since the lens and screen are the same for everyone.

---

### Putting it all together

Following the pipeline logic, for each vertex we need to apply three matrices in sequence:

$$
P \cdot View \cdot M \cdot v
$$

Where:
- **P** → projection matrix: transforms camera space into clip space (3D → 2D)
- **View** → view matrix: transforms world space into camera space
- **M** → model matrix: transforms the object's local space into world space
- **v** → the actual vertices of our object

The naive approach would be to apply each matrix to every vertex one by one:

$$
P \cdot (View \cdot (M \cdot v))
$$

This means feeding the GPU **hundreds and hundreds of vertex lines**, applying
each matrix separately to every single one. Very expensive.

---

But recall that matrix multiplication is **associative**:

$$
A \cdot (B \cdot C) = (A \cdot B) \cdot C
$$

Applying this to our case:

$$
P \cdot (View \cdot (M \cdot v)) = (P \cdot View \cdot M) \cdot v
$$

This small reordering changes everything. Notice that **P** and **View** are
**constant across the entire scene** — the camera and projection do not change
between vertices.

 **M** is constant **per object** (or per rigid sub-part) —
it does not change between vertices of the same object, but each independent
object or sub-part will have its own Model matrix. (More on this in next chapters)

So for each object (or sub-part), we can precompute their product **just once**:

$$
MVP = P \cdot View \cdot M
$$

And then apply this single combined matrix to all vertices:

$$
v_{clip} = MVP \cdot V
$$

Instead of feeding the GPU hundreds of vertex lines and doing three matrix
multiplications per vertex, we now:

1. Compute **MVP once** (three small 4×4 matrices multiplied together)
2. Apply it to each vertex — **one multiplication per vertex**

> *The insight*: it is always cheaper to collapse constant matrices into one
> combined matrix first, than to repeatedly apply them to hundreds of vertices.
> This is why the combined matrix is called **MVP** — Model, View, Projection.

---
### Revise: matrices

| Matrix | Controls | Shared across objects? |
|--------|----------|------------------------|
| **M** | object position, rotation, scale | ❌ No — each object has its own |
| **V** | camera position and orientation | ✅ Yes — one camera per scene |
| **P** | projection type, FOV, near/far | ✅ Yes — one screen per scene |

---

---

## 8. Applying Projection and View (Step-by-step Example)

Suppose a vertex in **world space**:

$$
v_{world} =
\begin{bmatrix} 2 \\ 3 \\ 5 \\ 1 \end{bmatrix}
$$

Camera at origin looking along -Z (no rotation), so view matrix is identity for simplicity:

$$
V = I =
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

Perspective projection matrix (simplified example):

$$
P =
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & -1 & -1 \\
0 & 0 & -1 & 0
\end{bmatrix}
$$

**Step 1: Apply View Matrix**

$$
v_{cam} = V \cdot v_{world} = I \cdot v_{world} =
\begin{bmatrix} 2 \\ 3 \\ 5 \\ 1 \end{bmatrix}
$$

**Step 2: Apply Projection Matrix**

$$
v_{clip} = P \cdot v_{cam} =
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & -1 & -1 \\
0 & 0 & -1 & 0
\end{bmatrix}
\cdot
\begin{bmatrix} 2 \\ 3 \\ 5 \\ 1 \end{bmatrix}
=
\begin{bmatrix} 2 \\ 3 \\ -6 \\ -5 \end{bmatrix}
$$

**Step 3: Perspective Division (to NDC)**

$$
v_{ndc} = \begin{bmatrix} x/w \\ y/w \\ z/w \end{bmatrix} = \begin{bmatrix} 2/-5 \\ 3/-5 \\ -6/-5 \end{bmatrix} = \begin{bmatrix} -0.4 \\ -0.6 \\ 1.2 \end{bmatrix}
$$

  
**Step 4: Map to Screen Coordinates**

The NDC coordinates (ranging from -1 to 1) are mapped to actual pixel 
coordinates depending on the screen resolution. 
*The details of this step will be covered in the next class.*


## **NOTE: bottom row in projection matrix**

In a perspective projection, the matrix looks like:

$$
\begin{bmatrix}
* & 0 & 0 & 0 \\
0 & * & 0 & 0 \\
0 & 0 & * & * \\
0 & 0 & -1 & 0
\end{bmatrix}
$$

Notice the last row is now:

$$
\begin{bmatrix}
0 & 0 & -1 & 0
\end{bmatrix}
$$

This means:
> w′ = −z

In *perspective projection*, objects farther away should look smaller.
To achieve that, the matrix is designed so that: w' = −z.

So, after transformation you get:
$$
\begin{bmatrix}
x' & y' & z' & w'
\end{bmatrix}
$$

And then the GPU performs the perspective divide:
$$
\begin{bmatrix}
x'/w' & y'/w' & z'/w'
\end{bmatrix}
$$

This division causes:
- Larger z (farther away) → larger w
- So (x/w, y/w) become smaller

✔️ That’s what creates perspective foreshortening

### Why this matters in perspective projection
 
This division:
- Makes far objects appear smaller
- Creates perspective depth
- Maps coordinates into Normalized Device Coordinates (NDC)

*Perspective projection* modifies the w component, requiring a division by w to create depth perception, while orthogonal projection keeps w = 1, so no division occurs.

### Why orthogonal projection does NOT apply this

In *orthogonal projection*, we don’t want distance to affect size.
So the matrix keeps:
> w′ = 1

That means the transformation is merely:
$$
\begin{bmatrix}
x' & y' & z' & w'
\end{bmatrix}
$$
And int GPU:
$$
\begin{bmatrix}
x' & y' & z'
\end{bmatrix}
$$

So, there is no division effect:
- Objects stay the same size regardless of distance
- Parallel lines remain parallel (no vanishing point)
---

## 9. Model, View, Projection Matrices (MVP) - summary


### Model Matrix (M)

* Moves object in world
* Position, rotation, scale
* Each *object* has its own

### View Matrix (V)

* Moves world relative to camera
* Represents camera position/orientation
* Usually inverse of camera transform
* *Only one per scene*

### Projection Matrix (P)

* Maps 3D → 2D clip space
* Perspective or orthographic
* Controls FOV, near/far planes

---

## 10. Multiple Objects

Each object has:

$$
P \cdot V \cdot M_i \cdot v_i
$$

* same **P**
* same **V**
* different **M**


When rendering a scene with **multiple objects**, each object does **not** use exactly the same transformations. Some transformations affect the **entire scene**, while others affect **only one object**.

For each object \( i \), we compute:

$$
P \cdot V \cdot M_i \cdot v_i
$$

Let’s revise **each matrix (shared or different) and focus on the model matrix**.

---

**Projection Matrix (P) — same for everyone**

The **projection matrix** defines: perspective vs orthographic; field of view (FOV); near and far planes; aspect ratio (screen shape).

These properties depend only on **the camera lens / screen**, not on the objects. This way every object is viewed with the same perspective, and is projected onto the same screen

If two objects used different projection matrices they would look like they were photographed with different lenses and the scene would look physically inconsistent.

Therefore: **same *P* for all objects**

---

**View Matrix (V) — same for everyone**

The **view matrix** represents the **camera position and orientation**. It answers: "From where are we looking at the world?". If the camera moves: everything in the scene appears to move together. 

For example: camera moves right → all objects appear left; camera moves forward → all objects appear closer. This movement affects the **entire world equally**, not individual objects.

So: 1) there is only one camera; 2) there is only one point of view

Therefore: **same *V* for all objects**

---

**Model Matrix $M_i$ — different for each object**

The **model matrix** describes object position, object rotation and object scale. This answers: "Where is THIS specific object in the world?".

Each object can move independently:

Examples:
- a car drives forward
- a tree stays still
- a bird flies
- a ball rotates

If all objects shared the same model matrix they would move together like they were glued. That would be wrong. 
So each object needs its **own transform**.

Therefore: **Different $M_i$ for each object**

---

**Vertex $v_i$ — specific to each object**

Each object also has different geometry and different vertices. 

Naturally: **Each object has its own vertices $v_i$**

---

## Final idea

You can think of it like taking a photo:

- Model → move the actors  
- View → move the camera  
- Projection → choose the lens  

Then take the picture.

---

## 11. Hierarchical Transformations (Parent–Child)

Objects can depend on others — when a parent object moves, all its children
move with it automatically. This is because each child **inherits** its parent's
transform, and adds its own on top.

Example:

* A **train** moves along a track
* A **character** is standing on the train
* The character's **arm** swings as it moves

Transforms accumulate down the hierarchy:
```
Train:      M(train)
Character:  M(train) · M(character)
Arm:        M(train) · M(character) · M(arm)
```

Each level adds its own local transformation **on top of** everything above it.
So if the train moves forward, the character moves with it — and so does the arm,
without needing to manually update each one.

This means that the full MVP equation for each object becomes:

$$
v_{clip} = P \cdot V \cdot M_{parent} \cdot M_{child} \cdot v
$$

And for a deeply nested object like the arm:

$$
v_{clip} = P \cdot V \cdot M_{train} \cdot M_{character} \cdot M_{arm} \cdot v
$$

Notice that **P** and **V** remain the same — only the model matrices accumulate.
Each added matrix represents one more level of the hierarchy.

This structure is called a **scene graph** — a tree of objects where each node
inherits the transforms of all its parents. It is the foundation of how
**hierarchical modeling** works in every modern 3D engine.

---

## 12. Rendering flow

1. Define object (vertices)
2. Apply Model transform → **world space**
3. Apply View transform → **camera space**
4. Apply Projection transform → **clip space**
5. Perspective divide + viewport transform → **screen space**
6. Rasterize
7. Shade fragments

---
---
# Key Takeaways

✅ Vertex → fragment pipeline

✅ Rasterization vs ray tracing

✅ Homogeneous coordinates enable translation

✅ Transformations use matrices

✅ MVP = Model · View · Projection

✅ Precompute matrices for performance

✅ Camera motion = inverse world motion

✅ Hierarchies use cumulative model matrices

---


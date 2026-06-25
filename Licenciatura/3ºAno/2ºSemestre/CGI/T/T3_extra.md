This extra paper focuses on understanding **camera space** and **clip space**. 

---
---

# Camera Space (View Space)

## Definition

**Camera space is the coordinate system where the camera is at the origin (0,0,0) and looking down a fixed direction (usually −Z).**

After applying the **View matrix**, every object is expressed **relative to the camera**, not the world.

$$
v_{cam} = V * v_{world}
$$

---

## Intuition

Think:

> "What does the world look like from the camera’s point of view?"

Instead of moving the camera, we:

- place the camera at the origin
- rotate the world opposite to camera rotation
- translate the world opposite to camera position

So:

| Before View               | After View                 |
| ------------------------ | -------------------------- |
| Camera somewhere in world | Camera at (0,0,0)          |
| Objects in world coords   | Objects relative to camera |

---

## Example

Camera is at:
$$
(0, 0, 10)
$$

Object is at:
$$
(0, 0, 15)
$$

From the camera’s perspective:
$$
15 - 10 = 5
$$

So in camera space:
$$
v_{cam} = (0,0,5)
$$

Meaning:

👉 the object is **5 units in front of the camera**

---

## Key properties

In most graphics APIs:

- camera at origin
- looking toward **−Z**
- +X → right
- +Y → up

So:

- negative Z → in front of camera (visible)
- positive Z → behind camera (not visible)

---
---

# Clip Space

## Definition

**Clip space is the coordinate system after applying the Projection matrix.**

$$
v_{clip} = P \cdot v_{cam}
$$

It is **not yet screen space**.

It is an **intermediate normalized space used for clipping and perspective**.

---

## Why do we need it?

Because we must:

### 1. Apply perspective

Make far objects appear smaller.

### 2. Clip geometry

Remove anything:

- too near
- too far
- outside left/right/top/bottom of the screen

---

## What projection does mathematically

Projection:

- compresses 3D into a cube
- encodes perspective into the **w** component

After multiplication you get:

$$
(x, y, z, w)
$$

Then GPU performs:

# Perspective division

$$
(x/w,\; y/w,\; z/w)
$$

This gives **Normalized Device Coordinates (NDC)**.

---

## Clip Space Range

Before division:

We keep points that satisfy:

$$
-w \le x,y,z \le w
$$

Everything else is clipped (discarded).

---

## After division (NDC)

Range becomes:

$$
-1 \le x,y,z \le 1
$$

This cube is then mapped to pixels.

---
---

# Full Pipeline Context

Here’s how they fit together:

## Step 1 — World → Camera

(View matrix)

$$
v_{cam} = V \cdot v_{world}
$$

👉 “Where is the object relative to the camera?”



## Step 2 — Camera → Clip

(Projection matrix)

$$
v_{clip} = P \cdot v_{cam}
$$

👉 “Apply perspective and prepare clipping”



## Step 3 — Clip → NDC

(Perspective divide)

$$
(x/w, y/w, z/w)
$$

👉 “Normalize into cube [-1,1]”



## Step 4 — NDC → Screen

(Viewport transform)

👉 convert to pixels

---

# Visual Intuition

Think of spaces like this:

```
World Space     →   Camera Space   →   Clip Space   →   Screen
(real world)        (camera POV)       (normalized)      (pixels)
```

... or ...

```
object positions
      ↓ View
relative to camera
      ↓ Projection
perspective applied
      ↓ divide by w
[-1,1] cube
      ↓ viewport
pixels
```

---

# One-sentence memory trick
- Camera space = "where things are relative to the camera"
- Clip space = "projection space used to apply perspective and clipping before screen mapping"
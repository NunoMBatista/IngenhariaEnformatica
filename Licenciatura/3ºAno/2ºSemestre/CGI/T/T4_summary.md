---

# CGI – Class 4 Summary

---

## 1. Orthographic Projection

### Definition

In **orthographic projection**, projection rays are **parallel** — they do not
converge to a point like in perspective projection. This means there is no
vanishing point and no sense of depth distortion. In other words, there is no infinity for this type of projeciton. We need to define "begin" and "end" planes.

### Near and Far planes

Because there is no infinity point, we must explicitly define the boundaries
of the visible region using two planes:

* **Near plane** → defines where rendering begins
* **Far plane** → defines where rendering ends

These two planes define the **depth clipping region** — anything outside them
is discarded.

---

### The Orthographic Projection Matrix

The goal of orthographic projection is to **transform the visible volume** (a rectangular prism defined by left, right, top, bottom, near, far) **into a unit cube** in Normalized Device Coordinates (NDC) space (ranging from -1 to 1 on all axes).

So originally, points are in 3D axis-aligned box (view volume):
$$
x \in [l, r] \\
y \in [b, t] \\
z \in [n, f] \\
$$

Orthographic projection maps these into Normalized Device Coordinates (NDC):
$$
x \in [-1, 1] \\
y \in [-1, 1]\\
z \in [-1, 1] \\
$$

This is done in two steps:
1. **Scale** — normalize the width, height and depth of the prism into $[-1, 1]$
2. **Translate** — shift the center of the prism to the origin

The resulting matrix is:

$$
M_{ortho} =
\begin{bmatrix}
\frac{2}{r-l} & 0 & 0 & -\frac{r+l}{r-l} \\
0 & \frac{2}{t-b} & 0 & -\frac{t+b}{t-b} \\
0 & 0 & -\frac{2}{f-n} & -\frac{f+n}{f-n} \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

Where:
* $r, l$ → right and left bounds (width of the prism)
* $t, b$ → top and bottom bounds (height of the prism)
* $f, n$ → far and near planes (depth of the prism)

#### Scaling terms

$$
\frac{2}{r-l}, \quad \frac{2}{t-b}, \quad -\frac{2}{f-n}
$$

These normalize width, height, and depth into $[-1, 1]$.

#### Translation terms

$$
-\frac{r+l}{r-l}, \quad -\frac{t+b}{t-b}, \quad -\frac{f+n}{f-n}
$$

These shift the center of the visible box to the origin.

---

## 2. Precision Loss and Homogeneous Normalization

### The problem

When applying a long chain of transformations, **floating point precision
can be lost**. The $w$ component of the homogeneous coordinate, which should
always remain $1$, can drift slightly:

$$
f \begin{bmatrix} x \\ y \\ z \\ 1 \end{bmatrix} = \begin{bmatrix} a \\ b \\ c \\ 0.998 \end{bmatrix}
$$

### The fix — divide by w

To recover a perfectly normalized coordinate, we divide the entire vector
by its $w$ component:

$$
\frac{1}{0.998}\begin{bmatrix} a \\ b \\ c \\ 0.998 \end{bmatrix} = \begin{bmatrix} a' \\ b' \\ c' \\ 1 \end{bmatrix}
$$

This restores $w = 1$ and corrects for any accumulated precision error.

Orthographic projection is purely linear and does not use depth-dependent division; any division by w in this case is only for *numerical normalization*, while perspective projection uses $w = −z$ to create depth-based scaling through the perspective divide.

---

## 3. Perspective Correction — Dividing by Z

A similar normalization trick is applied during *perspective projection*, but this time we **divide by $z$** instead of $w$.

### Why?

After applying the projection matrix $P$ to a vertex:

$$
P \cdot \begin{bmatrix} x \\ y \\ z \\ 1 \end{bmatrix} = \begin{bmatrix} a \\ b \\ c \\ * \end{bmatrix}
$$

We replace the $w$ component with $z$:

$$
\Rightarrow \begin{bmatrix} a \\ b \\ c \\ z \end{bmatrix}
$$

And then normalize by dividing by $z$:

$$
\frac{1}{z}\begin{bmatrix} a \\ b \\ c \\ z \end{bmatrix} = \begin{bmatrix} a/z \\ b/z \\ c/z \\ 1 \end{bmatrix}
$$

This correctly projects the vertex onto the plane we want.
We've described this also in the previous lesson, in the section "**NOTE: bottom row in projection matrix**".

---

### Updated Matrix (Orthographic-base mixed with perspective correction)

Incorporating this $z$-based normalization, the corrected matrix becomes:

$$
M_{ortho}^{(new)} =
\begin{bmatrix}
\frac{2}{r-l} & 0 & -\frac{r+l}{r-l} & 0 \\
0 & \frac{2}{t-b} & -\frac{t+b}{t-b} & 0 \\
0 & 0 & -\frac{f+n}{f-n} & -\frac{2}{f-n} \\
0 & 0 & -1 & 0
\end{bmatrix}
$$

After multiplication, this produces $w' = -z$, so the perspective divide
($\div w'$) automatically restores correct scaling.

---

### Why do we do all of this?

This is the foundation for understanding how **OpenGL applies its transformations**.
OpenGL uses a similar matrix, with one small difference — its reference axis is $-z$ (not $z$), so their matrix appears with a negative value in $z$.

---

## 4. Depth Perception and Near Plane Distance AKA Depth Buffer Precision problem

### Intuition

The closer an object is to the camera, the more sensitive the depth perception. Think of holding a pen close to your face and rotating it — from your point of view it moves a lot, but from far away it barely moves.

This means that **more precision should be reserved for closer distances** — depth values near the camera need finer resolution than those far away.

> 💡 This is why the placement of the **near plane** matters a lot — setting it too close wastes depth precision, while setting it correctly ensures smooth depth rendering.


### Core idea
After perspective projection and division, the depth stored in the depth buffer is not linear in z, but roughly proportional to *1/z*. So:
​
$$
v_{depth} ∝ 1/z
$$

This is why:
- Near objects → huge depth precision
- Far objects → very little precision

### What this means physically
If an object is close to the camera:
- Small change in z
  - large change in 1/z
  - big change in depth value

If an object is far away:
- Same change in z
  - tiny change in 1/z
  - almost no difference in depth

### Why it happens?
It comes directly from perspective projection. Recall:

$$
w' = -z
$$
$$
z_{ndc} = z' / w'
$$

This introduces the non-linearity.

### Shape of the curve
It’s not truly exponential, rather a hyperbolic curve (1/z).

So the graph looks like:
- Very steep near the camera
- Flattening out quickly with distance


---

## 5. Anti-Aliasing (Extra Topic)

**Anti-aliasing** is a technique to smooth out jagged edges that appear when
rendering geometry onto a pixel grid.

### Common techniques

**FXAA (Fast Approximate Anti-Aliasing)**
* Checks the **derivatives** of neighboring pixels
* Where the derivative is high (sharp edge), applies a **blur**
* Very fast, done entirely in screen space

**MSAA (Multisample Anti-Aliasing)**
* Calculates **multiple virtual sub-pixel samples** per pixel (e.g. 4x)
* Averages the samples to produce a smoother edge
* More accurate than FXAA but more expensive

**SMAA (Subpixel Morphological Anti-Aliasing)**
* A higher quality variant combining edge detection with morphological filters

> Some pipelines apply multiple techniques simultaneously for best results.

---

## Key Takeaways

✅ Orthographic projection uses **near/far planes** instead of a vanishing point  
✅ The orthographic matrix **scales + translates** the visible volume into $[-1, 1]$  
✅ Precision loss in $w$ is corrected by **dividing by $w$**  
✅ Perspective correction divides by **$z$** to project onto the correct plane  
✅ The corrected matrix is the basis for **OpenGL's projection matrix**  
✅ Near plane distance affects **depth precision** — closer = more sensitive  
✅ Anti-aliasing smooths jagged edges using sampling or blur techniques  

# Aula: Introdução à Geometria Procedimental
# Class: Introduction to Procedural Geometry

> **Trabalho 3 — 11/03**  
> Tópicos abordados / Topics covered: **Texturas, UV Mapping, Material Capture, Interpolação de Texturas, Filtros de Magnificação e Minificação**

---

## 1. Texturas / Textures

### PT
As **texturas** são imagens aplicadas à superfície de objetos 3D para lhes conferir cor, padrão ou detalhe visual sem aumentar a complexidade geométrica do modelo. Em vez de modelar cada detalhe de uma superfície (e.g., as veias de uma pedra), usamos uma imagem 2D que é "colada" sobre a malha 3D.

Nas notas, é ilustrado que um cilindro 3D com múltiplos vértices pode ser simplificado numa representação mais abstrata composta por:
- **Vértices da face superior (base)**
- **Dois triângulos** (que formam o corpo lateral)
- **Vértices da face inferior (base)**

Esta decomposição em triângulos é fundamental, pois as GPUs processam geometria essencialmente como listas de triângulos.

### EN
**Textures** are images applied to the surface of 3D objects to give them colour, pattern, or visual detail without increasing the geometric complexity of the model. Instead of modelling every surface detail (e.g., veins in stone), we use a 2D image that is "wrapped" over the 3D mesh.

The notes illustrate that a 3D cylinder with many vertices can be simplified into a more abstract representation made up of:
- **Vertices of the top face (base)**
- **Two triangles** (forming the lateral body)
- **Vertices of the bottom face (base)**

This triangle decomposition is fundamental, as GPUs process geometry essentially as lists of triangles.

---

## 2. UV Mapping

### PT
O **UV Mapping** é o processo de projetar uma textura 2D sobre a superfície de um objeto 3D. As letras **U** e **V** representam os eixos da textura 2D (análogos aos eixos X e Y no espaço 3D, evitando confusão com as coordenadas do espaço tridimensional).

O espaço UV é normalizado: as coordenadas vão de **(0,0)** (canto inferior esquerdo) a **(1,1)** (canto superior direito). Isto significa que o tamanho real da textura em píxeis **não importa** para as coordenadas — o mapeamento é independente da resolução. Nas notas, é referido: *"porque o tamanho da textura é variável"*.

Cada vértice da malha 3D recebe um par de coordenadas UV que indica qual parte da textura lhe corresponde. O motor gráfico faz depois a interpolação para os fragmentos (píxeis) entre vértices.

### EN
**UV Mapping** is the process of projecting a 2D texture onto the surface of a 3D object. The letters **U** and **V** represent the axes of the 2D texture space (analogous to X and Y in 3D space, avoiding confusion with the three-dimensional coordinates).

The UV space is normalised: coordinates range from **(0,0)** (bottom-left corner) to **(1,1)** (top-right corner). This means the actual texture size in pixels **does not matter** for the coordinates — the mapping is resolution-independent. The notes state: *"because the texture size is variable"*.

Each vertex of the 3D mesh is assigned a UV coordinate pair indicating which part of the texture it corresponds to. The graphics engine then interpolates for the fragments (pixels) between vertices.

---

## 3. Material Capture (MatCap)

### PT
O **Material Capture**, ou **MatCap**, é uma técnica de renderização que codifica toda a informação de iluminação e reflexo de um material numa única textura esférica. Em vez de calcular a iluminação em tempo real, o motor usa a normal do vértice para fazer lookup diretamente na textura MatCap, obtendo assim a cor final do fragmento de forma muito eficiente.

Nas notas são mencionados recursos e ferramentas úteis:

- **[Poly Haven](https://polyhaven.com)** — site com texturas e HDRIs de alta qualidade gratuitos (*"site com texturas"*).
- **Encoding → dds** — formato de textura otimizado para GPU (*"imagens para estarem na GPU"*). O formato DDS (*DirectDraw Surface*) suporta compressão direta por hardware, reduzindo a largura de banda de memória durante a renderização.
- **GIMP** — editor de imagem gratuito e open-source utilizado para criar, editar ou converter texturas.
- **Texel** — a unidade mínima de uma textura (equivalente ao píxel numa imagem), podendo conter vários valores por posição. Nas notas: *"vários taxels por pixel — está longe"* — referindo-se à situação em que um pixel no ecrã cobre vários texels da textura, o que se relaciona com os filtros (ver secção 5).

### EN
**Material Capture**, or **MatCap**, is a rendering technique that encodes all lighting and reflection information of a material into a single spherical texture. Instead of computing lighting in real time, the engine uses the vertex normal to look up directly into the MatCap texture, obtaining the final fragment colour very efficiently.

The notes mention useful resources and tools:

- **[Poly Haven](https://polyhaven.com)** — website with free, high-quality textures and HDRIs (*"site with textures"*).
- **Encoding → dds** — GPU-optimised texture format (*"images to be on the GPU"*). The DDS (*DirectDraw Surface*) format supports direct hardware compression, reducing memory bandwidth during rendering.
- **GIMP** — free and open-source image editor used to create, edit, or convert textures.
- **Texel** — the minimum unit of a texture (equivalent to a pixel in a plain image), which may contain multiple values per position. The notes read: *"several texels per pixel — it's far away"* — referring to the situation where one screen pixel covers multiple texels of the texture, which relates to filters (see section 5).

---

## 4. Interpolação de Texturas / Texture Interpolation

### PT
Quando uma textura é mapeada sobre uma superfície, raramente existe uma correspondência perfeita 1-para-1 entre os texels e os píxeis no ecrã. Por isso, é necessário **interpolar** o valor da textura. Existem três métodos principais, mencionados nas notas:

| Método | Descrição | Custo |
|---|---|---|
| **Nearest (Neighbor)** | Usa o texel mais próximo. Rápido mas pode produzir uma aparência "pixelizada" (*blocky*). | Baixo |
| **Bilinear** | Faz uma média ponderada dos 4 texels mais próximos. Resultado mais suave. | Médio |
| **Bicubic** | Usa os 16 texels vizinhos para uma interpolação cúbica. Resultado mais suave mas *"menor mas custa"* (menor qualidade de artefactos, mas mais computacionalmente caro). | Alto |

A fórmula geral de interpolação linear apresentada nas notas é:

```
f(a, b, t) = a · (1 - t) + b · t
```

Onde `t` é o parâmetro de interpolação entre 0 e 1. Casos especiais:
- `f(a, b, 1) = b` — quando `t = 1`, obtemos o valor de `b`.
- `f(a, b, 0.5) = (a + b) / 2` — quando `t = 0.5`, obtemos a média dos dois valores.

### EN
When a texture is mapped onto a surface, there is rarely a perfect 1-to-1 correspondence between texels and screen pixels. Therefore, it is necessary to **interpolate** the texture value. Three main methods are mentioned in the notes:

| Method | Description | Cost |
|---|---|---|
| **Nearest (Neighbor)** | Uses the closest texel. Fast but can produce a "pixelated" (*blocky*) look. | Low |
| **Bilinear** | Weighted average of the 4 nearest texels. Smoother result. | Medium |
| **Bicubic** | Uses the 16 neighbouring texels for cubic interpolation. Even smoother but *"menor mas custa"* (fewer artefacts, but more computationally expensive). | High |

The general linear interpolation formula presented in the notes is:

```
f(a, b, t) = a · (1 - t) + b · t
```

Where `t` is the interpolation parameter between 0 and 1. Special cases:
- `f(a, b, 1) = b` — when `t = 1`, we get the value of `b`.
- `f(a, b, 0.5) = (a + b) / 2` — when `t = 0.5`, we get the average of both values.

---

## 5. Filtros Min e Mag / Min and Mag Filters

### PT
Os **filtros de minificação e magnificação** controlam como a textura é amostrada consoante a distância do objeto à câmara:

- **Mag Filter (Magnificação)**: Usado quando o objeto está **perto** da câmara e um único texel é expandido para cobrir múltiplos píxeis. Os filtros bilinear ou nearest são os mais comuns.
- **Min Filter (Minificação)**: Usado quando o objeto está **longe** da câmara e múltiplos texels contribuem para um único píxel. Aqui entram técnicas como **Mipmapping**.

Nas notas é referida uma referência para aprender mais: **git.books.io** (*"min and mag filters → git.books.io"*).

Também é mencionado o conceito de **vee (view) texture mapping** (possivelmente *view-dependent texture mapping*), uma técnica onde a textura aplicada varia conforme o ângulo de visão do observador.

### EN
**Minification and magnification filters** control how the texture is sampled depending on the object's distance from the camera:

- **Mag Filter (Magnification)**: Used when the object is **close** to the camera and a single texel is expanded to cover multiple pixels. Bilinear or nearest filters are most common.
- **Min Filter (Minification)**: Used when the object is **far** from the camera and multiple texels contribute to a single pixel. This is where techniques like **Mipmapping** come in.

The notes reference **git.books.io** as a resource to learn more (*"min and mag filters → git.books.io"*).

Also mentioned is the concept of **vee (view) texture mapping** (possibly *view-dependent texture mapping*), a technique where the applied texture varies according to the observer's viewing angle.

---

## Resumo / Summary

| Conceito / Concept | PT | EN |
|---|---|---|
| **Textura** | Imagem 2D aplicada a geometria 3D | 2D image applied to 3D geometry |
| **UV Mapping** | Projeção de textura no espaço (0,0)–(1,1) | Texture projection in (0,0)–(1,1) space |
| **MatCap** | Textura esférica com iluminação pré-calculada | Spherical texture with pre-baked lighting |
| **Texel** | Unidade mínima da textura | Minimum unit of a texture |
| **Interpolação Bilinear** | Média dos 4 texels mais próximos | Average of the 4 nearest texels |
| **Mag Filter** | Filtro quando objeto está perto | Filter when object is close |
| **Min Filter** | Filtro quando objeto está longe | Filter when object is far |
| **DDS** | Formato de textura comprimido para GPU | GPU-compressed texture format |

---

*Notas retiradas da aula de Introdução à Geometria Procedimental — 11/03.*  
*Notes taken from the Introduction to Procedural Geometry class — 11/03.*

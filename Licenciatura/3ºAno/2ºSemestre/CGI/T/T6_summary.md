
# 📚 CGI – Class 6 Summary
**Data:** 18-03-2026

## ILUMINAÇÃO

## 1. Introdução à Iluminação Difusa — *Diffuse Reflection*

A iluminação de uma superfície depende da **orientação relativa entre a fonte de luz e a normal da superfície**. Considera os seguintes casos ilustrativos:

![Direction of light](./images/T6/light-position.png =700x)


- Superfície A: 
  - orientado de forma perpendicular à direção da luz
  - objeto A acumula maior intensidade por área!

- Superfície B: 
    - orientado de forma oblíqua à luz
    - recebe menos por unidade de área (comparando com A)

- Superfície C:
    - tem alguma luz (orientada num ângulo intermédio)

- Superfície D:
    - não recebe qualquer luz — a sua orientação é igual à da fonte de luz!
    - normal paralela ao vetor L → dot product = 0




> **A** recebe mais luz do que **B**, pois acumula maior intensidade por área.
> **D** não recebe qualquer luz — a sua orientação é igual à da fonte de luz.


![Direction of light](./images/T6/light-position-article.png =400x)


---
## 2. Componentes de iluminação e modelo Phong

Vamos abordar três componentes universais de iluminação presentes em praticamente todos os modelos de rendering.
Estes são: *Diffuse*, *Specular* e *Ambient*; e não são exclusivos do modelo de Phong. 
O que distingue os modelos (Phong, Blinn-Phong, GGX, etc.) não é quais componentes usam, mas como calculam matematicamente cada um deles. O Phong é apenas uma das formas possíveis de modelar estas três contribuições de luz.

![Components of light](./images/T6/light_components_amb_diff_spec.png =600x)

---

### 2.1 Componente _Diffuse_ — Vetores L e N

A componente *Diffuse* descreve a luz que atinge uma superfície e é dispersa igualmente em todas as direções, independentemente de onde o observador está. É o que dá a um objeto a sua "cor base" — a forma como a sua superfície absorve e reflete luz de forma uniforme. Superfícies mate (papel, argila, betão) são predominantemente difusas.

![Components of light](./images/T6/diffuse_light.png =700x)

Para calcular a iluminação difusa, usam-se dois vetores:

| Vetor | | Significado |
|---|---|---|
| **L** | (Light) | Vetor que aponta da superfície para a fonte de luz / iluminação |
| **N** | (Normal) | Vetor normal à superfície do objeto |

A intensidade difusa é dada pelo **produto escalar** entre L e N:
$$
Diffuse = dot(L, N) = Lx·Nx + Ly·Ny + Lz·Nz
$$

> - **Nota:** tanto **L** como **N** são vetores **unitários** (normalizados,
> ou seja, ‖L‖ = ‖N‖ = 1). Consideramos 3 parcelas — uma por cada eixo (x, y, z).
> - Isto garante que o produto escalar resulta
> diretamente no cosseno do ângulo θ entre eles — eliminando a influência
> da magnitude e medindo apenas a relação de **direção** entre a luz e a
> normal da superfície.

> - **Lógica:** quando cada componente tem a mesma direção, contribui para a luz - o produto escalar dos vetores não é nulo.


### 2.1.1 Problema: intensidade negativa

O modelo simples permite intensidades de luz **negativas** (quando o ângulo entre L e N é obtuso) — o que não faz sentido fisicamente!

**Solução — clamp para zero:**

```
max(dot(L, N), 0)
```

Quando a contribuição é negativa é como se a luz e o objeto estivessem voltados para sentidos opostos (*facing away from each other*). Do nosso ponto de vista, isso já não interessa considerar — é simplesmente sombra.

---

### 2.2 Reflexão Especular — *Specular Reflection*


![Components of light](./images/T6/specular_Reflection.png =450x)


| Vetor | Significado |
|---|---|
| **N** | Normal da superfície |
| **L** | Vetor de luz (*light*) |
| **R** | Vetor de reflexão — a direção que a luz segue depois de ser refletida pela superfície atingida |
| **V** | Vetor de visão (*view*) — direção do observador |

A lógica para o cálculo é semelhante à difusa. 
Contudo, queremos focar-nos na **reflexão especular**, que vem da comparação entre V e R:

```
dot(V, R)  →  max(dot(V, R), 0)
```

Novamente, se o ponto de vista for tanto mais semelhante ao vetor de reflexão da luz, mais componente _specular_ teremos.

Mesmo assim, ainda se usa uma variável adicional para caracterizar o brilho concentrado (*highlight*) que vemos em objetos especulares.


### 2.2.1 Shininess (Brilho)

Introduzimos o parâmetro **shininess** que controla o quão concentrado e brilhante é o destaque especular:

```
max(dot(V, R), 0)^shininess
```

| Shininess = 0 | Shininess = 35 |
|---|---|
| Brilho difuso, espalhado | Brilho muito concentrado e vívido |

> Quanto maior o valor, mais brilhante e restrito será o destaque especular.

--- 
## Comparação entre componentes *diffuse* e *specular*

![Components of light](./images/T6/types-of-reflection-specular-diffuse.jpg =450x)

---

### 2.3 Componente Ambiente — *Ambient*

Falta ainda o **ambient** — consideramos que é uma **constante**. Representa a luz indireta que ilumina mesmo zonas sem luz direta (reflexões do ambiente, luz difusa global).

---

### 2.3 Modelo de Phong Completo

```
Phong = Diffuse + Specular + Ambient
```

Ou, com múltiplas luzes e cores:

```
Phong = ΣDiff + ΣSpec + Amb
               (diferentes cores)
```

> ❓ **Porquê o Ambient é apenas uma soma (constante)?**
> Porque é uma aproximação — o modelo de Phong não simula luz indireta real. O Ambient é um "cheat" que evita que zonas de sombra fiquem completamente pretas.

Podemos ainda convencionar:
- **Diff** pode ser de qualquer cor (a cor do objeto)
- **Spec** será branco (reflexo de espelho)

**Resumo das fórmulas:**

```
Diff:  max(dot(N, L), 0)
Spec:  max(dot(R, V), 0)^shininess
Amb:   constante
```

---

## 3. Blinn-Phong

O **Blinn-Phong** é uma variação do Phong que introduz uma pequena mudança na forma como se calcula o specular.

Em vez de usar o vetor de reflexão **R**, usa-se o **half vector H** — o vetor a meio caminho entre **L** e **V**:

```
        💡 (luz)
        /
       / H (half vector)
      /↗
  ─────────────  superfície
  ↖
   V (view)
```

Com o mesmo shininess, a luz é um pouco "mais" refletida com Blinn-Phong — vemos imagens diferentes dependendo do modelo usado.

Para ver exemplos, aceder a:
https://learnopengl.com/Advanced-Lighting/Advanced-Lighting:

> "Below is a comparison between both specular reflection models with the Phong exponent set to 8.0 and the Blinn-Phong component set to 32.0:"

![Specular: Phong VS Blinn-Phong of light](./images/T6/specular_phong_VS_blingPhong.png =650x)

### Theory

> "The Blinn-Phong model is largely similar, but approaches the specular model slightly different which as a result overcomes our problem. Instead of relying on a reflection vector we're using a so called halfway vector that is a unit vector exactly halfway between the view direction and the light direction. 
> 
> *The closer this **halfway vector** aligns with the surface's **normal vector**, the **higher the specular** contribution.*"

*From: https://learnopengl.com/Advanced-Lighting/Advanced-Lighting*

> "You can see that the Blinn-Phong specular exponent is bit sharper compared to Phong. It usually requires a bit of tweaking to get similar results as to what you previously had with Phong shading. It's worth it though as Blinn-Phong shading is generally more realistic compared to default Phong shading.
"

---

## 4. BRDF — *Bidirectional Reflectance Distribution Function*

A **BRDF** é a função matemática geral que descreve como a luz é refletida numa superfície, dependendo de:
- **Direção da luz** (vetor L / ângulos θ, φ)
- **Direçao do observador**
- **Normal da superfície** (N)

A aparência de brilho, *shininess*, corresponde a um pico na função BRDF na direção da reflexão. Noutras palavras, existe reflexão forte quando a direção *view* é tanto mais semelhante à direção de reflexão. É isto que cria a aparÊncia de *highligths* e *glossy appearance*.

> Para a maioria dos objetos, uma superfície perfeitamente lisa não é suficiente para traduzir o aspeto do mundo real. Os objetos são **rugosos** — há uma perturbação da superfície para a qual este modelo é demasiado simplista.
>
> "A BRDF defines how light is reflected at a surface by mapping incoming light directions and outgoing view directions to reflected intensity. It does not directly define properties like shininess, but different BRDF models can produce shiny or matte appearances depending on how reflection is distributed."

### Extensões da BRDF (para aulas futuras):

| Modelo | Descrição |
|---|---|
| **Blinn-Phong** | O primeiro caso — o artista define o shininess manualmente. Conveniente, mas exige tweaking. |
| **GGX** | Modelo mais recente — inclui a interação entre a luz e os microfacetes. Já tem a componente do material com luz em interação. Mais realista para superfícies rugosas. |

---

## 5. Fontes de Luz — *Light Types*

> A realidade vai depender também de quão realisticamente conseguimos representar a própria luz!

### Comparação Visual

```
  Directional            Point                 Spotlight
  ═══════════         ═══════════             ═══════════
  L = constante       L = (P - source)      L = (P - source)
                      A = 1/‖P-source‖²     A = 1/‖P-source‖² · (L·Louter)
```
![Types of light: Directional, point, spotlight](./images/T6/types_light_dir_point_spot.jpg =550x)



---

### 5.1 Directional Light

Assume-se que a fonte de luz produz luz como **um único vetor constante** — os raios são todos paralelos.

$$
\vec{L} = const.
$$

> Modelo simples do sol: infinitamente distante, logo todos os raios chegam com a mesma direção.

---

### 5.2 Point Light

Um emissor emite raios de luz em **todas as direções**.

Calculado por ponto:
$$
\vec{L} = (P - source)
$$

Para obter a direção, calculamos a diferença entre dois pontos e obtemos a direção. O vetor é **normalizado** (módulo = 1).

**Atenuação com a distância:**

A ideia base é:
```
A = |P - source|        ← distância
```

Mas quanto mais perto, mais ela aumenta! E quanto mais longe, maior a atenuação:
A fórmula correta usa o **quadrado da distância**:

```
A = 1 / ‖P - source‖²
```

> ⚠️ Este é um parâmetro provisório — a distância real é usada para atenuar (*"attenuation"*) a luz. Temos que adicionar esta "atenuação" de luz à nossa equação de Phong.

---

### 5.3 Spotlight

Semelhante ao Point Light, mas a luz só ilumina dentro de um **cone**.


$$
\vec{L} = (P - source)
A = 1/‖P-source‖² · (\vec{L} · \vec{L}_{outer})
$$

Atenção:
$$
(\vec{L} · \vec{L}_{outer})
$$
Corresponde ao dot product da luz com a direção do cone.

O **light vector L** não muda no spotlight — o que muda é a **atenuação**.

Temos **diferentes zonas**: (recordar na imagem acima)
- **Zona A** (interior, perto do centro): atenuação menor → mais luz
- **Zona B** (exterior, borda do cone): atenuação maior → menos luz, até desaparecer

A equação muda consoante saímos da zona A: a partir do momento em que saímos da zona A, passa a considerar-se que a luz é "semelhante" à que vem de outro lado. Ou seja, temos a linha divisória — um degradê mais suave e credível até a luz desaparecer.

> ⚠️ Atenção: esta não é a fórmula especial — mas a ideia geral traduz-se para isto.

---

## 6. BRDF e Ray Tracing

```
θR, φe  ←──  Li(θR, Δe)   } Formulação da BRDF
Ray i   ←──  Li(Rw, N)    }
A       ←──  SampleLi(θR, ΔR)   } Isto é para aplicar
A       =    SampleLi(Ray)       } Ray Tracing
```

> Ou seja, só depois de disparar estes raios é que se sabe onde vai parar cada um — e aí conseguimos gerar a BRDF, ou seja, gerar a função que depois nos dá a intensidade de luz em determinado ponto de distância do objeto.

O prof tinha um quadro com **3 colunas** para ilustrar isto:

```
  Amb  +  Diff  +  Spec  =  BRDF
   ○       ∩        𝓁        ⊗
(ponto)  (meia   (reflexo  (resultado
         esfera)  estreito)  final)
```

> A BRDF é a unificação das três componentes que estudámos — Ambient, Diffuse e Specular — numa única função de distribuição de reflectância bidirecional.

Em aula o professor mostrou esquemas disto em 3D. 

Tentei encontrar imagens semelhantes:

Em gráfico que mostra três componentes individualmente:

![BDRF](./images/T6/brdf_examples_chin.jpg =450x)

Em gráfico que mostra intensidade de luz com código de cores:

![BDRF](./images/T6/measurement-of-hemispherical-BDR-for-a-sample-of-diffuse-gold.webp =450x)

Em gráficos que mostra intensidade de luz com cores e com somas de intensidades de vetores de cada componente.

![BDRF](./images/T6/brd_function_example.jpg =350x)


---

## Resumo Geral

| Componente | Fórmula | Descrição |
|---|---|---|
| **Diffuse** | `max(dot(N, L), 0)` | Luz espalhada uniformemente |
| **Specular (Phong)** | `max(dot(R, V), 0)^s` | Brilho especular com vetor R |
| **Specular (Blinn-Phong)** | `max(dot(N, H), 0)^s` | Brilho com half-vector H |
| **Ambient** | `constante` | Luz indireta global |
| **Phong Total** | `Diff + Spec + Amb` | Modelo completo |
| **Atenuação (Point/Spot)** | `1 / ‖P - source‖²` | Queda de intensidade com distância |

---

*Notas da aula CGI — T6 — 18/03/2026*
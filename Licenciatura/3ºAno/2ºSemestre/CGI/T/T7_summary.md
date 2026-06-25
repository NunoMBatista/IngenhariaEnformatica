# 📚 CGI – Class (Lighting & Texturing) Summary  
**Tema:** Iluminação, Normais e Texturas

---

# 💡 1. Tipos de Luz (*Light Types*)

A forma como calculamos a iluminação depende do tipo de fonte de luz.

---

## 1.1 Directional Light

- A luz vem de uma direção constante (como o Sol)
- Não depende da posição

$$
\vec{L} = const.
$$

- Atenuação:
$$
a = 1
$$

👉 Não há perda de intensidade com a distância.

---

## 1.2 Point Light

- A luz emana de um ponto no espaço

$$
dist = \|P_{world} - S_{world}\|
$$

$$
\vec{L} = \frac{P_{world} - S_{world}}{dist}
$$

Com P = point of material, S = Source.

- Atenuação:

$$
a = \frac{1}{dist^2}
$$

### ❓ Porquê \(1 / dist^2\) ?

Porque a luz se distribui numa área crescente (esfera):

- quanto mais longe → maior área
- mesma energia → menor intensidade

👉 Resultado: comportamento fisicamente mais realista

---

### ⚠️ Nota (do professor)

$$
a = \frac{P}{dist^2}
$$

- P = potência da luz -> multiplicação de atenuação por potência da luz
- usado apenas para **visualização**, não físico

---

## 1.3 Spotlight

- Luz dentro de um **cone**

$$
\vec{L} = \frac{P_{world} - S_{world}}{dist}
$$

$$
a = \frac{1}{dist^2} \cdot (\text{fator angular})
$$

### Fator angular (ideia):

$$
(\cos(\theta) - cutoff) / range
$$

👉 Não é necessário decorar — apenas compreender:

- centro do cone → intensidade máxima  
- bordas → fade suave  
- fora → zero  


### 💡 Intuição (Spotlight)

A utilização do **cosseno do ângulo** permite medir o quão alinhado está o fragmento com a direção central do cone de luz.  
Esse valor serve como base para a **atenuação angular**, que define a intensidade da luz não de forma abrupta, mas progressiva.

Na prática:
- fragmentos próximos do eixo central → recebem **intensidade máxima**
- à medida que o ângulo aumenta → a intensidade vai **diminuindo gradualmente**
- ao atingir o ângulo de cutoff → a luz torna-se **zero**

👉 Isto permite criar uma **transição suave (fade)** entre a zona iluminada e a zona sem luz, evitando fronteiras artificiais e garantindo um resultado mais **realista e visualmente agradável**.

---

# ⚠️ 2. Problema: Transformação de Normais

Podemos transformar pontos e vetores:

$$
p' = M \cdot p
$$
$$
\vec{n'} = M \cdot \vec{n}
$$
, com p representando pontos do objeto e n as respetivas normais.

Mas isto pode ser **incorreto para normais**.

❗ Problema: A normal deve ser **perpendicular à superfície**:

$$
\vec{n} \cdot \vec{T} = 0
$$

, com T = vetor tangente.

Se aplicarmos \(M\) diretamente esta propriedade pode ser perdida!
Quando aplicamos uma transformação \(M\) diretamente a uma normal, corremos o risco de **quebrar a sua propriedade fundamental**: ela deve permanecer **perpendicular à superfície** (ou seja, ortogonal à tangente).




### ✅ Solução correta

Transformar normais com:

$$
(M^{-1})^T
$$
ou seja
$$
\vec{n'} = (M^{-1})^T \cdot \vec{n}
$$

Porque: 
- o **inverso** “anula” a deformação introduzida pela matriz original  
- a **transposta** ajusta corretamente a orientação da normal  
(existe prova matemática, mas não nos importa focar nisso)

👉 Resultado:

- a normal transformada continua **perpendicular à superfície**
- a condição mantém-se válida:


### 💡 Regra importante

> Sempre que transformamos **direções (normais)**, usar:
>
> $$
> (M^{-1})^T
> $$
>
> Não estamos a transformar a normal da mesma forma que transformamos posições —  
> estamos a garantir que ela continua a representar corretamente a **orientação da superfície** após a transformação.

---

# 🧱 3. Texture Atlas

## Definição

Um **texture atlas** é uma única imagem grande que contém várias texturas.


## 💡 Motivação

- Upload de texturas para GPU é **caro**
- Muitas imagens pequenas → overhead elevado
- Uma imagem grande → mais eficiente

---

## ⚖️ Comparação

| Abordagem | Custo |
|----------|------|
| Muitas imagens pequenas | ❌ Alto |
| Um atlas grande | ✅ Mais eficiente |

---

## 🧠 Intuição

- carregamos **uma vez**
- reutilizamos partes via **UV coordinates**

---

## 🎮 Exemplo: *Minecraft*

- usa atlas para vários blocos
- evita múltiplos uploads
- melhora performance

---

## 🔧 Nine-Slicing

- divide textura em 9 partes:
  - cantos (fixos)
  - arestas (escaláveis)
  - centro (flexível)

👉 útil para UI (botões, caixas, etc.)

---

# 🎨 4. Material Maps

Materiais podem conter múltiplos mapas:

- **Diffuse** → cor base  
- **Displacement** → altura  
- **Normal Map** → detalhe de superfície  
- **ARM** → Ambient, Roughness, Metalness  

---

# 🧭 5. Normal Mapping (TBN Space)

### **Recordar: o que é Normal Map?**

De Geeks for Geeks:
- Normal Maps: Normal maps are 2D colorful images that show information about the normal vectors of 3D objects. A pixel in a normal map has three channels R, G, and B whose values represent the X, Y, and Z values of the normal vector of that point.
- For example, if a pixel is completely red meaning that its RGB value is (1,0,0), then the normal of the vector would point in the X direction. If we see more green then the normal leans toward Y; if more blue, then normal points outward (Z).

From: https://www.geeksforgeeks.org/computer-graphics/normal-mapping-in-computer-graphics/

O normal map não traduz as suas "normais" em world space.  
Ele contém normais em **texture space (tangent space)** — ou seja, um espaço local que existe em cada ponto da superfície.

O objetivo do TBN é: 
```
---> Converter a normal vinda da textura para o espaço do mundo, para que ela possa interagir corretamente com a luz.
```
$$
N_{bumped} = N_{in Normal Map}
$$

Ler mais aqui:
- https://learnopengl.com/Advanced-Lighting/Normal-Mapping

---

## 🧮 Passos (em síntese)

#### A) Calcular normal no mundo

Isto é a geometria “real” do objeto. 

$$
N_{world}
$$

#### B) Calcular Tangente (T) e Bitangente (B)

Aqui está a ideia essencial:

- Tangente (T)  -->  *RED*
  - direção em que o **UV aumenta no eixo U**
  - corresponde ao “lado horizontal” da textura

- Bitangente (B)  -->  *GREEN*
  - direção em que o **UV aumenta no eixo V**
  - corresponde ao “lado vertical” da textura

- Normal (N)  -->  *BLUE*

#### C) Construir matriz TBN

Agora juntamos tudo num sistema de coordenadas:

$$
[TBN] =
\begin{bmatrix}
T & B & N
\end{bmatrix}
$$

Isto significa:
- coluna 1 = direção “U”
- coluna 2 = direção “V”
- coluna 3 = direção “perpendicular à superfície”

#### D) Aplicar textura

$$
N_{bumped} = [TBN] \cdot texture(normalMap, uv)
$$

- O que isto significa fisicamente?
- É como dizer:
  - “pega neste vetor definido localmente na textura e traduz-o para o espaço real do mundo usando os eixos do objeto”


#### E) Normalizar

$$
N_{bumped} = normalize(...)
$$


- Porque:
  - interpolação distorce comprimento
  - transformação pode escalar o vetor
  - iluminação precisa de vetores unitários

👉 só queremos direção, não magnitude

---

### 📌 Fórmula final

$$
N_{bumped} = normalize([TBN] \cdot texture(normalMap, uv))
$$

Tangent and bitangent are not “extra math vectors” —
they are simply the 3D directions of the texture grid glued onto the surface.

---

## ⚠️ Nota importante

- Normais vêm em espaço local (texture space)
- Precisam ser convertidas para world space

```
Pegamos no vetor que vem da textura, que está definido num sistema de coordenadas local.
Depois usamos os eixos do objeto (T, B, N) para perceber como esse vetor se orienta no espaço 3D.
Assim, conseguimos convertê-lo para o espaço do mundo.
```

```
O vetor da textura não está no mesmo “referencial” do mundo.
Usamos o TBN para o “traduzir” para a orientação real da superfície no espaço 3D.
```

```
A textura diz-nos como a superfície está inclinada localmente.
O TBN pega nessa inclinação e coloca-a corretamente no mundo, de acordo com a orientação do objeto.
```


---

# ⚠️ 6. Observações do Professor

- Topo do cilindro pode ficar estranho com normal mapping  
- Problemas com continuidade de UVs  

---

# 🧪 7. Derivadas (Nota avançada)

- derivadas de mundo e UV:
  - medem variação no espaço
  - usadas para comparar escalas

👉 útil para:
- calcular tangentes
- fazer correções (“hacks”)

---

# 🧾 Resumo Geral

| Tema | Ideia principal |
|------|----------------|
| Directional | Luz constante |
| Point | Atenuação \(1/dist^2\) |
| Spotlight | Cone + ângulo |
| Normais | usar \((M^{-1})^T\) |
| Texture Atlas | performance |
| Normal Mapping | detalhe via textura |
| TBN | conversão para world space |

---

# 🧠 Ideia-chave final

- Luz depende de **posição + direção + distância**
- Normais devem ser tratadas com cuidado
- Texturas permitem **ganhar detalhe sem custo geométrico**

---

*Notas organizadas da aula de CGI — Lighting & Texturing*
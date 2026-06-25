# Aula: Introdução à Geometria Procedimental


> **Trabalho 3 — 11/03**  
> Tópicos abordados: **Texturas, UV Mapping, Material Capture, Interpolação de Texturas, Filtros de Magnificação e Minificação**

---

## 1. Texturas / Textures

As **texturas** são imagens aplicadas à superfície de objetos 3D para lhes conferir cor, padrão ou detalhe visual sem aumentar a complexidade geométrica do modelo. Em vez de modelar cada detalhe de uma superfície (e.g., as veias de uma pedra), usamos uma imagem 2D que é "colada" sobre a malha 3D.

Nas notas, é ilustrado que um cilindro 3D com múltiplos vértices pode ser simplificado numa representação mais abstrata composta por:
- **Vértices da face superior (base)**
- **Dois triângulos** (que formam o corpo lateral)
- **Vértices da face inferior (base)**

Esta decomposição em triângulos é fundamental, pois as GPUs processam geometria essencialmente como listas de triângulos.

---

## 2. UV Mapping

O **UV Mapping** é o processo de projetar uma textura 2D sobre a superfície de um objeto 3D. As letras **U** e **V** representam os eixos da textura 2D (análogos aos eixos X e Y no espaço 3D, evitando confusão com as coordenadas do espaço tridimensional).

O espaço UV é normalizado: as coordenadas vão de **(0,0)** (canto inferior esquerdo) a **(1,1)** (canto superior direito). Isto significa que o tamanho real da textura em píxeis **não importa** para as coordenadas — o mapeamento é independente da resolução. Nas notas, é referido: *"porque o tamanho da textura é variável"*.

Cada vértice da malha 3D recebe um par de coordenadas UV que indica qual parte da textura lhe corresponde. O motor gráfico faz depois a interpolação para os fragmentos (píxeis) entre vértices.


### 2.1. ❓ Se a textura é pequena, como cobre um objeto grande sem perder qualidade?

Exemplos de solução são: 

#### Tiling (Repetição)

A opção mais comum: a textura **repete-se** (*tiles*) ao longo da superfície. As coordenadas UV não estão limitadas ao intervalo `[0,1]`. Se deres a um vértice a coordenada UV `(3, 2)`, o motor gráfico interpreta isso como *"repete a textura 3 vezes em U e 2 vezes em V"*. Resultado: a pequena imagem da textura é copiada lado a lado, como azulejos numa parede — daí o nome *tiling*.

Isto funciona **muito bem** para materiais que são naturalmente repetitivos: tijolos, madeira, betão, tecido, relva, etc. A textura pode ser minúscula em píxeis e cobrir uma parede enorme sem perda de qualidade *aparente*, porque o padrão se repete de forma contínua.

**O que realmente acontece (nível técnico)**

O GPU usa essencialmente a parte fracionária das coordenadas UV. No fundo, equivale ao módulo 1:

$$
u_{final} = u_{mod}1
​$$

**🔍 Exemplo**

Se tens:
$$
(u,v)=(3.2,2.7)
$$

Então:
$$
u_{final} = 0.2
​$$
$$
v_{final} = 0.7
​$$

O GPU vai buscar o pixel da textura em (0.2, 0.7).

*Então... há “cópia da textura”?* 
Não literalmente. A textura:
- não é duplicada fisicamente
- não são criadas cópias lado a lado

**Então porque se fala em “tiling”?**

Porque o efeito visual é idêntico a repetir a imagem. À medida que $u$ cresce:
- 0 → 1 → 2 → 3 → ...

O valor efetivo volta sempre ao intervalo [0,1]. Resultado visual: parece que a textura está “colada várias vezes”.


#### Mas e se se notar a repetição?

Aí entra o conceito de **seamless texture** (textura sem costura) — uma textura especialmente desenhada para que as bordas esquerda/direita e cima/baixo coincidam perfeitamente. Quando se repete, não se vê onde uma acaba e a outra começa. O GIMP tem ferramentas para criar texturas seamless.

Ainda assim, em superfícies muito grandes pode notar-se um padrão repetitivo. Para combater isso usam-se truques como:
- Misturar várias texturas com escalas diferentes
- Adicionar ruído *procedural* por cima
- Usar **stochastic tiling** (repetição com variação aleatória)


#### A alternativa: Stretching (Esticamento)

Se as coordenadas UV forem definidas para cobrir a parede inteira com um único `[0,1]→[0,1]`, a textura é **esticada** para caber. Neste caso sim, perde-se resolução — cada texel da imagem pequena tem de cobrir uma área enorme da parede. É o equivalente a fazer zoom numa imagem até ficar desfocada.

#### Em resumo

| Situação | O que acontece |
|---|---|
| UV além de `[0,1]`, e.g. `(3,2)` | Tiling — a textura repete-se |
| UV dentro de `[0,1]` numa área grande | Stretching — perde resolução |
| Textura seamless + tiling | Cobertura grande sem costuras visíveis |

Portanto, a "magia" está nas **coordenadas UV que o artista define**. A mesma textura pequena pode cobrir uma parede enorme com qualidade total — desde que se use tiling e a textura seja seamless.

---

## 3. Material Capture (MatCap)

O **Material Capture**, ou **MatCap**, é uma técnica de renderização que codifica toda a informação de iluminação e reflexo de um material numa única textura esférica. Em vez de calcular a iluminação em tempo real, o motor usa a normal do vértice para fazer lookup diretamente na textura MatCap, obtendo assim a cor final do fragmento de forma muito eficiente.

#### 💡 Porquê uma esfera? — Raciocínio por trás do MatCap

A esfera é a estrutura ideal para capturar informação de iluminação porque, pela sua
simetria, recolhe luz de **todas as direções possíveis** numa única superfície.
A iluminação é calculada apenas **uma vez** (*baked*) e guardada na textura esférica.

Em tempo de renderização, cada ponto da superfície do objeto tem uma **normal**
associada. Essa normal é transformada para **espaço de câmara** e usada diretamente
como coordenada UV para consultar a textura MatCap — se a normal apontar para cima,
vai buscar o topo da esfera; se apontar para a direita, vai buscar o lado direito, etc.

Isto significa que **não se escolhe a direção da luz** em tempo real — ela está
pré-definida na textura. A consequência é que o MatCap é muito eficiente, mas tem
uma limitação: a iluminação é **estática**. Se o objeto ou a câmara rodar, a
iluminação "cola-se" ao objeto em vez de reagir corretamente ao ambiente.

#### ⚙️ Porquê se computa apenas uma vez? - RECAP 
- o MatCap "congela" todo o ambiente de iluminação num instante e reutiliza essa informação indefinidamente.
- *"a luz não muda"*, mas _a limitação é mais abrangente_:
- ao criar um MatCap, assume-se que *são fixos e conhecidos à priori*:
  - A posição e intensidade das fontes de luz
  - As reflexões do ambiente (*environment lighting*)
  - As propriedades do material (rugosidade, metalicidade, etc.)
- O MatCap **nunca é recalculado em runtime** &rarr; abdica-se de iluminação dinâmica em troca de **performance**.
- Por isso que o MatCap é popular em contextos onde o tempo de renderização é crítico (*sculpting* em tempo real (e.g. ZBrush); visualizações rápidas de modelos 3D).


Nas notas são mencionados recursos e ferramentas úteis:

- **[Poly Haven](https://polyhaven.com)** — site com texturas e HDRIs de alta qualidade gratuitos (*"site com texturas"*).
- **Encoding → dds** — formato de textura otimizado para GPU (*"imagens para estarem na GPU"*). O formato DDS (*DirectDraw Surface*) suporta compressão direta por hardware, reduzindo a largura de banda de memória durante a renderização.
- **GIMP** — editor de imagem gratuito e open-source utilizado para criar, editar ou converter texturas.
- **Texel** — a unidade mínima de uma textura (equivalente ao píxel numa imagem), podendo conter vários valores por posição. Nas notas: *"vários taxels por pixel — está longe"* — referindo-se à situação em que um pixel no ecrã cobre vários texels da textura, o que se relaciona com os filtros (ver secção 5).

---

## 4. Interpolação de Texturas 

Quando uma textura é mapeada sobre uma superfície, raramente existe uma correspondência perfeita 1-para-1 entre os texels e os píxeis no ecrã. 

🌠 **_The more you know_** 🌠
- A texel, or texture element, is the basic unit of a _texture map_ used in computer graphics
- A pixel, or picture element, is the basic unit of a _digital image_ displayed on a screen.


Por isso, é necessário **interpolar** o valor da textura. Existem três métodos principais, mencionados nas notas:

| Método | Descrição | Custo |
|---|---|---|
| **Nearest (Neighbor)** | Usa o texel mais próximo. Rápido mas pode produzir uma aparência "pixelizada" (*blocky*). | Baixo |
| **Bilinear** | Faz uma média ponderada dos 4 texels mais próximos. Resultado mais suave. | Médio |
| **Bicubic** | Usa os 16 texels vizinhos para uma interpolação cúbica. Resultado mais suave mas *"menor mas custa"* (menor qualidade de artefactos, mas mais computacionalmente caro). | Alto |

A fórmula geral de interpolação linear apresentada nas notas é:

$$
f(a, b, t) = a · (1 - t) + b · t
$$

Onde `t` é o parâmetro de interpolação entre 0 e 1. Casos especiais:
- `f(a, b, 1) = b` — quando `t = 1`, obtemos o valor de `b`.
- `f(a, b, 0) = a` — quando `t = 0`, obtemos o valor de `a`.
- `f(a, b, 0.5) = (a + b) / 2` — quando `t = 0.5`, obtemos a média dos dois valores.

---

## Qual a diferença entre os problemas identificados em _2.1._ e _4_?

Cada ponto identifica dois problemas diferentes que surgem em momentos diferentes.

- **O primeiro problema (_Tiling_)** é um problema de **cobertura geométrica**: 
  - *"como faço a minha textura cobrir uma área maior do que ela própria?"* A solução é repetir — as coordenadas UV excedem o intervalo [0,1] e a textura é copiada lado a lado. Isto acontece **antes** de se pensar em píxeis ou resolução.

- **O segundo problema (_Interpolação_)** é um problema de **amostragem e resolução**: 
  - *"dado que já sei qual parte da textura corresponde a cada ponto da superfície, como converto essa informação num píxel no ecrã?"* Aqui o problema é que texels e píxeis raramente têm uma correspondência 1-para-1 — um píxel pode cair entre dois texels, ou um texel pode ter de cobrir vários píxeis. A interpolação resolve essa ambiguidade.



Metáfora: imaginar um mapa de uma cidade.
- **Tiling** é decidir quantas vezes dobras o mapa para cobrir uma mesa grande — é um problema de *escala e repetição*.
- **Interpolação** é decidir o que desenhas num ponto do mapa que cai *entre* duas ruas impressas — é um problema de *precisão e amostragem*.

São ortogonais: podes ter tiling sem interpolação e vice-versa. Na prática, a *pipeline* gráfica resolve os dois em sequência — primeiro o UV mapping determina *onde* na textura estás, depois a interpolação determina *como* se lê esse valor.

---

## 5. Filtros Min e Mag


Os **filtros de minificação e magnificação** controlam como a textura é amostrada consoante a distância do objeto à câmara:

- **Mag Filter (Magnificação)**: Usado quando o objeto está **perto** da câmara e um único texel é expandido para cobrir múltiplos píxeis. Os filtros bilinear ou nearest são os mais comuns.
- **Min Filter (Minificação)**: Usado quando o objeto está **longe** da câmara e múltiplos texels contribuem para um único píxel. Aqui entram técnicas como **Mipmapping**.

Nas notas é referida uma referência para aprender mais: **git.books.io** (*"min and mag filters → git.books.io"*).

**...novamente...**

## 5. Texture Filtering: Minification vs Magnification

### Que problema se encontra?
Ao mapear uma textura para um objeto, há um desfasamento entre:
* **Texels** (da textura)
* **Pixels** (do ecrã)

É necessário definir como obter a cor final de cada pixel.

### Magnification Filter (MAG)
É usado quando:
* O objeto está **próximo da câmara**
* Um texel cobre **vários píxeis**

O objetivo é interpolar valores para evitar efeito pixelizado.

#### Métodos
* `NEAREST`: escolhe o texel mais próximo (rápido, mas blocado)
* `LINEAR`: interpola entre texels (mais suave)

### Minification Filter (MIN)
É usado quando:
* O objeto está **distante**
* Muitos texels contribuem para **um único píxel**

#### Problema:
Aliasing (ruído, flickering)

#### Solução:
Podem utilizar-se **Mipmaps**:
* Versões reduzidas da textura
* GPU escolhe o nível adequado

#### Métodos comuns:
* `NEAREST_MIPMAP_NEAREST`
* `LINEAR_MIPMAP_LINEAR` (trilinear filtering)

### 🧠 Intuição

- **Magnification** → falta de detalhe → **interpolação**
- **Minification** → excesso de detalhe → **redução (mipmaps)**

> Magnification filters suavizam texturas ampliadas, enquanto minification filters reduzem aliasing usando mipmaps quando a textura é vista à distância.

---

## 6. Triplanar mapping

**Triplanar mapping** é uma técnica de mapeamento de texturas usada em computação gráfica que projeta a mesma textura em três planos ortogonais (X, Y e Z). Em vez de depender de coordenadas UV tradicionais, mistura essas três projeções com base na orientação da superfície (normal). Isso evita distorções e “stretching”, especialmente em geometrias complexas ou geradas proceduralmente. É muito usado em terrenos, rochas e materiais onde o UV mapping seria difícil ou pouco prático.
- https://www.martinpalko.com/triplanar-mapping/

![Triplanarmapping1](./images/3PlanarMaps_step1.jpg =500x)
![Triplanarmapping2](./images/3PlanarMaps_step2.jpg =500x)

*Figure: So, what is this __triplanar mapping__, and what’s it for? The general idea is that we map a texture three times with planar maps (thus the tri-planar bit) along the X, Y, and Z axes, and then blend between these three samples based on the angle of the face, using the one that fits best with the least stretching.*

---

## Resumo

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

*Notas geradas a partir das fotos dos apontamentos da **R.** (Introdução à Geometria Procedimental) — 11/03.*  

# ECAC - Exame de Época Normal 2025/2026

Estas são as perguntas que saíram no exame. Por falta de tempo, não deu para passar tabelas com pontos/valores e opções de cada escolha múltipla. 

**O objetivo deste documento é dar uma noção do tipo de perguntas/matéria que saiu.**

## Table of Contents

- [ECAC - Exame de Época Normal 2025/2026](#ecac---exame-de-época-normal-20252026)
  - [Table of Contents](#table-of-contents)
  - [Módulo A:](#módulo-a)
    - [Pergunta 1](#pergunta-1)
      - [Pergunta 1.1](#pergunta-11)
      - [Pergunta 1.2](#pergunta-12)
      - [Pergunta 1.3](#pergunta-13)
      - [Pergunta 1.4](#pergunta-14)
    - [Pergunta 2](#pergunta-2)
      - [Pergunta 2. a)](#pergunta-2-a)
      - [Pergunta 2. b)](#pergunta-2-b)
    - [Pergunta 3](#pergunta-3)
      - [Pergunta 3. a)](#pergunta-3-a)
      - [Pergunta 3. b)](#pergunta-3-b)
  - [Módulo B](#módulo-b)
    - [Pergunta 4.](#pergunta-4)
      - [Pergunta 4.1](#pergunta-41)
      - [Pergunta 4.2](#pergunta-42)
      - [Pergunta 4.3](#pergunta-43)
      - [Pergunta 4.4](#pergunta-44)
    - [Pergunta 5](#pergunta-5)
      - [Pergunta 5.1](#pergunta-51)
      - [Pergunta 5.2](#pergunta-52)
    - [Pergunta 6](#pergunta-6)
      - [Pergunta 6.1](#pergunta-61)


## Módulo A:

### Pergunta 1

Escolhas múltiplas

#### Pergunta 1.1 

Numa tarefa de classificação por recurso à aprendizagem computacional observa-se:
  - As escolhas mencionavam desbalanceamento dos dados e data augmentation (SMOTE)

#### Pergunta 1.2

Numa análise preliminar dos dados é conveniente:
  - As escolhas mencionavam análise lambda, regressão logística e Spearman

#### Pergunta 1.3 

Preparação dos dados de entrada para uma tarefa de aprendizagem computacional:
  - Não retirei a matéria mencionada nas perguntas

#### Pergunta 1.4

Numa modelação linear para imputação de dados observa-se que:
  - A dimensão do modelo é determinável a partir dos dados.
  - A dimensão do modelo não é determinável a partir dos dados.
  - As outras duas opções eram sobre a utilização de variáveis independentes e contínuas

### Pergunta 2

Considere o seguinte conjunto de varáveis categóricas adquiridas de um conjunto de 12 doentes:

| Doente | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Passos Diários** | 640 | 1650 | 1700 | 1800 | 2000 | 3000 | 540 | 5000 | 6500 | 5500 | 7000 | 3000 |
| **Fumador** | S | N | S | S | N | N | N | S | S | S | N | S |
| **Creatinina Alta** | S | N | N | S | S | N | N | N | N | N | N | S |
| **Risco Cardiovascular** | B | B | A | A | B | B | B | B | A | A | A | B |

Assumindo que S=”Sim”, N=”Não” e A="Alto", B="Baixo".


#### Pergunta 2. a) 

Considere que lhe é pedido que projete um preditor do risco cardiovascular usando um dos atributos categóricos como entrada. Nesse contexto, apresente o ranking de atributos usando a metodologia baseada em Informação Mútua. Apresente todos os cálculos; não use implementações computacionais.

#### Pergunta 2. b) 

Apresente a primeira iteração do algoritmo de binning da variável "Passos Diários" baseado na entropia correspondente à avaliação de um eventual split no 5º doente (comsidere que pertence ao 1º set). Apresente os seus cálculos.

### Pergunta 3

Eram dados 30 e poucos pontos (com um Xmax de 40 e Ymax de 40) que pertenciam a duas classes (C1 e C2).

#### Pergunta 3. a)

Aplicar o DBSCAN com um epsilon = 10 e minPts = 4 a um ponto e dizer como se classifica esse ponto.

#### Pergunta 3. b)

Davam um ponto novo (15, 15) com uma classe desconhecida e era para fazermos imputação.

## Módulo B

### Pergunta 4.

Escolhas Múltiplas

#### Pergunta 4.1

Era mencionado o overfitting em teste de um modelo em dados não vistos.

#### Pergunta 4.2

Avaliação de modelos e eram mencionados:
  - hold-out
  - K-fold
  - leave out

#### Pergunta 4.3

Tinham 4 opções com 4 métricas de desempenho (cada opção tinha uma definição do que era a métrica)
  - Accuracy
  - Precision
  - Recall
  - F1-measure
  
#### Pergunta 4.4

Pergunta sobre o dilema viés-variância

### Pergunta 5

Considere a seguinte imagem em escala de cinzentos (tamanho: 6*5, níveis possíveis: {0,1,2})

#### Pergunta 5.1

Apresente a Gray-Level Co-occurance Matrix (GLCM), considerando a direção vertical e a distância de 3 pixéis.

#### Pergunta 5.2

Calcule o contraste a partir da GLCM. Nota: se não resolveu o exercício anterior, considere a seguinte GLCM

### Pergunta 6

Davam uma tabela com w, S(w) e os angulos

#### Pergunta 6.1

Calcular a largura de banda espectral

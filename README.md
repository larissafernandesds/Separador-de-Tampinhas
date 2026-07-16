<h1 align="center">♻️ Separador de Tampinhas ♻️</h1>
<h3 align="center">Grupo 4</h3>

<div align="center">

![made-with-cpp](https://img.shields.io/badge/Made%20with-C%2FC%2B%2B-green)
![platform](https://img.shields.io/badge/Platform-Arduino-blue)
![Commits](https://img.shields.io/github/commit-activity/t/larissafernandesds/Separador-de-Tampinhas?label=Commits)  

<img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/cplusplus/cplusplus-original.svg" width="40"/>
<img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/arduino/arduino-original.svg" width="40"/>
<img src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/github/github-original.svg" width="40"/>

</div>

---

## 📝 Descrição do Projeto  

O **Grupo 4** desenvolveu um **Separador Automático de Tampinhas de Garrafa PET por Cor**. O protótipo possui um **mecanismo alimentador**, no qual o usuário deposita várias tampinhas. Esse mecanismo organiza e **enfileira as tampinhas**, permitindo que elas passem **uma de cada vez** pelo sensor de cores, garantindo uma leitura precisa. Após a leitura, o **Arduino Uno** processa os valores obtidos pelo sensor, identifica a cor da tampinha por meio de perfis previamente calibrados e aciona **servomotores**, responsáveis por direcioná-la ao compartimento correspondente. O sistema também conta com um **display LCD 16×2 com módulo I2C**, utilizado para fornecer e coletar informações ao usuário durante a calibração.  

---  

## 🎯 Objetivo  

Desenvolver um sistema automatizado capaz de identificar e classificar tampinhas plásticas por cor, aplicando conceitos de sistemas embarcados, eletrônica, automação e programação para tornar o processo de separação mais eficiente e preciso.

---

## 💡 Motivação

A separação de tampinhas plásticas por cor é uma etapa importante do processo de reciclagem, pois aumenta a qualidade do material reciclado e facilita seu reaproveitamento. Quando diferentes cores são misturadas, o material obtido geralmente apresenta menor valor comercial e menor qualidade, dificultando sua reutilização em novos produtos.

Apesar de sua importância, esse processo ainda é realizado manualmente por diversas organizações, cooperativas e iniciativas sociais, tornando a atividade repetitiva, lenta e suscetível a erros. Além disso, a separação manual exige esforço físico constante e limita a quantidade de material que pode ser processada diariamente.

Além do benefício ambiental, a arrecadação de tampinhas possui um importante impacto social. Diversas organizações utilizam a venda do material reciclável para financiar projetos beneficentes e ações de proteção ao meio ambiente. Entre elas destacam-se:

- **Tampinhas que Curam**: iniciativa que arrecada tampinhas plásticas para auxiliar no tratamento de crianças com câncer.
- **Ecopatas**: ONG que arrecada tampinhas plásticas e lacres de alumínio para contribuir com a preservação ambiental e o resgate de animais abandonados.
- **Tampinha do Bem**: projeto voltado à coleta e reciclagem de tampinhas, destinando os recursos obtidos para instituições sociais e ações de proteção animal.
- **Pontos de coleta**: como o instalado no Shopping Vitória (ES), que recebem tampinhas e lacres destinados posteriormente a projetos sociais e ambientais.

---


## 🌱 Impactos e ODSs

<img width="1920" height="900" alt="ods" src="https://github.com/user-attachments/assets/be299f3c-95e6-4ad5-b57b-84ad1ee2d3fe" />

---

## ⚙️ Funcionamento Geral



---

## 🛠️ Arquitetura do Sistema



---

# 🧠 Software

## ⚙️ Funcionamento do Sistema

---

## 🤖 Modos de 



---

## 🤚 Gestos Reconhecidos

---

# 🔌 Hardware

## 🖨 Modelos 3D

Todas as peças mecânicas do projeto foram modeladas no **Autodesk Inventor**.

Os modelos encontram-se na pasta `Hardware/Modelos 3D/` e são disponibilizados no formto .stl.

---
## 🌈 Sensor de Cor

O sistema utiliza o sensor de cores **TCS3200**, responsável por realizar a identificação das tampinhas plásticas através da análise da luz refletida pela superfície de cada material.

O TCS3200 é um sensor óptico composto por uma matriz de fotodiodos e um conversor de corrente para frequência. Ele possui filtros internos que permitem selecionar a leitura das componentes de cor **vermelho (R), verde (G) e azul (B)**, além de uma leitura sem filtro.

O funcionamento do sensor ocorre da seguinte forma: uma fonte de luz interna ilumina o objeto posicionado à frente do sensor. A luz refletida pela tampinha atinge os fotodiodos, que geram uma corrente elétrica proporcional à intensidade luminosa recebida. Essa corrente é convertida internamente pelo circuito do TCS3200 em um sinal digital de frequência.

Dessa forma, quanto maior a intensidade da cor selecionada, maior será a frequência do sinal de **saída (OUT)** gerado pelo sensor.

O Arduino não recebe diretamente valores de vermelho, verde e azul. Em vez disso, ele mede a frequência produzida pelo sensor para cada filtro de cor e utiliza esses valores para calcular uma representação RGB da tampinha analisada.

---

## ⏳ Estado Atual do Projeto

✔️ Completo

**Progresso estimado: ~100%**

---
## 🚀 Verificação dos Objetivos e Metas desenvolvidos no Pitch!!

 
 
---

## 📁 Estrutura do Repositório

```text
Separador-de-Tampinhas/
├── Software/
│   ├── Enfileirador.ino
│   └── Separador.ino
│
├── Hardware/
│   ├── Modelos 3D/
│   └── Imagens/
│      
└── README.md
```
---

## 📷 Como tudo começou...

<div align="center" style="display:flex; gap:20px;">
  <img src= "./Hardware/Imagens/tubo.jpeg" style="width:300px; height:250px; object-fit:contain;"/>
  <img src="./Hardware/Imagens/sensor.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
  <img src="./Hardware/Imagens/lcd.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
  <img src="./Hardware/Imagens/lab.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
  <img src="./Hardware/Imagens/enfileirador.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
 <img src="./Hardware/Imagens/capa.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
</div>

---

## 📷 Imagens do Projeto

<div align="center" style="display:flex; gap:20px;">
 <img src="./Hardware/Imagens/proj2.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
 <img src="./Hardware/Imagens/proj1.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
 <img src="./Hardware/Imagens/proj3.jpeg"style="width:300px; height:250px; object-fit:contain;"/>
</div>

---

## 📋 Pré-requisitos



---

## ▶️ Instruções de Execução


---

## 🎥 Vídeo Pitch

Um pitch do projeto está disponível no link abaixo:

🔗 [Clique aqui para assistir ao vídeo do projeto](https://www.youtube.com/watch?v=MaOEnb36ciA)

---

## 🏁 Conclusão

O projeto permitiu a aplicação prática de conceitos de eletrônica, programação embarcada e automação, proporcionando aprendizado técnico, trabalho em equipe e desenvolvimento de uma solução tecnológica integrada.

---

## 👩‍💻 Colaboradores

| <img loading="lazy" src="https://avatars.githubusercontent.com/u/102550275?v=4" width=115><br><sub>André Silvares</sub> | <img loading="lazy" src="https://avatars.githubusercontent.com/u/113942011?v=4" width=115><br><sub>Daniel Moulin</sub> |<img loading="lazy" src="https://avatars.githubusercontent.com/u/97314555?v=4" width=115><br><sub>Larissa Fernandes</sub> | <img loading="lazy" src="https://avatars.githubusercontent.com/u/73800969?v=4" width=115><br><sub>Rafael Santiago</sub> |
| :---: | :---: | :---: | :---: |
---

## 📌 Observação

O projeto possui caráter **acadêmico e experimental**, sendo desenvolvido para a
disciplina **Projeto Integrado de Computação II** da UFES, com foco na integração entre
software, hardware e sociedade.

---

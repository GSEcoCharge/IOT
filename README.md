# 🌐 IOT

<details open>
    <summary><h3><strong>📑 Sumário</strong></h3>
        <ol>
            <li><a href="#pre-requisitos">Pré-requisitos</a></li>
            <li><a href="#estrutura">Estrutura do Projeto</a></li>
            <li><a href="#passos">Passos para Replicar e Testar a Solução</a></li>
            <li><a href="#requisitos">Requisitos do Projeto de IOT</a></li>
            <li><a href="#entregaveis">Entregáveis</a></li>
        </ol>
    </summary>
</details>

<h2>📚 Descrição do Projeto</h2>

Este projeto de IoT foi desenvolvido para monitorar a disponibilidade de vagas em um posto de carregamento elétrico para ser utilizado no aplicativo principal da solução EcoCharge ([Link da Organização](https://github.com/GSEcoCharge)), utilizando um sensor de distância ultrassônico para detectar a presença de veículos em um ponto específico. O sistema envia os dados do sensor para um dashboard em tempo real, onde é possível visualizar o status do ponto (Vazio ou Ocupado) e a distância do sensor em relação ao veículo.

<h2 id="pre-requisitos">🔧 Pré-requisitos</h2>

Antes de começar, você precisa garantir que tem os seguintes itens instalados ou configurados:

- [Node-RED](https://nodered.org/docs/getting-started/) instalado ou acessível
- O arquivo `flows.json` deste repositório

<h2 id="estrutura">📁 Estrutura do Projeto</h2>

- **Código Arduino (Wokwi)**: O código do Arduino, que controla sensores e LEDs, está disponível na plataforma Wokwi neste [Link](https://wokwi.com/projects/414012916024807425).
- **flows.json**: O arquivo de configuração do fluxo do Node-RED, com todos os nós necessários para visualizar os dados, está disponível neste repositório.

<h2>🚀 Passos para Replicar e Testar a Solução</h2>

### 1. **Configuração do Wokwi (Código Arduino)**

1. Acesse o código Arduino simulado na plataforma Wokwi: [Projeto Wokwi](https://wokwi.com/projects/414012916024807425)
2. Simule o código no Wokwi para garantir que os dados sejam enviados corretamente para o broker MQTT.

![Wokwi Simulation](Utils/imgs/wokwi-simulation.png)

### 3. **Configuração do Node-RED**

1. Clone o repositório deste projeto (Opcional):

   ```bash
   git clone https://github.com/GSEcoCharge/IOT
   cd IOT
   ```

2. Importe o arquivo `flows.json` no seu ambiente Node-RED:

   - Abra o Node-RED em seu navegador (geralmente acessível por http://localhost:1880).
   - No menu do Node-RED, clique em **Import**.
   - Selecione o arquivo `flows.json` do repositório clonado.
   - Clique em **Importar** para carregar o fluxo no Node-RED.

3. Verifique se o fluxo está configurado para se conectar corretamente ao seu MQTT Broker e exibir os dados recebidos.

### 4. **Configuração do Dashboard**

1.  Caso o Node-RED ainda não tenha a paleta de dashboard instalada, instale a paleta necessária:

    - No menu do Node-RED, vá em **Manage palette**.
    - Busque por `node-red-dashboard` e instale a paleta.

2.  Verifique o painel de controle do Node-RED. O fluxo importado deve configurar um dashboard acessível em http://localhost:1880/dashboard/home, onde você pode visualizar os dados dos sensores e os LEDs.

### 5. **Testando a Solução**

1.  Com o Wokwi simulando o código Arduino e enviando dados via MQTT, abra o dashboard no Node-RED em http://localhost:1880/dashboard/home.
2.  O painel deve começar a exibir as informações do sensor de distância e o Status do ponto (Vazio ou Ocupado), com base nos dados recebidos do Arduino via MQTT.
3.  Para testar a solução, movimente o sensor de distância e verifique como os dados são atualizados automaticamente no dashboard.

![Node-RED Dashboard](Utils/imgs/node-red-dashboard.png)

<h2 id="requisitos">📋 Requisitos do Projeto de IOT</h2>

- [x] **Sistema IoT**: Integrar, no mínimo, 2 dispositivos (sensores/atuadores ou ambos), coletando, processando ou interagindo com base nos dados obtidos.
- [x] **Hardware**: Construir a solução utilizando uma das plataformas abordadas em aula.
- [x] **Dashboard**: Implementar um painel de controle para monitorar e/ou controlar em tempo real os dados e ações dos dispositivos IoT.
- [x] **Gateway**: Utilizar Node-RED, thinger.io, Thingspeak ou plataformas similares para gerenciar a comunicação entre dispositivos IoT e o dashboard, incluindo fluxos de processamento, análise e visualização de dados.
- [x] **Protocolos**: Empregar o protocolo MQTT ou HTTP para a troca de mensagens entre dispositivos e gateway.
- [x] **Desenvolvimento**: A solução pode ser desenvolvida e testada no simulador Wokwi ou com hardware real.

### Entregáveis

- [x] **Protótipo Funcional** (até 60 pts): Protótipo operacional da solução IoT, simulado ou real.
- [x] **Documentação** (até 30 pts): Repositório GitHub contendo:
  - [x] Descrição detalhada da solução com texto e imagens.
  - [x] Códigos-fonte.
  - [x] Fluxos do Node-RED (se aplicável).
  - [x] README detalhado com instruções para replicar e testar a solução.
- [ ] **Vídeo de Apresentação** (até 10 pts): Vídeo de até 3 minutos, apresentando a solução, funcionamento e benefícios.

### Entrega Final

- [ ] Arquivo .txt no portal contendo:
  - [x] Nomes dos integrantes do grupo.
  - [x] Link do repositório GitHub.
  - [ ] Link do vídeo no YouTube.

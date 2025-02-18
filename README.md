## 🔥 Projeto: Conversores A/D - Projeto de Controle de LEDs e Display com Joystick na BitDogLab

## 📌 Descrição do Projeto

Este projeto tem como objetivo consolidar os conceitos de uso de conversores analógico-digitais (ADC) no RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab. O projeto utiliza um joystick para controlar LEDs RGB, exibir a posição do joystick em um display SSD1306 e implementar funcionalidades adicionais com botões.

## Objetivos

- Compreender o funcionamento do ADC no RP2040.
- Utilizar PWM para controlar a intensidade de LEDs RGB com base nos valores do joystick.
- Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
- Aplicar o protocolo de comunicação I2C na integração com o display.

## Funcionalidades do Projeto

### 1. Controle dos LEDs Azul e Vermelho

### 2. Exibição de um Quadrado no Display SSD1306

### 3. Alternar o LED Verde e Modificar a Borda do Display

### 4. Ativar/Desativar LEDs PWM com o Botão A

## 📋 Requisitos do Projeto

1. **Uso de Interrupções**: Todas as funcionalidades relacionadas aos botões são implementadas usando rotinas de interrupção (IRQ).
2. **Debouncing**: É obrigatório implementar o tratamento do bouncing dos botões via software.
3. **Display SSD1306**: O display é controlado via protocolo I2C, com uso de ferramentas gráficas para exibição.
4. **Organização do Código**: O código está estruturado e comentado para facilitar o entendimento.

## 🛠️ Configurando o Projeto no VS Code

1. **Clonar o Repositório**:
```
git clone https://github.com/deciofrancis/EmbarcatechConverterAD.git
cd EmbarcatechConverterAD
```

2. **Pra o primeiro projeto - Abra o VS Code** e **importe o projeto**:
   - Vá até a **Extensão Raspberry Pi Pico**.
   - Selecione **Import Project**.
   - Escolha a pasta do repositório clonado.
   - Clique em **Import**.

3. **Compilar o código**:
   - Utilize a opção de **Build** da extensão.

4. **Rodar no BitDogLab**:
   - Na extensão do VSCode Raspberry PI PICO Project.
   - Clique em **Run Project(USB)** para enviar o arquivo para a BitDogLab.

## Desenvolvedor

[Décio Francis](https://www.linkedin.com/in/deciofrancis/)

## Vídeo de Demonstração

[Assista ao vídeo de demonstração aqui](https://youtu.be/0xHN_Vn1yF8)
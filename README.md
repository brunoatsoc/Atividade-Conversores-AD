# Controle de LEDs e Display com Joystick - BitDogLab

## Autor
Bruno Santos Costa

## Descrição
Este projeto utiliza a placa de desenvolvimento **BitDogLab** para controlar LEDs RGB e exibir a posição de um joystick em um display **SSD1306** usando um quadrado desenhado na tela. O sistema faz uso de **ADC**, **PWM** e **I2C** para interagir com os componentes.

## Componentes Utilizados
- **Placa BitDogLab** (baseada no RP2040)
- **Joystick** (conectado aos GPIOs **26 e 27**)
- **LED RGB** (conectado aos GPIOs **11, 12 e 13**)
- **Botão do Joystick** (GPIO **22**)
- **Botão A** (GPIO **5**)
- **Display OLED SSD1306** (I2C nos GPIOs **14 e 15**)

## Funcionamento
- O joystick fornece valores analógicos para os **eixos X e Y**, utilizados para controlar:
  - **LED Azul** - intensidade ajustada conforme o eixo **Y**.
  - **LED Vermelho** - intensidade ajustada conforme o eixo **X**.
  - Os LEDs são controlados via **PWM** para permitir uma variação suave.
- A posição do joystick é representada no **display OLED** por um quadrado de 8x8 pixels.
- O **botão do joystick** alterna o estado do **LED verde** e modifica a borda do display para uma borda tracejada.
- O **botão A** ativa/desativa os LEDs PWM.

## Como Rodar o Projeto
### Compilar e Enviar para a Placa BitDogLab
1. Conectar a placa **BitDogLab** ao computador.
2. Colocar a placa em **modo BOOTSEL** (pressionar o botão **BOOTSEL** e conectar o cabo USB).
3. Compilar o código.
4. Enviar o arquivo `.uf2` gerado para a unidade USB da **BitDogLab**.
5. A placa reiniciará automaticamente e executará o programa.

## Saída Esperada
- O LED **vermelho** e o **azul** variarão conforme a posição do **joystick**.
- O **display OLED** exibirá um **quadrado móvel** representando a posição do joystick.
- O **LED verde** alternará entre ligado/desligado ao pressionar o **botão do joystick**.
- A borda do display também mudará ao pressionar o **botão do joystick**.
- O **botão A** ativará/desativará os LEDs PWM.

## Link do Vídeo de Apresentação
https://youtu.be/KQiMeRSd2sE?si=9_LGcoKJ7q8otrc8


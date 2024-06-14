# Jogo de Adivinhação de Datas com Arduino

Este projeto é um jogo interativo baseado em Arduino que utiliza RFID, uma tela LCD e um módulo de reprodução de áudio MP3 para criar uma experiência divertida e educativa. O objetivo do jogo é adivinhar a data correta armazenada em tags RFID. Quando a data correta é adivinhada, o sistema reproduz uma música específica associada à tag RFID.

## Funcionalidades

- **Leitor RFID (MFRC522)**: Utiliza tags RFID para armazenar e ler informações sobre a data correta e o número da música a ser reproduzida.
- **Tela LCD 16x2**: Exibe mensagens e instruções para o jogador, como a data atual selecionada e feedback sobre a adivinhação.
- **Botões de Controle**: Três botões são usados para selecionar o dia, o mês e confirmar a data escolhida.
- **Módulo MP3 (DFPlayer Mini)**: Reproduz uma música específica quando a data correta é adivinhada.
- **Feedback em Tempo Real**: Mensagens no display LCD e reprodução de áudio proporcionam uma experiência interativa.

## Componentes Utilizados

- **Arduino Uno**: Placa principal do projeto.
- **Leitor RFID (MFRC522)**: Para leitura e gravação de tags RFID.
- **Tela LCD 16x2 com I2C**: Para exibição de informações.
- **Módulo MP3 (DFPlayer Mini)**: Para reprodução de áudio.
- **Botões**: Para controle do jogo (seleção de dia, mês e confirmação).
- **Tags RFID (NTAG215)**: Para armazenar a data e o número da música.

## Estrutura dos Dados na Tag RFID

- **Bloco 4**:
  - Byte 0: Dia (1-31)
  - Byte 1: Mês (1-12)
  - Byte 2: Número da música (1-255)

## Como Funciona

1. **Inicialização**: O sistema exibe uma mensagem solicitando que uma tag RFID seja aproximada do leitor.
2. **Leitura da Tag**: A data correta e o número da música são lidos da tag RFID.
3. **Seleção da Data**: O jogador usa os botões para selecionar o dia e o mês.
4. **Confirmação**: Ao confirmar a data, o sistema verifica se está correta.
5. **Reprodução da Música**: Se a data estiver correta, a música associada é reproduzida e uma mensagem é exibida no LCD.

## Configuração do Sistema

1. **Conecte os Componentes**: 
   - Leitor RFID aos pinos SPI do Arduino.
   - Tela LCD aos pinos I2C.
   - Módulo MP3 aos pinos RX e TX.
   - Botões aos pinos digitais com pull-up interno ativado.

2. **Carregue o Código**: Utilize a Arduino IDE para carregar o código no Arduino.

3. **Teste o Sistema**: Aproximar uma tag RFID, selecionar a data e confirmar. Verifique se a música correta é reproduzida e se o feedback é exibido no LCD.

## Requisitos de Software

- Arduino IDE
- Bibliotecas:
  - `MFRC522`
  - `LiquidCrystal_I2C`
  - `DFRobotDFPlayerMini`
  - `SoftwareSerial`

## Requisitos de Hardware

- Arduino Uno
- Leitor RFID (MFRC522)
- Tela LCD 16x2 com I2C
- Módulo MP3 (DFPlayer Mini)
- Três botões
- Tags RFID (NTAG215)

Este projeto é uma excelente maneira de aprender sobre integração de hardware e software, uso de RFID para armazenamento de dados e controle de reprodução de áudio com Arduino.

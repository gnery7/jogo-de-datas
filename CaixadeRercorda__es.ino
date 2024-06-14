#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Inicializa o display I2C (endereço, colunas, linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Substitua 0x27 pelo endereço do seu display, se necessário

// Definindo os pinos dos botões
const int diaButtonPin = 6;
const int mesButtonPin = 7;
const int confirmarButtonPin = 8;

// Definindo os pinos do RC522
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Criando instância do MFRC522

// Definindo os pinos para o módulo MP3
const int RX_PIN = 4; // Conectado ao TX do MP3
const int TX_PIN = 5; // Conectado ao RX do MP3

SoftwareSerial mp3Serial(RX_PIN, TX_PIN); // Configura a comunicação serial com o módulo MP3
DFRobotDFPlayerMini myDFPlayer; // Cria instância do player

// Variáveis para armazenar o dia e o mês selecionados pelo usuário
int dia = 1;
int mes = 1;

// Variáveis para armazenar a data correta e a música
int diaCorreto = 0;
int mesCorreto = 0;
int musica = 0;

// Variáveis para debounce
unsigned long lastDebounceTimeDia = 0;
unsigned long lastDebounceTimeMes = 0;
unsigned long lastDebounceTimeConfirmar = 0;
const unsigned long debounceDelay = 50; // 50 ms debounce time

// Estado atual e anterior dos botões
int buttonStateDia = HIGH;
int lastButtonStateDia = HIGH;

int buttonStateMes = HIGH;
int lastButtonStateMes = HIGH;

int buttonStateConfirmar = HIGH;
int lastButtonStateConfirmar = HIGH;

// Variável para armazenar o estado do jogo
bool jogoIniciado = false;

// Contador de tentativas
int tentativasRestantes = 3;

void setup() {
  // Configura os pinos dos botões como entrada com pull-up interno
  pinMode(diaButtonPin, INPUT_PULLUP);
  pinMode(mesButtonPin, INPUT_PULLUP);
  pinMode(confirmarButtonPin, INPUT_PULLUP);

  // Inicializa o display I2C
  lcd.init();
  lcd.backlight();
  lcd.print("Aproxime a tag");

  // Inicializa o leitor RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600); // Inicializa comunicação serial para debugging
  Serial.println("Sistema inicializado. Aproxime a tag para iniciar o jogo.");

  // Inicializa o módulo MP3
  mp3Serial.begin(9600);
  if (!myDFPlayer.begin(mp3Serial)) {
    Serial.println("Inicialização do DFPlayer falhou!");
    while (true); // Trava o sistema se o DFPlayer não inicializar
  }
  myDFPlayer.volume(30); // Define o volume (0-30)
  Serial.println("DFPlayer Mini inicializado.");
}

void loop() {
  if (!jogoIniciado) {
    iniciarJogo();
  } else {
    jogar();
  }
}

void jogar() {
  // Leitura atual dos botões
  int readingDia = digitalRead(diaButtonPin);
  int readingMes = digitalRead(mesButtonPin);
  int readingConfirmar = digitalRead(confirmarButtonPin);

  // Verifica se o botão do dia foi pressionado
  if (readingDia != lastButtonStateDia) {
    lastDebounceTimeDia = millis();
  }
  if ((millis() - lastDebounceTimeDia) > debounceDelay) {
    if (readingDia != buttonStateDia) {
      buttonStateDia = readingDia;
      if (buttonStateDia == LOW) {
        dia++;
        if (dia > 31) {
          dia = 1;
        }
        atualizarDisplay();
      }
    }
  }
  lastButtonStateDia = readingDia;

  // Verifica se o botão do mês foi pressionado
  if (readingMes != lastButtonStateMes) {
    lastDebounceTimeMes = millis();
  }
  if ((millis() - lastDebounceTimeMes) > debounceDelay) {
    if (readingMes != buttonStateMes) {
      buttonStateMes = readingMes;
      if (buttonStateMes == LOW) {
        mes++;
        if (mes > 12) {
          mes = 1;
        }
        atualizarDisplay();
      }
    }
  }
  lastButtonStateMes = readingMes;

  // Verifica se o botão de confirmar foi pressionado
  if (readingConfirmar != lastButtonStateConfirmar) {
    lastDebounceTimeConfirmar = millis();
  }
  if ((millis() - lastDebounceTimeConfirmar) > debounceDelay) {
    if (readingConfirmar != buttonStateConfirmar) {
      buttonStateConfirmar = readingConfirmar;
      if (buttonStateConfirmar == LOW) {
        if (dia == diaCorreto && mes == mesCorreto) {
          lcd.clear();
          lcd.print("Data correta!");
          Serial.println("Data correta!");
          myDFPlayer.play(musica); // Toca a música lida da tag
          lcd.setCursor(0, 1);
          lcd.print("Musica tocando...");

          // Aguarda a música terminar de tocar
          while (myDFPlayer.readType() != DFPlayerPlayFinished) {
            delay(100); // Aguarda um pouco antes de verificar novamente
          }

          reiniciarJogo();
        } else {
          tentativasRestantes--;
          lcd.clear();
          if (tentativasRestantes > 0) {
            lcd.print("Data incorreta!");
            lcd.setCursor(0, 1);
            lcd.print("Tentativas: ");
            lcd.print(tentativasRestantes);
            Serial.print("Data incorreta! Tentativas restantes: ");
            Serial.println(tentativasRestantes);
            delay(2000); // Aguarda 2 segundos antes de permitir nova tentativa
            atualizarDisplay();
          } else {
            lcd.print("Sem tentativas");
            Serial.println("Sem tentativas restantes.");
            delay(2000); // Aguarda 2 segundos antes de reiniciar o jogo
            reiniciarJogo();
          }
        }
      }
    }
  }
  lastButtonStateConfirmar = readingConfirmar;
}

void atualizarDisplay() {
  lcd.clear();
  lcd.print("Dia: ");
  lcd.print(dia);
  lcd.setCursor(0, 1);
  lcd.print("Mes: ");
  lcd.print(mes);
}

void iniciarJogo() {
  Serial.println("Tentando ler a tag...");
  // Verifica se uma nova tag RFID está presente
  if (!mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("Nenhuma tag presente.");
    return;
  }

  // Verifica se conseguimos ler a tag
  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Falha ao ler a tag.");
    return;
  }

  // Define o bloco a ser lido
  byte block = 4;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Tenta ler o bloco
  MFRC522::StatusCode status;
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    lcd.clear();
    lcd.print("Leitura falhou");
    Serial.println("Leitura do bloco falhou");
    delay(2000);
    return;
  }

  // Extrai dia, mês e número da música dos dados lidos
  diaCorreto = buffer[0];
  mesCorreto = buffer[1];
  musica = buffer[2];

  Serial.print("Dia correto: ");
  Serial.print(diaCorreto);
  Serial.print(" Mes correto: ");
  Serial.print(mesCorreto);
  Serial.print(" Música: ");
  Serial.println(musica);

  // Exibe a mensagem de início do jogo
  lcd.clear();
  lcd.print("Adivinhe a data!");
  atualizarDisplay();
  jogoIniciado = true;
  tentativasRestantes = 3;

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

void reiniciarJogo() {
  dia = 1;
  mes = 1;
  jogoIniciado = false;
  tentativasRestantes = 3;
  lcd.clear();
  lcd.print("Aproxime a tag");
  Serial.println("Reiniciando jogo...");
  // Reset the RFID reader
  mfrc522.PCD_Init();
  delay(100); // Breve atraso para garantir a reinicialização completa do leitor RFID
}

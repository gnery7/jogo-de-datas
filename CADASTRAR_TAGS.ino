#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Inicializa o display I2C (endereço, colunas, linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Substitua 0x27 pelo endereço do seu display, se necessário

// Definindo os pinos do RC522
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Criando instância do MFRC522

// Defina a data e música corretas aqui
const int diaCorreto = 31; // Substitua pelo dia desejado
const int mesCorreto = 5; // Substitua pelo mês desejado
const int musica = 10;     // Substitua pelo número da música desejada

void setup() {
  // Inicializa o display I2C
  lcd.init();
  lcd.backlight();
  lcd.print("Aproxime a tag");

  // Inicializa o leitor RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600); // Inicializa comunicação serial para debugging
}

void loop() {
  // Verifica se uma nova tag RFID está presente
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Verifica se conseguimos ler a tag
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Define o bloco a ser escrito
  byte block = 4;

  // Converte o dia, mês e música para bytes
  byte dataBlock[16] = {0};
  dataBlock[0] = diaCorreto;
  dataBlock[1] = mesCorreto;
  dataBlock[2] = musica;

  // Escreve o bloco de dados
  MFRC522::StatusCode status = mfrc522.MIFARE_Write(block, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    lcd.clear();
    lcd.print("Protegido");
    Serial.println(F("Bloco protegido"));
    return;
  }

  // Leitura após a escrita para verificar se os dados foram escritos corretamente
  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    lcd.clear();
    lcd.print("Leitura falhou");
    Serial.println(F("Leitura falhou apos escrita"));
    return;
  }

  // Verifica se os dados foram escritos corretamente
  bool escritaCorreta = true;
  for (byte i = 0; i < 16; i++) {
    if (dataBlock[i] != buffer[i]) {
      escritaCorreta = false;
      break;
    }
  }

  if (escritaCorreta) {
    lcd.clear();
    lcd.print("Escrita OK");
    Serial.println(F("Escrita OK"));
  } else {
    lcd.clear();
    lcd.print("Escrita falhou");
    Serial.println(F("Escrita falhou"));
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  delay(3000);

  lcd.clear();
  lcd.print("Aproxime a tag");
}

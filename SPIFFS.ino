#include <FS.h>
#include "SPIFFS.h"

#define ledPin 33
#define btPin 19
#define filePath "/estado_led.txt"

int btState;
bool ledStatus = false;

void setup() {
  Serial.begin(9600);

  if (SPIFFS.begin()) {
    delay(500);
    Serial.println('\n');
    Serial.println("------SPIFFS inicializado com sucesso------");
  } else {
    Serial.println("Falha ao inicializar SPIFFS.");
  }

  pinMode(btPin, INPUT);
  pinMode(ledPin, OUTPUT);

  if (SPIFFS.exists(filePath)) {
    String arquivo = lerArquivo(filePath);
    ledStatus = (arquivo.toInt() == 1);
    if (ledStatus) {
      Serial.println("LED Ligado!");
      Serial.println('\n');
    } else {
      Serial.println("LED Desligado!");
      Serial.println('\n');
    }
  } else {
    Serial.println("Arquivo não encontrado. Utilizando estado padrão (LED desligado).");
  }

  atualizaLED();  // Atualização do LED com o último estado lido

  delay(1000);  // Atraso para visualização do estado do LED
}

void loop() {
  delay(50);  // Atraso para evitar leituras rápidas do botão
  btState = digitalRead(btPin);

  if (btState) {
    ledStatus = !ledStatus;
    atualizaLED();

    if (ledStatus) {
      escreveArquivo("1", filePath);
      Serial.println("LED Ligado!");
      Serial.println('\n');
    } else {
      escreveArquivo("0", filePath);
      Serial.println("LED Desligado!");
      Serial.println('\n');
    }

    delay(200);
  }
}

void atualizaLED() {
  digitalWrite(ledPin, ledStatus ? HIGH : LOW);
}

String lerArquivo(String path) {
  File rFile = SPIFFS.open(path);
  if (!rFile) {
    Serial.println("Erro ao abrir o arquivo!");
    delay(1000);
    return "0";
  } else {
    String estadoStr = rFile.readString();
    Serial.println('\n');
    Serial.print("Leu: ");
    Serial.println(estadoStr);
    rFile.close();
    return estadoStr;
  }
}

void escreveArquivo(String estado, String path) {
  File rFile = SPIFFS.open(path, FILE_WRITE);
  if (!rFile) {
    Serial.println("Erro ao abrir o arquivo!");
  } else {
    rFile.print(estado + "\n");
    Serial.print("Gravou: ");
    Serial.println(estado);
    rFile.close();
  }
}

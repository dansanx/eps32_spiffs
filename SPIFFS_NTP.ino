#include <FS.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define ledPin 33
#define btPin 19
#define filePath "/estado_hora_led.txt"

int btState;
bool ledStatus = false;

// Definições para configuração do NTP
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600; // Fuso horário do Brasil (BRT - Horário de Brasília)
const int daylightOffset_sec = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

void setup() {
  Serial.begin(9600);

  if (SPIFFS.begin()) {
    Serial.println('\n');
    Serial.println("SPIFFS inicializado com sucesso.");
  } else {
    Serial.println("Falha ao inicializar SPIFFS.");
  }

  // Conectar-se à rede WiFi
  conectarWiFi();

  pinMode(btPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Inicializar cliente NTP
  timeClient.begin();

  // Leitura do último estado do LED gravado no arquivo
  if (SPIFFS.exists(filePath)) {
    String arquivo = lerArquivo(filePath);
    ledStatus = (arquivo.toInt() == 1);
  } else {
    Serial.println("Arquivo não encontrado. Utilizando estado padrão (LED desligado).");
  }

  atualizaLED(); // Atualização do LED com o último estado lido

  delay(1000); // Atraso para visualização do estado do LED
}

void loop() {
  timeClient.update(); // Atualizar cliente NTP

  delay(50); // Atraso para evitar leituras rápidas do botão
  btState = digitalRead(btPin);

  if (btState != (digitalRead(btPin))) { // Verificar mudança de estado do botão
    salvarLog(); // Salvar log ao detectar mudança de estado
  }

  if (btState) {
    ledStatus = !ledStatus;
    atualizaLED();

    if (ledStatus) {
      escreveArquivo("1", filePath);
      Serial.println("LED Ligado!");
    } else {
      escreveArquivo("0", filePath);
      Serial.println("LED Desligado!");
    }

    salvarLog(); // Salvar log após alterar o estado do LED

    delay(200);
  }
}

void conectarWiFi() {
  // Substitua "SSID" e "senha" pelos detalhes da sua rede WiFi
  const char *ssid = "DanHouse";
  const char *senha = "house#391";

  Serial.println("Conectando à rede WiFi...");
  WiFi.begin(ssid, senha);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }

  Serial.println("Conectado à rede WiFi");
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

void salvarLog() {
  // Obter data e hora do servidor NTP
  time_t rawTime = timeClient.getEpochTime();
  struct tm *timeinfo;
  timeinfo = localtime(&rawTime);
  
  // Formatar a data e a hora
  char dataHora[50];
  strftime(dataHora, sizeof(dataHora), "%Y-%m-%d %H:%M:%S", timeinfo);

  // Imprimir data e hora na tela
  //Serial.println("Data e Hora: " + String(dataHora));

  // Abrir o arquivo no modo "anexar" (append)
  File rFile = SPIFFS.open(filePath, FILE_APPEND);

  if (rFile) {
    // Escrever o estado do botão e log NTP no arquivo
    rFile.print(String(btState) + "\n" + dataHora + "\n");
    Serial.println("Log salvo: Estado do botão = " + String(btState) + ", Data e Hora = " + String(dataHora));
    Serial.println('\n');
    rFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo para anexar log.");
  }
}



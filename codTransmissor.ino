#include <esp_now.h>
#include <WiFi.h>

// Substitua pelo endereço MAC do receptor
uint8_t broadcastAddress[] = {0x08, 0xd1, 0xf9, 0x27, 0xda, 0xf0};

#define sensorDeCima 22 // Pino do sensor de cima
#define sensorDeBaixo 23 // Pino do sensor de baixo
#define ledConexao 18
#define ledEnchendo 19
#define ledEsvaziando 21

bool statusCaixa = false;
bool sDeCima;
bool sDeBaixo;
bool isPeerConnected = false;

// Estrutura de dados para enviar
typedef struct struct_message {
  bool a;
} struct_message;

// Cria um struct_message chamado myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback quando os dados são enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus do último pacote enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso na entrega" : "Falha na entrega");
  
  if (status == ESP_NOW_SEND_SUCCESS) {
    piscaLedConexao(true);
  } else {
    piscaLedConexao(false);
    isPeerConnected = false; // Marca como desconectado
  }
}

// Função para tentar adicionar o peer
void addPeer() {
  // Remove o peer primeiro, se já estiver adicionado
  esp_now_del_peer(broadcastAddress);
  
  // Configura as informações do peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.println("Peer adicionado com sucesso");
    isPeerConnected = true;
    piscaLedConexao(true);
  } else {
    Serial.println("Falha ao adicionar peer, tentando novamente...");
    isPeerConnected = false;
    piscaLedConexao(false);
  }
}
 
void setup() {
  // Inicializa o monitor serial
  Serial.begin(115200);
  pinMode(sensorDeCima, INPUT_PULLUP);
  pinMode(sensorDeBaixo, INPUT_PULLUP);
  pinMode(ledConexao, OUTPUT);
  pinMode(ledEnchendo, OUTPUT);
  pinMode(ledEsvaziando, OUTPUT);

  // Configura o dispositivo como uma estação Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inicializa ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  // Registra o callback para o status de envio
  esp_now_register_send_cb(OnDataSent);

  addPeer(); // Adiciona o peer
}
 
void loop() {
  sDeCima = digitalRead(sensorDeCima);
  sDeBaixo = digitalRead(sensorDeBaixo);

  if(!sDeCima && !sDeBaixo && !statusCaixa){
    Serial.println("1 - Esvaziando");
    digitalWrite(ledEsvaziando, HIGH);
    digitalWrite(ledEnchendo, LOW);
    myData.a = 0;
  } else if(sDeCima && !sDeBaixo && !statusCaixa){
    Serial.println("2 - Esvaziando");
    digitalWrite(ledEsvaziando, HIGH);
    digitalWrite(ledEnchendo, LOW);
    myData.a = 0;
  } else if(sDeCima && sDeBaixo && !statusCaixa){
    Serial.println("3 - Vazio");
    digitalWrite(ledEsvaziando, HIGH);
    digitalWrite(ledEnchendo, LOW);
    myData.a = 1;
    statusCaixa = true;
  } else if(sDeCima && sDeBaixo && statusCaixa){
    Serial.println("4 - Enchendo");
    digitalWrite(ledEnchendo, HIGH);
    digitalWrite(ledEsvaziando, LOW);
    myData.a = 1;
  } else if(sDeCima && !sDeBaixo && statusCaixa){
    Serial.println("5 - Enchendo");
    digitalWrite(ledEnchendo, HIGH);
    digitalWrite(ledEsvaziando, LOW);
    myData.a = 1;
  } else if(!sDeCima && !sDeBaixo && statusCaixa){
    Serial.println("6 - Cheio");
    digitalWrite(ledEnchendo, HIGH);
    digitalWrite(ledEsvaziando, LOW);
    myData.a = 0;
    statusCaixa = false;
  } else {
    Serial.println("7 - Erro");
    digitalWrite(ledEnchendo, LOW);
    digitalWrite(ledEsvaziando, LOW);
    myData.a = 0;
  }
 
  // Envia mensagem via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Enviada com sucesso");
  } else {
    Serial.println("Erro ao enviar os dados");
    if (!isPeerConnected) {
      addPeer(); // Tenta adicionar o peer novamente
    }
  }
  delay(1000);

  // Tenta reconectar se o peer não estiver conectado
  if (!isPeerConnected) {
    addPeer();
  }
}

void piscaLedConexao(bool statusLed){
  if(statusLed){
    digitalWrite(ledConexao, HIGH);
  } else {
    for(int i = 0; i < 3; i++) {
      digitalWrite(ledConexao, HIGH);
      delay(500);
      digitalWrite(ledConexao, LOW);
      delay(500);
    }
  }
}

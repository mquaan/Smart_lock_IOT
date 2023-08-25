#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D3, D4);  // RX, TX

const char* ssid = "Shiphuu";
const char* password = "211103sphu";
char receivedChar = '0';

//pin
int relay_pin = 14;   //D5
int buzzer_pin = 12;  //D6

//***Set server***
const char* mqttServer = "broker.hivemq.com";
int port = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* host = "maker.ifttt.com";
const char* request_sos = "/trigger/sos/with/key/dziiS-eaWT58pIL2qhbB4n";

void sendRequest() {
  WiFiClient client;
  while (!client.connect(host, 80)) {
    Serial.println("connection fail");
    delay(1000);
  }
  client.print(String("GET ") + request_sos + " HTTP/1.1\r\n"
               + "Host: " + host + "\r\n"
               + "Connection: close\r\n\r\n");
  delay(500);

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}


// Connect Wifi
void wifiConnect() {
  WiFi.begin(ssid, password);
  // Waiting until the connection is successful
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi Connected!");
  mySerial.print("Wifi");
}

// Connect  MQTT
void mqttConnect() {
  while (!mqttClient.connected()) {
    Serial.println("Attemping MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connected");
      //***Subscribe all topic you need***
      mqttClient.subscribe("04_Dynamic_pass");
      mqttClient.subscribe("04_login_remote");

      String pass = "1234";
      mqttClient.publish("04_admin_password", pass.c_str());
    } else {
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

//MQTT Receiver
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println(topic);
  String strMsg;
  for (int i = 0; i < length; i++) {
    strMsg += (char)message[i];
  }
  if (strMsg == "Remote") {  // unlock by remote
    mySerial.print("Remote");
    soundCorrect(buzzer_pin);
    digitalWrite(relay_pin, HIGH);
    delay(4000);
    digitalWrite(relay_pin, LOW);
  } else {  // dynamic pass
    mySerial.print(strMsg);
    Serial.print(strMsg);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relay_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  mySerial.begin(9600);
  // We start by connecting to a Wifi network
  Serial.println();
  Serial.print("Connecting to WiFi");
  wifiConnect();

  // MQTT
  mqttClient.setServer(mqttServer, port);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive(90);
}

void soundCorrect(int pin) {
  tone(pin, 2048);
  delay(90);
  noTone(pin);
}

void soundWrong(int pin) {
  tone(pin, 400);
  delay(100);
  noTone(pin);
}

void soundSOS(int pin) {
  for (int i = 0; i < 10; i++) {
    tone(pin, 2548);
    delay(350);
    noTone(pin);
    delay(10);
  }
}

void handle_serial(char receivedChar) {
  if (receivedChar == '7') {  // button
    digitalWrite(relay_pin, HIGH);
    delay(4000);
    digitalWrite(relay_pin, LOW);
  } else if (receivedChar == '1') {  // keypad
    soundCorrect(buzzer_pin);
    digitalWrite(relay_pin, HIGH);
    while (true) {
      if (mySerial.available()) {
        String receivedString = mySerial.readString();
        mqttClient.publish("04_Keypad", receivedString.c_str());
        Serial.print(receivedString);
        break;
      }
    }
    delay(4000);
    digitalWrite(relay_pin, LOW);
  } else if (receivedChar == '2') {  // fingerprint
    soundCorrect(buzzer_pin);
    digitalWrite(relay_pin, HIGH);
    while (true) {
      if (mySerial.available()) {
        String receivedString = mySerial.readString();
        mqttClient.publish("04_Fingerprint", receivedString.c_str());
        Serial.print(receivedString);
        break;
      }
    }
    delay(4000);
    digitalWrite(relay_pin, LOW);
  } else if (receivedChar == '3') {  // dynamic pass
    soundCorrect(buzzer_pin);
    digitalWrite(relay_pin, HIGH);
    String data = "1";
    mqttClient.publish("04_Dynamic_password", data.c_str());
    delay(4000);
    digitalWrite(relay_pin, LOW);
  } else if (receivedChar == '4') {  // sai mk
    soundWrong(buzzer_pin);
  } else if (receivedChar == '5') {  // sai mk 5 lan
    sendRequest();
    soundSOS(buzzer_pin);
  } else if (receivedChar == '6') {  // truyen admin pass
    while (true) {
      if (mySerial.available()) {
        String receivedString = mySerial.readString();
        mqttClient.publish("04_admin_password", receivedString.c_str());
        break;
      }
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    mqttConnect();
  }
  mqttClient.loop();
  if (mySerial.available()) {
    receivedChar = mySerial.read();  // Đọc dữ liệu gửi từ uno
    handle_serial(receivedChar);
    receivedChar = '0';
  }
}
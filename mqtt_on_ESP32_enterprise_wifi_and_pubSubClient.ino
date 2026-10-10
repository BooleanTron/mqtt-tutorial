#include <WiFi.h>
#include <PubSubClient.h>

// =====================================================
// Wi-Fi Configuration
// =====================================================
const char* ssid     = "Student";
const char* username = "YOUR_NET_ID";
const char* password = "YOUR_PASSWORD_WIFI";


// =====================================================
// MQTT Configuration
// =====================================================
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;

const char* subscribe_topic = "test/subscribe";
const char* publish_topic   = "test/publish";


// =====================================================
// Network / MQTT Client
// =====================================================
WiFiClient espClient;
PubSubClient client(espClient);


// =====================================================
// Wi-Fi Connection
// =====================================================
void connectWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(1000);

  // WPA2-Enterprise PEAP authentication
  WiFi.begin(
    ssid,
    WPA2_AUTH_PEAP,
    username,
    username,
    password
  );

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


// =====================================================
// MQTT Message Callback
// =====================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


// =====================================================
// MQTT Connection
// =====================================================
void connectMQTT() {

  while (!client.connected()) {

    Serial.print("Connecting to MQTT... ");

    // No MQTT username/password required
    if (client.connect("ESP32")) {

      Serial.println("connected");

      client.subscribe(subscribe_topic);
      Serial.print("Subscribed to: ");
      Serial.println(subscribe_topic);

    } else {

      Serial.print("failed, rc=");
      Serial.println(client.state());

      delay(5000);
    }
  }
}


// =====================================================
// Setup
// =====================================================
void setup() {

  Serial.begin(115200);

  connectWiFi();

  // Configure MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  connectMQTT();

  randomSeed(analogRead(0));
}


// =====================================================
// Main Loop
// =====================================================
const unsigned long publish_interval = 3000;
unsigned long last_publish_time = 0;

void loop() {

  // Reconnect if MQTT connection is lost
  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  // Publish every 3 seconds
  if (millis() - last_publish_time >= publish_interval) {
    last_publish_time = millis();

    float temperature = random(200, 351) / 10.0;  // 20.0 - 35.0 °C
    float humidity     = random(300, 801) / 10.0;  // 30.0 - 80.0 %

    char payload[80];
    snprintf(
      payload, sizeof(payload),
      "{\"temperature\":%.1f,\"humidity\":%.1f}",
      temperature, humidity
    );

    client.publish(publish_topic, payload);

    Serial.print("Published: ");
    Serial.println(payload);
  }
}

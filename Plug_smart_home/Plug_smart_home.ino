#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;

#define ResetPin D5

bool MQTT_STT = false;
// MQTT SERVER

const char *mqtt_server = "broker.hivemq.com";

char *MAC = "";
char *topic_in = "/v1/device/NTQ/"+MAC+"/telemetry" 
char *topic_out = "/v1/device/NTQ/"+MAC+"/request/+"

//========================= FUnction ============================


void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  Serial.print((char)payload[0]);
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
    Serial.print("Turn on led\n");
    // but actually the LED is on; this is because

    // it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
    Serial.print("Turn off led\n");
  }
}
//void mqttReconnect() {
//    // reconnect code from PubSubClient example
//}

void mqttReconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "clientId-jgJEbXcFr1";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //    const char* id = "ESP";
    //    const char* user = "xqdufvgb";
    //    const char* passw = " 5RNJC91AjlAv";
    if (client.connect(clientId.c_str()))
    {
      //if (client.connect(id, user, passw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("TNQ_MQTT");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
// Check button reset
int reset_stt_old = 1;
int reset_stt_new = 1;


void Check_reset() {
  //  Serial.println("Check_reset");
  reset_stt_new = digitalRead(ResetPin);
  //  Serial.println(reset_stt_new);
  if (reset_stt_new == 0 && reset_stt_old == 1 ) {
    Serial.println("//============== > RESET MODE < ============//");
    MQTT_STT = false;

    delay(1000);
    if (reset_stt_new == 0 && reset_stt_old == 1 ) {
      delay(1000);
      reset_stt_new = digitalRead(ResetPin);
      Serial.println(reset_stt_new);
      if (reset_stt_new == 0 && reset_stt_old == 1 ) {
        delay(1000);
        reset_stt_new = digitalRead(ResetPin);
        Serial.println(reset_stt_new);
        if (reset_stt_new == 0 && reset_stt_old == 1 ) {
          delay(1000);
          reset_stt_new = digitalRead(ResetPin);
          Serial.println(reset_stt_new);
          if (reset_stt_new == 0 && reset_stt_old == 1 ) {
            delay(1000);
            reset_stt_new = digitalRead(ResetPin);
            Serial.println(reset_stt_new);
            if (reset_stt_new == 0 && reset_stt_old == 1 ) {
              wifiManager.resetSettings();
              reset_stt_old = reset_stt_new;
              Serial.println("//==============< RESET DONE >============//");
              ESP.restart();
            }
          }
        }
      }

    }

  }
}

//// Cho den nhay chop  Them ham nay trong thu vien WiFiManager.h
//unsigned long time1 = 0;
//
//void DisMode_Setup(){
//  if ( (unsigned long) (millis() - time1) > 1000 )
//    {
//        if ( digitalRead(BUILTIN_LED) == LOW )
//        {
//            digitalWrite(BUILTIN_LED, HIGH);
//        } else {
//            digitalWrite(BUILTIN_LED, LOW );
//        }
//        time1 = millis();
//    }
//}
void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(ResetPin, INPUT_PULLUP);

  wifiManager.setTimeout(180);

  if (!wifiManager.autoConnect("Plug_SmartHome_HAIMD")) {
    MQTT_STT = false;
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  } else {
    MQTT_STT = true;
  }

  Serial.println("=== > Connected ! Let's go :)");

  client.setServer("broker.hivemq.com", 1883);
  client.setCallback(mqttCallback);
}

void loop() {
  if (MQTT_STT == true) {
    if (!client.connected()) {
      mqttReconnect();
    }
    client.loop();
  }
  Check_reset(); // Check button RESET
  yield();
}

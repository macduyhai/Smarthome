#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJWT.h>

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;

#define ResetPin D13

#define Button_1 D12
#define Button_2 D11

#define Relay_1 D15
#define Relay_2 D14


bool stt_bt1 = false;
bool stt_bt2 = false;

char *key = "lvJvDWKiv0";

// {"key":"lvJvDWKiv0","mac":"asdsaffsasf"}

bool MQTT_STT = false;
// MQTT SERVER

const char *mqtt_server = "broker.hivemq.com";

String MAC = "";
String topic_in = "/v1/devices/NTQ/" + MAC + "/telemetry";
String topic_out = "/v1/devices/NTQ/" + MAC + "/request/+";

//StaticJsonBuffer<200> jsonBuffer;

// ==================== Check button ============================


//========================= FUnction ============================
void UpdateStt(String &topicOut)
{
  String msg = "{\"update_stt\":\"1\"}";
  //String EncodeJWT(msg);
  char charBuf_out[50];
  topicOut.toCharArray(charBuf_out, 50);
  //  client.publish(charBuf_out, msg);
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String json_str = "";
  for (int i = 0; i < length; i++)
  {
    //Serial.print((char)payload[i]);
    json_str += (char)payload[i];
  }
  Serial.println(json_str);
  Serial.println();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &msg = jsonBuffer.parseObject(json_str);
  //  Serial.printf(msg));
  if (!msg.success())
  {
    Serial.println("parseObject() failed");
    //return false;
  }
  else
  {
    Serial.print("ParseObject() Okie ");

  }
  if (msg["id"] == 1)
  {
    if (msg["value"] == "1")
    {
      if (stt_bt1 == false) {
        digitalWrite(Relay_1, HIGH);
        Serial.println("Bat den 1");
      } else {
        Serial.println("Đen 1 dang bat");
      }
      stt_bt1 = true;

    }
    else
    {
      if (stt_bt1 == true) {
        digitalWrite(Relay_1, LOW);
        Serial.println("Tat den 1");
      } else {
        Serial.println("Đèn 1 đang tắt");
      }
      stt_bt1 = false;

    }
  }
  else if (msg["id"] == 2)
  {
    if (msg["value"] == "1")
    {
      if (stt_bt2 == false) {
        digitalWrite(Relay_2, HIGH);
        Serial.println("Bat den 2");
      } else {
        Serial.println("Đèn 2 đang bat");
      }
      stt_bt2 = true;

    }
    else
    {
      if (stt_bt2 == true) {
        digitalWrite(Relay_2, LOW);
        Serial.println("Tat den 2");
      } else {
        Serial.println("Đèn 2 đang tat");
      }
      stt_bt2 = false;

    }
  }
  else if ((char)payload[0] == '1')
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
      Serial.print("Topic in subcriber: ");
      Serial.println(topic_in);
      Serial.print("Topic in publisher: ");
      Serial.println(topic_out);
      // Once connected, publish an announcement...
      //      client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("TNQ_MQTT");
      char charBuf[50];
      topic_in.toCharArray(charBuf, 50);
      Serial.println(charBuf);
      client.subscribe(charBuf);
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

void Check_reset()
{
  //  Serial.println("Check_reset");
  reset_stt_new = digitalRead(ResetPin);
  //  Serial.println(reset_stt_new);
  if (reset_stt_new == 0 && reset_stt_old == 1)
  {
    Serial.println("//============== > RESET MODE < ============//");
    MQTT_STT = false;

    delay(500);
    if (reset_stt_new == 0 && reset_stt_old == 1)
    {
      delay(500);
      reset_stt_new = digitalRead(ResetPin);
      Serial.println(reset_stt_new);
      if (reset_stt_new == 0 && reset_stt_old == 1)
      {
        delay(500);
        reset_stt_new = digitalRead(ResetPin);
        Serial.println(reset_stt_new);
        if (reset_stt_new == 0 && reset_stt_old == 1)
        {
          delay(500);
          reset_stt_new = digitalRead(ResetPin);
          Serial.println(reset_stt_new);
          if (reset_stt_new == 0 && reset_stt_old == 1)
          {
            delay(500);
            reset_stt_new = digitalRead(ResetPin);
            Serial.println(reset_stt_new);
            if (reset_stt_new == 0 && reset_stt_old == 1)
            {
              wifiManager.resetSettings();
              reset_stt_old = reset_stt_new;
              Serial.println("//==============< RESET DONE >============//");
              ESP.restart();
            } else {
              MQTT_STT = true;
            }
          } else {
            MQTT_STT = true;;
          }
        } else {
          MQTT_STT = true;;
        }
      } else {
        MQTT_STT = true;;
      }
    } else {
      MQTT_STT = true;;
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

bool State_B1_old = true;
bool State_B1_new = true;
bool State_B2_old = true;
bool State_B2_new = true;

void Handler_button1() {
  State_B1_new = digitalRead(Button_1);

  if (State_B1_new != State_B1_old) {
    delay(5);
    State_B1_new = digitalRead(Button_1);
    if (State_B1_new != State_B1_old) {
      if (stt_bt1 == false)
      {
        digitalWrite(Relay_1, HIGH);
        Serial.println("Bat den 1-->Handler_button1() ");
        stt_bt1 = true;
      }
      else
      {
        digitalWrite(Relay_1, LOW);
        Serial.println("Tat den 1---> Handler_button1()");
        stt_bt1 = false;
      }
      State_B1_old = State_B1_new;
    } else {
      State_B1_old = State_B1_new;
    }
  } else {
    State_B1_old = State_B1_new;
  }
}

void Handler_button2()
{
  State_B2_new = digitalRead(Button_2);

  if (State_B2_new != State_B2_old) {
    delay(5);
    State_B2_new = digitalRead(Button_2);
    if (State_B2_new != State_B2_old) {
      if (stt_bt2 == false)
      {
        digitalWrite(Relay_2, HIGH);
        Serial.println("Bat den 2-->Handler_button2() ");
        stt_bt2 = true;
      }
      else
      {
        digitalWrite(Relay_2, LOW);
        Serial.println("Tat den 2---> Handler_button2()");
        stt_bt2 = false;
      }
      State_B2_old = State_B2_new;
    } else {
      State_B2_old = State_B2_new;
    }
  } else {
    State_B2_old = State_B2_new;
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  pinMode(ResetPin, INPUT_PULLUP);
  pinMode(Button_1, INPUT_PULLUP);
  pinMode(Button_2, INPUT_PULLUP);
  pinMode(Relay_1, OUTPUT);
  pinMode(Relay_2, OUTPUT);

  //   WIFI
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  MAC = WiFi.macAddress();
  MAC.replace(":", "");
  topic_in = "/v1/devices/NTQ/" + MAC + "/telemetry";
  topic_out = "/v1/devices/NTQ/" + MAC + "/request/";
  Serial.println(MAC);
  Serial.println("Cac thong so cua wifi sau khi ket noi");
  Serial.println(WiFi.status());
  Serial.println("-------------------");

  wifiManager.setTimeout(180);

  if (!wifiManager.autoConnect("Plug_SmartHome_HAIMD"))
  {
    MQTT_STT = false;
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  else
  {
    MQTT_STT = true;
  }

  Serial.println("=== > Connected ! Let's go :)");

  client.setServer("broker.hivemq.com", 1883);
  client.setCallback(mqttCallback);
}

void loop()
{
  if (MQTT_STT == true)
  {
    if (!client.connected())
    {
      mqttReconnect();
    }
    client.loop();
  }
  Check_reset(); // Check button RESET
  Handler_button1();
  Handler_button2();
  yield();
}

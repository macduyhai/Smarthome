#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJWT.h>

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;

#define ResetPin D5

#define Button_1 D11
#define Button_2 D12
#define Relay_1 D1
#define Relay_2 D7


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

StaticJsonBuffer<200> jsonBuffer;

//char *MAC="";
//char *topic_in = "/v1/device/NTQ/"+MAC+"/telemetry" ;
//char *topic_out = "/v1/device/NTQ/"+MAC+"/request/";
//
String dev_key = "eNhomKou0CMJ694nK281vghbb6UtIQB2";
//String message = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
String msg = "{\"sensor\":\"gps\",\"time\":1351824120}";

ArduinoJWT jwt = ArduinoJWT(dev_key);
// ==================== Check button ============================

//=========================== JWT ===============================
String DecodeJWT(String &message)
{

  // The original Message
  Serial.println();
  Serial.println("1) message: ");
  Serial.println(message);
  Serial.println();
  // JWT - Decode a JWT and retreive the payload
  String MsgDecode = "x";
  for (int i = 0; i < message.length(); i++)
    MsgDecode[i] += 'x';
  boolean decodedToken = jwt.decodeJWT(message, MsgDecode);
  Serial.println();
  //jwt.decodeJWT(encodedJWT, MsgDecode);
  Serial.println("5) MsgDecode: ");
  Serial.println(MsgDecode);
  return MsgDecode;
}

String EncodeJWT(String &message)
{

  // The original Message
  Serial.println();
  Serial.println("1) message: ");
  Serial.println(message);
  Serial.println();
  //delay(1000);

  // JWT - Encode a JSON Web Token
  String encodedJWT = jwt.encodeJWT(message);
  Serial.println("2) encodedJWT: ");
  //Serial.println(encodedJWT);
  //delay(1000);
  return encodedJWT;
  //
  //    // Get the calculated length of a JWT
  //    int encodedJWTLength = jwt.getJWTLength(encodedJWT);
  //    Serial.println("3) encodedJWTLength: ");
  //    Serial.println(encodedJWTLength);
  //    Serial.println();
  //    //delay(1000);
  //
  //    // Get the length of the decoded payload from a JWT
  //    int encodedJWTPayloadLength = jwt.getJWTPayloadLength(encodedJWT);
  //    Serial.println("4) encodedJWTPayloadLength: ");
  //    Serial.println(encodedJWTPayloadLength);
  //    //delay(1000);
  //
  //    // JWT - Decode a JWT and retreive the payload
  //    String testDecoded = "x";
  //    for (int i = 0 ; i < message.length() ; i++) testDecoded[i] += 'x';
  //    boolean decodedToken = jwt.decodeJWT(encodedJWT, testDecoded);
  //    Serial.println();
  //
  //    //jwt.decodeJWT(encodedJWT, decodedPayload);
  //    Serial.println("5) testDecoded: ");
  //    Serial.println(testDecoded);
  //    delay(5000);
}

//========================= FUnction ============================
void UpdateStt(String &topicOut)
{
  String msg = "{\"update_stt\":\"1\"}";
  //String EncodeJWT(msg);
  char charBuf_out[50];
  topicOut.toCharArray(charBuf_out, 50);
  //  client.publish(charBuf_out, msg);
}
bool stt_relay1_old = false;
bool stt_relay1_new = false;
bool stt_relay2_old = false;
bool stt_relay2_new = false;

void ControlDevice()
{
  stt_relay1_new = stt_bt1;
  stt_relay2_new = stt_bt2;

  if (stt_bt1 == true)
  {
    if (stt_relay1_new == true && stt_relay1_old == false) {
      digitalWrite(Relay_1, HIGH);
      Serial.println("Bat den 1");
    }
    //    Serial.println("Bat den 1");
  }
  else
  {
    if (stt_relay1_new == false && stt_relay1_old == true) {
      digitalWrite(Relay_1, LOW);
      Serial.println("Tat den 1");
    }

    //    Serial.println("Tat den 1");
  }
  if (stt_bt2 == true)
  {
    if (stt_relay2_new == true && stt_relay2_old == false) {
      digitalWrite(Relay_2, HIGH);
      Serial.println("Bat den 2");
    }
    //    Serial.println("Bat den 2");
  }
  else
  {
    if (stt_relay2_new == false && stt_relay2_old == true) {
      digitalWrite(Relay_2, LOW);
      Serial.println("Tat den 2");
    }
    //    Serial.println("Tat den 2");
  }
  stt_relay1_old = stt_relay1_new;
  stt_relay2_old = stt_relay2_new;
}
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String json_str = "";
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    json_str += (char)payload[i];
  }
  Serial.println(json_str);
  Serial.println();
  JsonObject &msg = jsonBuffer.parseObject(json_str);
  if (!msg.success())
  {
    Serial.println("parseObject() failed");
    //return false;
  }
  else
  {
    Serial.print("ParseObject() Okie, Payload: ");

  }
  if (msg["id"] == 1)
  {
    if (msg["value"] == "1")
    {
      stt_bt1 == true;
    }
    else
    {
      stt_bt1 = false;
    }
  }
  else if (msg["id"] == 2)
  {
    if (msg["value"] == "0")
    {
      stt_bt2 == true;
    }
    else
    {
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

    delay(1000);
    if (reset_stt_new == 0 && reset_stt_old == 1)
    {
      delay(1000);
      reset_stt_new = digitalRead(ResetPin);
      Serial.println(reset_stt_new);
      if (reset_stt_new == 0 && reset_stt_old == 1)
      {
        delay(1000);
        reset_stt_new = digitalRead(ResetPin);
        Serial.println(reset_stt_new);
        if (reset_stt_new == 0 && reset_stt_old == 1)
        {
          delay(1000);
          reset_stt_new = digitalRead(ResetPin);
          Serial.println(reset_stt_new);
          if (reset_stt_new == 0 && reset_stt_old == 1)
          {
            delay(1000);
            reset_stt_new = digitalRead(ResetPin);
            Serial.println(reset_stt_new);
            if (reset_stt_new == 0 && reset_stt_old == 1)
            {
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

void Handler_button1()
{
  if (stt_bt1 == false)
  {
    stt_bt1 = true;
  }
  else
  {
    stt_bt1 = false;
  }
}
void Handler_button2()
{
  if (stt_bt2 == false)
  {
    stt_bt2 = true;
  }
  else
  {
    stt_bt2 = false;
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  pinMode(ResetPin, INPUT_PULLUP);
  
  //  pinMode(Button_1, INPUT_PULLUP);
  //  pinMode(Button_2, INPUT_PULLUP);
  //  attachInterrupt(digitalPinToInterrupt(Button_1), Handler_button1, CHANGE);
  //  attachInterrupt(digitalPinToInterrupt(Button_2), Handler_button2, CHANGE);
//  delay(5000);
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

  pinMode(Relay_1, OUTPUT);
  pinMode(Relay_2, OUTPUT);


  //String x = EncodeJWT(msg);
  //Serial.println(x);
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
//  ControlDevice();
  yield();
}

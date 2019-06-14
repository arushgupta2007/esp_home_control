//#include <StaticThreadController.h>
//#include <Thread.h>
//#include <ThreadController.h>
#include <ACS712.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//#define FIREBASE_HOST "fireapp-6fd92.firebaseio.com"
//#define FIREBASE_AUTH "eI7Nn1DvTkaTfDhUuNpATgzaeDuBDRSFMxcFPgbo"
//#define FCM_SERVER_KEY "AAAAXefIZYk:APA91bGTht1SigrealJbUmpaU3BBZW5G7xCd2CqkRgy4cqvfIMw_e4fi6ca0TU_GzB8Wv8pyEjNurApRO6gg2t8GKIZ4ujunVOcAEj0qA3cYIqQezv9eeeJc07uqdzqpuXOtFIqvFseo"
//#define WIFI_SSID "Alok"
//#define WIFI_PASSWORD "alok1970"

unsigned long previousMillis = 0;
const long interval = 1000;

int delayto = 2000;
bool has_changed = false;
String path = "";
int till = 0;
int debug = 0;

FirebaseData firebaseData;
FirebaseData firebaseDatadiffrent_obj;
DynamicJsonBuffer json_buffer;
ACS712 sensor(ACS712_05B, A0);
//Thread serialThread = Thread();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin("Alok", "alok1970");
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);

  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(D2, INPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  sensor.calibrate();
  
  attachInterrupt(digitalPinToInterrupt(D2), zcd, FALLING);

//  serialThread.onRun(SerialStuff);
//  serialThread.setInterval(1000);

  Firebase.begin("fireapp-6fd92.firebaseio.com", "eI7Nn1DvTkaTfDhUuNpATgzaeDuBDRSFMxcFPgbo");
  Firebase.reconnectWiFi(true);

  //  firebaseData.fcm.begin(FCM_SERVER_KEY);
  //  firebaseData.fcm.setPriority("high");
  //  firebaseData.fcm.setTimeToLive(60);
  //  firebaseData.fcm.setNotifyMessage("Notification", "Hello Bye See You Dont see you", "firebase-logo.png", "http://www.google.com");
  //  firebaseData.fcm.setTopic("general");
  //
  //  if (Firebase.sendTopic(firebaseDxata))
  //  {
  //    //Success, print the result returned from server
  //    Serial.println(firebaseData.fcm.getSendResult());
  //  }
  //  else
  //  {
  //    //Failed, print the error reason
  //    Serial.println(firebaseData.errorReason());
  //  }

  if (Firebase.getString(firebaseData, "/arushpass/device_path/esp{2615}")) {
    path = String("/arushpass" + firebaseData.stringData());
    //    Serial.println(path);
  }

  bool donestream = !Firebase.beginStream(firebaseData, String(path + "/controls"));
  while (donestream) {
    Serial.println(firebaseData.errorReason());
    donestream = !Firebase.beginStream(firebaseData, path + "/controls");

  }
  wifi_set_sleep_type(NONE_SLEEP_T);
}

void loop() {
  // put your main code here, to run repeatedly:
  //  yield();
//  if (serialThread.shouldRun()) {
//    serialThread.run();
//  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    SerialStuff();
  }
  if (!Firebase.readStream(firebaseData))
  {
    yield();
    Serial.println("Can't read stream data");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }

  if (firebaseData.streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();

    Firebase.endStream(firebaseData);
    if (!Firebase.beginStream(firebaseData, String(path + "/controls")))
    {
      Serial.println("Could not begin stream");
    }
  }

  if (firebaseData.streamAvailable())
  {
    Serial.println("------------------------------------");
    Serial.println("Stream Data Available...");
    Serial.println("STREAM PATH: " + firebaseData.streamPath());
    Serial.println("EVENT PATH: " + firebaseData.dataPath());
    Serial.println("DATA TYPE: " + firebaseData.dataType());
    Serial.println("EVENT TYPE: " + firebaseData.eventType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "boolean") {
      has_changed = !has_changed;
      Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
      Serial.println("------------------------------------");
    } else if (firebaseData.dataType() == "int") {
      if (!firebaseData.dataPath().substring(1).equals("watts_used")) {
        Serial.println(firebaseData.intData());
        Serial.println("------------------------------------");
        detachInterrupt(digitalPinToInterrupt(D2));
        delayto = map(firebaseData.intData(), 1, 100, 9000, 2000);
        attachInterrupt(digitalPinToInterrupt(D2), zcd, FALLING);
      }
    }
  }
}

void zcd() {
//  if (debug >= 50) {
//    Serial.println("50 hz");
//    debug = 0;
//  }
//  debug += 1;
  if (has_changed) {
    delayMicroseconds(delayto);
    digitalWrite(D8, HIGH);
    delayMicroseconds(50);
    digitalWrite(D8, LOW);
  } else {
    delayMicroseconds(delayto);
    digitalWrite(D7, HIGH);
    delayMicroseconds(50);
    digitalWrite(D7, LOW);
  }
}

void SerialStuff() {
  float amps = sensor.getCurrentAC() - 0.01;
  Serial.println(amps);
  if (amps <= 0.04 && amps > 0.0) {
    Firebase.setBool(firebaseDatadiffrent_obj, String(path + "/status"), false);
  } else if (amps > 0.04) {
    if (Firebase.getFloat(firebaseDatadiffrent_obj, String(path + "/watts_used"))) {
      if (till >= 10) {
        float watts_to_be_updated = firebaseDatadiffrent_obj.floatData() + ((amps * 230) / 360);
        Firebase.setFloat(firebaseDatadiffrent_obj, String(path + "/watts_used"), watts_to_be_updated);
        till = 0;
      }
      till += 1;
      Firebase.setBool(firebaseDatadiffrent_obj, String(path + "/status"), true);
//      delay(1);
    }
  }
}

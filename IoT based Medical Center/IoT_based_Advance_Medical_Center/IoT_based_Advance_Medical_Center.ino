#include <WiFiManager.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <time.h>
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 5 * 3600;
const int   daylightOffset_sec = 3600;
#include <TFT_eSPI.h>
#include <MFRC522.h>
#include <color.h>
#include <TJpg_Decoder.h>
#include <FS.h>
#include "SPIFFS.h"

// Links
#define FIREBASE_HOST "https://clients-4b500-default-rtdb.firebaseio.com/" // Firebase HOST
#define FIREBASE_AUTH "6YSK32KbwLHkY6weAKu4kNIIPklhsZwy1dl6Iplz"           // Firebase AUTH
FirebaseData firebaseData;
FirebaseJson json;


MFRC522 rfid(5, 21);
TFT_eSPI tft = TFT_eSPI();


// Picture name
#define RFID "/rfid.jpg"
#define Halimabig "/docone.jpg"
#define Halimasmall "/docones.jpg"
#define Jibonbig "/doctwo.jpg"
#define Jibonsmall "/doctwos.jpg"
#define Fatemasmall "/docthrees.jpg"
#define Fatemabig "/docthree.jpg"


// font
#define FF18 &FreeSans12pt7b
#define CF_OL24 &Orbitron_Light_24
#define CF_OL32 &Orbitron_Light_32
#define CF_RT24 &Roboto_Thin_24
#define CF_S24  &Satisfy_24
#define CF_Y32  &Yellowtail_32

// variables name
uint32_t current_time;
uint32_t pageTime = 0;
int pageNumber = 0;
bool doctorOne = false, doctorTwo = false, doctorThree = false;
String ack = "false";

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

int doctor_A_patient_counter = 0, doctor_B_patient_counter = 0, doctor_C_patient_counter = 0;
String doctor_A_patient_list = "", doctor_B_patient_list = "", doctor_C_patient_list = "";
int doctor_A_max = 30, doctor_B_max = 30, doctor_C_max = 30;
String req_name = "", req_id = "";
String req_doct = "";
int req_doc;
String doctors_name[3] = {"Dr_Halima_Khatun", "Dr_Jibon_Krishna", "Dr_Fatema_Khatun"};
String docOne = "", docTwo = "", docThree = "";
bool doctorA_present = false, doctorB_present = false, doctorC_present = false;
int activepatientA = 0, activepatientB = 0, activepatientC = 0;
bool PatientLeftA = true, PatientLeftB = true, PatientLeftC = true;

//buttons
int Up = 27, Middle = 26, Down = 35;
int doctorAbutton = 36, doctorBbutton = 39, doctorCbutton = 34;
int roomA_light = 22, roomB_light = 32, roomC_light = 33;
int indicatorA = 4, indicatorB = 16, indicatorC = 25;

struct patient_details {
  String Pname = "";
  String id = "";
  int Serial_number = -1;
  String approx_time = "";
  bool complete = false;
  int active = 0;
};

struct doctor_details {
  int maximum_patient = 30;
  int patient_number = 0;
  int start_time = 1020;
  patient_details patient[100];
};

doctor_details doctor[3];


void setup() {
  Serial.begin(9600);
  // inilize the SPIFFS
  if (!SPIFFS.begin()) {
    while (1) yield();
  }
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  // button initilize
  pinMode(Up, INPUT);
  pinMode(Down, INPUT);
  pinMode(Middle, INPUT);
  pinMode(doctorAbutton, INPUT);
  pinMode(doctorBbutton, INPUT);
  pinMode(doctorCbutton, INPUT);
  // light initilize
  pinMode(roomA_light, OUTPUT);
  pinMode(roomB_light, OUTPUT);
  pinMode(roomC_light, OUTPUT);
  pinMode(indicatorA, OUTPUT);
  pinMode(indicatorB, OUTPUT);
  pinMode(indicatorC, OUTPUT);

  tft.setCursor(tft.width() / 2 - 50, tft.height() / 2);
  tft.fillScreen(TFT_BLUE);            // Clear screen
  tft.setTextColor(BLACK);
  tft.print("<< CONNECTING >>");

  // Wifi Router Connecting setup......
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("MedicalCenter");


  if (res) {
    tft.setCursor(tft.width() / 2 - 30, tft.height() / 2);
    tft.fillScreen(GREEN);            // Clear screen
    tft.print("CONNECTED");
    delay(2000);

  } else {
    tft.setCursor(tft.width() / 2 - 20, tft.height() / 2);
    tft.fillScreen(RED);            // Clear screen
    tft.print("FAILED");
    delay(200);
  }
  // -----Connection End------------//

  tft.fillScreen(WHITE);
  tft.setTextColor(BLACK);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  tft.setFreeFont();

  // FIREBASE
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");


  //----------------------------------
  SPI.begin();
  rfid.PCD_Init();

  current_time = millis();
  pageTime = millis();

  if (Firebase.ready()) {
    Firebase.deleteNode(firebaseData, "/IoT_Medical_Center/201513522");
  }

  //B3 4D A7 1E
  //F3 13 82 1E
  //12 AE 55 22
}


char command;


void loop() {
  localTime();
  if (digitalRead(Middle)) {
    tft.fillScreen(WHITE);
    CheckCommand();
  }

  if (digitalRead(doctorAbutton) && doctorA_present) {
    FollowDoctorOne();
  }

  if (digitalRead(doctorBbutton) && doctorB_present) {
    FollowDoctorTwo();
  }

  if (digitalRead(doctorCbutton) && doctorC_present) {
    FollowDoctorThree();
  }

  if (current_time + 5000 < millis()) {
    if (Firebase.ready()) {
      Firebase.getString(firebaseData, "/IoT_Medical_Center/ack");
      ack = firebaseData.to<const char *>();
      if (ack == "true") {
        // take the Request ID
        Firebase.getString(firebaseData, "/IoT_Medical_Center/req_id");
        req_id = firebaseData.to<const char *>();
        req_id.replace("\"", "");
        req_id.replace("\\", "");
        // Take thre Request Name
        Firebase.getString(firebaseData,  "/IoT_Medical_Center/req_name");
        req_name = firebaseData.to<const char *>();
        req_name.replace("\"", "");
        req_name.replace("\\", "");
        // TAke the Request Doctor
        Firebase.getString(firebaseData,  "/IoT_Medical_Center/req_doc");
        req_doct = firebaseData.to<const char *>();
        req_doc = req_doct.toInt();


        if (req_doc == 0) {
          if (doctor[0].patient_number < doctor[0].maximum_patient) {
            doctor[0].patient_number = doctor_A_patient_counter;
            doctor[0].patient[doctor_A_patient_counter].Pname = req_name;
            doctor[0].patient[doctor_A_patient_counter].id = req_id;

            Firebase.setInt(firebaseData, "/IoT_Medical_Center/" + req_id + "/serial" , doctor_A_patient_counter + 1);
            int calculated_time = doctor[0].start_time + (15 * doctor_A_patient_counter);
            doctor[0].patient[doctor_A_patient_counter].approx_time = String((calculated_time / 60)) + "~" + String((calculated_time % 60));
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/time" , doctor[0].patient[doctor_A_patient_counter].approx_time);
            Firebase.setString(firebaseData, "/IoT_Medical_Center/ack", "False");
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/name" , req_name);
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/doc" , doctors_name[0]);
            doctor_A_patient_counter++;
          }
        }

        if (req_doc == 1) {
          if (doctor[1].patient_number < doctor[1].maximum_patient) {
            doctor[1].patient_number = doctor_B_patient_counter;
            doctor[1].patient[doctor_B_patient_counter].Pname = req_name;
            doctor[1].patient[doctor_B_patient_counter].id = req_id;
            Firebase.setInt(firebaseData, "/IoT_Medical_Center/" + req_id + "/serial" , doctor_B_patient_counter + 1);
            int calculated_time = doctor[1].start_time + (15 * doctor_B_patient_counter);
            doctor[1].patient[doctor_B_patient_counter].approx_time = String((calculated_time / 60)) + "~" + String((calculated_time % 60));
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/time" , doctor[1].patient[doctor_B_patient_counter].approx_time);
            Firebase.setString(firebaseData, "/IoT_Medical_Center/ack", "False");
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/name" , req_name);
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/doc" , doctors_name[1]);
            doctor_B_patient_counter++;
          }
        }

        if (req_doc == 2) {
          if (doctor[2].patient_number < doctor[2].maximum_patient) {
            doctor[2].patient_number = doctor_C_patient_counter;
            doctor[2].patient[doctor_C_patient_counter].Pname = req_name;
            doctor[2].patient[doctor_C_patient_counter].id = req_id;
            Firebase.setInt(firebaseData, "/IoT_Medical_Center/" + req_id + "/serial" , doctor_C_patient_counter + 1);
            int calculated_time = doctor[2].start_time + (15 * doctor_C_patient_counter);
            doctor[2].patient[doctor_C_patient_counter].approx_time = String((calculated_time / 60)) + "~" + String((calculated_time % 60));
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/time" , doctor[2].patient[doctor_C_patient_counter].approx_time);
            Firebase.setString(firebaseData, "/IoT_Medical_Center/ack", "False");
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/name" , req_name);
            Firebase.setString(firebaseData, "/IoT_Medical_Center/" + req_id + "/doc" , doctors_name[2]);
            doctor_C_patient_counter++;
          }
        }
      }
    }
    current_time = millis();
  }


  if (pageTime + 10000 < millis()) {
    tft.fillScreen(WHITE);
    showPage(pageNumber);
    pageNumber++;
    if (pageNumber > 2) {
      pageNumber = 0;
    }
    //tft.drawString(doctor[0].patient[index].Pname, 5, 10 + (index * 15), 1);
    pageTime = millis();
  }


  if ( ! rfid.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select cards
  if ( ! rfid.PICC_ReadCardSerial())
  {
    return;
  }
  //for find the UID number;
  String content = "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  /// animation section.................................
  tft.fillScreen(WHITE);
  int imgNum = 0;
  while (imgNum < 103) {
    String imgPath = "/";
    if (imgNum < 10) {
      imgPath += "00" + String(imgNum);
    }  else if (imgNum < 100 && imgNum >= 10) {
      imgPath += "0" + String(imgNum);
    } else {
      imgPath += "" + String(imgNum);
    }

    imgPath += ".jpg";
    showPic(tft.width() / 2 - 60, tft.height() / 2 - 60, imgPath);
    imgNum++;
  }



  if (content.substring(1) == "B3 4D A7 1E") {
    tft.fillScreen(WHITE);
    if (doctorA_present) {
      showPic(tft.width() / 2 - 50, tft.height() / 2 - 50, Halimabig);
      for (int index = 0; index < 4; index++) {
        tft.drawCircle(tft.width() / 2, tft.height() / 2, 50 + index, RED);
      }
      doctorA_present = false;
      digitalWrite(roomA_light, LOW);
    } else {
      showPic(tft.width() / 2 - 50, tft.height() / 2 - 50, Halimabig);
      for (int index = 0; index < 4; index++) {
        tft.drawCircle(tft.width() / 2, tft.height() / 2, 50 + index, GREEN);
      }
      doctorA_present = true;
      digitalWrite(roomA_light, HIGH);
      doctor[0].patient[activepatientA].active = 1;
    }
  }

  else if (content.substring(1) == "F3 13 82 1E") {
    tft.fillScreen(WHITE);
    if (doctorB_present) {
      showPic(tft.width() / 2 - 50, tft.height() / 2 - 50, Jibonbig);
      for (int index = 0; index < 4; index++) {
        tft.drawCircle(tft.width() / 2, tft.height() / 2, 50 + index, RED);
      }
      doctorB_present = false;
      digitalWrite(roomB_light, LOW);
    } else {
      showPic(tft.width() / 2 - 50, tft.height() / 2 - 50, Jibonbig);
      for (int index = 0; index < 4; index++) {
        tft.drawCircle(tft.width() / 2, tft.height() / 2, 50 + index, GREEN);
      }
      doctorB_present = true;
      digitalWrite(roomB_light, HIGH);

      doctor[1].patient[activepatientB].active = 1;
    }
  }

  else if (content.substring(1) == "12 AE 55 22") {
    tft.fillScreen(WHITE);
    if (doctorC_present) {
      showPic(tft.width() / 2 - 50, tft.height() / 2 - 50, Fatemabig);
      for (int index = 0; index < 4; index++) {
        tft.drawCircle(tft.width() / 2, tft.height() / 2, 50 + index, RED);
      }
      doctorC_present = false;
      digitalWrite(roomC_light, LOW);
    } else {
      showPic(tft.width() / 2 - 50, tft.height() / 2 - 50, Fatemabig);
      for (int index = 0; index < 4; index++) {
        tft.drawCircle(tft.width() / 2, tft.height() / 2, 50 + index, GREEN);
      }
      doctorC_present = true;
      digitalWrite(roomC_light, HIGH);

      doctor[2].patient[activepatientC].active = 1;
    }
  }


  else{
    tft.fillScreen(RED);
    delay(500);
    tft.fillScreen(WHITE);
    delay(500);
    tft.fillScreen(RED);
    delay(500);
    tft.fillScreen(WHITE);
  }

  delay(1000);

}

void showPic(int x, int y, String f_name) {
  TJpgDec.drawFsJpg(x, y, f_name);
}

int found(String list) {
  for (int index = 0; index < list.length(); index++) {
    if (list.charAt(index) == '$') {
      return index;
    }
  }
  return 0;
}



/*
  Procedure:
  what will be happened in this system:

  - After turning on the system, it will always be tring to find the network and try to connect.
  - After complention, it inilite the system , variables, structure etc.

  - People show their card to the RFID module.
  - Pass the UID number to the checkID() funcion.
   checkID()
   @take the value and check the record, whether the id is there or not.
   @if matched, return the "name" and set the enum for people type (STUDENT, TEACHER or , HEAD)
   @if not matched, then return "Access Denied"
  - After got the valid name, check the enum type (here p)
   @if p is "TEACHER"
   # ask for which class, that he want to take class.
   # check the input ( if selected class is active , print - "Already is using")
   # if not active , active the class(bool variable) and turn on the button led for indicting that it already in used.
   # set the timmer for this class
   # update the database.

   @if p is "STUDENT"
   # ask for which class, that he/she want to read.
   # check the selected class in active or not (if not active show the message "Unableable")
   # if time over some times. print message "you are absent"
   # update the database.

   @if p is "HEAD"
   # he can access there any time.


*/

/*
   ====================   LIBRARIES    ========================
*/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <time.h>
#include <ChandanMax.h>
#include <WiFiManager.h>

/*
   ====================   LINKS    ========================
*/
#define FIREBASE_HOST "https://clients-4b500-default-rtdb.firebaseio.com/" // Firebase HOST
#define FIREBASE_AUTH "6YSK32KbwLHkY6weAKu4kNIIPklhsZwy1dl6Iplz"           // Firebase AUTH
const char* ntpServer = "pool.ntp.org";                                    // NTP link


/*
   ====================   OBJECTS    ========================
*/
FirebaseData firebaseData;
FirebaseJson json;
MFRC522 rfid(2, 4);  // 2 - SDA , 4 - RESET
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*
   RFID ------ ESP32
   3.3v         3.3v
   RST          4
   GND          GND
   MOSI         23
   MISO         19
   SCK          18
   SDA          2
*/

/*
   LCD -------- ESP32
   SDA          21
   SCL          22
*/



/*
   ====================   VARIABLES / CONSTANTS    ========================
*/
const long  gmtOffset_sec = 5 * 3600;
const int   daylightOffset_sec = 3600;
int Number_of_Students = 5;
int Number_of_Teachers = 3;
bool ActiveClassA = false, ActiveClassB = false, ActiveClassC = false;
int buttonA = 35, buttonB = 34, buttonC = 39, buttonD = 36;
int indicatorA = 33, indicatorB = 25, indicatorC = 26;
int BuzzerA = 14, BuzzerB = 32, BuzzerC = 27;
int Control_Room_Light = 5;
int class_A_student_count = 0;
int class_B_student_count = 0;
int class_C_student_count = 0;
int activeMembers = 0;
bool CenterClose = true;
uint64_t timmer = 0;
int durationA = 0;
int durationB = 0;
int durationC = 0;
bool Control_room = false;


int classDuration = 300000;
int lateInterval = 120000;

/*
   ====================   DATA    ========================
*/

String StudentNames[5] = {"Shamim_Hosain", "Imroj_Ahmed", "Hera_Khatun", "Kawsar_Ali", "Adip"};
String StudentID[5] = {"F1 6F DC 1B", "83 E2 70 2D", "B3 A2 3D 1E", "E3 CC 08 2E", "93 7A B1 2D"};
//String StudentID[5] = {"F1 6F DC 1B", "83 E2 70 2D", "", "", ""};


String TeacherName[3] = {"Arifuzzaman", "Robiul_Islam", "Abdullah_Al_Asad"};
String TeacherID[3] = {"C1 E7 F5 19", "83 C4 2A 2D", "93 13 56 2D"};

String Active[40] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};

String HEAD_ID = "21 09 AC 1D";



/*
   ====================   STRUCTURE    ========================
*/
typedef struct {
  String name = "";
  String id = "";
} people;

people Student[5];
people Teacher[3];




/*
   ====================   ENUM    ========================
*/
enum people_type {
  TEACHER,
  STUDENT,
  HEAD,
  UNKNOWN_PERSON
};
people_type p;

class Member_info {
  public:
    int Position;
    int Class_Index = 5;
    int Class_Roll = -1;
    int Present = false;
    String Type = "";

    // time
    String Start_time = "";
    String End_time = "";
};

Member_info Form[40];




void setup() {
  Serial.begin(9600);
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
  pinMode(buttonC, INPUT);
  pinMode(buttonD, INPUT);
  pinMode(indicatorA, OUTPUT);
  pinMode(indicatorB, OUTPUT);
  pinMode(indicatorC, OUTPUT);
  pinMode(BuzzerA, OUTPUT);
  pinMode(BuzzerB, OUTPUT);
  pinMode(BuzzerC, OUTPUT);
  pinMode(Control_Room_Light, OUTPUT);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);

  WiFi.mode(WIFI_STA);
  lcd.clear();
  lcd.home();
  lcd.print("Connecting...");
  lcd.setCursor(0, 1);
  lcd.print("IP:192.168.4.1");
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("TrainingCenter");

  if (!res) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed");
    delay(1000);
    lcd.clear();
  } else {
    lcd.clear();
    lcd.home();
    lcd.print("CONNECTED");
    delay(1000);
    lcd.clear();
  }



  init_people();

  // RFID
  SPI.begin();
  rfid.PCD_Init();
  delay(4);


  // FIREBASE
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  p = UNKNOWN_PERSON;

  if (Firebase.ready()) {
    Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassA", ActiveClassA);
    Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassB", ActiveClassB);
    Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassC", ActiveClassC);
  }

}


void loop() {
  // print time on the lcd screen
  if (!ActiveClassA && !ActiveClassB && !ActiveClassC) {
    CenterClose = true;
  } else {
    CenterClose = false;
  }

  if (digitalRead(buttonD)) {
    if (Control_room) {
      Control_room = false;
      frClear();
      lcd.home();
      lcd.print("CLOSE");
      digitalWrite(Control_Room_Light, LOW);
      if (Firebase.ready()) {
        Firebase.setInt(firebaseData, "/IoT_Training_Center/ControlRoom", Control_room);
      }
      delay(1000);
      frClear();
    }
  }
  String current_time = localTime();
  lcd.setCursor(0, 0);
  lcd.print("SCAN YOUR CARD");

  if (ActiveClassA) {
    if (durationA + classDuration < millis()) {
      frClear();
      lcd.home();
      lcd.print("Class A END");
      for (int buz_count = 0; buz_count < 10; buz_count++) {
        delay(250);
        digitalWrite(BuzzerA, HIGH);
        delay(500);
        digitalWrite(BuzzerA, LOW);
      }
      digitalWrite(indicatorA, LOW);
      ActiveClassA = false;
      frClear();

      makeClassOff(0);
    }
  }

  if (ActiveClassB) {
    if (durationB + classDuration < millis()) {
      frClear();
      lcd.home();
      lcd.print("Class B END");
      for (int buz_count = 0; buz_count < 10; buz_count++) {
        delay(250);
        digitalWrite(BuzzerB, HIGH);
        delay(500);
        digitalWrite(BuzzerB, LOW);
      }
      digitalWrite(indicatorB, LOW);
      ActiveClassB = false;
      frClear();

      makeClassOff(1);
    }
  }

  if (ActiveClassC) {
    if (durationC + classDuration < millis()) {
      frClear();
      lcd.home();
      lcd.print("Class C END");
      for (int buz_count = 0; buz_count < 10; buz_count++) {
        delay(250);
        digitalWrite(BuzzerC, HIGH);
        delay(500);
        digitalWrite(BuzzerC, LOW);
      }
      digitalWrite(indicatorC, LOW);
      ActiveClassC = false;
      frClear();

      makeClassOff(2);
    }
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
  String feedback = checkID(content.substring(1));

  frClear();
  lcd.setCursor(0, 0);
  lcd.print(feedback);
  delay(1500);



  String root = "", ClassRoom = "";
  bool hit_firebase = false;
  //take input
  if (feedback != "Access Denied" && feedback != "Duplicate" && feedback != "OK") {
    timmer = millis();
    if (p == HEAD) {
      frClear();
      lcd.setCursor(0, 0);
      lcd.print("Hello Sir");
      lcd.setCursor(0, 1);
      lcd.print("COMMAND");

      bool wait_for_select = true;
      while (wait_for_select) {
        if (timmer + 3000 <  millis()) {
          wait_for_select = false;
          lcd.clear();
        }

        if (digitalRead(buttonD)) {
          if (Firebase.ready()) {
            Firebase.setInt(firebaseData, "/IoT_Training_Center/refresh", 1);
            delay(1000);
            Firebase.setInt(firebaseData, "/IoT_Training_Center/refresh", 0);
            wait_for_select = false;
            lcd.clear();
            RefreshDB();
          }
        }

        if (digitalRead(buttonC)) {
          Control_room = true;
          frClear();
          lcd.home();
          lcd.print("Done");
          digitalWrite(Control_Room_Light, HIGH);
          if (Firebase.ready()) {
            Firebase.setInt(firebaseData, "/IoT_Training_Center/ControlRoom", Control_room);
          }
          delay(1000);
          frClear();
        }
      }
    }

    if (p == STUDENT && (ActiveClassA || ActiveClassB || ActiveClassC)) {
      bool wait_for_select = true;
      frClear();
      while (wait_for_select) {
        // 5 seconds delay
        if (timmer + 3000 <  millis()) {
          wait_for_select = false;
          activeMembers--;
          Active[activeMembers] = "";
        }
        lcd.setCursor(0, 0);
        lcd.print("Select Room:");
        //------------Button A
        if (digitalRead(buttonA) && ActiveClassA) {
          if (durationA + lateInterval < millis()) {
            // late...
            frClear();
            lcd.home();
            lcd.print("You Are Late");
            delay(2000);
            frClear();
          } else {
            wait_for_select = false;
            ClassRoom = "A";
            class_A_student_count++;
            frClear();
            lcd.setCursor(0, 0);
            lcd.print("---ClassA---");
            delay(2000);
            frClear();
            lcd.home();
            lcd.print("TOP-LEFT");
            delay(3000);
            frClear();
            lcd.home();
            lcd.print("Taking Photo");
            // take image....
            if (Firebase.ready()) {
              Firebase.setString(firebaseData, "/IoT_Training_Center/req_name", feedback);
              Firebase.setString(firebaseData, "/IoT_Training_Center/command", "true");
            }
            delay(1000);
            frClear();
            hit_firebase = true;
            Form[activeMembers - 1].Class_Index = 0;
            Form[activeMembers - 1].Present = true;
            Form[activeMembers - 1].Start_time = localTime();
          }
        }

        if (digitalRead(buttonA) && !ActiveClassA) {
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("ClassA Deactive");
          delay(2000);
          frClear();
        }
        //------------Button B
        if (digitalRead(buttonB) && ActiveClassB) {
          if (durationB + lateInterval < millis()) {
            // late...
            frClear();
            lcd.home();
            lcd.print("You Are Late");
            delay(2000);
            frClear();
          } else {
            wait_for_select = false;
            ClassRoom = "B";
            class_B_student_count++;
            frClear();
            lcd.setCursor(0, 0);
            lcd.print("---ClassB---");
            delay(2000);
            frClear();
            frClear();
            lcd.home();
            lcd.print("TOP-RIGHT");
            delay(3000);
            frClear();
            lcd.home();
            lcd.print("Taking Photo");
            // take image....
            if (Firebase.ready()) {
              Firebase.setString(firebaseData, "/IoT_Training_Center/req_name", feedback);
              Firebase.setString(firebaseData, "/IoT_Training_Center/command", "true");
            }
            delay(1000);
            frClear();
            hit_firebase = true;
            Form[activeMembers - 1].Class_Index = 1;
            Form[activeMembers - 1].Present = true;
            Form[activeMembers - 1].Start_time = localTime();
          }
        }

        if (digitalRead(buttonB) && !ActiveClassB) {
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("ClassB Deactive");
          delay(2000);
          frClear();
        }

        //------------Button C
        if (digitalRead(buttonC) && ActiveClassC) {
          if (durationC + lateInterval < millis()) {
            // late...
            frClear();
            lcd.home();
            lcd.print("You Are Late");
            delay(2000);
            frClear();
          } else {
            wait_for_select = false;
            ClassRoom = "C";
            class_C_student_count++;
            frClear();
            lcd.setCursor(0, 0);
            lcd.print("---ClassC---");
            delay(3000);
            frClear();
            lcd.home();
            lcd.print("Taking Photo");
            if (Firebase.ready()) {
              Firebase.setString(firebaseData, "/IoT_Training_Center/req_name", feedback);
              Firebase.setString(firebaseData, "/IoT_Training_Center/command", "true");
            }
            delay(1000);
            frClear();
            hit_firebase = true;
            Form[activeMembers - 1].Class_Index = 2;
            Form[activeMembers - 1].Present = true;
            Form[activeMembers - 1].Start_time = localTime();
          }
        }

        if (digitalRead(buttonC) && !ActiveClassC) {
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("ClassC Deactive");
          delay(2000);
          frClear();
        }
      }
    } else if (p == STUDENT && (!ActiveClassA && !ActiveClassB && !ActiveClassC)) {
      frClear();
      lcd.setCursor(0, 0);
      lcd.print("CENTER CLOSE!");
      delay(2000);
      frClear();
    }
    //-------------TEACHER
    if (p == TEACHER) {
      bool wait_for_select = true;
      frClear();
      while (wait_for_select) {
        // 5 seconds delay
        if (timmer + 3000 <  millis()) {
          wait_for_select = false;
          activeMembers--;
          Active[activeMembers] = "";
        }
        lcd.setCursor(0, 0);
        lcd.print("Select Room:");
        //------------Button A
        if (digitalRead(buttonA) && !ActiveClassA) {
          wait_for_select = false;
          ActiveClassA = true;
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("---ClassA---");
          delay(2000);
          frClear();
          ClassRoom = "A";
          digitalWrite(indicatorA, HIGH);
          hit_firebase = true;
          durationA = millis();
          Form[activeMembers - 1].Class_Index = 0;
          Form[activeMembers - 1].Present = true;
          Form[activeMembers - 1].Start_time = localTime();
        }

        else if (digitalRead(buttonA) && ActiveClassA) {
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("A Unable Sir");
          delay(2000);
          frClear();
        }
        //------------Button B
        if (digitalRead(buttonB) && !ActiveClassB) {
          wait_for_select = false;
          ActiveClassB = true;
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("---ClassB---");
          delay(2000);
          frClear();
          ClassRoom = "B";
          digitalWrite(indicatorB, HIGH);
          hit_firebase = true;
          durationB = millis();
          Form[activeMembers - 1].Class_Index = 1;
          Form[activeMembers - 1].Present = true;
          Form[activeMembers - 1].Start_time = localTime();
        }

        else if (digitalRead(buttonB) && ActiveClassB) {
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("B Unable Sir");
          delay(2000);
          frClear();
        }

        //------------Button C
        if (digitalRead(buttonC) && !ActiveClassC) {
          wait_for_select = false;
          ActiveClassC = true;
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("---ClassC---");
          delay(2000);
          frClear();
          ClassRoom = "C";
          digitalWrite(indicatorC, HIGH);
          hit_firebase = true;
          durationC = millis();
          Form[activeMembers - 1].Class_Index = 2;
          Form[activeMembers - 1].Present = true;
          Form[activeMembers - 1].Start_time = localTime();
        }

        else if (digitalRead(buttonC) && ActiveClassC) {
          frClear();
          lcd.setCursor(0, 0);
          lcd.print("C Unable Sir");
          delay(2000);
          frClear();
        }
      }
    }

  }


  /*
     sample student > Student(Room ID)(postition)_name
     StudentA1_name,
     StudentB1_name,
     StudentC1_name,
     StudnetA3_name etc

     sample teacher > Teacher(Room ID)_name
     TeacherA_name
  */


  if (p == STUDENT) {
    root += "Student";
  }
  if (p == TEACHER) {
    root += "Teacher";
  }

  root += ClassRoom;

  if (p == STUDENT) {
    if (ClassRoom == "A") {
      Form[activeMembers - 1].Class_Roll = class_A_student_count;
      root += String(class_A_student_count);
    }
    if (ClassRoom == "B") {
      Form[activeMembers - 1].Class_Roll = class_B_student_count;
      root += String(class_B_student_count);
    }
    if (ClassRoom == "C") {
      Form[activeMembers - 1].Class_Roll = class_C_student_count;
      root += String(class_C_student_count);
    }
  }



  if (hit_firebase) {
    if (Firebase.ready()) {
      Firebase.setString(firebaseData, "/IoT_Training_Center/" + root + "_name", feedback);
      Firebase.setString(firebaseData, "/IoT_Training_Center/" + root + "_time", localTime());
      Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassA", ActiveClassA);
      Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassB", ActiveClassB);
      Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassC", ActiveClassC);
    }
  }

}

void init_people() {
  for (int index = 0; index < Number_of_Students; index++) {
    Student[index].name = StudentNames[index];
    Student[index].id = StudentID[index];
  }
  for (int index = 0; index < Number_of_Teachers; index++) {
    Teacher[index].name = TeacherName[index];
    Teacher[index].id = TeacherID[index];
  }
}

void frClear() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
}



void FollowDoctorOne() {
  if (isPatientleft(0)) {
    for (int index = 0; index < 10; index++) {
      digitalWrite(indicatorA, HIGH);
      delay(100);
      digitalWrite(indicatorA, LOW);
      delay(100);
    }

    doctor[0].patient[activepatientA].complete = true;
    if(Firebase.ready()){
      Firebase.deleteNode(firebaseData, "/IoT_Medical_Center/"+doctor[0].patient[activepatientA].id);
    }
    tft.fillScreen(WHITE);
    ISRDoctorA();
  }
}

void FollowDoctorTwo() {
  if (isPatientleft(1)) {
    for (int index = 0; index < 10; index++) {
      digitalWrite(indicatorB, HIGH);
      delay(100);
      digitalWrite(indicatorB, LOW);
      delay(100);
    }

    doctor[1].patient[activepatientB].complete = true;
    if(Firebase.ready()){
      Firebase.deleteNode(firebaseData, "/IoT_Medical_Center/"+doctor[1].patient[activepatientB].id);
    }
    tft.fillScreen(WHITE);
    ISRDoctorB();
  }
}

void FollowDoctorThree() {
if (isPatientleft(2)) {
    for (int index = 0; index < 10; index++) {
      digitalWrite(indicatorC, HIGH);
      delay(100);
      digitalWrite(indicatorC, LOW);
      delay(100);
    }

    doctor[2].patient[activepatientC].complete = true;
    if(Firebase.ready()){
      Firebase.deleteNode(firebaseData, "/IoT_Medical_Center/"+doctor[2].patient[activepatientC].id);
    }
    tft.fillScreen(WHITE);
    ISRDoctorC();
  }
}


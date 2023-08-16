void ISRDoctorA() {
  PatientLeftA = false;

  // Check the patients, if all of the patient "complete" status will be true, then no patient left...
  for (int index = 0; index < doctor_A_patient_counter; index++) {
    if (doctor[0].patient[index].complete == false) {
      PatientLeftA = true;
    }
  }

  // if and only if doctor active and patient left
  if (doctorA_present && PatientLeftA) {
    activepatientA++;
    if (activepatientA > doctor_A_patient_counter - 1) {
      activepatientA = 0;
    }

    // keep increasing until it find the incomplete patient........
    if (doctor[0].patient[activepatientA].complete) {
      ISRDoctorA();
    }

    for (int index = 0; index < doctor_A_patient_counter; index++) {
      if (index == activepatientA) {
        doctor[0].patient[index].active = 1;
      } else {
        doctor[0].patient[index].active = 0;
      }
    }
  }

  showPage(0);

}

void ISRDoctorB() {

  PatientLeftB = false;

  // Check the patients, if all of the patient "complete" status will be true, then no patient left...
  for (int index = 0; index < doctor_B_patient_counter; index++) {
    if (doctor[1].patient[index].complete == false) {
      PatientLeftB = true;
    }
  }

  // if and only if doctor active and patient left
  if (doctorB_present && PatientLeftB) {
    activepatientB++;
    if (activepatientB > doctor_B_patient_counter - 1) {
      activepatientB = 0;
    }

    // keep increasing until it find the incomplete patient........
    if (doctor[1].patient[activepatientB].complete) {
      ISRDoctorB();
    }

    for (int index = 0; index < doctor_B_patient_counter; index++) {
      if (index == activepatientB) {
        doctor[1].patient[index].active = 1;
      } else {
        doctor[1].patient[index].active = 0;
      }
    }
  }

  showPage(1);

}

void ISRDoctorC() {
  PatientLeftC = false;

  // Check the patients, if all of the patient "complete" status will be true, then no patient left...
  for (int index = 0; index < doctor_C_patient_counter; index++) {
    if (doctor[2].patient[index].complete == false) {
      PatientLeftC = true;
    }
  }

  // if and only if doctor active and patient left
  if (doctorC_present && PatientLeftC) {
    activepatientC++;
    if (activepatientC > doctor_C_patient_counter - 1) {
      activepatientC = 0;
    }

    // keep increasing until it find the incomplete patient........
    if (doctor[2].patient[activepatientC].complete) {
      ISRDoctorC();
    }

    for (int index = 0; index < doctor_C_patient_counter; index++) {
      if (index == activepatientC) {
        doctor[2].patient[index].active = 1;
      } else {
        doctor[2].patient[index].active = 0;
      }
    }
  }

  showPage(2);
}

void CheckCommand() {
  bool wait_here = true;
  uint32_t hold_time = millis();
  int page_check = 0;
  showPage(0);
  while (wait_here) {

    if (digitalRead(Down)) {
      if (page_check == 0) {
        ISRDoctorA();
      } else if (page_check == 1) {
        ISRDoctorB();
      } else if (page_check == 2) {
        ISRDoctorC();
      }
      delay(500);
      hold_time = millis();
    }

    if (digitalRead(Up)) {
      page_check++;
      if (page_check > 2) {
        page_check = 0;
      }
      tft.fillScreen(WHITE);
      showPage(page_check);
      delay(500);
      hold_time = millis();
    }

    if (hold_time + 5000 < millis()) {
      wait_here = false;
    }
  }
}


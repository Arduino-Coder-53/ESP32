void showPage(int pageIndex) {
  if (pageIndex == 0) {
    if (doctorA_present) {
      showPic(160 - 100, 128 - 100, Halimabig);
    } else {
      tft.setCursor(50, tft.height() - 10);
      tft.setTextColor(BLUE);
      tft.print("Dr. Halima Khatun");
    }


    int TextCursor = 0;
    for (int index = 0; index < doctor_A_patient_counter; index++) {
      //    Serial.print(doctor[0].patient[index].active);
      if (!doctor[0].patient[index].complete) {
        tft.setTextColor( (doctor[0].patient[index].active == 1) ? GREEN : BLACK);
        tft.drawString(String(index + 1) + ". " + doctor[0].patient[index].Pname, 5, 10 + (TextCursor * 15), 1);
        TextCursor++;
      }
    }
    //   Serial.println();
  }
  else if (pageIndex == 1) {
    if (doctorB_present) {
      showPic(160 - 100, 128 - 100, Jibonbig);
    } else {
      tft.setCursor(50, tft.height() - 10);
      tft.setTextColor(BLUE);
      tft.print("Dr. Jibon Krishna");
    }

    int TextCursor = 0;
    for (int index = 0; index < doctor_B_patient_counter; index++) {
      if (!doctor[1].patient[index].complete) {
        tft.setTextColor( (doctor[1].patient[index].active == 1) ? GREEN : BLACK);
        tft.drawString(String(index + 1) + ". " + doctor[1].patient[index].Pname, 5, 10 + (TextCursor * 15), 1);
        TextCursor++;
      }
    }
  }
  else if (pageIndex == 2) {
    if (doctorC_present) {
      showPic(160 - 100, 128 - 100, Fatemabig);
    } else {
      tft.setCursor(50, tft.height() - 10);
      tft.setTextColor(BLUE);
      tft.print("Dr. Fatema Khatun");
    }

    int TextCursor = 0;
    for (int index = 0; index < doctor_C_patient_counter; index++) {
      if (!doctor[2].patient[index].complete) {
        tft.setTextColor( (doctor[2].patient[index].active == 1) ? GREEN : BLACK);
        tft.drawString(String(index + 1) + ". " + doctor[2].patient[index].Pname, 5, 10 + (TextCursor * 15), 1);
        TextCursor++;
      }
    }
  }
}


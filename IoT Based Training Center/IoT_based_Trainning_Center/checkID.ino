/*
   CheckID()

   - Try to match the UID.
   - Set the p (TEACHER, STUDENT, HEAD)
   - Set the memberIndex
   - Check the duplication (Search in the Active members Array)
   @ If no duplication error
   - Store the UID in the Active members array
   - Increment the Active members
   - return the Name
*/

String checkID(String incoming_ID) {
  bool found = false; // __________when the vaild UID found then "found" will be activated_____________
  int memberIndex = -1;
  bool duplicate = false;  // ----------when the duplication error occured then "duplicate" will be activated---------------

  //------------------TRY TO MATCH THE UID.......................................
  for (int index = 0; index < Number_of_Students; index++) {
    if (Student[index].id == incoming_ID) {
      found = true;
      p = STUDENT; // Set the p
      memberIndex = index; // Set the memberIndex
      break;
    }
  }

  for (int index = 0; index < Number_of_Teachers; index++) {
    if (Teacher[index].id == incoming_ID) {
      found = true;
      p = TEACHER; // Set the p
      memberIndex = index; // Set the memberIndex
      break;
    }
  }



  if (incoming_ID == HEAD_ID) {
    p = HEAD; // Set the p
    // no need to set the memberIndex
    found = true;
  }


  //______________CHECK THE DUPLICATION (don't go through to it if THE "found" is DEACTIVE)________________________
  if (found) {
    if (p == STUDENT) {
      for (int index = 0; index < 40; index++) {
        if (Active[index] ==   Student[memberIndex].id) {
          duplicate = true;
          break;
        }
      }
    }



    if (p == TEACHER) {
      for (int index = 0; index < 40; index++) {
        if (Active[index] == Teacher[memberIndex].id) {
          duplicate = true;
          break;
        }
      }
    }

    //___________________WHEN WE GOT THE "DUPLICATION" ERROR_____________________________
    if (duplicate) {
      frClear();
      lcd.home();
      lcd.print("You are in Class");
      lcd.setCursor(0, 1);
      lcd.print("EXIT?");
      bool wait_here = true;
      timmer = millis();
      while (wait_here) {
        if (timmer + 3000 <  millis()) {
          wait_here = false;
        }

        if (digitalRead(buttonD)) {
          makehimAbsent(incoming_ID);
          wait_here = false;
          lcd.clear();
          return "OK";
        }
      }
      delay(1500);
      lcd.clear();
      return "Duplicate";
    } else { //____________________OTHERWISE________________________________________
      if (incoming_ID != HEAD_ID && (! CenterClose || p == TEACHER)) {
        Active[activeMembers] = incoming_ID; // ___________Save the UID
        activeMembers++; // ______________Increment the activeMembers (GLOBAL)
      }


      //____________RETURN THE NAME___________________
      if (p == STUDENT) {
        if (activeMembers > 0) {
          Form[activeMembers - 1].Position = activeMembers - 1;
          Form[activeMembers - 1].Type = "Student";
          return Student[memberIndex].name;
        }else{
          return "CENTER CLOSE";
        }
      }
      else if (p == TEACHER) {
        Form[activeMembers - 1].Type = "Teacher";
        return Teacher[memberIndex].name;
      }
      else if (p == HEAD) {
        return "Hello Sir";
      }
    }
  }  else { //____________IF UID CARD IS NOT FOUND__________________
    return "Access Denied";
  }
}


void RefreshDB() {

  class_A_student_count = 0;
  class_B_student_count = 0;
  class_C_student_count = 0;
  activeMembers = 0;
  CenterClose = true;

  for (int index = 0; index < 40; index++) {
    Active[index] = "";
    Form[index].Present = false;
  }

  ActiveClassA = false;
  ActiveClassB = false;
  ActiveClassC = false;

  digitalWrite(indicatorA, LOW);
  digitalWrite(indicatorB, LOW);
  digitalWrite(indicatorC, LOW);
}


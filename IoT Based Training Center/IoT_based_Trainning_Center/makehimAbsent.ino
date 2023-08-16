void makehimAbsent(String id_number) {
  for (int index = 0; index < 40; index++) {
    if (Active[index] == id_number) {
      Active[Form[index].Position] = "";
      Form[index].Present = false;
      Form[index].End_time = localTime();
      break;
    }
  }
}


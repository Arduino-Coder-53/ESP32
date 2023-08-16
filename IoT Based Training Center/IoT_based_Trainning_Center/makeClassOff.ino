void makeClassOff(int target_index) {

  if (Firebase.ready()) {
    if (target_index == 0) {
      Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassA", false);
    } else if (target_index == 1) {
      Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassB", false);
    } else if (target_index == 2) {
      Firebase.setInt(firebaseData, "/IoT_Training_Center/ClassC", false);
    }
  }


  for (int index = 0; index < 40; index++) {
    if (Form[index].Class_Index == target_index) {
      Active[Form[index].Position]="";
      if (Form[index].Present == true) {
        Form[index].Present == false;
      } else {
        String make_root = "";
        make_root += Form[index].Type;
        if (Form[index].Class_Index == 0) {
          make_root += "A";
        }
        else if (Form[index].Class_Index == 1) {
          make_root += "B";
        }
        else if (Form[index].Class_Index == 2) {
          make_root += "C";
        }

        if (Form[index].Type == "Student") {
          make_root += String(Form[index].Class_Roll);
        }

        make_root += "_time";


        //Dubug

        String Time_stamp = Form[index].Start_time + "(" + Form[index].End_time + ")";
        if (Firebase.ready()) {
          Firebase.setString(firebaseData, "/IoT_Training_Center/" + make_root, Time_stamp);
        }
      }
    }
  }
}


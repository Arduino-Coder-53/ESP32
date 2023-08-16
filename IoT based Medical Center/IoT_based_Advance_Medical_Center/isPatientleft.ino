bool isPatientleft(int doctor_index){
  int initial_counter = 0;
  int final_counter = 0;
  if(doctor_index==0){
    final_counter = doctor_A_patient_counter;
  }else if(doctor_index==1){
    final_counter = doctor_B_patient_counter;
  }else if(doctor_index == 2){
    final_counter = doctor_C_patient_counter;
  }

  for(int index = initial_counter; index < final_counter; index++){
    if(!doctor[doctor_index].patient[index].complete){
      return true;
    }
  }
  return false;
}


String localTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "--:--";
  }

  char Hour[3];
  char Minu[3];
  char Seco[3];

  strftime(Hour, 3, "%H", &timeinfo);
  strftime(Minu, 3, "%M", &timeinfo);
  strftime(Seco, 3, "%S", &timeinfo);
  lcd.setCursor(8,1);
  lcd.print(Hour);
  lcd.setCursor(10,1);
  lcd.print(":");

  lcd.setCursor(11,1);
  lcd.print(Minu);
  lcd.setCursor(13,1);
  lcd.print(":");

  lcd.setCursor(14,1);
  lcd.print(Seco);
  

  return String(Hour) + "~" + String(Minu);
}


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
  String Clock = String(Hour)+":"+String(Minu);

  tft.setTextColor(BLACK, WHITE);
  tft.drawString(Clock, 125, 5);

  return String(Hour) + "~" + String(Minu) + "~" + String(Seco);
}

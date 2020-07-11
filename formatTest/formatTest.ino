int getDigit(const int32_t num, const int32_t n){
  int32_t r = pow(10, n);
  int32_t x = (num / r) % 10;
  return x;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // Format UBlox GPS latitude
  char ublox_gps_lat[9];
  //int32_t lat = gps->getLatitude();
  int32_t lat = 421234567;
  uint32_t ulat;
  char dir;
  if(lat >= 0){
    ulat = lat;
    dir = 'N';
  }
  else{
    ulat = -lat;
    dir = 'S';
  }
  int32_t minutes = (ulat - int32_t(ulat/10000000)*10000000) * 60;
  //Serial.println(ulat);
  snprintf(ublox_gps_lat, 9, "%d%d%d%d.%d%d%c", getDigit(ulat, 8), getDigit(ulat, 7),
           getDigit(minutes, 7), getDigit(minutes, 6), getDigit(minutes, 5), getDigit(minutes, 4), dir);
  Serial.println(ublox_gps_lat);
  
  // Format UBlox GPS longitude
  char ublox_gps_lon[10];
  //int32_t lon = gps->getLongitude();
  int32_t lon = -1641234567;
  uint32_t ulon;
  if(lon >= 0){
    ulon = lon;
    dir = 'E';
  }
  else{
    ulon = -lon;
    dir = 'W';
  }
  minutes = (ulon - int32_t(ulon/10000000)*10000000) * 60;
  snprintf(ublox_gps_lon, 10, "%d%d%d%d%d.%d%d%c", getDigit(ulon, 9), getDigit(ulon, 8), getDigit(ulon, 7),
           getDigit(minutes, 7), getDigit(minutes, 6), getDigit(minutes, 5), getDigit(minutes, 4), dir);
  Serial.println(ublox_gps_lon);
  delay(5000);
}

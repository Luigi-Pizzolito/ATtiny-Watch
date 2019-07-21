extern "C++" {
  void refreshRTCI2c();
  void setRTCI2c(const uint8_t second, const uint8_t minute, const uint8_t hour, const uint8_t dayOfMonth, const uint8_t month, const uint8_t year, const uint8_t dayOfWeek);
  float tempRTCI2c();
}

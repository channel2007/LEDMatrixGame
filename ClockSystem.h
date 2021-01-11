#ifndef _CLOCK_SYSTEM_h
#define _CLOCK_SYSTEM_h

class ClockSystem
{
public:
  // 動畫座標.
  //int x = 0;
  //int y = 0;

  //------------------------------------------------------------------------
  //     功能說明:建構式.
  // 傳入參數說明:
  //------------------------------------------------------------------------
  ClockSystem() {
    // 初始時脈.
    initClock();
  }

  //----------------------------------------------------------------------------
  // 初始時脈.
  //----------------------------------------------------------------------------
  void initClock(byte n = 255) {
    if (n == 255) {
      for (byte i = 0; i < (sizeof(clockSys) / sizeof(unsigned long)); i++)
        clockSys[i] = millis();
    }
    else {
      clockSys[n] = millis();
    }
  }

  //----------------------------------------------------------------------------
  // 檢查啟動時脈.
  //----------------------------------------------------------------------------
  bool checkClock(byte id, unsigned long t) {
    unsigned long diff = millis() - clockSys[id];
    if (diff > t) {
      clockSys[id] = millis() - (diff - t);
      return true;
    }
    return false;
  }

private:  
  // 時脈.
  unsigned long clockSys[9];

};
#endif

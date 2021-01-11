// GamePlay.h
#ifndef _GAMEPLAY_h
#define _GAMEPLAY_h

#define LED_WIDTH             32
#define LED_HIGHT             16

#define JUMP_PIX_MAX          4     // 主角跳躍值.
#define JUMP_PIX_TREASURE_MAX 5     // 主角跳躍值-寶物.

#define SNOWBALL_DATA_MAX     3     // 雪球數量.
#define TREASURE_DATA_MAX     1     // 寶物數量.

#define SNOWBALL_APPEAR_TIME_MIN  3000
#define SNOWBALL_APPEAR_TIME_MAX  8000

// 物件結構.
struct ObjectData {
  bool use;
  byte radius;
  byte type;
  byte x;
  byte y;
};

// 遊戲狀態.
enum GameMode { READY, RUN, DIE, SCORE};

// 寶物物件.
ObjectData treasureData[TREASURE_DATA_MAX];
// 雪球物件.
ObjectData snowballData[SNOWBALL_DATA_MAX];

class GamePlay
{
public:
  //---------------------------------------------------------------------------
  // 時脈變數說明.
  // RUN:
  //   2:主角動畫速度.
  //   3:主角移動速度.
  //   4:跳躍.
  //   5:物件移動速度.
  //   6:產生物件時間.
  //   7:寶物倒數時間.
  //   8:寶物時間到閃爍.
  //
  // DIE:
  //   2:閃爍速度.
  //
  // SCORE:
  //   2:捲動速度.
  //---------------------------------------------------------------------------

  // LED物件.
  RGBmatrixPanel *matrix;
  // 時脈系統.
  ClockSystem *clockSystem;

  // 動畫狀態.
  // 0:等待.
  // 1.走動.
  byte aniState = 0;
  // 動畫.
  byte ani = 1;

  // 主角位置.
  short rolesX = 0;
  short rolesY = 0;
  // 主角跳躍值.
  byte rolesJumpMax = JUMP_PIX_MAX;
  // 主角顏色.
  //   0:藍(跳高).
  //   1:紅(無敵)
  byte rolesColor = 8;
  // 主角吃到的寶物編號.
  //   0:藍(跳高).
  //   1:紅(無敵)
  byte rolesTreasure = 8;
  // 寶物時間.
  byte treasureTime = 0;
  
  // 跳躍旗標.
  // 0:無.
  // 1:往上.
  // 2:掉落.
  byte  jumpFlag = 0;
  short jumpPix = JUMP_PIX_MAX;

  // 遮罩清除用.
  // 0:等待.
  // 1.走動.
  byte aniStateTemp = 0;
  short xTemp = 0;
  short yTemp = 0;

  // 死亡閃爍.  
  byte dieFlash = 1;

  // 分數-文字
  int textScoreX = 32;
  String textStr = "SCORE:";

  // 設定是否為等待狀態.
  bool idle = true;
  bool idleTemp = false;

  // 等級1.
  // 雪球移動速度.
  byte  snowballSpeed = 200;
  // 雪球出現時間(1/1000 sec).
  // 3000 ~ 8000;
  short snowballAppearMin = SNOWBALL_APPEAR_TIME_MIN;
  short snowballAppearMax = SNOWBALL_APPEAR_TIME_MAX;
  short snowballAppear = 2000;

  // 等級.
  byte levelUp = 1;
  // 總分.
  unsigned short score = 0;

  // 遊戲狀態.
  GameMode gameMode = READY;

  //------------------------------------------------------------------------
  // 初始.
  //------------------------------------------------------------------------
  GamePlay(RGBmatrixPanel *mx, ClockSystem *cs) {
    matrix = mx;
    clockSystem = cs;
    // 初始雪球.
    ObjectDataInit();
  }

  //------------------------------------------------------------------------
  // 初始寶物、雪球物件.
  //------------------------------------------------------------------------
  void  ObjectDataInit() {
    // 初始寶物物件.
    for (byte i = 0; i < TREASURE_DATA_MAX; i++) {
      treasureData[i].use = false;
      treasureData[i].radius = 0;
      treasureData[i].type = 8;
      treasureData[i].x = 0;
      treasureData[i].y = 0;
    }
    // 初始雪球.
    for (byte i = 0; i < SNOWBALL_DATA_MAX; i++) {
      snowballData[i].use = false;
      snowballData[i].radius = 0;
      snowballData[i].type = 0;
      snowballData[i].x = 0;
      snowballData[i].y = 0;
    }
  }

  //------------------------------------------------------------------------
  // 產生物件.
  //------------------------------------------------------------------------
  void ObjectGen() {
    if (clockSystem->checkClock( 6, snowballAppear)) {
      // 判斷出現寶物.
      byte k = random(5);
      if (k == 1 && rolesTreasure == 8 && levelUp > 1) {
        for (byte i = 0; i < TREASURE_DATA_MAX; i++) {
          if (!treasureData[i].use) {
            treasureData[i].use  = true;
            treasureData[i].radius = 8;
            treasureData[i].type = random(2);
            //treasureData[i].type = 1;
            treasureData[i].x = 31;
            treasureData[i].y = 8;
            // 寶物出現時間.
            snowballAppear = random(snowballAppearMin, snowballAppearMax);
            return;
          }         
        }
      }
      // 判斷生雪球.
      for (byte i = 0; i < SNOWBALL_DATA_MAX; i++) {
        if (!snowballData[i].use) {
          byte j = random(3);
          // 球1.
          if (j == 0) {
            snowballData[i].use = true;
            snowballData[i].radius = snowballData[i].type = 1;
            snowballData[i].x = 31;
            snowballData[i].y = 14;

          // 球2.
          }
          else if (j == 1) {
            snowballData[i].use = true;
            snowballData[i].radius = snowballData[i].type = 2;
            snowballData[i].x = 31;
            snowballData[i].y = 13;

          // 球3.
          }
          else if (j == 2) {
            snowballData[i].use = true;
            snowballData[i].radius = snowballData[i].type = 3;
            snowballData[i].x = 31;
            snowballData[i].y = 12;
          }
          // 雪球出現時間.
          snowballAppear = random(snowballAppearMin, snowballAppearMax);
          return;
        }
      }
    }
  }

  //------------------------------------------------------------------------
  // 更新物件.
  //------------------------------------------------------------------------
  void ObjectUpdate() {
    if (clockSystem->checkClock(5, snowballSpeed)) {
      // 寶物.
      for (byte i = 0; i < TREASURE_DATA_MAX; i++) {
        if (treasureData[i].use) {
          // 往左移動.
          treasureData[i].x -= 1;
          // 關閉寶物.
          if (treasureData[i].x < 1) {
            treasureData[i].x = 0;
            treasureData[i].use = false;
          }
          // 寶物-遮罩.
          fruit(treasureData[i].x+1, treasureData[i].y, treasureData[i].type, true);
          // 寶物-顯示.
          if (treasureData[i].use) {
            fruit(treasureData[i].x, treasureData[i].y, treasureData[i].type);
          }
        }
      }

      // 雪球.
      for (byte i = 0; i < SNOWBALL_DATA_MAX; i++) {
        if (snowballData[i].use) {
          // 往左移動.
          snowballData[i].x -= 1;
          // 關閉雪球.
          if (snowballData[i].x < 1) {
            snowballData[i].x = 0;
            snowballData[i].use = false;
            // 加分數.
            score += snowballData[i].type;            
            // 等級提升.
            LevelUp();
          }
          // 球-遮罩.
          matrix->fillCircle((snowballData[i].x + 1), snowballData[i].y, snowballData[i].radius, matrix->Color888(0, 0, 0));
          // 球-顯示.
          if(snowballData[i].use)
            matrix->fillCircle(snowballData[i].x, snowballData[i].y, snowballData[i].radius, matrix->Color888(128, 128, 128));
        }
      }
    }
  }

  //------------------------------------------------------------------------
  // 更新寶物倒數.
  //------------------------------------------------------------------------
  void TreasureUpdate() {

    if (treasureTime != 0) {
      // 倒數.
      if (clockSystem->checkClock( 7, 1000))
        treasureTime--;         
      // 閃爍人物.
      if (treasureTime < 3) {
        if (clockSystem->checkClock(8, 200)) {
          // 0:藍色(跳高).
          if (rolesTreasure == 0) {
            if (rolesColor == 8)
              rolesColor = 0;
            else if (rolesColor == 0)
              rolesColor = 8;           
            
            roles_idle(rolesX, rolesY);
            
            if (treasureTime == 0)
              rolesColor = 0;
          }
          // 1:紅色(無敵).
          else if (rolesTreasure == 1) {
            if (rolesColor == 8)
              rolesColor = 1;
            else if (rolesColor == 1)
              rolesColor = 8;
            
            roles_idle(rolesX, rolesY);

            if (treasureTime == 0)
              rolesColor = 1;
          }
        }
      }else{
        clockSystem->initClock(8);
      }
    }

    // 0:寶物狀態等跳躍完恢復狀態.
    if (rolesColor == 0 && rolesTreasure == 0 && treasureTime == 0 && jumpFlag == 0) {
      // 恢復跳躍值.
      rolesJumpMax = JUMP_PIX_MAX;      
      // 恢復顏色.
      rolesColor = rolesTreasure = 8;
      // 更新主角圖.
      rolesY = 0;
      roles_idle(rolesX, rolesY);
    }
    // 1:.
    if (rolesColor == 1 && rolesTreasure == 1 && treasureTime == 0) {
      // 恢復顏色.
      rolesColor = rolesTreasure = 8;
      roles_idle(rolesX, rolesY);
    }
  }

  //------------------------------------------------------------------------
  // 重新遊戲.
  //------------------------------------------------------------------------
  void ResetGame() {
    // 設定是否為等待狀態.
    idle = true;
    idleTemp = false;
    // 等級1.
    // 雪球移動速度.
    snowballSpeed = 200;
    // 雪球出現時間(1/1000 sec).
    snowballAppearMin = SNOWBALL_APPEAR_TIME_MIN;
    snowballAppearMax = SNOWBALL_APPEAR_TIME_MAX;
    snowballAppear = 2000;

    // 等級.
    levelUp = 1;
    // 總分.
    score = 0;

    // 動畫狀態.
    // 0:等待.
    // 1.走動.
    aniState = 0;
    // 動畫.
    ani = 1;

    // 主角位置.
    rolesX = 0;
    rolesY = 0;   
    rolesColor = rolesTreasure = 8;

    // 寶物時間.
    treasureTime = 0;

    // 跳躍旗標.
    // 0:無.
    // 1:往上.
    // 2:掉落.
    jumpFlag = 0;
    jumpPix = rolesJumpMax = JUMP_PIX_MAX;

    // 遮罩清除用.
    // 0:等待.
    // 1.走動.
    aniStateTemp = 0;
    xTemp = 0;
    yTemp = 0;

    // 死亡閃爍.
    dieFlash = 1;

    // 文字
    textScoreX = 32;
    textStr = "SCORE:";

    // 初始雪球.
    ObjectDataInit();

    // 清除畫面.
    matrix->fillScreen(0);
    // 更新主角圖.
    roles_idle(rolesX, rolesY);

    // 時脈系統.
    clockSystem->initClock();

    // 遊戲狀態.
    gameMode = RUN;

  }

  //------------------------------------------------------------------------
  // 判斷碰撞.
  //------------------------------------------------------------------------
  void Collision() {
    short b_x, b_y, b_w, b_h;
    short r_x, r_y, r_w, r_h;

    // 碰撞-主角.
    r_x = rolesX + 6;
    r_y = rolesY;
    r_w = 3;
    r_h = 16;

    // 判斷寶物物件.
    for (byte i = 0; i < TREASURE_DATA_MAX; i++) {
      if (treasureData[i].use) {
        // 碰撞-寶物.
        b_x = treasureData[i].x;
        b_y = treasureData[i].y;
        b_w = treasureData[i].radius;
        b_h = treasureData[i].radius;
        // 判斷寶物碰撞.
        if (b_x < (r_x + r_w) &&
          (b_x + b_w) > r_x &&
          b_y < (r_y + r_h) &&
          (b_h + b_y) > r_y) {
          // 寶物-遮罩.
          fruit(treasureData[i].x, treasureData[i].y, treasureData[i].type, true);
          // 寶物-關閉.
          treasureData[i].use = false;
          treasureData[i].x = 31;
          // 改變主角顏色.
          rolesColor = rolesTreasure = treasureData[i].type;
          // 更新主角圖.
          roles_idle(rolesX, rolesY);
          // 設定寶物時間.
          treasureTime = 8;

          // 0:藍(跳高).
          if (rolesTreasure == 0) {
            // 設定跳躍高度.
            rolesJumpMax = JUMP_PIX_TREASURE_MAX;
          }
          // 1:紅(無敵).
          else if (rolesTreasure == 1) {}

          // 初始寶物倒數時脈.
          clockSystem->initClock(7);
        }
      }
    }

    // 判斷球物件.
    for (byte i = 0; i < SNOWBALL_DATA_MAX; i++) {      
      if (snowballData[i].use) {
        // 碰撞-球.
        b_x = snowballData[i].x - snowballData[i].radius;
        b_y = snowballData[i].y - snowballData[i].radius;
        b_w = (snowballData[i].radius << 1) + 1;
        b_h = (snowballData[i].radius << 1) + 1;
        //matrix->drawRect(b_x, b_y, b_w, b_h, matrix->Color888(255, 0, 0));

        // 判斷碰撞.
        if (b_x < (r_x + r_w) &&
          (b_x + b_w) > r_x &&
          b_y < (r_y + r_h) &&
          (b_h + b_y) > r_y) {          

          // 1:紅(無敵).
          if (rolesTreasure == 1) {
            // 球-遮罩.
            matrix->fillCircle((snowballData[i].x + 1), snowballData[i].y, snowballData[i].radius, matrix->Color888(0, 0, 0));
            // 加分數.
            score += snowballData[i].type;
            // 等級提升.
            LevelUp();
            // 關閉雪球.
            snowballData[i].use = false;
            // 更新主角.
            roles_idle(rolesX, rolesY);

          // 設定主角死亡.
          }else {           
            gameMode = DIE;
          }
          return;
        }
      }
    }
  }

  //------------------------------------------------------------------------
  // 等級提升.
  //------------------------------------------------------------------------
  void LevelUp() {

    // 等級提升.    
    // 1升2.
    // 2升3.
    // 3升4.
    if ( ((score > 10) && levelUp == 1) ||
       ((score > 20) && levelUp == 2) ||
       ((score > 30) && levelUp == 3)
      ){
      levelUp++;
    }

    // level 1.
    if (levelUp == 1) {
      // 雪球移動速度.
      snowballSpeed = 200;
      // 雪球出現時間(1/1000 sec).
      snowballAppearMin = SNOWBALL_APPEAR_TIME_MIN;
      snowballAppearMax = SNOWBALL_APPEAR_TIME_MAX;
      //snowballAppear = 3000;

    // level 2.
    }
    else if (levelUp == 2) {
      // 雪球移動速度.
      snowballSpeed = 150;
      // 雪球出現時間(1/1000 sec).
      snowballAppearMin = SNOWBALL_APPEAR_TIME_MIN - 500;
      snowballAppearMax = SNOWBALL_APPEAR_TIME_MAX - 3000;
      //snowballAppear = 3000;

    // level 3.
    }
    else if (levelUp == 3) {
      // 雪球移動速度.
      snowballSpeed = 100;
      // 雪球出現時間(1/1000 sec).
      snowballAppearMin = SNOWBALL_APPEAR_TIME_MIN - 1500;
      snowballAppearMax = SNOWBALL_APPEAR_TIME_MAX - 5000;
      //snowballAppear = 3000;

    // level 4.
    }
    else if (levelUp == 4) {
      // 雪球移動速度.
      snowballSpeed = 50;
      // 雪球出現時間(1/1000 sec).
      snowballAppearMin = SNOWBALL_APPEAR_TIME_MIN - 3000;
      snowballAppearMax = SNOWBALL_APPEAR_TIME_MAX - 6500;
      //snowballAppear = 3000;
    }
  }

  //------------------------------------------------------------------------
  // 主角-跳.
  //------------------------------------------------------------------------
  void Jump() {   
    if (jumpFlag == 0) {
      jumpPix = rolesJumpMax;
      jumpFlag = 1;
    }
  }

  //------------------------------------------------------------------------
  // PS2x-開始鈕.
  //------------------------------------------------------------------------
  void PS2x_START() { 
    // 準備開始遊戲狀態、顯示分數狀態.
    if (gameMode == READY || gameMode == SCORE) {     
      ResetGame();  // 重新開始遊戲.
    }
  }

  //------------------------------------------------------------------------
  // PS2x-選擇鈕.
  //------------------------------------------------------------------------
  void PS2x_SELECT() {}
  
  //------------------------------------------------------------------------
  // PS2x-左十字鈕-上.
  //------------------------------------------------------------------------
  void PS2x_UP() {}

  //------------------------------------------------------------------------
  // PS2x-左十字鈕-下.
  //------------------------------------------------------------------------
  void PS2x_DOWN() {}

  //------------------------------------------------------------------------
  // PS2x-左十字鈕-右.
  //------------------------------------------------------------------------
  void PS2x_RIGHT() {
    // 遊戲中操作.
    if (gameMode == RUN) {
      // 設定不是等待狀態.
      idle = idleTemp = false;
      // 設定走動狀態.
      aniState = 1;
      // 用時脈控制移動速度.
      if (clockSystem->checkClock(3, 50)) {
        rolesX++;
        if (rolesX > LED_WIDTH)
          rolesX = LED_WIDTH;
      }
    }
  }

  //------------------------------------------------------------------------
  // PS2x-左十字鈕-左.
  //------------------------------------------------------------------------
  void PS2x_LEFT() {
    // 遊戲中操作.
    if (gameMode == RUN) {
      // 設定不是等待狀態.
      idle = idleTemp = false;
      // 設定走動狀態.
      aniState = 1;
      // 用時脈控制移動速度.
      if (clockSystem->checkClock(3, 50)) {
        if (rolesX > 0)
          rolesX--;
        if (rolesX < 1)
          rolesX = 0;
      }
    }
  }

  //------------------------------------------------------------------------
  // PS2x-三角按鈕.
  //------------------------------------------------------------------------
  void PS2x_GREEN() {
    Jump(); // 主角-跳.
  }

  //------------------------------------------------------------------------
  // PS2x-圓形按鈕.
  //------------------------------------------------------------------------
  void PS2x_RED() {
    Jump(); // 主角-跳.
  }

  //------------------------------------------------------------------------
  // PS2x-口按鈕.
  //------------------------------------------------------------------------
  void PS2x_PINK() {    
    Jump(); // 主角-跳.
  }

  //------------------------------------------------------------------------
  // PS2x-X按鈕.
  //------------------------------------------------------------------------
  void PS2x_BLUE() {
    Jump(); // 主角-跳.
  }

  //------------------------------------------------------------------------
  // 設定等待狀態.
  //------------------------------------------------------------------------
  void SetIdle() {
    //Serial.println(idle);
    // .
    if (idle) {
      // 設定成等待狀態.
      aniState = 0;
      // 初始時脈.
      clockSystem->initClock(3);

      if (idleTemp != idle) {
        // 清除畫面.
        //matrix->fillScreen(0);
        // 更新圖.
        roles_idle(rolesX, rolesY);
        idleTemp = idle;
      }
    }
  }

  //------------------------------------------------------------------------
  // 更新.
  //------------------------------------------------------------------------
  void Update() {
    // 準備開始遊戲狀態.
    if(gameMode == READY){
      // 秀分數.
      matrix->setTextColor(matrix->Color888(128, 128, 128));
      matrix->setCursor(18, 1);
      matrix->print("G");
      //fruit(18, 8);

    // 遊戲狀態.
    }else if (gameMode == RUN) {
      //--------------------------------------------------------------------------
      // 繪圖.
      //--------------------------------------------------------------------------
      // 跳躍.
      if ((clockSystem->checkClock(4, 100)) && (jumpFlag > 0)) {
        // 往上.
        if (jumpFlag == 1) {
          rolesY -= jumpPix;
          jumpPix--;
          if (jumpPix < 0) {
            jumpPix = 0;
            jumpFlag = 2;
          }
        }
        // 往下.
        if (jumpFlag == 2) {
          rolesY += jumpPix;
          jumpPix++;
          if (jumpPix > rolesJumpMax) {
            jumpFlag = rolesY = 0;
          }
        }
        // 清除畫面.
        //matrix->fillScreen(0);
        // 更新圖.
        roles_idle(rolesX, rolesY);
      }

      //--------------------------------------------------------------------------
      // 走動.
      if (clockSystem->checkClock(2, 150)) {
        // 1.走動.
        if (aniState == 1) {
          // 清除畫面.
          //matrix->fillScreen(0);
          //matrix->setRotation(30);
          ani++;
          if (ani > 1)
            ani = 0;
          // 角色-等待.
          if (ani == 0) {
            roles_idle(rolesX, rolesY);
          // 角色-走路.
          }
          else {
            roles_walk(rolesX, rolesY);
          }
        }
      }

      //------------------------------------------------------------------------
      // 產生物件.
      ObjectGen();
      // 更新物件.
      ObjectUpdate();
      // 更新寶物倒數.
      TreasureUpdate();

      //------------------------------------------------------------------------
      // 判斷碰撞.
      Collision();
    
    // 死亡狀態.
    }else if (gameMode == DIE) {
      // 時脈.
      if (clockSystem->checkClock( 2, 150)) {
        dieFlash++;
        if (dieFlash > 254)
          dieFlash = 1;

        // 0:等待.  
        if (aniState == 0 || ani == 0) {
          if(dieFlash%2==0)
            roles_idle(rolesX, rolesY,true);
          else
            roles_idle(rolesX, rolesY);
        // 1.走動.
        }else if (aniState == 1 && ani == 1) {
          if (dieFlash % 2 == 0)
            roles_walk(rolesX, rolesY, true);
          else
            roles_walk(rolesX, rolesY);
        }
        // 進入顯示分數.
        if (dieFlash >= 8) {
          // 組合分數.
          textStr.concat(score);
          // 進入顯示分數狀態.
          gameMode = SCORE;
        }         
      }

    // 顯示分數狀態.    
    }else if (gameMode == SCORE) {
      // 清除畫面.
      matrix->fillScreen(0);
      // 秀分數.
      matrix->setTextColor(matrix->Color888(128, 128, 128));
      matrix->setCursor(textScoreX, 1);
      matrix->print(textStr);
      // 移動分數.
      if (clockSystem->checkClock(2, 80))
        textScoreX--;
      //.
      int len = textStr.length() * -12;
      if (textScoreX < len)
        textScoreX = 32;
    }

  }

private:

  //------------------------------------------------------------------------
  // 角色-遮罩.
  //------------------------------------------------------------------------
  void roles_mask() {
    // 0:等待.    
    if(aniStateTemp == 0){
      roles_idle(xTemp, yTemp, true);
    // 1.走動.
    }else if(aniStateTemp == 1) {
      roles_walk(xTemp, yTemp, true);
    }
  }

  //------------------------------------------------------------------------
  // 角色-等待.
  //------------------------------------------------------------------------
  void roles_idle(short x, short y, bool mask = false) {
    byte i;
    short y1;
    int c1, c2, c3, c4, c5, c6;

    if (mask) {
      c1 = c2 = c3 = c4 = c5 = c6 = matrix->Color888(0, 0, 0);
    }else{
      // 遮罩.
      roles_mask();
      // 定義顏色.
      c1 = matrix->Color888(255, 204, 102);   // 膚色.
      
      // 0-藍.
      if(rolesColor ==0)
        c2 = matrix->Color888(102, 204, 255);     
      // 1-紅.
      else if (rolesColor == 1)       
        c2 = matrix->Color888(255, 0, 0);
      // 2-黃.
      else if (rolesColor == 2)
        c2 = matrix->Color888(255, 255, 0);
      // 3-白.
      else if (rolesColor == 3)
        c2 = matrix->Color888(255, 255, 255);
      // 8-綠.
      else if (rolesColor == 8)
        c2 = matrix->Color888(51, 204, 51);

      c3 = matrix->Color888(153,102,51);      // 土色.
      c4 = matrix->Color888(255,255,255);     // 白色.
      c5 = matrix->Color888(102,102,255);     // 藍色.
      c6 = matrix->Color888(255,102,51);      // 橘色.
    }

    // 0.   
    y1 = y;
    if(y1>=0){
      matrix->drawPixel(x + 4, y, c1);
      matrix->drawPixel(x + 5, y, c1);
      for (i = 6; i < 10; i++)
        matrix->drawPixel(x + i, y, c2);
    }
    // 1.
    y1 = 1+y;
    if (y1 >= 0) {
      for (i = 4; i < 8; i++)
        matrix->drawPixel(x + i, y + 1, c1);

      matrix->drawPixel(x + 8, y + 1, c2);

      for (i = 9; i < 13; i++)
        matrix->drawPixel(x + i, y + 1, c3);
    }
    // 2.
    y1 = 2 + y;
    if (y1 >= 0) {
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 2, c2);
      for (i = 5; i < 8; i++)
        matrix->drawPixel(x + i, y + 2, c1);
      for (i = 8; i < 14; i++)
        matrix->drawPixel(x + i, y + 2, c3);
    }
    // 3.
    y1 = 3 + y;
    if (y1 >= 0) {
      for (i = 1; i < 5; i++)
        matrix->drawPixel(x + i, y + 3, c2);
      for (i = 5; i < 10; i++)
        matrix->drawPixel(x + i, y + 3, c3);
      for (i = 10; i < 13; i++)
        matrix->drawPixel(x + i, y + 3, c3);
    }
    // 4.
    y1 = 4 + y;
    if (y1 >= 0) {
      for (i = 1; i < 5; i++)
        matrix->drawPixel(x + i, y + 4, c2);
      for (i = 5; i < 9; i++)
        matrix->drawPixel(x + i, y + 4, c3);
      matrix->drawPixel(x +  9, y + 4, c1);
      matrix->drawPixel(x + 10, y + 4, c1);
      matrix->drawPixel(x + 11, y + 4, c2);
      matrix->drawPixel(x + 12, y + 4, c1);
    }
    // 5.
    y1 = 5 + y;
    if (y1 >= 0) {
      for (i = 0; i < 7; i++)
        matrix->drawPixel(x + i, y + 5, c4);
      matrix->drawPixel(x + 7, y + 5, c3);
      matrix->drawPixel(x + 8, y + 5, c3);
      matrix->drawPixel(x + 9, y + 5, c1);
      matrix->drawPixel(x + 10, y + 5, c1);
      matrix->drawPixel(x + 11, y + 5, c3);
      for (i = 12; i < 15; i++)
        matrix->drawPixel(x + i, y + 5, c1);
    }
    // 6.
    y1 = 6 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 6, c4);
      for (i = 1; i < 6; i++)
        matrix->drawPixel(x + i, y + 6, c5);
      matrix->drawPixel(x + 6, y + 6, c4);
      matrix->drawPixel(x + 7, y + 6, c3);
      for (i = 8; i < 13; i++)
        matrix->drawPixel(x + i, y + 6, c1);
    }
    // 7.
    y1 = 7 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 7, c4);
      matrix->drawPixel(x + 1, y + 7, c5);
      matrix->drawPixel(x + 2, y + 7, c4);
      matrix->drawPixel(x + 3, y + 7, c5);
      matrix->drawPixel(x + 4, y + 7, c4);
      matrix->drawPixel(x + 5, y + 7, c5);
      matrix->drawPixel(x + 6, y + 7, c4);
      matrix->drawPixel(x + 7, y + 7, c3);
      matrix->drawPixel(x + 8, y + 7, c2);
      for (i = 9; i < 13; i++)
        matrix->drawPixel(x + i, y + 7, c1);
    }
    // 8.
    y1 = 8 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 8, c4);
      matrix->drawPixel(x + 1, y + 8, c5);
      matrix->drawPixel(x + 2, y + 8, c5);
      matrix->drawPixel(x + 3, y + 8, c4);
      matrix->drawPixel(x + 4, y + 8, c5);
      matrix->drawPixel(x + 5, y + 8, c5);
      matrix->drawPixel(x + 6, y + 8, c4);
      matrix->drawPixel(x + 7, y + 8, c3);
      for (i = 8; i < 11; i++)
        matrix->drawPixel(x + i, y + 8, c2);
    }
    // 9.
    y1 = 9 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 9, c4);
      matrix->drawPixel(x + 1, y + 9, c5);
      matrix->drawPixel(x + 2, y + 9, c6);
      matrix->drawPixel(x + 3, y + 9, c5);
      matrix->drawPixel(x + 4, y + 9, c6);
      matrix->drawPixel(x + 5, y + 9, c5);
      matrix->drawPixel(x + 6, y + 9, c4);
      for (i = 7; i < 12; i++)
        matrix->drawPixel(x + i, y + 9, c2);
    }
    // 10.
    y1 = 10 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 10, c4);
      matrix->drawPixel(x + 1, y + 10, c5);
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 10, c6);
      matrix->drawPixel(x + 5, y + 10, c5);
      matrix->drawPixel(x + 6, y + 10, c4);
      for (i = 7; i < 12; i++)
        matrix->drawPixel(x + i, y + 10, c2);
    }
    //11.
    y1 = 11 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 11, c4);
      matrix->drawPixel(x + 1, y + 11, c5);
      matrix->drawPixel(x + 2, y + 11, c5);
      matrix->drawPixel(x + 3, y + 11, c6);
      matrix->drawPixel(x + 4, y + 11, c5);
      matrix->drawPixel(x + 5, y + 11, c5);
      matrix->drawPixel(x + 6, y + 11, c4);
      for (i = 7; i < 11; i++)
        matrix->drawPixel(x + i, y + 11, c2);
      matrix->drawPixel(x + 11, y + 11, c3);
    }
    //12.
    y1 = 12 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 1, y + 12, c4);
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 12, c5);
      matrix->drawPixel(x + 5, y + 12, c4);
      for (i = 6; i < 12; i++)
        matrix->drawPixel(x + i, y + 12, c3);
    }
    // 13.
    y1 = 13 + y;
    if (y1 >= 0) {
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 13, c4);
      for (i = 5; i < 12; i++)
        matrix->drawPixel(x + i, y + 13, c2);
    }
    // 14.
    y1 = 14 + y;
    if (y1 >= 0) {
      for (i = 5; i < 9; i++)
        matrix->drawPixel(x + i, y + 14, c3);
    }
    // 15.
    y1 = 15 + y;
    if (y1 >= 0) {
      for (i = 5; i < 10; i++)
        matrix->drawPixel(x + i, y + 15, c3);
    }
    // 動畫狀態.
    // 0:等待.
    // 1.走動.
    aniStateTemp = 0;
    xTemp = x;
    yTemp = y;

    //c1(膚色).
    //c2(綠色).
    //c3(土色).
    //c4(白色).
    //c5(藍色).
    //c6(橘色).
  }

  //------------------------------------------------------------------------
  // 角色-走路.
  //------------------------------------------------------------------------
  void roles_walk(short x, short y, bool mask = false) {
    byte i;
    short y1;
    int c1, c2, c3, c4, c5, c6, c7;

    if (mask) {
      c1 = c2 = c3 = c4 = c5 = c6 = c7 = matrix->Color888(0, 0, 0);
    }else {
      // 遮罩.
      roles_mask();
      // 定義顏色.
      c1 = matrix->Color888(255, 204, 102);   // 膚色.
      //c2 = matrix->Color888(51, 204, 51);   // 綠色.
      // 0-藍.
      if (rolesColor == 0)
        c2 = matrix->Color888(102, 204, 255);     
      // 1-紅.
      else if (rolesColor == 1)
        c2 = matrix->Color888(255, 0, 0);       
      // 2-黃.
      else if (rolesColor == 2)
        c2 = matrix->Color888(255, 255, 0);
      // 3-白.
      else if (rolesColor == 3)
        c2 = matrix->Color888(255, 255, 255);
      // 8-綠.
      else if (rolesColor == 8)
        c2 = matrix->Color888(51, 204, 51);

      c3 = matrix->Color888(153, 102, 51);    // 土色.
      c4 = matrix->Color888(255, 255, 255);   // 白色.
      c5 = matrix->Color888(102, 102, 255);   // 藍色.
      c6 = matrix->Color888(255, 102, 51);    // 橘色.
      c7 = matrix->Color888(0, 0, 0);         // 黑色.
    }
    
    // 1.
    y1 = 1 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 4, y + 1, c1);
      matrix->drawPixel(x + 5, y + 1, c1);
      for (i = 6; i < 10; i++)
        matrix->drawPixel(x + i, y + 1, c2);
    }
    // 2.
    y1 = 2 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 2, y + 2, c2);
      matrix->drawPixel(x + 3, y + 2, c2);
      for (i = 4; i < 8; i++)
        matrix->drawPixel(x + i, y + 2, c1);
      matrix->drawPixel(x + 8, y + 2, c2);
      for (i = 9; i < 13; i++)
        matrix->drawPixel(x + i, y + 2, c3);
    }
    // 3.
    y1 = 3 + y;
    if (y1 >= 0) {
      for (i = 1; i < 5; i++)
        matrix->drawPixel(x + i, y + 3, c2);
      for (i = 5; i < 8; i++)
        matrix->drawPixel(x + i, y + 3, c1);
      for (i = 8; i < 14; i++)
        matrix->drawPixel(x + i, y + 3, c3);
    }
    // 4.
    y1 = 4 + y;
    if (y1 >= 0) {
      for (i = 1; i < 5; i++)
        matrix->drawPixel(x + i, y + 4, c2);
      for (i = 5; i < 13; i++)
        matrix->drawPixel(x + i, y + 4, c3);
    }
    // 5.
    y1 = 5 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 3, y + 5, c2);
      matrix->drawPixel(x + 4, y + 5, c2);
      for (i = 5; i < 9; i++)
        matrix->drawPixel(x + i, y + 5, c3);
      matrix->drawPixel(x +  9, y + 5, c1);
      matrix->drawPixel(x + 10, y + 5, c1);
      matrix->drawPixel(x + 11, y + 5, c2);
      matrix->drawPixel(x + 12, y + 5, c1);
    }
    // 6.
    y1 = 6 + y;
    if (y1 >= 0) {
      for (i = 0; i < 7; i++)
        matrix->drawPixel(x + i, y + 6, c4);
      matrix->drawPixel(x + 7, y + 6, c3);
      matrix->drawPixel(x + 8, y + 6, c3);
      matrix->drawPixel(x + 9, y + 6, c1);
      matrix->drawPixel(x + 10, y + 6, c1);
      matrix->drawPixel(x + 11, y + 6, c3);
      for (i = 12; i < 15; i++)
        matrix->drawPixel(x + i, y + 6, c1);
    }
    // 7.
    y1 = 7 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 7, c4);
      for (i = 1; i < 6; i++)
        matrix->drawPixel(x + i, y + 7, c5);
      matrix->drawPixel(x + 6, y + 7, c4);
      matrix->drawPixel(x + 7, y + 7, c3);
      for (i = 8; i < 13; i++)
        matrix->drawPixel(x + i, y + 7, c1);
    }
    // 8.
    y1 = 8 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 8, c4);
      matrix->drawPixel(x + 1, y + 8, c5);
      matrix->drawPixel(x + 2, y + 8, c4);
      matrix->drawPixel(x + 3, y + 8, c5);
      matrix->drawPixel(x + 4, y + 8, c4);
      matrix->drawPixel(x + 5, y + 8, c5);
      matrix->drawPixel(x + 6, y + 8, c4);
      matrix->drawPixel(x + 7, y + 8, c3);
      matrix->drawPixel(x + 8, y + 8, c2);
      for (i = 9; i < 13; i++)
        matrix->drawPixel(x + i, y + 8, c1);
    }
    // 9.
    y1 = 9 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 9, c4);
      matrix->drawPixel(x + 1, y + 9, c5);
      matrix->drawPixel(x + 2, y + 9, c5);
      matrix->drawPixel(x + 3, y + 9, c4);
      matrix->drawPixel(x + 4, y + 9, c5);
      matrix->drawPixel(x + 5, y + 9, c5);
      matrix->drawPixel(x + 6, y + 9, c4);
      matrix->drawPixel(x + 7, y + 9, c3);
      for (i = 8; i < 11; i++)
        matrix->drawPixel(x + i, y + 9, c2);
    }
    // 10.
    y1 = 10 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 10, c4);
      matrix->drawPixel(x + 1, y + 10, c5);
      matrix->drawPixel(x + 2, y + 10, c6);
      matrix->drawPixel(x + 3, y + 10, c5);
      matrix->drawPixel(x + 4, y + 10, c6);
      matrix->drawPixel(x + 5, y + 10, c5);
      matrix->drawPixel(x + 6, y + 10, c4);
      for (i = 7; i < 12; i++)
        matrix->drawPixel(x + i, y + 10, c2);
    }
    // 11.
    y1 = 11 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 11, c4);
      matrix->drawPixel(x + 1, y + 11, c5);
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 11, c6);
      matrix->drawPixel(x + 5, y + 11, c5);
      matrix->drawPixel(x + 6, y + 11, c4);
      for (i = 7; i < 12; i++)
        matrix->drawPixel(x + i, y + 11, c2);
    }
    // 12.
    y1 = 12 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 12, c4);
      matrix->drawPixel(x + 1, y + 12, c5);
      matrix->drawPixel(x + 2, y + 12, c5);
      matrix->drawPixel(x + 3, y + 12, c6);
      matrix->drawPixel(x + 4, y + 12, c5);
      matrix->drawPixel(x + 5, y + 12, c5);
      matrix->drawPixel(x + 6, y + 12, c4);
      for (i = 7; i < 11; i++)
        matrix->drawPixel(x + i, y + 12, c2);
      matrix->drawPixel(x + 11, y + 12, c3);
    }
    // 13.
    y1 = 13 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 1, y + 13, c4);
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 13, c5);
      matrix->drawPixel(x + 5, y + 13, c4);
      matrix->drawPixel(x + 6, y + 13, c2);
      for (i = 7; i < 12; i++)
        matrix->drawPixel(x + i, y + 13, c3);
      matrix->drawPixel(x + 12, y + 13, c2);
    }
    // 14.
    y1 = 14 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 1, y + 14, c3);
      for (i = 2; i < 5; i++)
        matrix->drawPixel(x + i, y + 14, c4);
      for (i = 5; i < 12; i++)
        matrix->drawPixel(x + i, y + 14, c2);
      matrix->drawPixel(x + 12, y + 14, c3);
      matrix->drawPixel(x + 13, y + 14, c3);
    }
    //15
    y1 = 15 + y;
    if (y1 >= 0) {
      for (i = 2; i < 13; i++)
        matrix->drawPixel(x + i, y + 15, c3);
    }

    // 動畫狀態.
    // 0:等待.
    // 1.走動.
    aniStateTemp = 1;
    xTemp = x;
    yTemp = y;

    //c1(膚色).
    //c2(綠色).
    //c3(土色).
    //c4(白色).
    //c5(藍色).
    //c6(橘色).
  }
  
  //------------------------------------------------------------------------
  // 寶物-水果.
  //------------------------------------------------------------------------
  void fruit(short x, short y, byte color = 0,bool mask = false) {
    byte i;
    short y1;
    int c1, c2, c3, c4, c5, c6, c7;

    if (mask) {
      c1 = c2 = c3 = c4 = c5 = c6 = c7 = matrix->Color888(0, 0, 0);
    }
    else {
      // 定義顏色.
      // 0:藍.
      if (color == 0) {
        c1 = matrix->Color888(102, 204, 255); // 藍0(淡).
        c2 = matrix->Color888(50, 96, 255);   // 藍1.
        c3 = matrix->Color888(24, 24, 255);   // 藍2(深).     
      // 1:紅.
      }else if (color == 1) {
        c1 = matrix->Color888(255, 0, 0);     // 紅0(淡).
        c2 = matrix->Color888(64, 0, 0);      // 紅1.
        c3 = matrix->Color888(16, 0, 0);      // 紅2(深).
      // 2:黃.
      }else if (color == 2) {
        c1 = matrix->Color888(255, 255, 0);   // 黃0(淡).
        c2 = matrix->Color888(192, 204, 0);   // 黃1.
        c3 = matrix->Color888(52, 74, 0);     // 黃2(深).
      // 3:白.
      }else if (color == 3) {
        c1 = matrix->Color888(255, 255, 255); // 白0(淡).
        c2 = matrix->Color888(64, 64, 64);    // 白1.
        c3 = matrix->Color888(16, 16, 16);    // 白2(深).       
      }
    }

    // 0.
    y1 = y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 4, y, c1);
    }
    // 1.
    y1 = 1 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 3, y+1, c1);
    }
    // 2.
    y1 = 2 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 1, y+2, c2);
      for (i = 2; i < 6; i++)
        matrix->drawPixel(x + i, y + 2, c1);
    }
    // 3.
    y1 = 3 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 3, c3);
      matrix->drawPixel(x + 1, y + 3, c2);
      for (i = 2; i < 7; i++)
        matrix->drawPixel(x + i, y + 3, c1);
    }
    // 4.
    y1 = 4 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 4, c3);
      matrix->drawPixel(x + 1, y + 4, c2);
      for (i = 2; i < 7; i++)
        matrix->drawPixel(x + i, y + 4, c1);
    }
    // 5.
    y1 = 5 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 5, c3);
      matrix->drawPixel(x + 1, y + 5, c2);
      for (i = 2; i < 7; i++)
        matrix->drawPixel(x + i, y + 5, c1);
    }
    // 6.
    y1 = 6 + y;
    if (y1 >= 0) {
      matrix->drawPixel(x + 0, y + 6, c3);
      matrix->drawPixel(x + 1, y + 6, c3);
      for (i = 2; i < 7; i++)
        matrix->drawPixel(x + i, y + 6, c2);
    }
    // 7.
    y1 = 7 + y;
    if (y1 >= 0) {
      for (i = 1; i < 6; i++)
        matrix->drawPixel(x + i, y + 7, c3);
    }
    //c1白0(淡).
    //c2白1.
    //c3白2(深).
  }
  
};

#endif

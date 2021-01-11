#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <PS2X_lib.h>		// for v1.6

#include "ClockSystem.h"
#include "GamePlay.h"

// MUST be on PORTB! (Use pin 11 on Mega)
#define CLK 8  
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2

// RGB LED.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// PS2X搖桿.
PS2X ps2x;

// 時脈系統.
ClockSystem clockSystem;

// GamePlay.
GamePlay gamePlay(&matrix, &clockSystem);

// PS2X搖桿變數.
int error = 0;
byte type = 0;
byte vibrate = 0;

//------------------------------------------------------------------------
// ps2x-初始.
//------------------------------------------------------------------------
void ps2xInit()
{
	//setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error.
	//error = ps2x.config_gamepad(13, 11, 10, 12, false, false);
	error = ps2x.config_gamepad(13, 11, 10, 12, false, false);
	if (error == 0) {
		Serial.println("Found Controller, configured successful");
		Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
		Serial.println("holding L1 or R1 will print out the analog stick values.");
		Serial.println("Go to www.billporter.info for updates and to report bugs.");
	}
	else if (error == 1)
		Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
	else if (error == 2)
		Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
	else if (error == 3)
		Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

	type = ps2x.readType();
	switch (type) {
	case 0:
		Serial.println("Unknown Controller type");
		break;
	case 1:
		Serial.println("DualShock Controller Found");
		break;
	case 2:
		Serial.println("GuitarHero Controller Found");
		break;
	}
}

//------------------------------------------------------------------------
// ps2x-更新.
//------------------------------------------------------------------------
void ps2xUpdate() {

	//read controller and set large motor to spin at 'vibrate' speed
	ps2x.read_gamepad(false, vibrate);

	// 設定主角等待狀態.
	gamePlay.idle = true;

	//will be TRUE as long as button is pressed
	// 開始按鈕.
	if (ps2x.ButtonPressed(PSB_START)) {
		//Serial.println("Start is being held");
		gamePlay.PS2x_START();
	}
	// 選擇按鈕.
	if (ps2x.ButtonPressed(PSB_SELECT)) {
		//Serial.println("Select is being held");
		gamePlay.PS2x_SELECT();
	}

	// will be TRUE as long as button is pressed
	// 左邊控制鍵-上. 
	if (ps2x.Button(PSB_PAD_UP)) {
		//Serial.print("Up held this hard: ");
		//Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
		gamePlay.PS2x_UP();
	}
	// 左邊控制鍵-右. 
	if (ps2x.Button(PSB_PAD_RIGHT)) {
		//Serial.print("Right held this hard: ");
		//Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
		gamePlay.PS2x_RIGHT();
	}
	// 左邊控制鍵-左. 
	if (ps2x.Button(PSB_PAD_LEFT)) {
		//Serial.print("LEFT held this hard: ");
		//Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
		gamePlay.PS2x_LEFT();
	}
	// 左邊控制鍵-下. 
	if (ps2x.Button(PSB_PAD_DOWN)) {
		//Serial.print("DOWN held this hard: ");
		//Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
		gamePlay.PS2x_DOWN();
	}

	// 設定等待狀態.
	gamePlay.SetIdle();

	//this will set the large motor vibrate speed based on 
	//how hard you press the blue (X) button    
	vibrate = ps2x.Analog(PSAB_BLUE);

	// ps2x-三角形. 
	if (ps2x.ButtonPressed(PSB_GREEN)) {
		//Serial.println("Triangle pressed");
		gamePlay.PS2x_GREEN();
	}

	//will be TRUE if button was JUST pressed
	// ps2x-圓形.
	if (ps2x.ButtonPressed(PSB_RED)) {
		//Serial.println("Circle just pressed");
		gamePlay.PS2x_RED();
	}

	//will be TRUE if button was JUST released
	// ps2x-四方型.
	if (ps2x.ButtonPressed(PSB_PINK)) {
		//Serial.println("Square just released");
		gamePlay.PS2x_PINK();
	}

	//will be TRUE if button was JUST pressed OR released
	// ps2x-X.
	if (ps2x.ButtonPressed(PSB_BLUE)) {
		//Serial.println("X just changed");
		gamePlay.PS2x_BLUE();
	}
}

//------------------------------------------------------------------------
// 設定.
//------------------------------------------------------------------------
void setup() {
	//.
	Serial.begin(57600);

	// LED.
	matrix.begin();
	matrix.setTextWrap(false); // Allow text to run off right edge
	matrix.setTextSize(2);

	// 時脈系統.
	clockSystem.initClock();

	// 初始ps2x.
	delay(500);
	ps2xInit();

}

//------------------------------------------------------------------------
// 主迴圈.
//------------------------------------------------------------------------
void loop() {
	// skip loop if no controller found.
	if (error == 1) {
		delay(2000);
		ps2xInit();
		return;
	}
		
	// FPS 16.
	if (clockSystem.checkClock(1, 16)) {
		// ps2x-更新.
		ps2xUpdate();
		// GamePlay更新.
		gamePlay.Update();
	}

}

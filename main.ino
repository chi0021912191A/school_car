#include <Wire.h>
#include <PS2X_lib.h>    // 包含PS2函式庫
#include <Adafruit_MotorShield.h>
#include "Emakefun_MotorDriver.h"


Emakefun_MotorDriver mMotorDriver = Emakefun_MotorDriver(0x60);

Emakefun_Servo *mServo1 = mMotorDriver.getServo(1);
Emakefun_Servo *mServo2 = mMotorDriver.getServo(2);


// 搖桿設定
#define PS2_DAT       12  //定義 DATA 接腳
#define PS2_CMD       11  //定義 COMMAND 接腳
#define PS2_ATT       10  //定義 ATTENTION 接腳
#define PS2_CLK       13  //定義 CLOCK 接腳

#define pressures     true  // 支援類比按鍵
#define rumble        true  // 支援震動

PS2X ps2x; // 建立PS2控制器的類別實體

int error = 0; 
byte type = 0;
byte vibrate = 0;


// 馬達設定
int speed = 150;

#define carSpeed  speed  //馬達速度，本範例固定這個速度

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *R0_Motor = AFMS.getMotor(2);  //右1馬達接在M2
Adafruit_DCMotor *R1_Motor = AFMS.getMotor(1);  //右2馬達接在M1
Adafruit_DCMotor *L0_Motor = AFMS.getMotor(4);  //左1馬達接在M4
Adafruit_DCMotor *L1_Motor = AFMS.getMotor(3);  //左2馬達接在M3


void setup() {
   Serial.begin(57600);

 // ********注意！******** 1.6版的新語法：
 // 控制器接腳設置並驗證是否有錯誤:  GamePad(時脈腳位, 命令腳位, 選取腳位, 資料腳位, 是否支援類比按鍵, 是否支援震動) 
 error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, pressures, rumble);
 
 if(error == 0) { // 如果控制器連接沒有問題，就顯示底下的訊息。
   Serial.println("Found Controller, configured successful");
   Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
   Serial.println("holding L1 or R1 will print out the analog stick values.");
   Serial.println("Go to www.billporter.info for updates and to report bugs.");
 }
   
  else if(error == 1) // 找不到控制器，顯示底下的錯誤訊息。
   Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)  // 發現控制器，但不接受命令，請參閱程式作者網站的除錯說明。
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3) // 控制器拒絕進入類比感測壓力模式，或許是此控制器不支援的緣故。
   Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
   type = ps2x.readType();  // 偵測控制器器的類型
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");   // 未知的控制器類型
       break;
       case 1:
        Serial.println("DualShock Controller Found");  // 發現DualShock控制器
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");  // 發現吉他英雄控制器
       break;
     }


  Serial.println("DC Motor test!");

  AFMS.begin();

  R0_Motor->setSpeed(0);
  R1_Motor->setSpeed(0);
  L0_Motor->setSpeed(0);
  L1_Motor->setSpeed(0);

  R0_Motor->run(FORWARD);
  R1_Motor->run(FORWARD);
  L0_Motor->run(FORWARD);
  L1_Motor->run(FORWARD);

  R0_Motor->run(RELEASE);
  R1_Motor->run(RELEASE);
  L0_Motor->run(RELEASE);
  L1_Motor->run(RELEASE);

  
}

  
void loop(){
  ps2x.read_gamepad(false, vibrate);			// 讀取控制器並且命令大的震動馬達以"vibrate"變數值的速度旋轉
    
  if(ps2x.Button(PSB_START))					// 查看「開始」鍵是否被按住
    Serial.println("Start is being held");
  if(ps2x.Button(PSB_SELECT))				// 查看「選擇」鍵是否被按住
    Serial.println("Select is being held");
         
  if(ps2x.Button(PSB_PAD_UP)) {         // 若「上」按鍵被按著
    R0_Motor->setSpeed(0);
    R1_Motor->setSpeed(0);
    L0_Motor->setSpeed(0);
    L1_Motor->setSpeed(0);
  }
  if(ps2x.Button(PSB_PAD_RIGHT)){
    Serial.print("Right held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
  }
  if(ps2x.Button(PSB_PAD_LEFT)){
    Serial.print("LEFT held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
  }
  if(ps2x.Button(PSB_PAD_DOWN)){
    Serial.print("DOWN held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
  }   

  float magnification; // Declare the variables outside of the if-else statements
  
  if(ps2x.Button(PSB_L1)){
    magnification = 0.5;
    Serial.println("0.5");
  }
  else if(ps2x.Button(PSB_R1)){
    magnification = 0.1;
  }
  else if(ps2x.Button(PSB_L2)){
    magnification = 2;
  }
  else if(ps2x.Button(PSB_R2)){
    magnification = 1.5;
  }
  else{
    magnification = 1;
    Serial.println("-1");
  }

  int PSS_LY_OR = ps2x.Analog(PSS_LY);
  int PSS_RY_OR = ps2x.Analog(PSS_RY);

  int LY;
  int RY;
  Serial.println(ps2x.Analog(PSS_RY));

  if(PSS_RY_OR>128){
    RY = abs((ps2x.Analog(PSS_RY)-128)*magnification);
    R0_Motor->setSpeed(RY);
    R1_Motor->setSpeed(RY);

    R0_Motor->run(BACKWARD);
    R1_Motor->run(BACKWARD);
 
  }

  else if(PSS_RY_OR<128){
    RY = abs((127-ps2x.Analog(PSS_RY))*magnification-255);
    R0_Motor->setSpeed(RY);
    R1_Motor->setSpeed(RY);

    R0_Motor->run(FORWARD);
    R1_Motor->run(FORWARD);
  }

  else{
    Serial.println("STOP");
    R0_Motor->setSpeed(0);
    R1_Motor->setSpeed(0);
   
    R0_Motor->run(RELEASE);
    R1_Motor->run(RELEASE);
  }

  if(PSS_LY_OR<128){
    LY = abs((127-(ps2x.Analog(PSS_LY)))*magnification-255);
    L0_Motor->setSpeed(LY);
    L1_Motor->setSpeed(LY);  

    L0_Motor->run(FORWARD);
    L1_Motor->run(FORWARD);
  }

  else if(PSS_LY_OR>128){
    LY = abs((127-(ps2x.Analog(PSS_LY)))*magnification);
    L0_Motor->setSpeed(LY);
    L1_Motor->setSpeed(LY);  

    L0_Motor->run(BACKWARD);
    L1_Motor->run(BACKWARD);
  }
  
  else{
    Serial.println("STOP");
    L0_Motor->setSpeed(0);
    L1_Motor->setSpeed(0);  

    L0_Motor->run(RELEASE);
    L1_Motor->run(RELEASE);
  }
  Serial.print(LY);
  Serial.print(";");
  Serial.println(RY);
}

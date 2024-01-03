// ============================================================
// Example:     LCDML: graphic display with u8g
// ============================================================
// Author:      Jomelo
// Last update: 21.01.2018
// License:     MIT
// ============================================================
// Description:
// This example shows how to use the u8glib with the LCDMenuLib
// The menu can placed in a box that can be placed anywhere on
// the screen.
// ============================================================
// *********************************************************************
// special settings
// *********************************************************************
// enable this line when you are not usigng a standard arduino
// for example when your chip is an ESP or a STM or SAM or something else
//#define _LCDML_cfg_use_ram

// include libs
#include <LCDMenuLib2.h>

// U8g2lib
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <Servo.h>
#include <EEPROMex.h>
#include <PID_v1.h>

// *********************************************************************
// Non menu defines
// *********************************************************************
//#define ANNEAL_PIN 6
#define ANNEAL_PIN 13
#define FAN_PIN 8
#define ON 1
#define OFF 0
//#define CURRENT_SENSOR_PIN A1
#define CURRENT_SENSOR_PIN A1
#define CURRENT_SENSOR_TYPE 30 //30 amp Hall effect current sensor
//#define SERVO_PIN A2
#define SERVO_PIN 9
//#define VIBRATION_SENSOR_PIN A3
#define VIBRATION_SENSOR_PIN A0
//#define FEEDER_PIN 8
#define FEEDER_PIN 12
#define ANNEAL_LOOP_INTERVAL_MS 10


// *********************************************************************
// Global variables for the OA project
// *********************************************************************
// Integer used to fetch values stored in eprom
int eepromOutput;
// Integer to store duty cycle
int dutyCycle = 50;
// Default hold time
int holdTime = 1000; //ms
// Default annealTime
int annealTime = 2500; //ms
// Variable for measured current (integer, sensor range is 1024)
float tmpCurrent = 0;
// Target Current
float targetCurrent = 0;
// Anneal mode
int annealMode = 2; //time = 2, manual current based = 1, magic = 3
// Anneal function step
int annealStep = 0;
// Location of servo home position
int servoHomePos = 600;
// Location of servo anneal position
int servoAnnealPos = 600;
// Location of servo drop pos
int servoDropPos = 1700;
// Keep track of current position of servo
int servoCurrentPos;
// Target position of servo
int servoTargetPos;
// Should disable servo?
bool disableServo = false;
// Servo "speed"
int servoDelay = 13;
// Servo move time
long myservo_movetime = 0;

int learningEnabled = 0;
int learningConfidence = 0;

// Iterative regression analysis stuff
int nGlowCurrent = 1000;
float meanGlowCurrentX = 7.02;
float meanGlowCurrentY = 8.65;
float varXGlowCurrent = 0.10;
float covXYGlowCurrent = 0.15;
float glowCurrentCoefficient = 1.50;
float glowCurrentConstant = -1.91;

// Old PID stuff

double Setpoint, Input, Output;
int WindowSize = 300;
unsigned long windowStartTime;

// Last display update time ms
unsigned long lastDisplayUpdate = 0;

// *********************************************************************
// U8GLIB
// *********************************************************************
// U8g2 Constructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
//U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 53, /* reset=*/ U8X8_PIN_NONE);     // (MEGA, ...
//U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 12, /* reset=*/ U8X8_PIN_NONE);   // (Uno and co
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* CS=*/ 12, /* reset=*/ U8X8_PIN_NONE);

// settings for u8g lib and LCD
#define _LCDML_DISP_w                 128            // LCD width
#define _LCDML_DISP_h                 64             // LCD height
// font settings
//#define _LCDML_DISP_font              u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
#define _LCDML_DISP_font             u8g2_font_6x13O_tr
#define _LCDML_DISP_font_w            6              // font width
#define _LCDML_DISP_font_h            13             // font height
// cursor settings
#define _LCDML_DISP_cursor_char       "X"            // cursor char
#define _LCDML_DISP_cur_space_before  2              // cursor space between
#define _LCDML_DISP_cur_space_behind  4              // cursor space between
// menu position and size
#define _LCDML_DISP_box_x0            0              // start point (x0, y0)
#define _LCDML_DISP_box_y0            0              // start point (x0, y0)
#define _LCDML_DISP_box_x1            128            // width x  (x0 + width)
#define _LCDML_DISP_box_y1            64             // hight y  (y0 + height)
#define _LCDML_DISP_draw_frame        1              // draw a box around the menu
// scrollbar width
#define _LCDML_DISP_scrollbar_w       6  // scrollbar width (if this value is < 3, the scrollbar is disabled)

// nothing change here
#define _LCDML_DISP_cols_max          ((_LCDML_DISP_box_x1-_LCDML_DISP_box_x0)/_LCDML_DISP_font_w)
#define _LCDML_DISP_rows_max          ((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0-((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0)/_LCDML_DISP_font_h))/_LCDML_DISP_font_h)

// rows and cols
// when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// the program needs more ram with this changes
#define _LCDML_DISP_rows              _LCDML_DISP_rows_max  // max rows
#define _LCDML_DISP_cols              20                   // max cols


// *********************************************************************
// Prototypes
// *********************************************************************
void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();


// *********************************************************************
// Objects
// *********************************************************************
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);


// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
// LCDML_0        => layer 0
// LCDML_0_X      => layer 1
// LCDML_0_X_X    => layer 2
// LCDML_0_X_X_X  => layer 3
// LCDML_0_...      => layer ...

LCDML_addAdvanced (0 , LCDML_0        , 1  , NULL,          ""                  , mDyn_annealMode,                0,   _LCDML_TYPE_dynParam);
LCDML_addAdvanced (1 , LCDML_0         , 2  , COND_hideAnalyse,  "Analyse"        , NULL,        0,   _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (2 , LCDML_0         , 3  , "Anneal"      , mFunc_anneal);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (3  , LCDML_0         , 4  , "Settings"        , NULL);        // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_addAdvanced (4 , LCDML_0_4         , 1  , NULL,          ""                  , mDyn_holdTime,                0,   _LCDML_TYPE_dynParam);
//LCDML_addAdvanced (4 , LCDML_0_4         , 1  , NULL,          ""                  , mDyn_dutyCycle,                0,   _LCDML_TYPE_dynParam);
LCDML_addAdvanced (5 , LCDML_0_4         , 2  , NULL,          ""                  , mDyn_learningEnabled,                0,   _LCDML_TYPE_dynParam);
LCDML_add         (6 , LCDML_0_4         , 3  , "Reset learning"      , mFunc_resetLearning);
LCDML_add         (7 , LCDML_0_4       , 4  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab

/*
  // For beginners
  // LCDML_add(id, prev_layer, new_num, lang_char_array, callback_function)
  LCDML_add         (0  , LCDML_0         , 1  , "Information"      , mFunc_information);       // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (1  , LCDML_0         , 2  , "Time info"        , mFunc_timer_info);        // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (2  , LCDML_0         , 3  , "Program"          , NULL);                    // NULL = no menu function
  LCDML_add         (3  , LCDML_0_3       , 1  , "Program 1"        , NULL);                    // NULL = no menu function
  LCDML_add         (4  , LCDML_0_3_1     , 1  , "P1 dummy"         , NULL);                    // NULL = no menu function
  LCDML_add         (5  , LCDML_0_3_1     , 2  , "P1 Settings"      , NULL);                    // NULL = no menu function
  LCDML_add         (6  , LCDML_0_3_1_2   , 1  , "Warm"             , NULL);                    // NULL = no menu function
  LCDML_add         (7  , LCDML_0_3_1_2   , 2  , "Cold"             , NULL);                    // NULL = no menu function
  LCDML_add         (8  , LCDML_0_3_1_2   , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (9  , LCDML_0_3_1     , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (10 , LCDML_0_3       , 2  , "Program 2"        , mFunc_p2);                // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (11 , LCDML_0_3       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (12 , LCDML_0         , 4  , "Special"          , NULL);                    // NULL = no menu function
  LCDML_add         (13 , LCDML_0_4       , 1  , "Go to Root"       , mFunc_goToRootMenu);      // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (14 , LCDML_0_4       , 2  , "Jump to Time info", mFunc_jumpTo_timer_info); // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (15 , LCDML_0_4       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab


  // Advanced menu (for profit) part with more settings
  // Example for one function and different parameters
  // It is recommend to use parameters for switching settings like, (small drink, medium drink, big drink) or (200ml, 400ml, 600ml, 800ml) ...
  // the parameter change can also be released with dynParams on the next example
  // LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (16 , LCDML_0         , 5  , NULL,          "Parameter"      , NULL,                0,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (17 , LCDML_0_5       , 1  , NULL,          "Parameter 1"      , mFunc_para,       10,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (18 , LCDML_0_5       , 2  , NULL,          "Parameter 2"      , mFunc_para,       20,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (19 , LCDML_0_5       , 3  , NULL,          "Parameter 3"      , mFunc_para,       30,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_add         (20 , LCDML_0_5       , 4  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab


  // Example for dynamic content
  // 1. set the string to ""
  // 2. use type  _LCDML_TYPE_dynParam   instead of    _LCDML_TYPE_default
  // this function type can not be used in combination with different parameters
  // LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (21 , LCDML_0         , 6  , NULL,          ""                  , mDyn_para,                0,   _LCDML_TYPE_dynParam);                     // NULL = no menu function

  // Example for conditions (for example for a screensaver)
  // 1. define a condition as a function of a boolean type -> return false = not displayed, return true = displayed
  // 2. set the function name as callback (remove the braces '()' it gives bad errors)
  // LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (22 , LCDML_0         , 7  , COND_hide,  "screensaver"        , mFunc_screensaver,        0,   _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab
*/
// ***TIP*** Try to update _LCDML_DISP_cnt when you add a menu element.

// menu element count - last element id
// this value must be the same as the last menu element
//#define _LCDML_DISP_cnt    22
#define _LCDML_DISP_cnt    7
// create menu
LCDML_createMenu(_LCDML_DISP_cnt);

// Create servo
Servo myservo;

// OLD PID
PID myPID(&Input, &Output, &Setpoint, 100, 50, 1, DIRECT);

// *********************************************************************
// SETUP
// *********************************************************************
void setup()
{
  u8g2.begin();

  // serial init; only be needed if serial control is used
  Serial.begin(9600);                // start serial
  //Serial.println(F(_LCDML_VERSION)); // only for examples
  
  // Read from eeprom to initialise global variables (if saved)
  eepromOutput = EEPROM.readInt(0); // Hiold time
  if (eepromOutput >= 0 && eepromOutput < 5000) {
    holdTime = eepromOutput;
  }
  /*
  eepromOutput = EEPROM.readInt(1); // Duty Cycle
  if (eepromOutput >= 0 && eepromOutput < 100) {
    dutyCycle = eepromOutput;
  }
  */
  eepromOutput = EEPROM.readInt(2); // Anneal mode
  if (eepromOutput >= 0 && eepromOutput < 4) {
    annealMode = eepromOutput;
  }
  eepromOutput = EEPROM.readInt(34); // Anneal mode
  if (eepromOutput > 0) {
    nGlowCurrent = eepromOutput;
    //Serial.println(eepromOutput);
    //Serial.println(nGlowCurrent);

    eepromOutput = EEPROM.readInt(10); // Anneal mode
    if (eepromOutput >= 0) {
      varXGlowCurrent = float(eepromOutput) / 1000;
      //Serial.println(eepromOutput);
      //Serial.println(varXGlowCurrent);
    }
    eepromOutput = EEPROM.readInt(14); // Anneal mode
    if (eepromOutput) {
      covXYGlowCurrent = float(eepromOutput) / 1000;
      //Serial.println(eepromOutput);
      //Serial.println(covXYGlowCurrent);
    }
    eepromOutput = EEPROM.readInt(18); // Anneal mode
    if (eepromOutput) {
      meanGlowCurrentX = float(eepromOutput) / 1000;
      //Serial.println(eepromOutput);
      //Serial.println(meanGlowCurrentX);
    }
    eepromOutput = EEPROM.readInt(22); // Anneal mode
    if (eepromOutput) {
      meanGlowCurrentY = float(eepromOutput) / 1000;
      //Serial.println(eepromOutput);
      //Serial.println(meanGlowCurrentY);
    }
    eepromOutput = EEPROM.readInt(26); // Anneal mode
    if (eepromOutput) {
      glowCurrentCoefficient = float(eepromOutput) / 1000;
      //Serial.println(eepromOutput);
      //Serial.println(glowCurrentCoefficient);
    }
    eepromOutput = EEPROM.readInt(30); // Anneal mode
    if (eepromOutput) {
      glowCurrentConstant = float(eepromOutput) / 1000;
      //Serial.println(eepromOutput);
      //Serial.println(glowCurrentConstant);
    }
  }
  // LCDMenuLib Setup
  LCDML_setup(_LCDML_DISP_cnt);

  // Enable Menu Rollover
  LCDML.MENU_enRollover();

  // Enable Screensaver (screensaver menu function, time to activate in ms)
  LCDML.SCREEN_enable(mFunc_screensaver, 10000); // set to 10 seconds
  //LCDML.SCREEN_disable();

  // Some needful methods
  pinMode(CURRENT_SENSOR_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_PIN, INPUT);
  pinMode(ANNEAL_PIN, OUTPUT);
  pinMode(FEEDER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(ANNEAL_PIN, OFF);
  digitalWrite(FEEDER_PIN, OFF);
  digitalWrite(FAN_PIN, ON);

  myservo.attach(SERVO_PIN);
  myservo.writeMicroseconds(servoHomePos);
  servoCurrentPos = servoHomePos;
  servoTargetPos = servoHomePos;

  // You can jump to a menu function from anywhere with
  //LCDML.OTHER_jumpToFunc(mFunc_p2); // the parameter is the function name
}

// *********************************************************************
// LOOP
// *********************************************************************
void loop()
{
  // this function must called here, do not delete it
  LCDML.loop();
  //Serial.println(analogRead(CURRENT_SENSOR_PIN));

  moveServo();
}


// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_glcd_tft_box_x1 > _LCDML_glcd_tft_w)
# error _LCDML_glcd_tft_box_x1 is to big
# endif

# if(_LCDML_glcd_tft_box_y1 > _LCDML_glcd_tft_h)
# error _LCDML_glcd_tft_box_y1 is to big
# endif

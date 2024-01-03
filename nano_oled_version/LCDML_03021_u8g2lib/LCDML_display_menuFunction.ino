/* ===================================================================== *
 *                                                                       *
 * Menu Callback Function                                                *
 *                                                                       *
 * ===================================================================== *
 *
 * EXAMPLE CODE:

// *********************************************************************
void your_function_name(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    //LCDML_UNUSED(param);
    // setup
    // is called only if it is started

    // starts a trigger event for the loop function every 100 milliseconds
    LCDML.FUNC_setLoopInterval(100);

    // uncomment this line when the menu should go back to the last called position
    // this could be a cursor position or the an active menu function
    // GBA means => go back advanced
    //LCDML.FUNC_setGBA() 

    //
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop
    // is called when it is triggered
    // - with LCDML_DISP_triggerMenu( milliseconds )
    // - with every button or event status change

    // uncomment this line when the screensaver should not be called when this function is running
    // reset screensaver timer
    //LCDML.SCREEN_resetTimer();

    // check if any button is pressed (enter, up, down, left, right)
    if(LCDML.BT_checkAny()) {
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // loop end
    // you can here reset some global vars or delete it
    // this function is always called when the functions ends.
    // this means when you are calling a jumpTo ore a goRoot function
    // that this part is called before a function is closed
  }
}


 * ===================================================================== *
 */

// *********************************************************************
void mFunc_screensaver(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // setup function
    u8g2.setFont(_LCDML_DISP_font);
    u8g2.firstPage();
    do {
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), "screensaver");
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "press any key");
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "to leave it");
    } while( u8g2.nextPage() );

    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      LCDML.FUNC_goBackToMenu();  // leave this function
    }
  }

  if(LCDML.FUNC_close())          // ****** STABLE END *********
  {
    // The screensaver go to the root menu
    LCDML.MENU_goRoot();
  }
}



// *********************************************************************
void mFunc_back(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // end function and go an layer back
    LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
  }
}


// *********************************************************************
void mFunc_goToRootMenu(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // go to root and display menu
    LCDML.MENU_goRoot();
  }
}




unsigned long coolDownTarget = 0;
unsigned long homePosTime = 0;
unsigned long annealPosTime = 0;
unsigned long dropPosTime = 0;
unsigned long annealTargetTime = 0;
unsigned long endHoldTime;
unsigned long annealStartTime;
//int servoPosWait = 1000;
int present_vs_condition = 0;
int previous_vs_condition = 0;
int countDown;
int elapsed = 0;
//bool foundHomePos = false;
//bool foundAnnealPos = false;
int progress = 0;
float baseCurrent = 0;
float maxCurrent = 0;
float holdCurrent = 0;
bool gotBaseCurrent = false;
bool goodAnneal = false;
static bool pidAnnealPinOn = false;
bool displayChange = false;


// *********************************************************************
void mFunc_anneal(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    LCDML.FUNC_setLoopInterval(ANNEAL_LOOP_INTERVAL_MS);
    //LCDML.FUNC_setLoopInterval(500);
    //drawAnnealerScreen();
    u8g2.firstPage();
do {
//Serial.println("PASADAS");
} while ( u8g2.nextPage() );

    servoTargetPos = servoHomePos;

    annealStep = 0;

    // *********************************************************************
    // annealStep 0 init and prepare
    // annealStep 1 wait for case
    // annealStep 2 case detected, prepare stuff
    // annealStep 3 anneal loop
    // annealStep 4 target reached
    // annealStep 5 hold
    // annealStep 6 intermediate between 1 and 2, write to screen
    // annealStep 7 hold time over, drop case
    // annealStep 8 "Hack step" to avoid infinite loop in step 0!!
    // annealStep 10 panic
    // annealStep 20 Good anneal? Modify algorithm
    // *********************************************************************
    
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // reset screensaver timer
    LCDML.SCREEN_resetTimer();
    if (annealStep != 3 && annealStep != 5) {
      digitalWrite(ANNEAL_PIN, OFF);
    }
    if (annealStep == 1) {
      digitalWrite(FEEDER_PIN, ON);
    } else {
      digitalWrite(FEEDER_PIN, OFF);
    }

    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      displayChange = true;
      if (LCDML.BT_checkUp()) // check if button left is pressed
      {
        LCDML.BT_resetUp(); // reset the left button
        if (annealMode == 2) {
          if (annealStep != 20) {
            annealTime -= 50;
            if (annealTime <= 0) {
              annealTime = 0;
            }
            //drawAnnealerScreen();
          } else {
            goodAnneal = !goodAnneal;
          }
        }
      }
      if (LCDML.BT_checkDown()) // check if button left is pressed
      {
        LCDML.BT_resetDown(); // reset the left button
        if (annealMode == 2) {
          if (annealStep != 20) {
            annealTime += 50;
            if (annealTime >= 16000) {
              annealTime = 16000;
            }
            //drawAnnealerScreen();
          } else {
            goodAnneal = !goodAnneal;
          }
        }  
      }

      if (LCDML.BT_checkEnter()) {
        if (annealStep < 20) {
          if (annealStep != 10) {
            // Panic
            annealStep = 10;
            digitalWrite(ANNEAL_PIN, OFF);
            digitalWrite(FEEDER_PIN, OFF);
            if (servoCurrentPos == servoDropPos) {
              //myservo.detach();
            } else {
              //myservo.attach(SERVO_PIN);//pinMode(SERVO_PIN, OUTPUT);
              servoTargetPos = servoDropPos;
            }
            //drawAnnealerScreen();
          } else {
            annealStep = 0;
          }
        } else {
          // If good anneal modify algorithm
          if (annealStep == 20) {
            if (goodAnneal) {
              if (gotBaseCurrent) {
                updateGlowCurrentFormula();
                annealStep = 21;
              } else {
                annealStep = 0;
              }
            } else {
              annealStep = 0;
            }
          }
          //annealStep = 0;
        }
      }
    }
    

    if (annealStep == 0) {
      //unsigned int servoPosWait = 1000;
      goodAnneal = false;
      homePosTime = millis();
      //myservo.attach(SERVO_PIN);//pinMode(SERVO_PIN, OUTPUT);
      servoTargetPos = servoHomePos;
      //if (millis() >= (homePosTime + servoPosWait)) {
      /*
      if (millis() >= (homePosTime + 1000)) {  
        //Serial.println(F("Hesty"));
        myservo.detach();
        annealStep = 1;
        displayChange = true;
        //drawAnnealerScreen();
        present_vs_condition = digitalRead(VIBRATION_SENSOR_PIN);
      } else {
        //Serial.println(F("Testy"));
        //drawAnnealerScreen();
      }
      */
      annealStep = 8; //Hack to avoid endless loop (setting homepos time every time) AND avoid writing to display in status 1 (need update speed to register vibration)
      //present_vs_condition = digitalRead(VIBRATION_SENSOR_PIN);
      countDown = annealTime;
      elapsed = 0;
      bool foundHomePos = false;
      bool foundAnnealPos = false;
      elapsed=0;
      progress = 0;
      gotBaseCurrent = false;
    }

    if (annealStep == 1) {
      if (servoCurrentPos == servoHomePos) {
        previous_vs_condition = present_vs_condition;
        present_vs_condition = digitalRead(VIBRATION_SENSOR_PIN); // Reading digital data from the A5 Pin of the Arduino.
        if (previous_vs_condition != present_vs_condition) {
          annealStep = 6;
        }
      }
    }

    if (annealStep == 6) {
      //drawAnnealerScreen();
      digitalWrite(ANNEAL_PIN, OFF);
      annealStep = 2;
      annealPosTime = millis();
      //pinMode(SERVO_PIN, OUTPUT);
      //myservo.attach(SERVO_PIN);//pinMode(SERVO_PIN, OUTPUT);
      servoTargetPos = servoAnnealPos;
    }
    
  }

  if (annealStep == 2) {
    if (servoCurrentPos == servoAnnealPos) {
      //if (millis() > (annealPosTime + servoPosWait)) {
      if (millis() > (annealPosTime + 1000)) {
        //myservo.detach();//pinMode(SERVO_PIN, INPUT);
        if (millis() > coolDownTarget) {
          annealStep = 3;
          annealStartTime = millis();
          if (annealMode == 2) {
            annealTargetTime = millis() + annealTime;
            endHoldTime = annealTargetTime + holdTime; // add holdTime ms hold time
          }
        }
      }
    }
  }

  if (annealStep == 3) {
    digitalWrite(ANNEAL_PIN, ON);
    tmpCurrent = readCurrent();
    if (annealMode == 2) {
      elapsed = annealTime -(annealTargetTime - millis());
      //Serial.println(elapsed);
      if (elapsed > annealTime) {
        elapsed = annealTime;
      }
      progress = int(100 * float(elapsed) / float(annealTime));
      //drawAnnealerScreen();
        
      if (millis() >= annealTargetTime) {
        //digitalWrite(ANNEAL_PIN, OFF);
        // Let PID controller handle turning off ANNEAL_PIN
        if (elapsed > annealTime) {
          elapsed = annealTime;
          //drawAnnealerScreen();
        }
          elapsed = annealTime;
          //drawAnnealerScreen();
          annealStep = 4;
          holdCurrent = tmpCurrent;
      }
    }
    
    if (annealMode == 1 || annealMode == 3) {
      elapsed = millis()-annealStartTime;
      if (tmpCurrent>=maxCurrent && gotBaseCurrent) {
        annealStep = 4;
        holdCurrent = maxCurrent;
        endHoldTime = millis() + holdTime;
      }
      if (maxCurrent >0) {
        progress = int(100 * float(tmpCurrent-baseCurrent) / float(maxCurrent-baseCurrent));
      }
    }
    
    // Get initial current
    if (elapsed >= 200 && !gotBaseCurrent) {
      baseCurrent = tmpCurrent;
      gotBaseCurrent = true;
      if (annealMode == 3) { // Magic mode, predict from baseCurrent
        maxCurrent = calcMaxCurrent(baseCurrent);
        //Serial.print("Max: ");
        //Serial.println(maxCurrent);
      }
    }
        
        
        
      
  }

  if (annealStep == 4) { // Setup pid
    annealStep = 5;
    /* old pid relay stuff */
    windowStartTime = millis();

    //initialize the variables we're linked to
    Setpoint = holdCurrent;
    
    //tell the PID to range between 0 and the full window size
    myPID.SetOutputLimits(100, WindowSize);

    //turn the PID on
    myPID.SetMode(AUTOMATIC);
    Input = readCurrent();
    digitalWrite(ANNEAL_PIN, OFF);
    pidAnnealPinOn = false;
    
  }

  if (annealStep == 5) {
    if (millis() > endHoldTime) {
      annealStep = 7;
      digitalWrite(ANNEAL_PIN, OFF);
      //myservo.attach(SERVO_PIN);//pinMode(SERVO_PIN, OUTPUT);
      servoTargetPos = servoDropPos;
      dropPosTime = millis();
      coolDownTarget = millis() + annealTime * (100 - dutyCycle) / dutyCycle;
      //drawAnnealerScreen();
    } else {
      
      //Serial.println("PWM:");
      //Serial.println(holdCurrent);
      //Serial.println(Input);
      //Serial.println(Output);
      /* old pid stuff */
      if (pidAnnealPinOn) {
        // Measure current while anneal pin is on
        Input = readCurrent();
        //Serial.println(Input);
      }
      myPID.Compute();
      if (Output > WindowSize - 100) {
        Output = 100;
      }
      //Serial.println(Output);
      unsigned long now = millis();
      if (now - windowStartTime > WindowSize)
        { //time to shift the Relay Window
          windowStartTime += WindowSize;
        }
      if (Output > now - windowStartTime) {
        digitalWrite(ANNEAL_PIN, ON);
        pidAnnealPinOn = true;
        //Input = readCurrent();
        //Serial.print("HOLD ON");
        //Serial.println(Output);
      } else {
        digitalWrite(ANNEAL_PIN, OFF);
        pidAnnealPinOn = false;
        //Serial.println("HOLD OFF");
      } 
      
    }
  }

  if (annealStep == 7) {
    if (servoCurrentPos == servoDropPos) {
      if (millis()>dropPosTime+1000) {
        //myservo.detach();
        if (learningEnabled && annealMode != 3) {
          annealStep = 20; // Ask if anneal was good if not Magic mode
        } else {
          annealStep = 0;
        }
      }
    }
  }

  if (annealStep == 8) {
      //Serial.println(F("In step 8"));
      //if (millis() >= (homePosTime + servoPosWait)) {
      if (millis() >= (homePosTime + 2000)) {
        displayChange = true;
        annealStep = 1;
        present_vs_condition = digitalRead(VIBRATION_SENSOR_PIN);
      }
      if (millis() >= (homePosTime + 1000 && annealStep != 1)) {
        //myservo.detach();
        //Serial.println(F("In step 8, goto 1"));
        //annealStep = 1;
        //displayChange = true;
        //drawAnnealerScreen();
        //present_vs_condition = digitalRead(VIBRATION_SENSOR_PIN);
      } else {
        //drawAnnealerScreen();
        //Serial.println(F("In step 8, not ready yet"));
      }
    }

    if (annealStep == 10 && servoCurrentPos == servoDropPos) {
      //myservo.detach();
    }

  drawAnnealerScreen();
  

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
    //Serial.print(F("Closing"));
    digitalWrite(FEEDER_PIN, OFF);
    digitalWrite(ANNEAL_PIN, OFF);
  }
}

static bool shouldUpdate(bool force = false) {
  //Serial.println((millis()-lastDisplayUpdate));
  //Serial.println(annealMode);
  if ((millis()-lastDisplayUpdate)>50 && annealStep != 1 || force || annealStep == 1 && displayChange) {
    //Serial.println(F("Updating"));
    lastDisplayUpdate = millis();
    displayChange = false;
    return true;
  } else {
    //Serial.println(F("NOT Updating"));
    return false;
  }
}

// *********************************************************************
static void drawAnnealerScreen()
// *********************************************************************
{
  //Serial.println(annealStep);
  if (shouldUpdate()) {
    
    //char str_ready[5] = "Ready";
    u8g2.firstPage();
    do {
      u8g2.setFont(_LCDML_DISP_font);
      if (annealMode == 2) {
        if (annealStep < 10) {
          u8g2.drawStr(25, 15, "TARGET"); 
          //u8g2.setFont(u8g_font_10x20);
          u8g2.setFont(u8g2_font_fub17_tn); 
          //u8g2.setFont(u8g2_font_t0_22_tn);
          char buf[4];
          sprintf (buf, "%d ms", (annealTime-elapsed));
          u8g2.drawStr(25, 35, buf);
          u8g2.setFont(_LCDML_DISP_font);
          if (annealStep == 1) {
          
            u8g2.drawStr(25, 57, "READY");
          } else
          if (annealStep == 5) {
            u8g2.drawStr(25, 57, "HOLDING");
          } else {
            if (annealStep != 3) {
              u8g2.drawStr(25, 57, "WAIT");
            }
          }
        }
        
        if (annealStep == 20) {
          switch(goodAnneal)
            {
              case true:
              u8g2.drawStr(25, 40, "Good? YES");
              break;

              case false:
              u8g2.drawStr(25, 40, "Good? NO");
              break;
            }
        }
        if (annealStep == 21) {
          //char buf[10];
          //sprintf (buf, "%s", glowCurrentCoefficient);
          //sprintf(buf, "%f.4", glowCurrentCoefficient);
          //Serial.println(buf);
          //sprintf(buf, "%f.4", glowCurrentConstant);
          //Serial.println(buf);
          //Serial.println(nGlowCurrent);
          annealStep = 0;
        }
      }
      if (annealMode == 3) {
        if (annealStep < 10) {
          u8g2.setFont(_LCDML_DISP_font);
          if (annealStep == 1) {
          
            u8g2.drawStr(25, 57, "READY");
          } else
          if (annealStep == 5) {
            u8g2.drawStr(25, 57, "HOLDING");
          } else {
            if (annealStep != 3) {
             u8g2.drawStr(25, 57, "WAIT");
            }
            if (annealStep == 3) {
               u8g2.drawStr(25, 15, "TARGET"); 
          //u8g2.setFont(u8g_font_10x20);
          u8g2.setFont(u8g2_font_fub17_tn); 
          //u8g2.setFont(u8g2_font_t0_22_tn);
          //char buf[4];
          //sprintf (buf, "%d A", (maxCurrent));
          //u8g2.drawStr(25, 35, buf);
              
            }
          }
        }
      }  
      if (annealMode != 2) {
        if (annealStep == 3) {
          
        }
      }
      if (annealStep == 3) {
        u8g2.drawFrame(0, 50, 128, 10);
        u8g2.drawBox(10, 52, progress, 5);
      }
      if (annealStep == 10) {
        u8g2.drawStr(25, 40, "PANIC");
      }
    } while( u8g2.nextPage() );
    
  }
}

/*
void moveServoOld()
{

  if (servoCurrentPos == servoTargetPos) {
    
  } else {
    disableServo = false;
    pinMode(SERVO_PIN, OUTPUT);
  }

  //float speedAdjFactor = 1.29;
  float speedAdjFactor = 1.5;
  if (!disableServo) {
    if (servoCurrentPos != servoTargetPos && millis() >= myservo_movetime) {
      if (servoCurrentPos < servoTargetPos) {
        //servoDelay = 25 + 40 * (servoTargetPos - servoCurrentPos) / servoTargetPos;
        servoDelay = 40 * (servoTargetPos - servoCurrentPos) / servoTargetPos;
        //servoCurrentPos = servoTargetPos - (servoTargetPos - servoCurrentPos)/1.3;
        if (int((servoTargetPos - servoCurrentPos) / speedAdjFactor) <= 10) {
          servoCurrentPos += 1;
        } else {
          servoCurrentPos = servoTargetPos - (servoTargetPos - servoCurrentPos) / speedAdjFactor;
        }
       
        myservo.writeMicroseconds(servoCurrentPos);

        
      }
      if (servoCurrentPos > servoTargetPos) {
        //servoDelay = 25 + 40 * (servoCurrentPos - servoTargetPos) / servoCurrentPos;
        servoDelay = 40 * (servoCurrentPos - servoTargetPos) / servoCurrentPos;
        //servoCurrentPos = servoTargetPos + (servoCurrentPos - servoTargetPos)/1.3;
        if (int((servoCurrentPos - servoTargetPos) / speedAdjFactor) <= 10) {
          servoCurrentPos -= 1;
        } else {
          servoCurrentPos = servoTargetPos + (servoCurrentPos - servoTargetPos) / speedAdjFactor;
        }
       
        myservo.writeMicroseconds(servoCurrentPos);
        
      }
      
      myservo_movetime = millis() + servoDelay;
    }
  }
}
*/
// *********************************************************************
void moveServo()
// *********************************************************************
{

  if (servoCurrentPos == servoTargetPos) {
    if (!disableServo) {
      //Serial.println("Disabling servo");
      disableServo = true;
      myservo.detach();
    }
  } else {
    if (disableServo) {
      //Serial.println("Enabling servo");
      pinMode(SERVO_PIN, OUTPUT);
      myservo.attach(SERVO_PIN);//pinMode(SERVO_PIN, OUTPUT);
      disableServo = false;
    }
  }

  //float speedAdjFactor = 1.29;
  float speedAdjFactor = 1.5;
  servoDelay = 70;
  int servoStep = 400;
  if (!disableServo) {
    if (servoCurrentPos != servoTargetPos && millis() >= myservo_movetime) {
      if (servoCurrentPos < servoTargetPos) {
        //servoDelay = 25 + 40 * (servoTargetPos - servoCurrentPos) / servoTargetPos;
        //servoDelay = 40 * (servoTargetPos - servoCurrentPos) / servoTargetPos;
        //servoCurrentPos = servoTargetPos - (servoTargetPos - servoCurrentPos)/1.3;
        if (int((servoTargetPos - servoCurrentPos)) <= servoStep) {
          servoCurrentPos = servoTargetPos;
        } else {
          servoCurrentPos += servoStep;
        }
       
        myservo.writeMicroseconds(servoCurrentPos);

        
      }
      if (servoCurrentPos > servoTargetPos) {
        //servoDelay = 25 + 40 * (servoCurrentPos - servoTargetPos) / servoCurrentPos;
        servoDelay = 40 * (servoCurrentPos - servoTargetPos) / servoCurrentPos;
        //servoCurrentPos = servoTargetPos + (servoCurrentPos - servoTargetPos)/1.3;
        if (int((servoCurrentPos - servoTargetPos)) <= servoStep) {
          servoCurrentPos = servoTargetPos;
        } else {
          servoCurrentPos -= servoStep;
        }
       
        myservo.writeMicroseconds(servoCurrentPos);
        
      }
      
      myservo_movetime = millis() + servoDelay;
      //Serial.println(servoCurrentPos);
    }
  }
}

float readCurrent() {
  pinMode(CURRENT_SENSOR_PIN, INPUT) ;
  //Serial.println(analogRead(A1));
  int readings = 200;
  float average = 0;
  for (int i = 0; i < readings; i++) {
    average = average + analogRead(CURRENT_SENSOR_PIN)/ float(readings); //30A
    //average = average + ((analogRead(CURRENT_SENSOR_PIN) / 1024.0 * 5000 - 2500) / 100 )/ readings; //20A
    //5A mode, if 20A or 30A mode, need to modify this formula to
    //(.19 * analogRead(A0) -25) for 20A mode and
    //(.044 * analogRead(A0) -3.78) for 30A mode
  }
  average = average -512;
  if (average < 0) average *= -1;
  //Serial.println(average);
    //Serial.println(analogRead(CURRENT_SENSOR_PIN));

  return (average);
}

/*
float readCurrent_org() {
  pinMode(CURRENT_SENSOR_PIN, INPUT) ;
  //Serial.println(analogRead(A1));
  int readings = 200;
  float average = 0;
  for (int i = 0; i < readings; i++) {
    average = average + ((analogRead(CURRENT_SENSOR_PIN) / 1024.0 * 5000 - 2500) / 66) / readings; //30A
    //average = average + ((analogRead(CURRENT_SENSOR_PIN) / 1024.0 * 5000 - 2500) / 100 )/ readings; //20A
    //5A mode, if 20A or 30A mode, need to modify this formula to
    //(.19 * analogRead(A0) -25) for 20A mode and
    //(.044 * analogRead(A0) -3.78) for 30A mode
  }
  if (average < 0) average *= -1;
  //Serial.println(average);

  return (average);
}

float readCurrentOld() {
  int readings = 200;
  float average = 0;
  for (int i = 0; i < readings; i++) {
    average = average + (analogRead(CURRENT_SENSOR_PIN)*1000) / readings;
    //average += analogRead(CURRENT_SENSOR_PIN);
  }
  average = average/float(readings);
  //Serial.println(analogRead(CURRENT_SENSOR_PIN));
  //Serial.println(average);
  float current = (2.5 - (average * (5.0 / 1024.0)) )/0.066; //30A type
  //float current = (2.5 - (average * (5.0 / 1024.0)) )/0.1; //20A type
  if (current < 0) current *= -1;
 // Serial.println(current);
 // Serial.println(analogRead(CURRENT_SENSOR_PIN));
  return (current);;
}
*/


void updateGlowCurrentFormula() {
  char buf[10];
  int sampleMultiplier = 100;
  for (int i=0; i<sampleMultiplier; i++) {
    nGlowCurrent++;
    float dx = baseCurrent - meanGlowCurrentX;
    float dy = holdCurrent - meanGlowCurrentY;
    varXGlowCurrent += ((float(nGlowCurrent-1)/float(nGlowCurrent))*dx*dx - varXGlowCurrent)/float(nGlowCurrent);
    covXYGlowCurrent += (((float(nGlowCurrent-1))/float(nGlowCurrent))*dx*dy - covXYGlowCurrent)/float(nGlowCurrent);
    meanGlowCurrentX += dx/float(nGlowCurrent);
    meanGlowCurrentY += dy/float(nGlowCurrent);
    glowCurrentCoefficient = covXYGlowCurrent/varXGlowCurrent;
    glowCurrentConstant =  meanGlowCurrentY - glowCurrentCoefficient*meanGlowCurrentX;
  }
  EEPROM.writeInt(34, nGlowCurrent);
  //Serial.print("n: ");
  //Serial.println(nGlowCurrent);
  EEPROM.writeInt(10, int(varXGlowCurrent*1000));
  //Serial.print("varX ");
  //Serial.println(varXGlowCurrent);
  EEPROM.writeInt(14, int(covXYGlowCurrent*1000));
  //Serial.print("covXY: ");
  //Serial.println(covXYGlowCurrent);
  EEPROM.writeInt(18, int(meanGlowCurrentX*1000));
  //Serial.print("mean: ");
  //Serial.println(meanGlowCurrentX);
  EEPROM.writeInt(22, int(meanGlowCurrentY*1000));
  //Serial.println(meanGlowCurrentY);
  EEPROM.writeInt(26, int(glowCurrentCoefficient*1000));
  //Serial.print("coeff: ");
  //Serial.println(glowCurrentCoefficient);
  EEPROM.writeInt(30, int(glowCurrentConstant*1000));
  //Serial.print("const: ");
  //Serial.println(glowCurrentConstant);
  /*
  Serial.print(F("Coefficient: "));
  Serial.println(glowCurrentCoefficient);
  Serial.print(F("Constant: "));
  Serial.println(glowCurrentConstant);
  Serial.print("Actual: ");
  Serial.println(holdCurrent);
  Serial.print("Predicted: ");
  Serial.println(calcMaxCurrent(baseCurrent));
  */

}

float calcMaxCurrent(float baseCurrent) {
  float predicted = glowCurrentCoefficient * baseCurrent + glowCurrentConstant;
  if (predicted < 3) predicted = 0;
  return predicted;
}

void mFunc_resetLearning(uint8_t param) {
  nGlowCurrent = 0;
  varXGlowCurrent = 0;
  covXYGlowCurrent = 0;
  meanGlowCurrentX = 0;
  glowCurrentCoefficient = 0;
  glowCurrentConstant = 0;
  LCDML.MENU_goRoot();
}  

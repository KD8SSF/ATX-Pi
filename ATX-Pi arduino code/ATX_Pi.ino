/*
    License:
    Copyright (C) 2013  Ian W. Kephart (KD8SSF@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Purpose:
    This code enables the use of any standard ATX 2.0 power supply with a Raspberry Pi. 
    Gerber files for board production are avilable at: <https://github.com/KD8SSF/ATX-PI>
    Inspired by the "ATX Raspi" from Low Power Lab. Please see <http://lowpowerlab.com/atxraspi/#installation> 
    Script installation instructions available at www.kd8ssf.com/atx-pi
    Please note with this implementation connections to pins 7 and 8 on the RPi GPIO are the only ones required, 
    the power connection will be handled by the USB-A port on the board. 
*/


// Pin assignments
const int CaseSwitch = 2;   //PC case power switch
const int CasePowerLED = 3; //PC case Power LED
const int CaseHDDLED = 4;   //PC case HDD LED
const int STDBYLED = 13;    //On-board standby indicator
const int PWRONLED = 12;    //On-board PWRON indicator
const int PWROFFLED = 11;   //On-board PWROFF indicator
const int PSON = 8;         //output to ATX power supply pin 16
const int RPISD = 10;       //output to RPi to signal shutdown
const int HALTOK = 9;       //input from RPi on good shutdown



// Variables
int STATUS = 0;             //track STDBY/PWRON/PWROFF modes
int buttonState = 1;        //state of the Case switch
int button_delay = 0;        //how many seconds the button has been held down
int psStatus = 0;           //is the PSU on?

void setup () {
  //Serial.begin(9600);
  pinMode(CaseSwitch, INPUT);                //setup pin modes
  pinMode(CasePowerLED, OUTPUT);
  pinMode(CaseHDDLED, OUTPUT);
  pinMode(STDBYLED, OUTPUT);
  pinMode(PWRONLED, OUTPUT);
  pinMode(PWROFFLED, OUTPUT);
  pinMode(PSON, OUTPUT);
  pinMode(RPISD, OUTPUT);
  pinMode(HALTOK, INPUT);
  digitalWrite(PSON, HIGH);                  //set initial pin values
  digitalWrite(CasePowerLED, LOW);
  digitalWrite(CaseHDDLED, LOW);
  digitalWrite(STDBYLED, HIGH);
  digitalWrite(PWRONLED, LOW);
  digitalWrite(PWROFFLED, LOW);
  digitalWrite(RPISD, LOW);
  //Serial.println("Setup complete,starting loop");
}

void loop () {
  //Serial.println("Running Loop");
  //Serial.println(psStatus);
  
  buttonState = digitalRead(CaseSwitch);    //read the switch at the start of the loop
  
  while (buttonState == LOW) {
    //Serial.println("Button pressed!");
    button_delay++;
    //Serial.println(button_delay);
    delay(100);
    buttonState = digitalRead(CaseSwitch);
    
    if(button_delay >= 4) {
        //Serial.println("Sending shut-down signal to Pi, STATUS = 1");
        digitalWrite(PWRONLED, LOW);
        digitalWrite(PWROFFLED, HIGH);
        digitalWrite(RPISD, HIGH);
        STATUS = 1;
      }
    else
    
        if(button_delay == 2  && psStatus == 0) {
         //Serial.println("Turning PSU on");
         digitalWrite(STDBYLED, LOW);
         digitalWrite(PWRONLED, HIGH);
         digitalWrite(CasePowerLED, HIGH);
         digitalWrite(PSON, LOW);
         psStatus = 1;
         button_delay = 0;
         break; 
        }
  }
  
  switch (STATUS) {                          //PSU-OFF, waiting for the RPi to signal that it has halted
                                            
    case 1:                
      if (digitalRead(HALTOK) == LOW) {
        //Serial.println("Halt-OK, PSU off, STATUS = 0");
        delay(5000);                        //wait five seconds for the RPi to finish shutdown
        digitalWrite(PWROFFLED, LOW);
        digitalWrite(CasePowerLED, LOW);
        digitalWrite(PSON, HIGH);
        digitalWrite(STDBYLED, HIGH);
        digitalWrite(RPISD, LOW);
        psStatus = 0;
        button_delay = 0;
        STATUS = 0;
      }
      else {
        if (digitalRead(HALTOK) == HIGH) {   //blink the case "HDD activity" LED while waiting on the RPi HALT-OK signal
          //Serial.println("Waiting on Halt-OK, STATUS = 1");
          digitalWrite(CaseHDDLED, HIGH);
          delay(10);
          digitalWrite(CaseHDDLED, LOW);
          STATUS = 1;
        }  
      }
        break;
  } //switch end
  
}

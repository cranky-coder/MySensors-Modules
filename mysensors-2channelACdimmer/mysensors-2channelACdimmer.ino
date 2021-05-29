/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2019 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - January 30, 2015 - Developed by GizMoCuz (Domoticz)
 *
 * DESCRIPTION
 * This sketch provides an example how to implement a Dimmable Light
 * It is pure virtual and it logs messages to the serial output
 * It can be used as a base sketch for actual hardware.
 * Stores the last light state and level in eeprom.
 *
 */

// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95
#define MY_NODE_ID 25
#include <MySensors.h>

//dimmer stuff

#include <TimerOne.h>
#define channel_1 5 
#define channel_2 6
#define SPEED 3

#define GATE_IMPULSE 5

#define FREQ 65

unsigned int  CH1, CH2;
unsigned int  buf_CH1, buf_CH2;
unsigned char clock_cn;    
unsigned int  clock_tick;   
unsigned char i;  

// end dimmer stuff

#define CHILD_ID_LIGHT1 1
#define CHILD_ID_LIGHT2 2

#define EPROM_LIGHT1_STATE 1
#define EPROM_DIMMER1_LEVEL 2
#define EPROM_LIGHT2_STATE 3
#define EPROM_DIMMER2_LEVEL 4


#define LIGHT_OFF 0
#define LIGHT_ON 1

#define SN "Dimmable Light"
#define SV "1.0"

int16_t LastLightState1=LIGHT_OFF;
int16_t LastDimValue1=100;
int16_t LastLightState2=LIGHT_OFF;
int16_t LastDimValue2=100;

MyMessage lightMsg1(CHILD_ID_LIGHT1, V_LIGHT);
MyMessage dimmerMsg1(CHILD_ID_LIGHT1, V_DIMMER);
MyMessage lightMsg2(CHILD_ID_LIGHT2, V_LIGHT);
MyMessage dimmerMsg2(CHILD_ID_LIGHT2, V_DIMMER);


void before()
{
  pinMode(channel_1, OUTPUT);
  pinMode(channel_2, OUTPUT);
  attachInterrupt(1, zero_crosss_int, RISING);
  Timer1.initialize(16); 
  Timer1.attachInterrupt( timerIsr );

  
	//Retreive our last light state from the eprom
	LastLightState1=loadState(EPROM_LIGHT1_STATE);
  LastDimValue1=loadState(EPROM_DIMMER1_LEVEL);
	if (LastLightState1==LIGHT_ON) {
	  // last light state for light 1 = on.  so we need to set the light to on and the dimmer value to the stored dimmer value
    //check to see if state = on and dimmer =0, if so set to 100%
    if ((LastLightState1==LIGHT_ON)&&(LastDimValue1==0)) 
      {
         LastDimValue1=100; 
         //since we updated it we need to save it.
         saveState(EPROM_DIMMER1_LEVEL, LastDimValue1);
      }

    // if light state = off set dimmer value to 0
    if (LastLightState1 == LIGHT_OFF) {
      buf_CH1=DIMM_VALUE(0);  
    } else {
       // the light state = on, set it to the last dimmer value
      buf_CH1=DIMM_VALUE(LastDimValue1);
    }  	
	}


 LastLightState2=loadState(EPROM_LIGHT2_STATE);
  LastDimValue2=loadState(EPROM_DIMMER2_LEVEL);
 if (LastLightState2==LIGHT_ON) {
    // last light state for light 1 = on.  so we need to set the light to on and the dimmer value to the stored dimmer value
    //check to see if state = on and dimmer =0, if so set to 100%
    if ((LastLightState2==LIGHT_ON)&&(LastDimValue2==0)) 
      {
         LastDimValue2=100; 
         //since we updated it we need to save it.
         saveState(EPROM_DIMMER2_LEVEL, LastDimValue2);
      }

    // if light state = off set dimmer value to 0
    if (LastLightState2 == LIGHT_OFF) {
      buf_CH2=DIMM_VALUE(0);  
    } else {
       // the light state = on, set it to the last dimmer value
      buf_CH2=DIMM_VALUE(LastDimValue2);
    }   
  }
 
  
  
  Serial.println("Pre load state1:");
  Serial.println(LastLightState1);
  Serial.println("Pre load state2:");
  Serial.println(LastLightState2);
  Serial.println("Pre load dim1:");
  Serial.println(LastDimValue1);
  Serial.println("Pre load dim2:");
  Serial.println(LastDimValue2);
	
	ShowFullStates();

	Serial.println( "Node ready to receive messages..." );
}
void zero_crosss_int()
{
  
  CH1=buf_CH1;
   CH2=buf_CH2;
//    CH3=buf_CH3;
//     CH4=buf_CH4;
  
  clock_tick=0;        
}

void timerIsr()
{    
    
    clock_tick++;

    if (clock_cn) 
     {
      clock_cn++;
      
       if (clock_cn==GATE_IMPULSE)
       {
        digitalWrite(channel_1, LOW); 
        digitalWrite(channel_2, LOW); 
//        digitalWrite(channel_3, LOW); 
//        digitalWrite(channel_4, LOW);
        clock_cn=0;
       }
     }
   
        if (CH1==clock_tick)
         {
          digitalWrite(channel_1, HIGH);
          clock_cn=1;
         }  
    
           if (CH2==clock_tick)
            {
             digitalWrite(channel_2, HIGH);
             clock_cn=1;
            }  
        
//              if (CH3==clock_tick)
//               {
//                digitalWrite(channel_3, HIGH);
//                clock_cn=1;
//               }  
//    
//                 if (CH4==clock_tick)
//                  {
//                   digitalWrite(channel_4, HIGH);
//                   clock_cn=1;
//                  }   

                  
}

 

void presentation()
{
	// Send the Sketch Version Information to the Gateway
	sendSketchInfo(SN, SV);

	present(1, S_DIMMER );
  present(2, S_DIMMER );
  ShowFullStates();
}

void loop()
{
}

void receive(const MyMessage &message)
{
  // lets get the state of the message
  int lstate= atoi( message.data );
  //check what type of message incoming
  if (message.getType() == V_LIGHT) {
    //got on/off message
    Serial.println( "V_LIGHT command received..." );
    // check which sensor
    
    if (message.sensor == 1) {
      //got child sensor 1, lets save the state to eeprom
      LastLightState1=lstate;
      Serial.println("Save Light 1 State");
      saveState(EPROM_LIGHT1_STATE, LastLightState1);
      // if the state is set to on, we need to use the dimmer value, but if dimmer val = 0 we need to correct it so we set it to 100
      if ((LastLightState1==LIGHT_ON)&&(LastDimValue1==0)) {
        LastDimValue1 = 100;
        // since we are changing the last Dim Value we need to save to eeprom incase of power outage
        saveState(EPROM_DIMMER1_LEVEL, LastDimValue1);
      }
      // if state is on and dim value is NOT 0 then we set state to on and leave dim value alone

      // if state is off we set state to off but leave dimmer value saved dimmer value alone, but set current value to 0
      if (LastLightState1==LIGHT_OFF) {
        buf_CH1=DIMM_VALUE(LIGHT_OFF);
      } else {
        buf_CH1=DIMM_VALUE(LastDimValue1);
      }

    } else {
      //got child sensor 2, lets save the state to eeprom
      LastLightState2=lstate;
      Serial.println("Save Light 2 State");
      saveState(EPROM_LIGHT2_STATE, LastLightState2);
      // if the state is set to on, we need to use the dimmer value, but if dimmer val = 0 we need to correct it so we set it to 100
      if ((LastLightState2==LIGHT_ON)&&(LastDimValue2==0)) {
        LastDimValue2 = 100;
        // since we are changing the last Dim Value we need to save to eeprom incase of power outage
        saveState(EPROM_DIMMER2_LEVEL, LastDimValue2);
      }
      // if state is on and dim value is NOT 0 then we set state to on and leave dim value alone

      // if state is off we set state to off but leave dimmer value saved dimmer value alone, but set current value to 0
      if (LastLightState2==LIGHT_OFF) {
        buf_CH2=DIMM_VALUE(LIGHT_OFF);
      } else {
        buf_CH2=DIMM_VALUE(LastDimValue2);
      }
    }
    
  } else if (message.getType() == V_DIMMER) {
    //got dimmer message
    Serial.println( "V_DIMMER command received..." );
    // check which sensor
    if (message.sensor == 1) {
       LastDimValue1=lstate;
       if ((LastDimValue1<0)||(LastDimValue1>100)) {
         Serial.println( "V_DIMMER data invalid (should be 0..100)" );
         return;
        }

       // if the dimmer value = 0 lets ignore saving the dimmer value and lets just turn the light off
       if (LastDimValue1==0)
       {
          LastLightState1 = LIGHT_OFF;
          buf_CH1=DIMM_VALUE(LIGHT_OFF);
          saveState(EPROM_LIGHT1_STATE, LastLightState1);
       } else {
          // dimmer is something other than 0 set make sure the light is turned on and set dimmer
          Serial.println("here");
          LastLightState1 = LIGHT_ON;
          saveState(EPROM_LIGHT1_STATE, LastLightState1);
          saveState(EPROM_DIMMER1_LEVEL, LastDimValue1);
          buf_CH1=DIMM_VALUE(LastDimValue1);
       }
    } else {
       // got child sensor 2
       LastDimValue2=lstate;
       if ((LastDimValue2<0)||(LastDimValue2>100)) {
         Serial.println( "V_DIMMER data invalid (should be 0..100)" );
         return;
        }

       // if the dimmer value = 0 lets ignore saving the dimmer value and lets just turn the light off
       if (LastDimValue2==0)
       {
          LastLightState2 = LIGHT_OFF;
          buf_CH2=DIMM_VALUE(LIGHT_OFF);
          saveState(EPROM_LIGHT2_STATE, LastLightState2);
       } else {
          // dimmer is something other than 0 set make sure the light is turned on and set dimmer
          LastLightState2 = LIGHT_ON;
          saveState(EPROM_LIGHT2_STATE, LastLightState2);
          saveState(EPROM_DIMMER2_LEVEL, LastDimValue2);
          buf_CH2=DIMM_VALUE(LastDimValue2);
       }
    }
    
  } else {
    Serial.println( "Invalid command received..." );
    return;
  }
  
	//Here you set the actual light state/level
	ShowFullStates();
}
unsigned int DIMM_VALUE (unsigned char level)
{
 unsigned int buf_level;
 if (level > 100) {level=100;}
 level=map(level,100,0,0,255);


 if (level < 26)  {level=26;}
 if (level > 220) {level=220;}
  
 return ((level*(FREQ))/256)*10;  
}
void ShowFullStates()
{
	if (LastLightState1==LIGHT_OFF) {
		Serial.println( "Light state: OFF" );
	} else {
		Serial.print( "Light state: ON, Level: " );
		Serial.println( LastDimValue1 );
	}
 if (LastLightState2==LIGHT_OFF) {
   Serial.println( "Light state2: OFF" );
  } else {
    Serial.print( "Light state2: ON, Level: " );
    Serial.println( LastDimValue2 );
  }

	//Send current state to the controller
	SendCurrentState2Controller();
}

void SendCurrentState2Controller()
{
	if ((LastLightState1==LIGHT_OFF)||(LastDimValue1==0)) {
		send(dimmerMsg1.set((int16_t)0));
	} else {
		send(dimmerMsg1.set(LastDimValue1));
	}
 if ((LastLightState2==LIGHT_OFF)||(LastDimValue2==0)) {
   send(dimmerMsg2.set((int16_t)0));
  } else {
    send(dimmerMsg2.set(LastDimValue2));
  }
}

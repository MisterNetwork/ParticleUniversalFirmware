// This #include statement was automatically added by the Particle IDE.
#include "OB_LED_Flasher_IV/OB_LED_Flasher_IV.h"

// This #include statement was automatically added by the Particle IDE.
#include "DS18B20/DS18B20.h"


 /******************************************************************************
 ****************************************************************************
 * 
 *    U     U    N     N    I
 *    U     U    N N   N    I              11        222     000
 *    U     U    N  N  N    I               1       2   2   0   0
 *    U     U    N   N N    I     v   v     1          2    0   0
 *    U     U    N    NN    I      v v      1        2      0   0
 *     UUUUU     N     N    I       v     11111 []  22222    000     
 * 
 *        http://misternetwork.com
 * 
 *    eMU Universal Core / Photon / Electron K9 Mobile Sentry Firmware 
 *   ------------------------------------------------------------------ 
 * 
 * 
 ***************************************************************************
 ******************************************************************************
 * MultiFunction Function
 * MultiSwitch Multiple Switch Variable
 * MultiVal Multiple Value Variable
 * Temperature Sensing
 * Motion Sensing (REMOVED)
 * Pusingbox notifications (by device)  
 * debug function (0/1) 
 * LED Status Update every 60 seconds
 * LED Control - Flash cycle count, on and off flash rates
 * 
 ****************************************************************************
 ******************************************************************************
 *
 */

#if (PLATFORM_ID == 0)  // Core + ShieldShield V2.0
char * platformID = "Core";
DS18B20 ds18b20 = DS18B20(D7);
OneWire one = OneWire(D7);  //Temp Sensor D280 Pin
     int relay1=D3;  // high temp relay
     int relay2=A7;  // over temp relay
// Your secret DevID from PushingBox.com. You can use multiple DevID  on multiple Pin if you want
const char * DEVID1 = "<< v111 if applicable >>";         // Scenario: 111
const char * DEVID2 = "<< v112 if applicable >>";         // Scenario: 112
const char * DEVID3 = "<< v113 if applicable >>";         // Scenario: 113

#elif (PLATFORM_ID == 6)  // Photon + ShieldShield V3.0
char * platformID = "Photon";
DS18B20 ds18b20 = DS18B20(D7);
OneWire one = OneWire(D7);  //Temp Sensor D280 Pin
     int relay1=D3;  // high temp relay
     int relay2=A7;  // over temp relay
// Your secret DevID from PushingBox.com. You can use multiple DevID  on multiple Pin if you want
const char * DEVID1 = "<< v211 if applicable >>";         // Scenario: 211
const char * DEVID2 = "<< v212 if applicable >>";         // Scenario: 212
const char * DEVID3 = "<< v213 if applicable >>";         // Scenario: 213

#elif (PLATFORM_ID == 8) // P1 Sparkfun
char * platformID = "P1";

// Electron

#elif (PLATFORM_ID == 10) 
     char * platformID = "Electron";
     DS18B20 ds18b20 = DS18B20(D6);
     OneWire one = OneWire(D6);  //Temp Sensor D280 Pin
     int relay1=D3;  // high temp relay
     int relay2=A7;  // over temp relay
     //  Default values redefined below in  "" if (!startFlag){...""
     const char * DEVID1 = "<< v331 if applicable >>";         // Scenario: 331
     const char * DEVID2 = "<< v332 if applicable >>";         // Scenario: 332
     const char * DEVID3 = "<< v333 if applicable >>";         // Scenario: 333


#endif

//****************************************************************************


 char * ETRON1 = "<< 1st electron device ID >>";
 char * ETRON2 = "<< 2nd electron device ID >>";
 char * ETRON3 = "<< 3rd electron device ID >>";

const char * serverName = "api.pushingbox.com";   // PushingBox API URL
boolean heatState = false;             // Save the last state for DEVID1
boolean overheatState = false;         // Save the last state for DEVID2

String myID = System.deviceID();


uint8_t rom[8];
uint8_t resp[9];
//char szInfo[64];
char cTempF[64];

char highTempValS[64];
char overTempValS[64];
char remainTimer[64];


double celsius;
double fahrenheit;
int ifahrenheitHold=0;
int ifahrenheit;
unsigned int Metric_Publish_Rate = 30000;
unsigned int MetricnextPublishTime;
int DS18B20nextSampleTime;
int DS18B20_SAMPLE_INTERVAL = 5000;
int dsAttempts = 0;


//   MultiSwitch Variable String ------------------------
//  Current String switch/flag variables (in order):
//  (notifyEnable,relaysEnable,highTempOverride,overTempOverride,
//   highTempEnable,overTempEnable,ledEnable,sleepEnable,rTimerEnable)
//

char multiSwitch[64];

//   MultiVal Variable String ------------------------
//  Current String values variables (in order):
//  (highTempVal,overTempVal,rTimerVal)
//
char multiVal[64];
char wigValueS[64];

// Temperature settings
int highTempFlag =0;    //Flag for heatflag / D3 Relay 4 aux fan
int highTempVal = 97; // high temp trigger value
int highTempEnable = 1;
int highTempOverride = 0;

int overTempFlag =0; //Flag for overheat D1 Relay 2 Alert
int overTempVal = 115; // OverTemperature trigger value
int overTempEnable = 1;
int overTempOverride = 0;

int somestring;

// other
int wigValueI = 0;
int wigSwitch = 0;


int relaysEnable = 1;
int notifyEnable = 1;
int sleepEnable = 0;
int ledEnable = 1;
int rTimerEnable = 0;
int rTimerVal=15;
int unusedEnable=0;

int LED_Red_Flag=0;
int LED_Yellow_Flag=0;
int LED_Green_Flag=0;
int LED_Orange_Flag=0;
int LED_Rainbow_Flag=0;
int LED_Red_FFlag=0;
int LED_Yellow_FFlag=0;
int LED_Green_FFlag=0;
int LED_Orange_FFlag=0;



uint32_t UPDATE_INTERVAL = 10; // in milliseconds
uint32_t FIFTEEN_MINUTES = 60*15*1000; // in milliseconds
uint32_t lastTime = 0; // in milliseconds
uint32_t lastTime30Seconds = 0; // in milliseconds
uint32_t lastTimeOneMin = 0;
uint32_t startTime = 0; // in milliseconds
uint32_t remainTime = 0;

// int wigvalnum = 0;

//Flag on boot/restart only to avoid first-pass activation of notifications/relays
int startFlag=1;  


   TCPClient client;


// Debug mode
boolean DEBUG = false;



/* This function is called once at start up ----------------------------------*/
void setup()
{

    Particle.function("multiwig", multiwidget);
    Particle.function("debug", debug);
    Particle.function("relayDef", relayDef);

	Particle.variable("cTempF", cTempF, STRING);

    Particle.variable("multiSwitch", multiSwitch, STRING);
    Particle.variable("multiVal", multiVal, STRING);

    Particle.variable("highTempVal", highTempValS, STRING);    
    Particle.variable("overTempVal", overTempValS, STRING);    
    Particle.variable("remainTime", &remainTime, INT);

    Particle.variable("platformID", platformID, STRING);


    Serial.begin(9600);  


        Serial.print("DeviceID: ");
        Serial.println(myID);


 }

/* This function loops forever --------------------------------------------*/
void loop()
{



    if (millis() > DS18B20nextSampleTime){
        getTemp();
         }


//Timers ----------------------------------------------------Timers
    if(millis() - lastTime > UPDATE_INTERVAL) {
    // Set a new last time
    lastTime = millis();

   //   30 Second Timer
        if(millis() - lastTime30Seconds > 30000) {
         lastTime30Seconds = millis();
         }

//****** 1 Minute Timer ****************************************************************

        if(millis() - lastTimeOneMin > 60000) {
           lastTimeOneMin = millis();
    
    //  check temp change every minute  
        ifahrenheit = int(fahrenheit);
        

        if (((ifahrenheitHold - 2) > ifahrenheit) || (ifahrenheit > (ifahrenheitHold + 2)))
        {(ifahrenheitHold = ifahrenheit);

           if (platformID == "Core") Particle.publish("CTemp", cTempF, PRIVATE);
           else 
           if (platformID == "Photon") Particle.publish("PTemp", cTempF, PRIVATE);
           else 
           if (platformID == "Electron");
          }  

//  Status LED Update  

 if (ledEnable)  {
     
    LED_White_Flash(3,2,1,255);
    LED_Orange_Flash(1,5,2,255);


    if (LED_Red_Flag) LED_Red_Flash(5,9,4,96);
    if (LED_Yellow_Flag) LED_Yellow_Flash(5,9,4,96);
    if (LED_Green_Flag) LED_Green_Flash(5,9,4,96);
    if (LED_Orange_Flag) LED_Orange_Flash(5,9,4,96);
    if (LED_Rainbow_Flag) LED_Rainbow_Flash(3,96);
    if (LED_Red_FFlag) LED_Red_Flash(3,5,2,96);
    if (LED_Yellow_FFlag) LED_Yellow_Flash(3,5,2,96);
    if (LED_Green_FFlag) LED_Green_Flash(3,5,2,96);
    if (LED_Orange_FFlag) LED_Orange_Flash(3,5,2,96);

}



// Timer control
    if(rTimerEnable) { 
          sprintf(remainTimer, "%d", remainTime);
    
//    if (remainTimer == "4" or "8" or "12" or "14")
//     Particle.publish("ERTime", remainTimer, PRIVATE);
          }  
    }
    
//******** 1 Minute Timer End **************************************************************




      if(rTimerEnable) {
          remainTime = ((millis()-startTime)/60000);
         if(millis() - startTime > FIFTEEN_MINUTES) {

           if (platformID == "Core") {Particle.publish("CNote", "15MinExp", PRIVATE);
              sendToPushingBox(DEVID3);}    
           else 
           if (platformID == "Photon") {Particle.publish("PNote", "15MinExp", PRIVATE);
              sendToPushingBox(DEVID3);}    
           else 
           if (platformID == "Electron"){Particle.publish("ENote", "15MinExp", PRIVATE);
           } 
           LED_Rainbow_Flash(3);
           startTime = millis();
//           rTimerEnable=0;
           }
       }
    }

if (!startFlag){                    //Flag on boot/restart only to avoid first-pass activation of notifications/relays
    if (fahrenheit >= 0 && fahrenheit < 180)   //Validity Check
    {
       if (fahrenheit >= highTempVal)  
       {highTempFlag=1;
      Serial.println(">heatflag Y");   } 
       else  
       {highTempFlag=0;
//      Serial.println(">heatflag N");
        }
    
       if (fahrenheit >= overTempVal)  
       {overTempFlag=1;
       Serial.println(">overheatflag Y");   } 
             else 
       {overTempFlag=0;
//      Serial.println(">overheatflag N");
       }
    }
    
 }
else 
 { 
     
     
    if (platformID == "Core") {
               pinMode(relay1,OUTPUT);
               digitalWrite(relay1,HIGH);
               pinMode(relay2,OUTPUT);
               digitalWrite(relay2,HIGH);
               }    
           else 
        if (platformID == "Photon") {
//          any photon specific start stuff    
               }    
           else 
        if (platformID == "Electron"){
               pinMode(relay1,OUTPUT);
               digitalWrite(relay1,HIGH);
               pinMode(relay2,OUTPUT);
               digitalWrite(relay2,HIGH);
               }  
               
        Serial.print("startif : ");               
        Serial.println(myID);

// Differentiate between different Electron Devices

if (myID == "<< 3rd electron device ID >>"){
            Serial.println("electron3");
        Serial.print("DeviceID: ");
        Serial.println(myID);
        
     DS18B20 ds18b20 = DS18B20(D6);
     OneWire one = OneWire(D6);  //Temp Sensor D280 Pin
     relay1=D1;  // high temp relay
     relay2=D2;  // over temp relay
// Your secret DevID from PushingBox.com. You can use multiple DevID  on multiple Pin if you want
 DEVID1 = "<< v331 if applicable >>";         // Scenario: 331
 DEVID2 = "<< v332 if applicable >>";         // Scenario: 332
 DEVID3 = "<< v333 if applicable >>";         // Scenario: 333
    }

else
    if (myID == "<< 2nd electron device ID >>"){
        Serial.println("electron2");
        Serial.print("DeviceID: ");
        Serial.println(myID);
        
     DS18B20 ds18b20 = DS18B20(D6);
     OneWire one = OneWire(D6);  //Temp Sensor D280 Pin
     relay1=D1;  // high temp relay
     relay2=D2;  // over temp relay
// Your secret DevID from PushingBox.com. You can use multiple DevID  on multiple Pin if you want
 DEVID1 = "<< v321 if applicable >>";         // Scenario: 321
 DEVID2 = "<< v322 if applicable >>";         // Scenario: 322
 DEVID3 = "<< v323 if applicable >>";         // Scenario: 323
     }  


else
    if (myID == "<< 1st electron device ID >>") {
        Serial.println("electron1");
        Serial.print("DeviceID: ");
        Serial.println(myID);

     DS18B20 ds18b20 = DS18B20(D6);
     OneWire one = OneWire(D6);  //Temp Sensor D280 Pin
     relay1=D3;  // high temp relay
     relay2=A7;  // over temp relay
// Your secret DevID from PushingBox.com. You can use multiple DevID
 DEVID1 = "<< v311 if applicable >>";         // Scenario: 311
 DEVID2 = "<< v312 if applicable >>";         // Scenario: 312
 DEVID3 = "<< v313 if applicable >>";         // Scenario: 313
     }


    startFlag=0;
}


//  -------------- MAIN TEMPERATURE ACTION CONTROL LOGIC
//        Controls LEDs, Relays, and Notifications
//
// Listening for Temperature over limits to send notification to pushingbox
////
    if (!sleepEnable) {
        updateMultiSwitch();
        updateMultiVal();

     if (overTempFlag  && !overheatState ) // Overheat condition actions 
    {
        overheatState = 1;
       updateMultiSwitch();

       if (relaysEnable && overTempEnable) {
            pinMode(relay2,OUTPUT);
            if (platformID == "Photon"){digitalWrite(relay2, HIGH);}
          else 
            {digitalWrite(relay2, LOW);}  //turn on overheat alert relay
       } 
       
        if (ledEnable) {
            LED_Red_Flash(3,5,2);
            }

        LED_Red_FFlag=1;

        if (platformID == "Core"){Particle.publish("CNote", "OvrTemp", PRIVATE);
                        Serial.println("overtemp core publish");
              sendToPushingBox(DEVID2);
            }    
        else 
        if (platformID == "Photon") {Particle.publish("PNote", "OvrTemp", PRIVATE);
                        Serial.println("overtemp photon publish");
             sendToPushingBox(DEVID2);
            }    
        else 
        if (platformID == "Electron") {Particle.publish("ENote", "OvrTemp", PRIVATE);
                        Serial.println("overtemp electron publish");
        }
    }
 
    else
     if (highTempFlag  && !heatState) // High Temp condition actions 
     
    {
        heatState = 1;
        updateMultiSwitch();
       
         if (relaysEnable && highTempEnable){
          pinMode(relay1,OUTPUT);

          if (platformID == "Photon"){digitalWrite(relay1, HIGH);}
          else
             {digitalWrite(relay1, LOW);}  //turn on aux fan relay
          if (ledEnable)  {
             LED_Yellow_Flash(3,5,2);
            }
         }

         LED_Yellow_FFlag=1;
          
        // Sending request to PushingBox on High-Temp 

        if (platformID == "Core") {Particle.publish("CNote", "HiTemp", PRIVATE);
                Serial.println("hi temp core publish");
              sendToPushingBox(DEVID1);
              }    
        else 
        if (platformID == "Photon") {Particle.publish("PNote", "HiTemp", PRIVATE);
             Serial.println("hi temp photon publish");
             sendToPushingBox(DEVID1);
             }    
        else 
        if (platformID == "Electron") {Particle.publish("ENote", "HiTemp", PRIVATE);
        Serial.println("hi temp photon publish");
        }
    }
    else
    if (!overTempFlag  && overheatState ) // switch on pinDevid1 is OFF
    {
        overheatState = 0;
        updateMultiSwitch();
        
       delay(500);
      if (!overTempOverride){
           pinMode(relay2,OUTPUT);
          if (platformID == "Photon"){digitalWrite(relay2,LOW);} 
           else
           {digitalWrite(relay2, HIGH);}  //turn off overheat alert relay
        LED_reset(); 
        LED_Red_FFlag=0;
        }
    }
    else
     if (!highTempFlag && heatState) // switch on pinDevid1 is OFF
    {
        heatState = 0;
        updateMultiSwitch();
        
       delay(500);
      if (!highTempOverride){
       pinMode(relay1,OUTPUT);
       if (platformID == "Photon"){digitalWrite(relay1,LOW);}
       else
          {digitalWrite(relay1, HIGH);} //turn off aux fan relay
       LED_reset();
       LED_Yellow_FFlag=0;
      }
    }
   }
}       //void loop() End ***********************************************



//    Get Temperature  *************************************************

void  getTemp()
{

  if(!ds18b20.search())
      {
//      ds18b20.resetsearch();
      celsius = ds18b20.getTemperature();
//      Serial.println(celsius);
      while (!ds18b20.crcCheck() && dsAttempts < 4){
        Serial.println("Caught bad value.");
        dsAttempts++;
        Serial.print("Attempts to Read: ");
        Serial.println(dsAttempts);
        if (dsAttempts == 3){
//        Particle.publish("ENote", "Tmp Rst", PRIVATE);
        LED_Red_Flash(3,5,2);
          delay(1000);
        }
        ds18b20.resetsearch();
        celsius = ds18b20.getTemperature();
        continue;
      }
      dsAttempts = 0;
    if (ds18b20.crcCheck()){
      fahrenheit = ds18b20.convertToFahrenheit(celsius);
      DS18B20nextSampleTime = millis() + DS18B20_SAMPLE_INTERVAL;
      }
      if(DEBUG){ 
        Serial.print("read temp F: ");
                 Serial.println(fahrenheit);
        Serial.print("read temp C ");
                 Serial.println(celsius);
      }
    }    

    sprintf(cTempF, "%2.1f", fahrenheit);
    
    sprintf(highTempValS, "%d", highTempVal);
    sprintf(overTempValS, "%d", overTempVal);

   }



int debug (String debugswitch){
    

        Serial.println("Debug function: ");
        Serial.print("Relay1: ");
        Serial.println(relay1);
        Serial.print("Relay2: ");
        Serial.println(relay2);

        Serial.print("DevID1: ");
        Serial.println(DEVID1);
        Serial.print("DevID2: ");
        Serial.println(DEVID2);
        Serial.print("DevID3: ");
        Serial.println(DEVID3);
        Serial.print("PlatformID: ");
        Serial.println(platformID);
        Serial.print("DeviceID: ");
        Serial.println(myID);

        

    if (debugswitch=="1"){
        DEBUG=true;}
        else
        if (debugswitch=="0"){
            DEBUG=false;}
        }



void sendToPushingBox(const char * devid)
{

    Serial.print("closing... ");
    client.stop();
    if (client.connect(serverName, 80)) {
        client.print("GET /pushingbox?devid=");
        client.print(devid);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverName);
        client.println("User-Agent: Spark");
        //client.println("Connection: close");
        client.println();
        client.flush();
//        digitalWrite(LED, LOW);           // sets the LED off
    } 

    else{
//        digitalWrite(LED, HIGH);          // sets the LED on. If always on, last connexion was failed
        Serial.println("connection failed");
    }
}


//  Relay redefinition   *****************************************

int relayDef(String relayString)
   {
      //find command in string (first 2 characters)
     String relay1S = relayString.substring(0,2); 

    //find value in string (next 2 characters)
     String  relay2S = relayString.substring(2,4);


     //convert ascii to integer
	int pinNumberRelay1 = relay1S.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumberRelay1< 0 || pinNumberRelay1 >7) return -101;

	if(relay1S.startsWith("D"))
	{
     relay1 =	pinNumberRelay1;
	 }
	else if(relay1S.startsWith("A"))
	{
	  relay1 = pinNumberRelay1+10;
	 }


	int pinNumberRelay2 = relay2S.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumberRelay2< 0 || pinNumberRelay2 >7) return -102;

	if(relay2S.startsWith("D"))
	{
      relay2 =	pinNumberRelay2;
	  }
	else if(relay2S.startsWith("A"))
	{
	  relay2 = pinNumberRelay2+10;
	  }

        Serial.print("relay1 now: ");
        Serial.println(relay1);
        Serial.println(relay1S);


        Serial.print("relay2 now: ");
        Serial.println(relay2);
        Serial.println(relay2S);
    
    return 200;   

}



// Build or update MutiSwitch values ------------------------------------------------------
//
//
//  MultiSwitch format in the following order is:
//  [1]  notifyEnable   - str[0]
//  [2]  relaysEnable    - str[2]
//  [3]  highTempOverride    - str[4]
//  [4]  overTempOverride    - str[6]
//  [5]  highTempEnable    - str[8]
//  [6]  overTempEnable   - str[10]
//  [7]  ledEnable    - str[12]
//  [8]  sleepEnable    - str[14]
//  [9]  unusedEnable   - str[16]
//  [10]  rTimerEnable   - str[18]
//  [11] heatState   - str[20]
//  [12] overheatState   - str[22]
//  [nn] - <<<<  add more here  >>>>>>>  and to the sprintf below 
//  

void updateMultiSwitch() {

//Update MultiSwitch Values    

      sprintf(multiSwitch, "%u%u%u%u%u%u%u%u%u%u%u%u",notifyEnable, relaysEnable, highTempOverride, overTempOverride, highTempEnable, overTempEnable, ledEnable, sleepEnable, unusedEnable, rTimerEnable, heatState, overheatState);   
}


// Build or update MutiVal values ------------------------------------------------------
//
//
//  MultiVal format in the following order is:
//  [1]  highTempVal   - str[0]
//  [2]  overTempVal    - str[2]

//  [nn] - <<<<  add more here  >>>>>>>  and to the sprintf below 

void updateMultiVal() {

//Update MultiVal Values    

      sprintf(multiVal, "%u,%u,",highTempValS, overTempValS);   

}



// This function multiwidget gets called whenever there is a matching API request
/*

Command string format is:  function value switch 0/1 or parameter
                              xxxxxnnnnnn
 for example: DREADA100    (digitalRead pin A1)
              DWRITD201    (digitalWrite pin D2 - High/On)
              AWRITA201    (analogWrite pin2 - 255)


Functions:

DWRIT  digitalWrite
DREAD  digitalRead
AWRIT  analogWrite
AREAD  analogRead
HTEMP  High Temp setting and on/off
HTRLY  High Temp Relay Override on/off
OTEMP  Over Temp setting and on/off
OTRLY  Over Temp Relay Override on/off
NOTFY  PushingBox Notification on/off 
RELAY  Notofications Only - No relay response
TIMER  Timer reminder setting and on/off
NOLED  Stealth mode on/off
SLEEP  Shuts off all processing
/*/


int multiwidget(String wigString)
{

//find command in string (first 5 characters)
  String wigCommand = String (wigString.substring(0,5)) ;

//find value in string (next 3 characters)
  String wigValueS = (wigString.substring(5,8));

// Check command start letter for D or A to prevent converting first character of value to a number
  if (wigCommand.charAt(0) != 'D' && wigCommand.charAt(0) != 'A'){
     if(DEBUG){Serial.println(">>Command Not D or A:");} 
     wigValueI = wigValueS.toInt();
    }
  
//find switch value in string and convert ascii to integer

    String wigSwitchS = (wigString.substring(8,10));
    wigSwitch = wigSwitchS.toInt();

   if(DEBUG){Serial.print(">wigSwitch:");
            Serial.println(wigSwitch);
        Serial.print(">wigSwitchS:");
            Serial.println(wigSwitchS);
        Serial.print(">wigCommand:");
            Serial.println(wigCommand);
        Serial.print(">wigValueS:");
            Serial.println(wigValueS);
        Serial.print(">wigString:");
            Serial.println(wigString);
                }

// Digital Write    -------------------------------------------------------------Digital Write DWRIT 
   if (wigCommand == "DWRIT")   // Digital Write
   {
       	//convert ascii to integer
	int pinNumber = wigValueS.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(wigValueS.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, wigSwitch);
		return 1;
	}
	else if(wigValueS.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, wigSwitch);
		return 1;
	}
	
   }
    else   

// Digital Read     -------------------------------------------------------------Digital Read DREAD
   if (wigCommand == "DREAD")  
    {
         	//convert ascii to integer
	int pinNumber = wigValueS.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -2;

	if(wigValueS.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (wigValueS.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	
   }
    
    else   
    
// Analog Read      -------------------------------------------------------------Analog Read AREAD
   if (wigCommand == "AREAD")
     {  
    
 	//convert ascii to integer
	int pinNumber = wigValueS.charAt(1) - '0';
   
   if(DEBUG){ 
       Serial.println("AREAD:");
       Serial.print(">pinNumber:");
        Serial.print(pinNumber);}

	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -3;

	if(wigValueS.startsWith("D"))
	{
		pinMode(pinNumber, INPUT);
		return analogRead(pinNumber);
	}
	else if (wigValueS.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT);
		return analogRead(pinNumber+10);
	}        
        
     }
    else      
// Anlog Write      -------------------------------------------------------------Analog Write AWRIT
    if (wigCommand == "AWRIT")
      {
           //Pin and Value (0 to 255)
   
	//convert ascii to integer
	int pinNumber = wigValueS.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -4;

//	String aValueS = wigValueS.substring(3);

if(DEBUG){ 
       Serial.println("AWRIT:");
       Serial.print(">pinNumber:");
        Serial.print(pinNumber);
       Serial.print(">wigSwitch:");
        Serial.print(wigSwitch);

   }
	if(wigValueS.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, wigSwitch);
		return 1;
	}
	else if(wigValueS.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, wigSwitch);
		return 1;
	}          

      }
    else   
  
// High Temp setting and toggle   ------------------------------------------------High Temp HTEMP
    if (wigCommand == "HTEMP")   
      {
       if(DEBUG){ Serial.print(">HTEMP:");
        Serial.print(">highTempVal before:");
        Serial.print(highTempVal);}
        highTempEnable = wigSwitch; 
    if(DEBUG){Serial.print(">highTempEnable:");
        Serial.print(highTempEnable);}
        highTempVal = wigValueI;
      if(DEBUG){Serial.print(">highTempVal after:");
        Serial.println(highTempVal);}

        updateMultiSwitch();
         return 200;       
        
      }
   else   
    
// High Temp Relay Override    ---------------------------------------------------High Temp Relay HTRLY
    if (wigCommand == "HTRLY")   
    {
   if(DEBUG){Serial.print(">HTRLY:");}
       highTempOverride = wigSwitch; 

       updateMultiSwitch();
     if(DEBUG){  Serial.println(highTempOverride);  }  
    if (highTempOverride) {
        pinMode(relay1,OUTPUT);
        if (platformID == "Photon"){digitalWrite(relay1, HIGH);}
        else
           {digitalWrite(relay1, LOW);}  //turn on high temp aux fan  relay
        LED_Green_Flag=1;
         if (ledEnable) {
            LED_Green_Flash(3,9,4);
         }
           return 200; 
    }
    else
    {
           pinMode(relay1,OUTPUT);
           if (platformID == "Photon"){digitalWrite(relay1, LOW);}
           else
               {digitalWrite(relay1, HIGH);}  //turn off high temp /aux fan  relay
             LED_Green_Flag=0;
             LED_reset();
          return 200;     
    }
    }
    else   

// OverTemp setting and toggle  ---------------------------------------------------Over-Temp OTEMP
    if (wigCommand == "OTEMP")   
     {  
    if(DEBUG){Serial.print(">OTEMP:");}
        overTempEnable = wigSwitch; 
        overTempVal = wigValueI;
        updateMultiSwitch();
    if(DEBUG){Serial.println(overTempVal);}  
        return 200;  
     }
    else      

// OverTemp Relay Override    ------------------------------------------------------Over-Temp Relay OTRLY
    if (wigCommand == "OTRLY")   
      {
   if(DEBUG){Serial.print(">OTRLY:");}
       overTempOverride = wigSwitch; 
    if(DEBUG){Serial.println(overTempOverride);}

    updateMultiSwitch();    

    if (overTempOverride) {
        pinMode(relay2,OUTPUT);
        if (platformID == "Photon"){digitalWrite(relay2, HIGH);}
        else
             {digitalWrite(relay2, LOW);}  //turn on overheat  relay
        LED_Red_Flag=1;
         if (ledEnable) {
            LED_Red_Flash(3,9,4);
         }

           return 200;
    }
    else
    {
           pinMode(relay2,OUTPUT);
           if (platformID == "Photon"){digitalWrite(relay2, LOW);}
               else
               {digitalWrite(relay2, HIGH);}  //turn off overheat alert relay
           LED_Red_Flag=0;
           LED_reset(); 
           return 200;
     }
     }
    else   
    
// PushingBox Notification toggle  --------------------------------------------PushingBox Notify  NOTFY
    if (wigCommand == "NOTFY")   
    {
     if(DEBUG){ Serial.print(">NOTFY:");}
      notifyEnable = wigSwitch; 
        updateMultiSwitch();
     if(DEBUG){Serial.println(notifyEnable);}   
        return 200;
    }
    else   
    
// Relay Operation  toggle  ------------------------------------------------ Relay 
    if (wigCommand == "RELAY")   
    {
     if(DEBUG){ Serial.print(">RELAY:");}  
       relaysEnable = wigSwitch;
       updateMultiSwitch();       
     if(DEBUG){ Serial.println(relaysEnable); }   
       return 200;
    }
   else   

// Timer Reminder setting and  toggle  ------------------------------------------------ TIMER 
    if (wigCommand == "TIMER")   
    {
       if(DEBUG){ Serial.print(">TIMER:");}
       rTimerEnable = wigSwitch;
       updateMultiSwitch();
       if (rTimerEnable)
          {startTime = millis();}
       else
          {remainTime=0;}

       if (wigValueI >= 1 && wigValueI <= 60) 
        {rTimerVal = wigValueI;}
        else     
        {rTimerVal = 15;}
      if(DEBUG){ Serial.println(rTimerEnable);
         Serial.println(rTimerVal);}      
       return 200;
    }
   else   

// Stealth Mode toggle (all LED functions disabled)  --------------------------Stealth Mode  STEAL
    if (wigCommand == "NOLED")   
     {  
     if(DEBUG){ Serial.print(">NOLED:");}
       ledEnable = wigSwitch; 
        updateMultiSwitch();
      if(DEBUG){Serial.println(ledEnable); }  
        return 200;
     }
    else      

// Shuts off all processing   ---------------------------------------------------------SLEEP
if (wigCommand == "SLEEP")   
      {
     if(DEBUG){Serial.print(">SLEEP:");}
      sleepEnable = wigSwitch;
      updateMultiSwitch();
       if(DEBUG){ Serial.println(sleepEnable); }   
        return 200;     
      }

    else 
    
   return -99;
}   

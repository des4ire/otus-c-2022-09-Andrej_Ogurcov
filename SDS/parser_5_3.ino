
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h> 
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <ctype.h>
#include <stdlib.h>


Adafruit_MCP4725 MCP4725; 
Adafruit_ADS1115 ADS1115;
Adafruit_MCP4725 GATE_DAC;
byte address=0x11; //potentiometer address
int CS= 10;
int i=0;

const float multiplier = 0.1875F;

//##################################   headers  ###################################
char ctrl1[10]="scan";  //start scan of the array
//char ctrl2[10]="stop"; //stop scanning
char ctrl3[10]="setgsv"; //set gate-drain voltage
char ctrl4[10]="setdsv"; //set source-drain voltage
char ctrl5[10]="getvolt"; //get voltage feedback
char ctrl6[10]="setpot"; //set potentiometer
//char ctrl7[10]="run"; //run scan
char ctrl8[10]="setcell"; //set cell order
char ctrl9[10]="help";
char ctrl10[11]="reboot"; //reboot MCU

String input;
char temp_buffer[20];
char buffer[7][20]; //store tokenized data

float drain_voltage[64]; //

uint16_t scan_buffer[3]; //store scan settings, start cell, end cell
uint16_t gate_buffer[2];// store gate voltage settings
uint16_t drain_buffer[2];//storer  source-drain voltage value

const int M=5; //ints to pass buffer array to a function
const int N=20;

//boolean execute=true; //loop breaking condition
const int MUX_enable = 8; //enable MUX output
const int RESET = 9; //reset signal 


const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
float floatFromPC = 0.0;

boolean newData = false;

//============

void setup() {
 // digitalWrite(RESET, HIGH); // Set digital pin to 5V
 // pinMode(RESET, OUTPUT); // Set the digital pin to an OUTPUT pin
  
  Wire.begin(); // i2c sacnner
  Serial.begin(9600);
  Serial.println(F("SENSOR NODE, ID: 909345678"));
  
  pinMode(MUX_enable, OUTPUT); //MUX
  digitalWrite(MUX_enable, HIGH); //OUTPUT

  
   
  i2c_scann(); //search for i2c devices
 
  
  Serial.println();
  
  Serial.println(F("SN: Enter command:"));
    //Serial.begin(9600);
    //Serial.println("This demo expects 3 pieces of data - text, an integer and a floating point value");
    //Serial.println("Enter data in this style <HelloWorld, 12, 24.7>  ");
    Serial.println();

      DDRD=B11111100; //D port as output for matrix
      
      MCP4725.begin(0x60); //drain-source drain
      ADS1115.begin(0x48);

      ADS1115.setGain(GAIN_TWOTHIRDS); //set +/-6 volt range 
      
      GATE_DAC.begin(0x61); //gate dac
      MCP4725.setVoltage(0, false); //swith off source-drain voltage
      GATE_DAC.setVoltage(1400, false); //0V at gate 
      
      pinMode(CS, OUTPUT); //SPI chip select
      SPI.begin();
      //digitalPotWrite(0x00);
      //delay(100);
        // adjust  wiper in the  Mid point  .
      digitalPotWrite(0x80);
      delay(100);

      // adjust Lowest Resistance .
      //digitalPotWrite(0xFF);
      //delay(100);
      
     
      
}      

//============

//################################################ POTENTIOMETER SECTION ##################################
int digitalPotWrite(int value)
{
  digitalWrite(CS, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(CS, HIGH);
}
void pot(){
 for (i = 0; i <= 255; i++)
    {
      digitalPotWrite(i);
      delay(10);
    }
    delay(500);
    for (i = 255; i >= 0; i--)
    {
      digitalPotWrite(i);
      delay(10);
    }
}
//################################################ CELL SCAN SECTION ######################################

 
 void matrix(uint16_t period, uint8_t start_cell, uint8_t end_cell)  
{
  uint8_t i=0;
  uint8_t y=0;
  uint32_t MCP4725_value;
  uint8_t j=0; //cell counter
 
  //unsigned int c;
  //unsigned long timer;

    MCP4725.setVoltage(drain_buffer[0], false);   
    
    digitalWrite(MUX_enable, LOW); //enable mux output
   
    for(i=(start_cell-1); i<=(end_cell-1); i++)
    {
     j++;
      if (Serial.available()>0){
  
        String data =Serial.readString();
        data.trim();
          if (data=="<scan:OFF>"){
          Serial.println("###### Aborted! #######");
         break;
          }
          }
          
         PORTD=i<<2; //write data to the port  
         //Serial.println(get_voltage()); //read voltage values
         drain_voltage[i]=get_voltage();
        delay(period);
        }
    
               MCP4725.setVoltage(0, false); //swith off source-drain voltage
      
     
                digitalWrite(MUX_enable, HIGH); //disable mux output
                Serial.println("###### Scan complete! #######");
                Serial.println("Voltage drop on shunt:");
                for(j=(start_cell-1); j<i; j++)
                {
                
               Serial.print("Cell#");
               Serial.print(j+1);
               Serial.print(" ");
               Serial.println(drain_voltage[j]);
                  }
              ///drain_voltage[64]={0};  
    
    
    }
//######################################### gate voltage control ###########################
/*
void setgs(){
GATE_DAC.setVoltage(550, false); //-2V
delay(5000);
GATE_DAC.setVoltage(975, false); //-1V
delay(5000);
GATE_DAC.setVoltage(1187, false); //-0.5V
delay(5000);
GATE_DAC.setVoltage(1400, false); //0V
delay(5000);
GATE_DAC.setVoltage(1825, false); //1v
delay(5000);
GATE_DAC.setVoltage(2037, false); //1.5v
delay(5000);
GATE_DAC.setVoltage(2250, false); //2V
delay(5000);
}
*/
//######################################### source-drain voltage controle ##########################3
  /*
  void setds(){
    uint32_t MCP4725_value;
    for (MCP4725_value = 0; MCP4725_value < 4096; MCP4725_value = MCP4725_value + 15)
    {
     
      MCP4725.setVoltage(MCP4725_value, false);
      delay(50);
     
      
      //Serial.print("MCP4725 Value: ");
      //Serial.print(MCP4725_value);
      
    }
    MCP4725.setVoltage(0, false); //swith off source-drain voltage
  }
  */
//########################################### ADC control ################################################
float get_voltage()
{
  
  int16_t adc0, adc1, adc2, adc3;
 
  // ads.setGain(GAIN_TWOTHIRDS);  +/- 6.144V  1 bit = 0.1875mV (default)
  // ads.setGain(GAIN_ONE);        +/- 4.096V  1 bit = 0.125mV
  // ads.setGain(GAIN_TWO);        +/- 2.048V  1 bit = 0.0625mV
  // ads.setGain(GAIN_FOUR);       +/- 1.024V  1 bit = 0.03125mV
  // ads.setGain(GAIN_EIGHT);      +/- 0.512V  1 bit = 0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    +/- 0.256V  1 bit = 0.0078125mV 
  
  adc0 = ADS1115.readADC_SingleEnded(0);
 // Serial.println(adc0);
 // adc1 = ADS1115.readADC_SingleEnded(1);
 // adc2 = ADS1115.readADC_SingleEnded(2);
 // adc3 = ADS1115.readADC_SingleEnded(3);
 // Serial.print("AIN0: "); Serial.println(adc0 * multiplier);
 // Serial.print("AIN1: "); Serial.println(adc1 * multiplier);
 // Serial.print("AIN2: "); Serial.println(adc2 * multiplier);
 // Serial.print("AIN3: "); Serial.println(adc3 * multiplier);
  //Serial.println(" ");
  return (adc0 * multiplier);
  //delay(1000);
  
  
 // } 
}

//################################### help section ##################################################
/*
void help(){
  Serial.println("Availiable commands:");
  Serial.println("setcell - scan options for sensor array."); 
  Serial.println(          "Arguments: period (50-2000 ms), start cell (1-64), end cell (1-64)");
  Serial.println(          "Example:<scan:1:10:60>");
  Serial.println("setdsv -  sets voltage level of source-drain channel"); 
  Serial.println(          "Arguments: voltage (0-4500 mV)");
  Serial.println(          "Example:<setdsv:3500>");
  
  }
 */
//################################## parser section ###################################################

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}



//============

void parsedata(){
  char * token; // this is used by strtok() as an index
  //char * token; // this is used by strtok() as an index
  int len=0;
  uint8_t counter = 0;
  int res;

  token = strtok(tempChars,":");      // get the first part - the string
   while (token != NULL) {
        //Serial.println(token);
        strcpy(buffer[counter], token);
        Serial.println(buffer[counter]);
        token = strtok(NULL, " : ");
        counter++;
       
    }
      Serial.print(F(" SN: Received number of tokens: ")); Serial.println(counter);
        
        
        //######################################## matrix parser #######################################      
        
        if ((res = strcmp(buffer[0], ctrl8)==0) && (counter != 4) )
        {
         Serial.println("Not enough arguments for <setcell> function!");
         Serial.println("Please specify delay (range: 50-2000 ms), start cell and end cell (range: 1 - 64) followed by <setcell>");
         Serial.println("Example: <setcell:250:5:50>");
         
          }
        else if ( (res = strcmp(buffer[0], ctrl8)==0) && (counter = 4) )
                {
               
                    if( (50<=atoi(buffer[1]) && atoi(buffer[1])<=2000) && (1<=atoi(buffer[2]) && atoi(buffer[2])<=64) && (1<=atoi(buffer[3]) && atoi(buffer[3])<=64) && (atoi(buffer[2]) <= atoi(buffer[3]))   )
                    {
                      
                      Serial.println("Setcell settings are OK!");
                           scan_buffer[0]=atoi(buffer[1]);
                           scan_buffer[1]=atoi(buffer[2]);
                           scan_buffer[2]=atoi(buffer[3]);
                           Serial.println(scan_buffer[0]);
                           Serial.println(scan_buffer[1]);
                           Serial.println(scan_buffer[2]);
                           //matrix(scan_buffer[0], scan_buffer[1], scan_buffer[2]);
                      }
                  
                   else
                    {
                      Serial.println("Setcell arguments are not valid!");
                      //Serial.println(buffer[1]);
                    }
  
              }
             
   
          //####################################### gate parser #####################################
   
         
        if((res = strcmp(buffer[0], ctrl3)==0) && (counter != 2) )
          {
         Serial.println("Not enough arguments for <setgsv> function!");
         Serial.println("Please specify voltage value (range: -2000/+2000 mV) followed by <setgsv>"); // setgsv
         Serial.println("Example: <setgsv:1000>");
        
          }
        else if ((res = strcmp(buffer[0], ctrl3)==0) && (counter = 2) )
                
                {
               
                    if(   (-2000<=atoi(buffer[1]) && atoi(buffer[1])<=2000)     )
                    {
                      Serial.println("Gate settings are OK!");
                          
                          gate_buffer[0]=(2000+(atoi(buffer[1])))/2.35+550;
                          
                           Serial.println(gate_buffer[0]);
                          
                   }
                  
                   else
                    {
                      Serial.println("Setgsv arguments are not valid!");
                      //Serial.println(buffer[1]);
                    }
  
              }
       //############################################## drain-source parser #################################
     
        if((res = strcmp(buffer[0], ctrl4)==0) && (counter != 2) ) ///1500 mV max equal to 1230 digit
          {
         Serial.println("Not enough arguments for <setdsv> function!");
         Serial.println("Please specify voltage value (range: 0 - 1500mV mV) followed by <setgsv>"); // setgsv
         Serial.println("Example: <setdsv:1000>");
        
          }
        else if ((res = strcmp(buffer[0], ctrl4)==0) && (counter = 2) )
                
                {
               
                    if(   (0<=atoi(buffer[1]) && atoi(buffer[1])<=4500)     )
                    {
                      Serial.println("Source-drain settings are OK!");
                          drain_buffer[0]=atoi(buffer[1])/1.220;
                          
                           Serial.println(drain_buffer[0]);
                          
                   }
                  
                   else
                    {
                      Serial.println("Setgsv arguments are not valid!");
                      //Serial.println(buffer[1]);
                    }
  
              }
       
       
       // ############################################### run scan  #####################################
          if((res = strcmp(buffer[0], ctrl1)==0) && (counter != 2) ) //
          {
           Serial.println("Command not valid!");
           Serial.println("Please specify voltage value (range: 0 - 1500mV mV) followed by <setgsv>"); // setgsv
           Serial.println("Example: <scan:ON>");
        
          }
             else if ((res = strcmp(buffer[0], ctrl1)==0) && (counter = 1) )
                
                {
               
                    if(   (res=strcmp(buffer[1], "ON")==0))
                    {
                      Serial.println("Running scan!");
                      matrix(scan_buffer[0], scan_buffer[1], scan_buffer[2]);    
                   } 
                  
                    else
                    {
                      Serial.println("Command not valid!");
                      
                    }
  
              }        
          if((res = strcmp(buffer[0], ctrl10)==0) && (counter = 1) ) //
          {
          reboot();
          //Serial.println("YO!");
          }        
          


}

void parse(){         
   recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        //parseData();
        parsedata();
        //showParsedData();
        newData = false;
  
  }
}
//########################################## reboot section ######################################
/*
void reboot(){
Serial.println("Rebooting...");
delay(250);
digitalWrite(RESET, LOW); //grounds RESET pin causing Nano to restart
}

*/
/*
void(* resetFunc) (void) = 0;

void reboot() {
  resetFunc();
}
*/


void reboot() {  //reboot using WDT
  Serial.println("Rebooting...");
  delay(2000);
  wdt_disable();
  wdt_enable(WDTO_30MS);
  
  while (1) {}
}

/*
void reboot() 
{ asm volatile ("jmp 0"); 
}
*/
//###############################################################################################
/*
void showParsedData() {
    Serial.print("Message ");
    Serial.println(messageFromPC);
    Serial.print("Integer ");
    Serial.println(integerFromPC);
    Serial.print("Float ");
    Serial.println(floatFromPC);
}
*/

//####################################### I2C device scanner ################################
void i2c_scann(){
  Serial.println("\nI2C Scanner");
   byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("Done\n");
  }
  delay(500);          
  }

//#################################################################################################
void loop() {
    parse();
    
  /*  recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        //parseData();
        parsedata();
        //showParsedData();
        newData = false;
        
    }
    */
     
}

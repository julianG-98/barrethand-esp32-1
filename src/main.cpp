//Barett hand  by Julian Guzman

#include <Arduino.h>        //library to use arduino ide  for programing platform
#include <ESP32Servo.h>		//library for ESP32 to use servos with PWM
#include "AX12A.h"			//open source library for arduino to use servo AX12A(uses a serial protocol)

//creat 3 servo objects for the pwm servos
Servo servo1; 
Servo servo2;
Servo servo3;

int servo1pin = 12;
int servo2pin = 14;
int servo3pin = 27;

// assign the pins for the set up of AX-12

#define DirectionPin 	(2u) 		// pin for the switch of thr tri buffer 
#define BaudRate  		(9600ul)    // baudarate
#define ID				(254u)		//every AX-12 has an ID form 0 to 253 (this value can change ussingf funtions from the library) the value 254 forces every servo to follow the comand    


#define RXD2 16			//comunication pins  for serialport 2
#define TXD2 17
#define GPS_BAUD 9600

HardwareSerial gpsSerial(2); //seting up serial cominication channel 2 (RX2 TX2)

// object name hand which has the psition of the 3 fingers and the thumb
class hand
{
public:
 int posd1 ;
 int posd2 ;
 int posd3 ;
 int posP ;
	
};


// funtion that returns a object hand and recieved a string as and input
// this funtion breaks  teh input mesage into  4 numbers (3 position of the fingers 1 the thumb position)
// expected input should look like  A**B**C**P** (A90B90C90P) A, B & C rpresent the fingers and P the thumb  
//fingers range 150 -70 thumb  810 - 210 (0 - 180)

hand separateData( String mess){
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
	int c4 = 0;

	int a = 0;
	int b = 0;
	String letter[4];
	hand r;
	
	for (int i = 0; i <= mess.length() ; i++){
		if(mess[i]=='A'){
			c1=i;
			//Serial.println(i);
		}
		if(mess[i]=='B'){
			c2=i;
			//Serial.println(i);
		}
		if(mess[i]=='C'){
			c3=i;
			//Serial.println(i);
		}
		if(mess[i]=='P'){
			c4=i;
			//Serial.println(i);
		}
	}
	
	for(int y=0; y<4; y++){
	switch (y)
	{
	case 0:
		a=c1+1;
		b=c2-1;
		break;

	case 1:
		a=c2+1;
		b=c3-1;
		break;
	
	case 2:
		a=c3+1;
		b=c4-1;
		break;
	
	case 3:
		a=c4+1;
		b=mess.length();
		break;
	default:
		break;
	}

	for(int u = a; u<=b; u++){
		letter[y]+=mess[u];
	}

	}

	r.posd1=letter[0].toInt();
	r.posd2=letter[1].toInt();
	r.posd3=letter[2].toInt();
	r.posP=letter[3].toInt();

	return r;

	}


hand handone ; //object that represents the hand

String buff ="";
String receivedMessage = "";  // Variable to store the complete message
char incomingChar ;


void setup()
{
 Serial.begin(9600); // serial comunication with serialport 1 (pc <--> microcontroler )
 
 gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2); // set up serial port 2 (microcontroler <--> AX-12)
 ax12a.begin(BaudRate, DirectionPin, &gpsSerial);   // set up servo AX-12
 
 // set up pwm servos 
 servo1.attach(servo1pin);
 servo2.attach(servo2pin);
 servo3.attach(servo3pin);

 Serial.println("ESP32 is ready");
}

void loop() {
	

  // Check if data is available in the Serial buffer
  while (Serial.available()) {
	incomingChar = Serial.read();  // Read each character from the buffer
    
    if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
      
	 handone = separateData(receivedMessage); //separet the message 
	
	 servo1.write(handone.posd1); // m ove avery servo to the position mention in the message 
	 servo2.write(handone.posd2);
	 servo3.write(handone.posd3);
	 ax12a.move(ID, handone.posP);
	  

      // Clear the message buffer for the next input
      receivedMessage = "";
    } 
	else {
      // Append the character to the message string
      receivedMessage += incomingChar;
    }
  }
	
}


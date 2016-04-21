/*
Greetings all from Chris Gozzard ZS1CDG. 

This is a mashup of two arduino sketches, an AD9850 test script 
and Simple morse Beacon. See documentation below.
The CW TX frequency is currently set for 28.222Mhz but I suggest changing to somewhere
else on the beacon section of the 10m Band.

Please note you need an Amateur radio licence to transmit. 

Have fun, 73 de ZS1CDG
*/


//
// Simple Arduino Morse Beacon
// Written by Mark VandeWettering K6HX
// Email: k6hx@arrl.net
// 
// This code is so trivial that I'm releasing it completely without 
// restrictions.  If you find it useful, it would be nice if you dropped
// me an email, maybe plugged my blog @ http://brainwagon.org or included
// a brief acknowledgement in whatever derivative you create, but that's
// just a courtesy.  Feel free to do whatever.
//
/*
 * A simple single freq AD9850 Arduino test script
 * Original AD9851 DDS sketch by Andrew Smallbone at www.rocketnumbernine.com
 * Modified for testing the inexpensive AD9850 ebay DDS modules
 * Pictures and pinouts at nr8o.dhlpilotcentral.com
 * 9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
 * Use freely
 */
 
 struct t_mtab { char c, pat; } ;
 
 struct t_mtab morsetab[] = {
  	{'.', 106},
	{',', 115},
	{'?', 76},
	{'/', 41},
	{'A', 6},
	{'B', 17},
	{'C', 21},
	{'D', 9},
	{'E', 2},
	{'F', 20},
	{'G', 11},
	{'H', 16},
	{'I', 4},
	{'J', 30},
	{'K', 13},
	{'L', 18},
	{'M', 7},
	{'N', 5},
	{'O', 15},
	{'P', 22},
	{'Q', 27},
	{'R', 10},
	{'S', 8},
	{'T', 3},
	{'U', 12},
	{'V', 24},
	{'W', 14},
	{'X', 25},
	{'Y', 29},
	{'Z', 19},
	{'1', 62},
	{'2', 60},
	{'3', 56},
	{'4', 48},
	{'5', 32},
	{'6', 33},
	{'7', 35},
	{'8', 39},
	{'9', 47},
	{'0', 63}
} ;

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))

#define FREQ (28222000) //Set to 28.222 Mhz at the moment 
#define SPEED  (12)

#define DOTLEN  (1200/SPEED)
#define DASHLEN  (3*(1200/SPEED))

void
dash()
{
  sendFrequency(FREQ);  // freq
  delay(DASHLEN);
  sendFrequency(FREQ+1000);  // freq
  delay(DOTLEN) ;
}
void
dit()
{
  sendFrequency(FREQ);  // freq
  delay(DOTLEN);
  sendFrequency(FREQ+1000);  // freq
  delay(DOTLEN);
}

void
send(char c)
{
  int i ;
  if (c == ' ') {

    delay(7*DOTLEN) ;
    return ;
  }
  for (i=0; i<N_MORSE; i++) {
    if (morsetab[i].c == c) {
      unsigned char p = morsetab[i].pat ;
      Serial.print(morsetab[i].c) ;

      while (p != 1) {
          if (p & 1)
            dash() ;
          else
            dit() ;
          p = p / 2 ;
      }
      delay(2*DOTLEN) ;
      return ;
    }
  }
  /* if we drop off the end, then we send a space */

}

void
sendmsg(char *str)
{
  while (*str)
    send(*str++) ;

}

 #define W_CLK 8       // Pin 8 - connect to AD9850 module word load clock pin (CLK)
 #define FQ_UD 9       // Pin 9 - connect to freq update pin (FQ)
 #define DATA 10       // Pin 10 - connect to serial data load pin (DATA)
 #define RESET 11      // Pin 11 - connect to reset pin (RST).
 
 #define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
 
 // transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}
 
 // frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}
 
void setup() {
 // configure arduino data pins for output
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
 
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);  // this pulse enables serial mode - Datasheet page 12 figure 10
}
 
void loop() {
  sendmsg("VVV DE YOURCALL  "); //Change YOURCALL to your call sign
  delay(5000) ;
}

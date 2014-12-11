 // This program is free software: you can redistribute it and/or modify
 // it under the terms of the GNU General Public License as published by
 // the Free Software Foundation, either version 3 of the License, or
 // (at your option) any later version.
 //
 // This program is distributed in the hope that it will be useful,
 // but WITHOUT ANY WARRANTY; without even the implied warranty of
 // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 // GNU General Public License for more details.
 //
 // You should have received a copy of the GNU General Public License
 // along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <Encoder.h>
//IO:
static unsigned rLed=9, gLed=6, bLed=5, encA=2, encB=3, encBTN=4;

//scaling:
static unsigned hFactor=3, vFactor=5, scrollFactor=4;

//colour values:
static unsigned s=255; //full sat, all the time
unsigned h=0, v=255;

Encoder myEnc(encA, encB);
long oldPosition  = -999;


void setup()
{
	pinMode(rLed,OUTPUT);
	pinMode(gLed,OUTPUT);
	pinMode(bLed,OUTPUT);

	pinMode(encBTN,INPUT);
	//pin mode for enc pins is handled by encoder library

	//while(!Serial); //leonardo specific
	Serial.begin(9600);

  	//nice and white.
	analogWrite(rLed,200);
  	analogWrite(gLed,0);
  	analogWrite(bLed,0);

  	Mouse.begin();
  	//Keyboard.begin();
}

int change=0;
int hChange=0;
int vChange=0;

void loop()
{
	unsigned char r, g, b;

	unsigned long newPosition = myEnc.read();

  	if (newPosition != oldPosition)
  	{
  		change += newPosition-oldPosition;
		oldPosition = newPosition;
  	}

	//if(change!=0)
	if(change<-3 || change>3) //divide ticks by four
	{

  		if(digitalRead(encBTN))  //VALUE
  		{
  			vChange=change*vFactor;
  			if((signed)v+vChange<0) v=0; //prevent int underflow
  			else if(v+vChange>255) v=255; //prevent int overflow
  			else v+=vChange;
  		}

  		else //HUE
  		{
   			hChange=change*hFactor;
  			if((signed)h+hChange<0) h=1541; //prevent int underflow
            else if(h+hChange>1541) h=0; //prevent int overflow
  			else h+=hChange;

			Mouse.move(0,0,-change/scrollFactor);
			//if(change<0) Keyboard.press(KEY_PAGE_UP);
			//else Keyboard.press(KEY_PAGE_DOWN);
  		}

  		hsv_to_rgb(&r, &g, &b);
		analogWrite(rLed,255-r);
  		analogWrite(gLed,255-g);
  		analogWrite(bLed,255-b);


		Serial.print("pos: ");
    	Serial.println(newPosition);
    	Serial.print("change: ");
    	Serial.println(change);
    	Serial.print(" r: ");
    	Serial.println(r);
    	Serial.print(" g: ");
    	Serial.println(g);
    	Serial.print(" b: ");
    	Serial.println(b);
		Serial.print(" h: ");
    	Serial.println(h);
		Serial.print(" v: ");
    	Serial.println(v);
    	Serial.print("Scroll: ");
    	Serial.println(-change/scrollFactor);
    	Serial.println();

  		change=0;
	}
}

/*
------------------------------------------------------------------------------
 HSV to RGB conversion
------------------------------------------------------------------------------
 H [0..1541]	angle 0 == 0deg, 1541 < 360deg
		sextants: [0..256], [257..513], [514..770], [771..1027], [1028..1284], [1285..1541]
	8-bit(+1) per sextant
		~0.2335 degrees per count
		This is the highest resolution possible with 8 bit target colors and is already
		slightly higher than necessay (max resolution is ~6 * 256). However, using the
		current setup makes calculation a lot easier by using 8-bit shifts.
 S [0..255]
 V [0..255]
*/
void hsv_to_rgb(/*unsigned h, unsigned s, unsigned v, */unsigned char *r, unsigned char *g, unsigned char *b)
{
	unsigned frac = h;
	unsigned char red, green, blue;

	if(!s)
		s++;	/* This fixes border case, marked !! below */
	if(h < 257) {
		frac -= 0;
		red   = v;
		green = (v * (unsigned char)(~((s * (256-frac)) >> 8))) >> 8;
		blue  = (v * (unsigned char)(~s + 1)) >> 8;
	} else if(h < 514) {
		frac -= 257;
		red   = (v * (unsigned char)(~((s * frac) >> 8))) >> 8;
		green = v;
		blue  = (v * (unsigned char)(~s + 1)) >> 8;
	} else if(h < 771) {
		frac -= 514;
		red   = (v * (unsigned char)(~s + 1)) >> 8;
		green = v;
		blue  = (v * (unsigned char)(~((s * (256-frac)) >> 8))) >> 8;
	} else if(h < 1028) {
		frac -= 771;
		red   = (v * (unsigned char)(~s + 1)) >> 8;
		green = (v * (unsigned char)(~((s * frac) >> 8))) >> 8;
		blue  = v;
	} else if(h < 1285) {
		frac -= 1028;
		red   = (v * (unsigned char)(~((s * (256-frac)) >> 8))) >> 8;
		green = (v * (unsigned char)(~s + 1)) >> 8;
		blue  = v;
	} else {
		frac -= 1285;
		red   = v;
		green = (v * (unsigned char)(~s + 1)) >> 8;
		blue  = (v * (unsigned char)(~((s * frac) >> 8))) >> 8;
	}
	*r = red;
	*g = green;
	*b = blue;
// FIXME: We should not do a linear search in the if() clauses, but do bisection
// to make the timing more stable and slightly faster...
}

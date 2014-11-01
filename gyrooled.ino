/*

  Universal 8bit Graphics Library, http://code.google.com/p/u8glib/
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
 ~~~~
This code is used for connecting arduino to serial mpu6050 module, and test in arduino uno R3 board.
 connect map:
 arduino   mpu6050 module
 VCC    5v/3.3v
 TX     RX<-0
 TX     TX->1
 GND    GND
 note: 
 because arduino download and mpu6050 are using the same serial port, you need to un-connect 6050 module when you want to download program to arduino.
 Created 14 Nov 2013
 by Zhaowen
 
 serial mpu6050 module can be found in the link below:
 http://item.taobao.com/item.htm?id=19785706431
 */

#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
int apointx = 32;
int apointy = 32;
int  arline =20;
int pi = 3.1415;
float ax ;
int bpointx = 84;
int bpointy = 32;
int  brline = 20;
float bx ;


unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float a[3],w[3],angle[3],T;
void setup() {
  // initialize serial:
  Serial.begin(115200);
  Serial1.begin(115200);
  //u8g
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
}


void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 2, 10, "angle_0");
    u8g.drawStr( 70, 10, "angle_1");
  u8g.drawLine(apointx+arline*sin(ax*pi/180), apointy-arline*cos(ax*pi/180), apointx, apointy+20);
  u8g.drawLine(bpointx+brline*sin(bx*pi/180), bpointy-brline*cos(bx*pi/180), bpointx, bpointy+20);
}



void loop() {
  if(sign)
  {  
    sign=0;
    if(Re_buf[0]==0x55)      //检查帧头
    {  
      switch(Re_buf [1])
      {
      case 0x51:
        a[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*16;
        a[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*16;
        a[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*16;
        T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
        break;
      case 0x52:
        w[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*2000;
        w[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*2000;
        w[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*2000;
        T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
        break;
      case 0x53:
        angle[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*180;
        angle[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*180;
        angle[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*180;
        T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
        //Serial.print("a:");
        //Serial.print(a[0]);Serial.print(",");
        //Serial.print(a[1]);Serial.print(",");
        //Serial.print(a[2]);Serial.print(",");
        //Serial.print("w:");
        //Serial.print(w[0]);Serial.print(",");
        //Serial.print(w[1]);Serial.print(",");
        //Serial.print(w[2]);Serial.print(",");
        //Serial.print("angle:");
        //Serial.print(angle[0]);Serial.print(",");
        //Serial.println(angle[1]);
        //Serial.print(",");
        //Serial.print(angle[2]);
        //Serial.println("");
        //Serial.print("T:");
        //Serial.println(T);
        ax = round(angle[1]);
        bx = round(angle[0]);
        // picture loop
        u8g.firstPage();  
        do {
          draw();
        } 
        while( u8g.nextPage() );

        // rebuild the picture after some delay
        delay(50);
        break;
      } 
    }
  } 



}


void serialEvent1() {
  while (Serial1.available()) {

    //char inChar = (char)Serial1.read(); Serial.print(inChar); //Output Original Data, use this code 

      Re_buf[counter]=(unsigned char)Serial1.read();
    if(counter==0&&Re_buf[0]!=0x55) return;      //第0号数据不是帧头              
    counter++;       
    if(counter==11)             //接收到11个数据
    {    
      counter=0;               //重新赋值，准备下一帧数据的接收 
      sign=1;
    }

  }
}




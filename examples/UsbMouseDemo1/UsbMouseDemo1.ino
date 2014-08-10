/*
UsbMouse
Library written (a lot of copy/paste) by Meir Michanie
based on UsbKeyboard and usbdrv mouse sample code.

*/
#include "UsbMouse.h"

#define LEFTBUTTON_PIN 12
#define MIDDLEBUTTON_PIN 11
#define RIGHTBUTTON_PIN 10
// If the timer isr is corrected
// to not take so long change this to 0.
#define BYPASS_TIMER_ISR 1

void setup() {
  pinMode(LEFTBUTTON_PIN, INPUT);
  digitalWrite(LEFTBUTTON_PIN, HIGH);
  pinMode(MIDDLEBUTTON_PIN, INPUT);
  digitalWrite(MIDDLEBUTTON_PIN, HIGH);
  pinMode(RIGHTBUTTON_PIN, INPUT);
  digitalWrite(RIGHTBUTTON_PIN, HIGH);
#if BYPASS_TIMER_ISR
  // disable timer 0 overflow interrupt (used for millis)
  TIMSK0 &= !(1 << TOIE0); // ++
#endif
  UsbMouse.update();
}


void loop() {
  demo2();
  UsbMouse.update();
#if BYPASS_TIMER_ISR  // check if timer isr fixed.
  delayMs(20);
#else
  delay(20);
#endif

}

void demo1(){
    UsbMouse.move(random(10) -5,random(10) -5,0);
}

void demo2(){
    UsbMouse.set_buttons(!digitalRead(LEFTBUTTON_PIN), !digitalRead(RIGHTBUTTON_PIN), !digitalRead(MIDDLEBUTTON_PIN));
}

#if BYPASS_TIMER_ISR
void delayMs(unsigned int ms) {
  /*
  */
  for (int i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
}
//
#endif

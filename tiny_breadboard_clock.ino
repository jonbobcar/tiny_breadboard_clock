#include <avr/io.h>
#include <avr/interrupt.h>
#include <TinyWireM.h>
#include <TinyRTClib.h>

const int latch = 5;
const int b1 = 2;
const int data = 7;
const int clk = 8;

int number[] = {
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110  // 9
};

int digit[] = {
  B00000000, // blank
  B10000000, // first
  B01000000, // second
  B00001000, // third
  B00000100, // fourth
};

int buttonState = HIGH;
int lastButtonState = HIGH;
int update_state = false;
unsigned long debounce = 0;
unsigned long debounceDelay = 50;

volatile int hour_val = 12;
volatile int minute_val = 34;
volatile int second_val = 0;
unsigned long previous_millis = 0;
const long interval = 1000;

RTC_DS1307 rtc;

void setup() {
  // put your setup code here, to run once:
  pinMode(latch, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(b1, INPUT);

  TinyWireM.begin();
  
  rtc.begin();
  rtc.adjust(DateTime(2020, 10, 29, hour_val, minute_val, 0));

}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(b1);

  unsigned long current_millis = millis();
  
  if (current_millis > interval) {
    update_state = true;
    DateTime now = rtc.now();           // Get the RTC info
    hour_val = now.hour();              // Get the hour
    minute_val = now.minute();          // Get the minute
    previous_millis = current_millis;
  }

  if (minute_val == 1) {
    update_state = false;
  }

  if(hour_val > 12) hour_val -= 12;   // Convert to 12 hour clock
  if(hour_val == 0) hour_val = 12;    // Convert to 12 hour clock


  if (buttonState == LOW && millis() > debounce) {
    debounce = millis() + debounceDelay;
    if (minute_val < 60) {
      minute_val++;
      rtc.adjust(DateTime(2020, 10, 29, hour_val, minute_val, 0));
    } else {
      hour_val++;
      minute_val = 0;
      rtc.adjust(DateTime(2020, 10, 29, hour_val, minute_val, 0));
    }
  }

   digitalWrite(latch, LOW);
   if ((hour_val / 10 % 10) == 0) {
    void();
   } else {
   shiftOut(data, clk, LSBFIRST, digit[1]);
   shiftOut(data, clk, LSBFIRST, number[(hour_val / 10) % 10]);
   digitalWrite(latch, HIGH);
   }
   digitalWrite(latch, LOW);
   shiftOut(data, clk, LSBFIRST, digit[2]);
   shiftOut(data, clk, LSBFIRST, number[hour_val % 10]);
   digitalWrite(latch, HIGH);
   digitalWrite(latch, LOW);
   shiftOut(data, clk, LSBFIRST, digit[3]);
   shiftOut(data, clk, LSBFIRST, number[(minute_val / 10) % 10]);
   digitalWrite(latch, HIGH);
   digitalWrite(latch, LOW);
   shiftOut(data, clk, LSBFIRST, digit[4]);
   shiftOut(data, clk, LSBFIRST, number[minute_val % 10]);
   digitalWrite(latch, HIGH);
}

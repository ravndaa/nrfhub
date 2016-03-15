#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>             //http://playground.arduino.cc/Code/Time
#include <Wire.h>
#include "LowPower.h"


// Use pin 2 as wake up pin
const int wakeUpPin = 2;

volatile boolean alarmIsrWasCalled = false;
void wakeUp()
{
    // Just a handler for the pin interrupt.
    alarmIsrWasCalled = true;
}

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8
RF24 radio(7, 8);


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D3LL };

//
// Payload
//

const int min_payload_size = 4;
const int max_payload_size = 32;

//
//  Sensor ID
//

const String sensorId = "02";
char receive_payload[max_payload_size + 1]; // +1 to allow room for a terminating NULL char

void setup(void)
{

  
  Serial.begin(115200);
  Serial.println(F("Temprature Sensor"));

  setSyncProvider(RTC.get);
    Serial.println("RTC Sync");
    if (timeStatus() != timeSet){
        Serial.println("Fail");
    }

  pinMode(wakeUpPin, INPUT_PULLUP);
  //RTC.squareWave(SQWAVE_NONE); 
  //RTC.setAlarm(ALM2_MATCH_MINUTES, 0, 30, 0, 0);    //Every hour when minutes is 30
  RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 1, 0);    //Every minute
  RTC.alarm(ALARM_2);                   //ensure RTC interrupt flag is cleared
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, true);

  radio.begin();
  radio.stopListening();
  // enable dynamic payloads
  radio.enableDynamicPayloads();
  radio.setPALevel(RF24_PA_MAX);

  // optionally, increase the delay between retries & # of retries
  //radio.setRetries(0, 0);

  radio.openWritingPipe(pipes[0]);


  attachInterrupt(0, wakeUp, LOW); 

  Serial.println("Setup Done");
  delay(1000);
}

void loop(void)
{

  sleep();  
  

  Serial.println("Outside alarm");
      if ( RTC.alarm(ALARM_2) ){
        
        radio.powerUp();
        radio.stopListening();
  
        //String str = "/Sensor/" + sensorId + "," + readTemp() + "+" + readVcc();
        String str = "/Sensor/" + sensorId + "," + readTemp() + "+" + readVcc();
        Serial.println(str);
        // Length (with one extra character for the null terminator)
        int str_len = str.length() + 1;
  
        // Prepare the character array (the buffer)
        char char_array[str_len];
  
        // Copy it over
        str.toCharArray(char_array, str_len);
  
        // Take the time, and send it.  This will block until complete
        Serial.print(F("Now sending length: "));
        Serial.println( sizeof(char_array) );
  
        radio.write( char_array, sizeof(char_array) );
  
        Serial.println("Data Sent");

        radio.powerDown();
        delay(500);
      }

      
}

void sleep()
{
      attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);    
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
      detachInterrupt(digitalPinToInterrupt(2));
      
}

int readTemp()
{
  int t = RTC.temperature();
  float celsius = t / 4.0;
  int tmp = (int) celsius;
  //Serial.println(tmp);
  return tmp;
}


long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}


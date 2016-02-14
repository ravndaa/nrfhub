#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8

RF24 radio(7,8);

// sets the role of this unit in hardware.  Connect to GND to be the 'pong' receiver
// Leave open to be the 'ping' transmitter
const int role_pin = 5;

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Payload
//

const int min_payload_size = 4;
const int max_payload_size = 32;


char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

void setup(void)
{
  
  delay(20); // Just to get a solid reading on the role pin

  Serial.begin(115200);
  Serial.println(F("Sensor"));


  radio.begin();

  // enable dynamic payloads
  radio.enableDynamicPayloads();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  radio.openWritingPipe(pipes[0]);
  //radio.openReadingPipe(1,pipes[0]);

  radio.printDetails();
}

void loop(void)
{
    static char send_payload[] = "/kos";
    
    // Take the time, and send it.  This will block until complete
    Serial.print(F("Now sending length "));
    Serial.println( sizeof(send_payload) );
    radio.write( send_payload, sizeof(send_payload) );

    // Try again 1s later
    delay(500);
  
}



#include "temperature_conversion.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "lib/rf24/RF24.h"
#include <vector>

using namespace std;

// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL };

//const int min_payload_size = 4;
const int max_payload_size = 32;
//const int payload_size_increments_by = 1;
//int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

int main(int argc, char** argv){


  // Print preamble:
  cout << "RFHUB/\n";

  // Setup and configure rf radio
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(5,15);
  radio.printDetails();

    radio.openReadingPipe(1,pipes[0]);
    radio.startListening();

    class mqtt_tempconv *tempconv;
	int rc;

	mosqpp::lib_init();

	tempconv = new mqtt_tempconv("HUB", "config.re", 1883);


    std::vector<string> vec;
    
    
// forever loop
	while (1)
	{


    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      uint8_t len;

      while (radio.available())
      {
        // Fetch the payload, and see if this was the last one.
	len = radio.getDynamicPayloadSize();
	radio.read( receive_payload, len );

	// Put a zero at the end for easy printing
	receive_payload[len] = 0;

	// Spew it
    
	printf("Got payload size=%i value=%s\n\r",len,receive_payload);
    
    std::istringstream ss(receive_payload);
    std::string token;

    while(std::getline(ss, token, ',')) {
       // std::cout << token << '\n';
        vec.push_back(token);
        
    }
    
    tempconv->publish(NULL,vec[0].c_str(),sizeof(vec[1]) +1,vec[1].c_str());
    
}

}
  
}

mosqpp::lib_cleanup();

return 0;
}



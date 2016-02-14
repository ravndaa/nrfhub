CFLAGS=-Wall -ggdb -I../ -I/home/pi/libs/mosquitto-1.4.7/lib -I/home/pi/libs/mosquitto-1.4.7/lib/cpp
LDFLAGS=-L/home/pi/libs/mosquitto-1.4.7/lib /home/pi/libs/mosquitto-1.4.7/lib/cpp/libmosquittopp.so.1 /home/pi/libs/mosquitto-1.4.7/lib/libmosquitto.so.1 -L/usr/lib -I/usr/include -lrf24-bcm


.PHONY: all clean

all : rfhub

rfhub : main.o temperature_conversion.o
	${CXX} $^ -o $@ ${LDFLAGS}

main.o : rfHub.cpp
	${CXX} -c $^ -o $@ ${CFLAGS}

temperature_conversion.o : temperature_conversion.cpp
	${CXX} -c $^ -o $@ ${CFLAGS}
    
clean : 
	-rm -f *.o mqtt_temperature_conversion

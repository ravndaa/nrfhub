CFLAGS=-Wall -ggdb -I./ -I./lib/mosquitto/ -I./lib/rf24/
LDFLAGS=-Wall -ggdb -I./ -I./lib/mosquitto/ -I./lib/rf24/ -L./lib/mosquitto-1.4.7/lib ./lib/mosquitto/libmosquittopp.so.1 ./lib/mosquitto/libmosquitto.so.1 ./lib/rf24/librf24-bcm.so.1.0


.PHONY: all clean

all : nrfhub

nrfhub : main.o
	${CXX} $^ -o $@ ${LDFLAGS}

main.o : nrfhub.cpp
	${CXX} -c $^ -o $@ ${CFLAGS}

#temperature_conversion.o : temperature_conversion.cpp
#	${CXX} -c $^ -o $@ ${CFLAGS}
    
clean : 
	-rm -f *.o mqtt_temperature_conversion

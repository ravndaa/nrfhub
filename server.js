'use strict';


// Azure libraries....
var Mqtt = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;
var Message = require('azure-iot-device').Message;

var connectionString = 'HostName=<CHANGE>.azure-devices.net;DeviceId=<CHANGE>;SharedAccessKey=<CHANGE>';

// fromConnectionString must specify a transport constructor, coming from any transport package.
var client = Client.fromConnectionString(connectionString, Mqtt);

var NRF24 = require("nrf"),
    spiDev = "/dev/spidev0.0",
    cePin = 22, irqPin = 23,
    pipes = [0xF0F0F0F0E1];

//Used for parse the struct from arduino.
var Parser = require('binary-parser').Parser;
var sensorValues = new Parser()
    .endianess('little')
    .uint16('ID')
    .uint16('batteryLevel')
    .uint8('hasTemperature')
    .uint16('temperature')


var nrf = NRF24.connect(spiDev, cePin, irqPin);


var connectCallback = function(err) {
    if (err) {
        console.err('Could not connect: ' + err.message);
    } else {
        console.log('Client connected');
        client.on('message', function(msg) {
            console.log('Id: ' + msg.messageId + ' Body: ' + msg.data);
            client.complete(msg, printResultFor('completed'));
            // reject and abandon follow the same pattern.
            // /!\ reject and abandon are not available with MQTT
        });

        // https://gist.github.com/rlogiacco/92efac849e0292b791af

        nrf.channel(0x6c).transmitPower('PA_MAX').dataRate('250kbps').begin(function() {
            var rx = nrf.openPipe('rx', pipes[0]);

            rx.on('data', function(d) {

                var buf = new Buffer(d, 'hex');
                buf.reverse();

                console.log(sensorValues.parse(buf));

                var data = JSON.stringify(sensorValues.parse(buf));
                var message = new Message(data);

                console.log('Sending message: ' + message.getData());
                client.sendEvent(message, printResultFor('send'));


            });


        });

        client.on('error', function(err) {
            console.error(err.message);
        });

        client.on('disconnect', function() {
            clearInterval(sendInterval);
            client.removeAllListeners();
            client.connect(connectCallback);
        });
    }
}


client.open(connectCallback);

// Helper function to print results in the console

function printResultFor(op) {
    return function printResult(err, res) {
        if (err) console.log(op + ' error: ' + err.toString());
        if (res) console.log(op + ' status: ' + res.constructor.name);
    };
}
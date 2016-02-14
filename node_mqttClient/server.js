var mqtt    = require('mqtt');
var client  = mqtt.connect('mqtt://config.re',{
        clientId:'Kongen'
    });

client.on('connect', function () {
  client.subscribe('/kos');
  //client.publish('presence', 'Hello mqtt');
});

client.on('message', function (topic, message) {
  // message is Buffer
  console.log(message.toString());
  //console.log(topic.toString());
  //client.end();
});
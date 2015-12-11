var xhrRequest = function(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
}

function locationSuccess(pos) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
    pos.coords.latitude + "&lon=" + pos.coords.longitude +
    '&appid=37c80345f40937d95134ee170de0b744';

  xhrRequest(url, 'GET',
    function(responseText) {
      console.log(responseText);
      var json = JSON.parse(responseText);

      var temperature = Math.round(json.main.temp - 273.15);
      console.log("Temperature is " + temperature);

      var conditions = json.weather[0].main;
      console.log('Conditions are ' + conditions);

      var dictionary = {
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions
      };

      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending weather info to Pebble!');
        }
      )
    }
  )
}

function locationError(pos) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");

    getWeather();
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received");
    getWeather();
  }
);
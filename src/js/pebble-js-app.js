
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
 
function getData(pos) {
  // Construct URL
  var url = "http://your-textdata-url-here"; //please get your own data, my twitter api has rate limits
 
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {       
      console.log(responseText);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_DATA": responseText
      };
 
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Data sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending data to Pebble!");
        }
      );
    }      
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!"); 
    // Get the initial
    getData();
  }
);
 
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getData();
  }                     
);

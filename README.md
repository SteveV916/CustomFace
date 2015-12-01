# CustomFace
Pebble watchface which shows custom text/data below the time.

Updates every 30 minutes, from an external URL you may specify.

<img src="https://github.com/SteveV916/CustomFace/blob/master/resources/images/download.png">

To install on your watch:

- Go to cloudpebble.net then import this project.
- Edit pebble-js-app.js with your own URL, which returns short (<30 characters) of plain text

(In my case, I wrote a service to display top trending twitter tags, and current BTC/USD price)

# espLupusStatus
A very Simple status indicator for a Lupus alarm and smart home system based on an NodeMCU ESP8266.

The indicator is passive. Which means it is not pulling for the status, but waiting for incoming commands. That means it must be set from the external system itself. In the Lupus system you can define rules. A rule can  trigger an action-URL. And this is, how this status indicator gets his information.
The trade-off of this passive indicator is, if it missed an action-URL call, the status can differ from the origin. 

<h2>LEDs:</h2>

<table>
  <tr><th>Color</th><th>I/O port</th><th>Purpose</th></tr>
  <tr><td>"red"</td><td>GPIO5 D1</td><td>status "armed" (no differenciation area 1 or 2)</td></tr>
  <tr><td>"yellow"</td><td>GPIO14 D5</td><td>status "home"</td></tr>
  <tr><td>"green1"</td><td>GPIO4 D2</td><td>open windows level 1</td></tr>
  <tr><td>"green2"</td><td>GPIO0 D3</td><td>open windows level 2</td></tr>
</table>


<h2>Commands:</h2>
<table>
<tr><th>URL</th><th>Purpose</th></tr>
<tr><td>'http://esp-ip'</td><td>Shows the Status in your browser (update 1s)</td></tr>
<tr><td>'http://esp-ip/arm'</td><td>Activates "arm" status indication (red led on)</td></tr>
<tr><td>'http://esp-ip/home'</td><td>Activates "home" status inidication (yellow led on)</td></tr>
<tr><td>'http://esp-ip/disarm'</td><td>De-activates arm and home status and (red and yellow led off)</td></tr>
<tr><td>'http://esp-ip/wopen1'</td><td>Activates "windows open level 1" status inidication (green1 led on)</td></tr>                     
<tr><td>'http://esp-ip/wopen2'</td><td>Activates "windows open level 2" status inidication (green2 led on)</td></tr>                      
<tr><td>'http://esp-ip/wclosed1'</td><td>De-Activates "windows open level 1" status (green1 led off)</td></tr>
<tr><td>'http://esp-ip/wclosed2'</td><td>De-Activates "windows open level 2" status (green2 led off)</td></tr>
<tr><td>'http://esp-ip/wclosed'</td><td>De-Activates "windows open both levels" status (green1 and green2 leds off)</td></tr>
<tr><td>'http://esp-ip/dopen'</td><td>“door open” status</td></tr>
<tr><td>'http://esp-ip/dclosed'</td><td>“door closed” status</td></tr>
<tr><td>'http://esp-ip/resetcnt'</td><td>Reset all counter</td></tr>
</table>



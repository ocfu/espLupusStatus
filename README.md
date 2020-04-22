# espLupusStatus
A very Simple status indicator for a Lupus alarm and smart home system based on an NodeMCU ESP8266.

The indicator is passive. Which means it is not pulling for the status, but waiting for incoming commands. That means it must be set from the external systemm itself. In the Lupus system you can define rules. A rule can  trigger an action-URL. And this is, how this status indicator gets his information.
The trade-off of this passive indicator is, if it missed an action-URL call, the status can differ from the origin. 

LEDs:
"red", at GPIO D1, status "armed" (no differenciation area 1 or 2)
"yellow", at GPIO D5, status "home"
"green1", at GPIO D2, open windows level 1
"green2", at GPIO D3, open windows level 2

Commands:
http://<esp-ip>         Shows the Status in your browser (update 1s)
http://<esp-ip>/arm     Activates "arm" status indication (red led on)
http://<esp-ip/home     Activates "home" status inidication (yellow led on)
http://<esp-ip/disarm   De-activates arm and home status and (red and yellow led off)
http://<esp-ip/wopen1   Activates "windows open level 1" status inidication (green1 led on)                      
http://<esp-ip/wopen1   Activates "windows open level 2" status inidication (green2 led on)                      
http://<esp-ip/wclosed1 De-Activates "windows open level 1" status (green1 led off)
http://<esp-ip/wclosed2 De-Activates "windows open level 2" status (green2 led off)
http://<esp-ip/wclosed  De-Activates "windows open both levels" status (green1 and green2 leds off)



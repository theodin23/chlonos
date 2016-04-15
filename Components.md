On 3rd May 2014 a set of detailed instructions were added.  See the link on the home page.  There were a few minor components omitted such as resistors and capacitors.  So check there for a complete list.

# Introduction #

The following components are all that are required to build your own Chlonos. Altogether around £40.


# Details #

  * 1 x **Arduino Uno** - I got mine [here](http://stores.ebay.co.uk/Hobby-Components)
  * 1 x **LoL Shield** - best if you get one of these pre-assembled to save you 5 hours of soldering! I got mine [here](http://stores.ebay.co.uk/olimex)
  * 1 x **Speakjet Speech Synthesiser chip**
  * 1 x **LM386 Audio Amplifier chip**
  * 1 x **8ohm 0.3W speaker**
  * 1 x **N55AY light dependent resistor**
  * 1 x **DS1307 Realtime Clock chip** - You can either buy the individual components such as the pull-up resistors, crystal, chip, battery backup holder, etc, or you can get it all for £2 assembled on a PCB from China, [here](http://stores.ebay.co.uk/Sinedy?_trksid=p2047675.l2563)
  * 1 x **IN914** signal diode - this will go between the speakjet chip's 'busy speaking' pin and the audio amp pin 7 to switch it off when you're not using the speech. Otherwise the LoL Shield will generate a ton of noise.
  * 1 x **Arduino Protoshield** - acts as a project board that plugs directly into the Arduino. Very handy. I got [this](http://cgi.ebay.co.uk/ws/eBayISAPI.dll?ViewItem&item=320870180117&ssPageName=ADME:L:OC:GB:3160) one.
  * 1 x **LM35** celsius temperature sensor
  * Some single strand signal wire
  * 1 x **9volt 2.1mm power supply**
  * 1 x **white enclosure** - For some reason they produce the LoL Shield with clear directional LEDs, not diffuse ones. An enclosure with semi-opaque white covering will disperse the light nicely. Alternatively, a blank sheet of paper over the LEDs will work too.
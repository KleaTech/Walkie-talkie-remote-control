# Walkie-talkie-remote-control
Switch relays on and off from a few kilometers

This is an Arduino project with the following purpose. I want to turn on and off a high power electric device from a few hundred meters. The cheapest and easiest way to do so is to use handheld radios aka. walkie-talkies.

The circuit consists of an Arduino Uno, a relay board, an SSD1306 128x32 OLED screen (as a VU meter), a 500R series resistor, an antiparallel diode pair and a walkie talkie. The circuit is simple:
```
━━━━━━━━━━━━━┑                ┏━━━━━━━━━━━━━
           + ┃━━━500Ω━━┯━━||━━┃ Ain
 Radio       ┃         ┃      ┃    Arduino
 headphone   ┃        ▲ ▼     ┃      Uno
 out         ┃         ┃      ┃
           - ┃━━━━━━━━━╋━━━━━━┃ GND
━━━━━━━━━━━━━┙         ⏚      ┗━━━━━━━━━━━━━
```                    

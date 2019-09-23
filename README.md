# Blinking Rubber Ducks

This is a fun project which I've created for a birthday party.
It is made of lot's of tiny, sweet rubber ducks that have been spiked by LED sticks.
The blinking part is done by an Arduino UNO microcontoller and a custom prototype shield.

Currently all the effect codes are stored in the Arduino itself.
I'm planning to use an ethernet shield and the MQTT protocol client
to create yet another IoT object (Internet of Things).
Another storage option could be the SD card slot on the ethernet shield.

## Ingredients:

1. Optical effects
    * 10 LED stripes (each with 8 white LEDs on a small PCB)
    * 10 cinch plugs (male RCA jacks)
    * 40 colored rubber ducks

2. Microcontroller
    * Arduino UNO
    * Ethernet shield (optional)
    * Prototype shield
        - 2x ULN2003
        - some shield connectors

3. A nice plastic box
    * 10 cinch sockets (female RCA jacks)
    * 2 push buttons
    * 1 power socket (5.5 / 2.1mm)
    * 1 potentiometer
    * several resistors
        - 2x 56k ohm
        - 1x 82k ohm

## Example

See this [video](https://docs.google.com/file/d/0B8jawPLVqY3PcjE2dVZwUlNvXzQ/edit?usp=sharing "DiscoDucks")
to get a quick impression of the blinking ducks in action.

### MFH19

  At MakerFaire Hannover 2019 I have presented the ["Disco-Ducks-Roulette"](https://drive.google.com/file/d/1-CSCXpq4Vx5WX7pSvrVt4ZidkViK5EeQ/view?usp=sharing)
  
  | Gallery |  |
  |---------|--|
  | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-001.jpg) | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-002.jpg) |
  | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-003.jpg) | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-004.jpg) |
  | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-008.jpg) | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-009.jpg) |
  | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-010.jpg) | ![](https://github.com/pagong/discoducks/blob/master/media/ddr-006.jpg) |


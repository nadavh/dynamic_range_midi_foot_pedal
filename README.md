# Dynamic Range MIDI Foot Pedal

## About
I was looking to buy a MIDI pedal with a good range of motion and a fairly low price (mainly to control virtual instrument's expression/volume while my hands are on the keyboard, and my right leg is on a sustain pedal).

I didn't find what I was looking for, so I figured it's a good motivation to improve my CAD skills, and make my own.

My idea was to let the user choose the range they want, so... this pedal supports up to 80 degrees. While not expected to be fully used (I know I don't have that flexibility) different ranges work for different positioning of the pedal, and personal comfort.

![controller](img/animation.webp)

## Behavior
* By default, the full range is used.
* A press on the blue side-button initiates range selection mode, which is enabled for 5 seconds. It's indicated by a blue light, and can be ended sooner, by an additional press on the button.
* Range is chosen by moving the foot pedal back and forward, over the desired range.
* After the range is set, it is saved to EEPROM (so it's not power depended).
* Movements in the range are indicated by the green light (indicating MIDI commands are sent to the host) while movements outside of the range, are ignored.

## Software
Developed in C with the Arduino framework.

## Hardware
Mostly 3d printed (PLA) and split to avoid support material, and to be strong (in terms of printing orientation).

### Additional parts:
- Arduino Mini.
- 10 x 12mm M3 screws, washers and nuts.
- 2 x 608 (8 x 22 x 7mm) bearings.
- [Micro switch with LED](https://www.aliexpress.com/item/Free-shipping-50pcs-lot-size-12X12X7-3-push-button-Led-Tact-Switch-illuminated-switch/1428645429.html?spm=a2g0s.9042311.0.0.366c4c4dAUs27O).
- Potentiometer.
- A glue of your choice (I've used hot-glue).
- 12 x Padding stickers (optional).

Models and assembly instructions can be found [here](https://www.printables.com/model/272769-dynamic-range-midi-foot-pedal).


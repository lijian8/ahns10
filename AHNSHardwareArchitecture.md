# Introduction #

This page contains information on the proposed hardware components for the project. Feel free to add, subtract, multiply and divide anything you want. <br /> Or just leave your thoughts. These are all just initial ideas, not final design solutions. Note that the prices are in US dollars, and do not include postage.


# Details #

## Flight Computer ##

The proposed Flight Computer is the Gumstix Overo Fire. <font color='#FF0000'> ($219.00 USD) </font>
<br />The following link points to the [Gumstix Overo Fire information page.](http://www.gumstix.com/store/catalog/product_info.php?products_id=227)


It uses the TI OMAP 3503 which we can install Linux or Windows CS on:
<br />The following link points to [TI OMAP 305 information.](http://focus.ti.com/lit/ds/symlink/omap3503.pdf)


The Pinto expansion board should be adequete for our purposes:    <font color='#FF0000'>    ($27.50 USD) </font>
<br />The following link points to the [Gumstix Pinto Expansion Board.](http://www.gumstix.com/store/catalog/product_info.php?products_id=239)

The Summit expansion board should also be purchased for debuging:     <font color='#FF0000'>    ($49.00 USD) </font>
<br />The following link points to the [Gumstix Summit Expansion Board.](http://www.gumstix.com/store/catalog/product_info.php?products_id=215)

This will give us access to (Pinto):
  * 2 x UARTS
  * 6 x PWM lines
  * 6 x ADC lines
  * 8 x GPIO
  * 1 x I2C
  * 1 x SPI with 2 CS lines (We can connect two devices to the SPI but can only communicate to one at a time.)
  * 1 x Image sensor input (including sensor control data).
  * 1 x Bluetooth
  * 1 x Wifi

https://wave.googleusercontent.com/wave/attachment/Overo%20File.jpg?id=vN0LcnC12&key=AH0qf5x9IB1huxvgRSHM0uw4FQQFtG4RlQ

## Blackfin Camera ##

SRV-1 Blackfin Camera with 500MHz Analog Devices Blackfin BF537 processor, 32MB SDRAM, 4MB Flash, and choice of Omnivision OV9655 <br />1.3 megapixel camera or OV7725 VGA low-light camera (up to 60fps), both with 3.6mm f2.0 lens (90-deg field-of-view). <br />
http://www.surveyor.com/blackfin/

Blackfin camera pointing downwards <font color='#FF0000'> ($195 USD) </font> <br />
http://surveyor-corporation.stores.yahoo.net/srblcabo.html

Lens Option Set (for the The right-angle adapter) <font color='#FF0000'> ($29 USD) </font><br />
http://surveyor-corporation.stores.yahoo.net/srwianleset.html

**Note this company states that they provide academic discount applied to orders from education institutions**

https://wave.googleusercontent.com/wave/attachment/bfboards1%5B1%5D.jpg?id=A9jXXCYI2&key=AH0qf5y13x2zc0J7OJDNYw11Y_gQhHXFJA

## Ultrasonic Range Sensor ##

I think the Maxbotix ultrasonic sensor should be used, We have one avaliable and it has good features. It outputs a range value via a UART in <br />inches (there's no need to figure out ADC conversations and deal with non linearities)

It has a reliable output in the range of approximately 15 cm to 6.25 meters. <br />
http://www.maxbotix.com/uploads/LV-MaxSonar-EZ0-Datasheet.pdf <font color='#FF0000'>($0.00 USD) </font>

https://wave.googleusercontent.com/wave/attachment/polo_0726_250%5B1%5D.jpg?id=wpUZ9wkd3&key=AH0qf5yvKu6VRngGB3VGOVRb1bG9oPps9Q

## Infrared Range Sensor ##

SHARP GP2Y0A710K0F, Measuring sensor already have one from last year and can be intergrated with the ultrasonic range sensor.

<font color='#FF0000'> ($0.00 USD) </font>

![http://au.farnell.com/productimages/farnell/standard/1618432-40.jpg](http://au.farnell.com/productimages/farnell/standard/1618432-40.jpg)

## Inertial Measurment Unit ##

Sensor Dynamics 6DOF IMU <br />
6 Degrees of Freedom with SPI or UART

http://www.sensordynamics.at/cms/cms.php?pageId=73# <font color='#FF0000'>    ($0.00 USD) </font>

https://wave.googleusercontent.com/wave/attachment/234%5B1%5D.gif?id=wpUZ9wkd5&key=AH0qf5zxlXuXRxeYmhhiy7zhO7d8YgsWXA

## Compass Module ##

HMC6343 from Sparkfun. <br />
It's the more expensive one but it has tilt compensation.<br />
I2C interface.<br />

http://www.sparkfun.com/commerce/product_info.php?products_id=8656 <font color='#FF0000'>($149.95 USD) </font>

https://wave.googleusercontent.com/wave/attachment/08656-01-L%5B1%5D.jpg?id=wpUZ9wkd6&key=AH0qf5zwiCBmcIeiwUZI9OsDjjPlyTvj9g

## Hardware Architecture ##

This is [revision 2](https://code.google.com/p/ahns10/source/detail?r=2) of the architecture, The entire Architecture has been revised. All mode control unit componants have been put into a single <br /> block and a design doc will explain how it all works.

[AHNS-2010-HA-rev2](http://ahns10.googlecode.com/svn/docproject/unofficial_ahns/AHNS-2010-HA-rev2.pdf)

## Finansial Budget ##

### International Orders: ###

| **Item** | **Cost** |
|:---------|:---------|
|Gumstix Overo Fire|$219.00 USD|
|Pinto expansion board|$27.50 USD|
|Summit expansion board|$49.00 USD|
|Gumstix Postage|$62.82 USD|
|Blackfin Camera and Processor|$195.00 USD|
|Lens Option Set|$29.00 USD|
|Blackfin Postage|$60.00 USD|
| **TOTAL:** | **$642.32 USD** |

### Australian Orders: ###

| **Item** | **Cost** |
|:---------|:---------|
|Avionics Enclosure x2  |$7.90 AUD |
|Coolumb Counter |$7.35 AUD |
|Temperature Sensor|$0.00 AUD |
|Mode Indicator |$4.95 AUD |
|MCU|$0.00 AUD|
| **TOTAL:** | **$20.20 AUD** |

## Weight Budget ##

Maximum additional weight applied to airframe is 350 grams (taken from last years documentation)

| **Item** | **Weight** |
|:---------|:-----------|
|Gumstix Overo Fire|? grams|
|Pinto expansion board |? grams|
|IMU|? grams |
|Blackfin Camera and Processor  |36 grams |
|Ultrasonic RF  |4.3 grams|
|Infrared RF |? grams|
|Coolumb Counter   |? grams|
|Temperature Sensor|? grams |
|Compass Module  |? grams |
|Mode Indicator  |? grams|
|MCU|? grams|
|WIFI Antenna|? grams|
|Avionics Enclosure|? grams |
|PCBs|? grams |
|Mounting Hardware|? grams|
| **TOTAL:** | **? grams** |
# NightscoutAlarmClock

## What is the project?

This project is a bedside alarm clock which shows the blood glucose of the person. It also has built in alarms for hipo and hyperglycemia prevention and will also display RGB colors related to the blood glucose level of the moment.

- Green is between the user goal of BG
- Blue is between 70 to lower user's limit and between higher user's limit and 180
- Red is lower than 70 and higher than 180. It will also play the alarm which can be snoozed by 15 minutes by pressing the button

## How does it work?

The hardware is connected to the wifi network and it requests periodically the blood glucose from the person's [Nightscout Server](http://www.nightscout.info/), which is a web application that anyone using a CGM(Continuous Glucose Monitor) is able to send the blood glucose to the cloud so it can be available everywhere.

## Demo Video

[You can check a demo video( in portuguese ) by clicking here](https://www.youtube.com/watch?v=l_BeDuxK0gk)

## Hardware and Software Setups

Hardware : 

- SAM E70 Xplained(ARM® Cortex®-M7 core- based)
- maXTouch Xplained ILI9488
- Atmel WINC-1500
- RGB LED strip
- Arcade Push Button
         
         
Software:
         
- Nightscout Heroku based server
- Source code written in C
         
         
         Responsibles:
         
         Gabriel Monteiro
         Guilherme Leite

         Insper
         São Paulo, Brazil
         2020


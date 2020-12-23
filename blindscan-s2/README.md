# README #
A C program to blindscan digital satellite signals. Taking advantage of the blindscan algorithm in the Linux driver for the Prof DVB-S2 cards, it will step through a range of transponders, find the symbol rate,and calculate for an LNB if you give it one.

*Blindscan tool for the Linux DVB S2 API


```
#!c

usage: blindscan-s2
-b        : run blind scan
-T        : Tune a specific transponder
-H        : only scan horizontal polarity
-V        : only scan vertical polarity
-N        : no polarity
-i        : interactive mode
-m        : monitor signal mode
-M        : monitor and re-tune each time
-a number : adapter number (default 0)
-f number : frontend number (default 0)
-F number : fec (default auto)
-s number : starting transponder frequency in MHz (default 950)
-e number : ending transponder frequency in MHz (default 1450)
-d number : delivery system 4=DSS 5=DVB-S 6=DVB-S2 (default 0)
-t number : step value for scan in MHz (default 20)
-R number : amount of times to try tuning each step (default 1)
-r number : symbol rate in MSym/s (default 16000)
-l number : local oscillator frequency of LNB (default 0)
-2        : enable 22KHz tone (default OFF)
-c number : use DiSEqC COMMITTED switch position N (1-4)
-u number : use DiSEqC uncommitted switch position N (1-4)
-U N.N    : orbital position for USALS
-G N.N    : site longtude
-A N.N    : site latitude
-W number : seconds to wait after usals motor move (default 45)
-v        : verbose

Example:
Default scans L-band range in steps of 20 on H and V polarity
blindscan-s2 -b
Scan 11700-11900 vertical in steps of 10, and calculate for lof
blindscan-s2 -b -s 11700 -e 11900 -V -t 10 -l 10750

```


### What is this repository for? ###

* chinesebob's blindscan-s2 program
modified to work with UDL's v4l-updatelee kernel modules

blindscan dvb-s(2) satellites using stv090x devices.

### How do I get set up? ###

* install UDL's v4l-updatelee kernel
* Configuration
* Dependencies - v4l-updatelee, gcc
optionally Qt and QtCreator which may be
useful for debugging 
*
* How to run tests - see help
* Deployment instructions - 
* git clone https://majortom@bitbucket.org/majortom/blindscan-s2.git
* cd blindscan-s2
* make

or open the project in QtCreator from the blindscan directory,
so ya can use QtCreator's debug functions. 
That s/be easier than using gdb in the console.

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* questions, look in the Computer and USB Satellite Receivers and Recording forum
 at http://rickcaylor.websitetoolbox.com/
TicketReaderSDK


This project includes the basic libraries and functions needed to implement all the basic operations and workflows of a barcode ticket reading machine.

It was originally written as the ground layer of a parking ticketing solution, which used Eltra devices to print and read barcode tickets. The ticket reader was an Eltra EL0540-N-30, controlled by a Raspberry PI which also operated the automatic barrier.
The solution also included a ticket printer operated by a Windows machine, and an integration with a server software which checked ticket validity.

###Devices
This project is tested working on Raspberry PI, but should also work without problems on any GNU/Linux system. It could potentially be portable to any OS.


###Scope
This project was developed and tested for a specific device, but could provide the basis for implementations for other devices of the same type.
The original SDK was provided only for .NET and Windows. Some function names and declarations have been modeled in a similar way to the original SDK functions, to provide easier switching between frameworks.

The project also contains a sample "test" application which showcases the basic operations. 

##Project structure

The **conf** directory contains the configuration files for the applications. The test application only needs the serial port device file path.

The **includes** directory contains a single file, **protocolconf.h**, where the specific hex commands for the device protocol should be configured. In this public repository the commands are intentionally left blank, since they can be found on the device manual.

The **src/sdk** directory contains the lowest level SDK libraries for communication with the devices:
* **RS232.c** implements the basic serial communication functions, to manage the serial port, write and read data.
* **genericcommands.c** implements the generic functions for sending commands to the device and return answers, both for simple hex commands and commands with data payload, and polling the device when operating to detect when an operation is completed.

The **src** directory contains the basic libraries needed to implement an application for the device:
* **configreader.c** implements an helper function to read values from the configuration file
* **commands.c** implements the main operations for the device, for setting device parameters, getting device sensor status, controlling the device leds, and inserting, ejecting, capturing and reading tickets on the device. It also implements helper functions to print messages and errors on standard output.

The **test** directory contains the sample test application:
* **Makefile** is the makefile used to build the test app (**test** target) or cleaning the project directory back to original status (**clean-all** target)
* **functions.c** implements some other generic helper functions used by the test application, to manage the idle statuses of the devices, polling the sensors until the status is changed, eventually having the leds blink in the meanwhile.
* **test.c** is a sample test application, showing basic interactivity with the device through a command line menu offering elementary operations and also some basic workflow:
  * get **s**ensor status (whether a ticket is present and where it is)
  * **i**nsert a ticket
  * **e**ject a ticket
  * **c**apture a ticket
  * **r**ead a ticket with full workflow: insert the ticket, read the barcode and eject the ticket
  * **l**eds on or off 
  * sample **a**utomated mode:
    * the device ejects an eventual ticket
    * the device goes idle, initially blinking the green leds for maximum 60 seconds, waiting for a ticket
    * when a ticket is detected in the mouth the device inserts the ticket, tries to read it for n times, turning the leds red or green depending on the result, and ejecting and reinserting the ticket in case of failure to read
    * after the ticket is successfully read or the retries are over, the device ejects the ticket
    * the device waits for the user to retrieve the ticket, with green leds on in case of success, or blinking red leds in case of failure to read
    * after a timeout, if the user didn't retrieve the ticket, the device captures it.
    * the device gets back to idle waiting for another ticket

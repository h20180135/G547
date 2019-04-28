1. PROJECT TITLE : I/O Driver for RaspberryPi.

final_proj
        |_driver
        |       |_ _I/O_driver.c
        |       |_ _Makefile


2.  SUMMARY :

The project demonstrates building our own I/O device driver for a RaspberryPi.Here,we are able to directly map to the GPIO Hardware registers through sysfs interface.The configuration of each pin as an input or an output can be made by the user.By default,each of the pins would either be in a pullup or pulldown state.We are able to read these pullup and pulldown values of each pin.The values read are displayed on the kernel log.In addition to that,we are also able to write into a particular pin,configuring it as an output.To display the pins as output,we have used LED.

For example,GPIO pin 2 is weakly pulled up by default ,so we can read its value in the Kernel log as 1.Similarly,GPIO pin 4 is pulled down,so we can read its value as 0.

3.  BUILDING THE CODE:

 3.1 Driver
 $ make clean
 $ make all
 $ sudo insmod I/O_driver.ko
s
 3.2 After inserting the module,we can see the device file creation using 
 $ cd /sys/class
 $ ls -l
  Check whether the device files are created or not.
  We have created two device files named read_and_write,for reading and writing to GPIO pins;and another named mode_select,for configuring the pin to be used as an input or an output pin.

 3.3 To provide the permissions,we use
 $ sudo su

 3.4 Then,we need to select whether to use the pin as an input or an output pin.For that,run the following commands
   3.4.1 To configure the pin as an output,
   $ cd /sys/devices/virtual/mode_select/mode_select
   $ echo 1 > mode
   $ cd
    mode is the name of the attribute to which the mode select value 1 is passed to.
   
   3.4.2 To configure the pin as an input,
   $ cd /sys/devices/virtual/mode_select/mode_select
   $ echo 0 > mode
   $ cd

 3.5 Next,to read or write to a GPIO pin,we use,
 
   3.5.1 To write into the pin when configured as an output pin.
   $ cd /sys/devices/virtual/read_and_write/read_and_write
   $ echo 1 > value
    After this command,LED connected to the pin,we are writing to,would get on.
   $ cd /sys/devices/virtual/read_and_write/read_and_write
   $ echo 0 > value
   $ cd
    After this command,LED connected to the pin,we are writing to,would get off.

   3.5.2 TO read from the pin configured as an input pin.
   $ cat /sys/devices/virtual/read_and_write/read_and_write
   $ dmesg -wH
    This would read the value of the pin and display on the Kernel log. 
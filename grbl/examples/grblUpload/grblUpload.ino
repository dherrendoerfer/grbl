/***********************************************************************
This sketch compiles and uploads Grbl to your 328p-based Arduino! 

To use:
- First make sure you have imported Grbl source code into your Arduino
  IDE. There are details on our Github website on how to do this.

- Select your Arduino Board and Serial Port in the Tools drop-down menu.
  NOTE: Grbl only officially supports 328p-based Arduinos, like the Uno.
  Using other boards will likely not work!

- Then just click 'Upload'. That's it!

For advanced users:
  If you'd like to see what else Grbl can do, there are some additional
  options for customization and features you can enable or disable. 
  Navigate your file system to where the Arduino IDE has stored the Grbl 
  source code files, open the 'config.h' file in your favorite text 
  editor. Inside are dozens of feature descriptions and #defines. Simply
  comment or uncomment the #defines or alter their assigned values, save
  your changes, and then click 'Upload' here. 

Copyright (c) 2015 Sungeun K. Jeon
Released under the MIT-license. See license.txt for details.
***********************************************************************/

/* This version is specifically tailored for uCNC_Controller based boards.
   It probably won't work on generic grbl boards, because it is designed
   for unipolar steppers and ULN2004 drivers. See the repo documentation
   for more info on this.

   Make sure that only one of the folowing two defines is set; they 
   are exclusive! 
   You can set these here, or in the grblUpload.ino file in examples. 
 */

/* This define will make the servo move when the spindle is turned on
   or off, the degree of movement from the middle position is defined
   by the spindle speed (Use M4 S90 to turn it 90 degrees)
   The spindle direction is honored: the servo will move in different 
   directions
 */
//#define SERVO_SPINDLE

/* This define will make the servo move according to the Z-axis position.
   You must use the Z-axis steps per millimeter setting to define the 
   degrees per milimeter.
   Upon reset or initialisation the servo will always move to the zero 
   position (all the way up), and must be reset to real zero before each
   use.
   This will also disable the Z-axis stepper !
 */
//#define SERVO_Z_PROPORTIONAL

#include <grbl.h>

// Do not alter this file!

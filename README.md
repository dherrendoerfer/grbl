![GitHub Logo](https://github.com/gnea/gnea-Media/blob/master/Grbl%20Logo/Grbl%20Logo%20250px.png?raw=true)
***

### This is Grbl for the uCNC_Controller boards. 

*This is a carefully modified version of Grbl to support boards with unipolar steppers*  
The original Grbl code (of which this is a fork) can be found here: https://github.com/grbl/grbl

The code currently supports this hardware board:  
https://easyeda.com/dherrend/uCNC_Controller_v1c-5575314283b24aa9b43c46027174ab2c

#### Supported features
- three unipolar steppers for X,Y,Z 
- spindle control to driver output for power
- spindle control to servo output for center/up/down (M5,M3,M4)
- servo angle control through spindle speed
- one end switch for all axis
- two multi-porpose inputs (for leveling) available

***

Grbl is a no-compromise, high performance, low cost alternative to parallel-port-based motion control for CNC milling. It will run on a vanilla Arduino (Duemillanove/Uno) as long as it sports an Atmega 328. 

The controller is written in highly optimized C utilizing every clever feature of the AVR-chips to achieve precise timing and asynchronous operation. It is able to maintain up to 30kHz of stable, jitter free control pulses.

It accepts standards-compliant g-code and has been tested with the output of several CAM tools with no problems. Arcs, circles and helical motion are fully supported, as well as, all other primary g-code commands. Macro functions, variables, and most canned cycles are not supported, but we think GUIs can do a much better job at translating them into straight g-code anyhow.

Grbl includes full acceleration management with look ahead. That means the controller will look up to 18 motions into the future and plan its velocities ahead to deliver smooth acceleration and jerk-free cornering.

* [Licensing](https://github.com/grbl/grbl/wiki/Licensing): Grbl is free software, released under the GPLv3 license.

* For more information and help, check out our **[Wiki pages!](https://github.com/grbl/grbl/wiki)** If you find that the information is out-dated, please to help us keep it updated by editing it or notifying our community! Thanks!

* Lead Developer [_2011 - Current_]: Sungeun(Sonny) K. Jeon, Ph.D. (USA) aka @chamnit

* Lead Developer [_2009 - 2011_]: Simen Svale Skogsrud (Norway). aka The Originator/Creator/Pioneer/Father of Grbl.

***

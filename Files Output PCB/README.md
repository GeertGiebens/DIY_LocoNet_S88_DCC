### Under construction (09/05/2023)

     PCB gerber file follow later

### Brief explanation of controlling outputs PCB via LocoNet commands:

The addresses to which the outputs react can be set in binary via DIP1-7 in steps of 16. (maximum range 2048)

- DIP9= OFF: output remains high when activated.

- DIP9= ON: output drops out after 250ms when activated. This time is sufficient for most alternating coils. (represented as 250 on PCB)

- DIP10= OFF: Each output responds to 1 address.

- DIP10= ON: A combination of 2 consecutive outputs respond to the same address. One output on switch command with DIR = '0' or L = '0' and next output on switch command with DIR = '1' or L = '1'. (represented on PCB as SR F-F)

- DIP11: because with DIP10=ON the 16 outputs can only switch at 8 addresses, this DIP can be used to set which series of 8 addresses the outputs respond to. DIP11=OFF then the outputs switch at the first 8 addresses, DIP11=ON the last 8 addresses. (Represented on PCB as 1-8/ 9-16)

- DIP S/I= OFF: the outputs respond to LocoNet command OPC_SW_REQ DIR='0' or '1'.

- DIP S/I= ON: the outputs respond to LocoNet command OPC_INPUT_REP L='0' or '1'. This makes it possible to make feedback messages visible on a synoptic board. By placing the series resistors of the LEDs on the PCB and using flat-cable where an LED output is alternated with ground, each LED can be soldered separately to its own 2 wires. (see example below)

<img alt="open opps 1" src=https://github.com/GeertGiebens/DIY_LocoNet_S88_DCC/blob/main/Files%20Output%20PCB/LocoNet_LED_connect.png>

### Under construction (24/maart/2023)

# Brief explanation of controlling outputs PCB via LocoNet commands:

The addresses to which the outputs react can be set in binary via DIP1-7 in steps of 16. (maximum range 2048)

- DIP9= OFF: output remains high when activated.

- DIP9= ON: output drops out after 250ms when activated. This time is sufficient for most alternating coils. (represented as 250 on PCB)

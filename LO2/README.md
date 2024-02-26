# Second Local Oscilator (LO2)
The design of the second local oscillator is much simpler. As the center frequency is precisely determined by the band-pass filter (after the first mixer), we only have about 10 MHz of bandwidth available, in which the frequency of the LO2 can be changed. For this task the well-tested solution with the MAX2871 chip and the GVA-63+ amplifier was choosen.
Like the STuW81300, the MAX2871 has an internal bank of oscillators between which the state machine switches, depending on the N divider setting. Due to operation in a narrow frequency band, switching between oscillators does not occur (in our case).

![LO2 picture](photo/LO2_in_case.jpg?raw=true "LO2 picture")

The diagram of the second local oscillator is shown in the figure below:

![schematic](LO2_schematic.svg?raw=true "LO2 schematic")

The reference clock is provided by a master 40 MHz crystal oscillator common to the LO1. The decrease in the the output power is compensated by the GVA-63+ amplifier. The output power reaches approximately +10 dBm. The circuit gets slightly warm to the touch during continuous operation in a closed case (the LD1117S33 linear regulator contributes the most to this). A light-emitting diode signals the successful PLL-lock state.
The phase detector operates at 10 MHz (the internal reference clock is divided by 4). Loop-filter is designed to have a 60° phase margin. The phase noise of the final prototype at 4.23 GHz is shown in the figure below:

![Phase noise](measurements/phaise_noise.svg?raw=true "Phase noise")

## Building

All capacitors below 100 nF are NP0/CG0 type. Some capacitors (eg 1uF and 100nF) are placed on top of each other due to lack of space on the PCB. Communication takes place via the SPI bus, and all connections to the circuit are made via SMD feed-trough capacitors. The local oscillator is built on two-layer FR4 board. In the presented prototype, MAX2870 is used instead of the MAX2871, since the latter was not available. The difference is lower power consumption (~20 mA), but worse phase noise (~5 dB) and larger reference clock crosstalk visible on the output spectrum, which appears ±40 MHz from the carrier signal with an amplitude of about – 55 dBc. The problem disappears by using the MAX2871, without any changes to the programming code or the loop filter. The PCB is installed in a brass case with a suitable cover. High-frequency signals are routed to and from the housing with coaxial cables.

## Testing

The module must be tested before installation in the housing. We do this with the help of the main MCU board and the test program, where the connections are created according to the following diagram:

![LO2 Test connections](LO2_test_program_connections.svg?raw=true "LO2 Test connections")

The test program, in form of a .hex file, can be [found here](LO2_test_program/bin/Release/basic_startup.hex). At startup, the program configures the PLL chip and tries to set the frequency to 4.23 GHz. Then checks the lock condition and performs a test read of the R6 register. Communication takes place via a USB-serial connection using a terminal (eg Putty). The default baud rate is 9600 bps, no parity, and 1 stop bit. It can then accepts commands but only two:

| Syntax  | Command description |
| ------------- | ------------- |
| `help`  | Lists out all commands with a brief description |
| `F xxxxxxx`  | Frequency in kHz  |

The output spectrum can be checked with an existing spectrum analyzer if needed.
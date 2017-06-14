#ifndef MCP23S17_H
#define MCP23S17_H

/*
	Using this IO expander:
	0. Strobe select to pick mode(1,1) otherwise mode(0,0)
	1. Select the chip
	2. Send the control byte OR'd with the address
	3. Send the address byte
	4. Send or receive data bytes
	5. Deselect the chip
*/

// Control bytes
#define MCP23S17_W (char)(0x40)
#define MCP23S17_R (char)(0x41)

// Valid chip addresses ("OR" with control bytes to address one of many chips
// possibly sharing a select line)
#define MCP23S17_ADDR0 (char)(0x00)
#define MCP23S17_ADDR1 (char)(0x02)
#define MCP23S17_ADDR2 (char)(0x04)
#define MCP23S17_ADDR3 (char)(0x06)
#define MCP23S17_ADDR4 (char)(0x08)
#define MCP23S17_ADDR5 (char)(0x0A)
#define MCP23S17_ADDR6 (char)(0x0C)
#define MCP23S17_ADDR7 (char)(0x0E)

// Special named bits in the IOCON register
#define MCP23S17_BANK (char)(0x80) // Enable banked addressing
#define MCP23S17_MIRROR (char)(0x40) // OR the interrupt pins together
#define MCP23S17_SEQOP (char)(0x20) // DISABLE address auto-increment
#define MCP23S17_DISSLW (char)(0x10) // DISABLE slew rate
#define MCP23S17_HAEN (char)(0x08) // Enable address bits
#define MCP23S17_ODR (char)(0x04) // Enable interrupts as open drains
#define MCP23S17_INTPOL (char)(0x02) // Enable interrupts as active-high

// Register addresses in paired mode
#define MCP23S17_P_IODIRA (char)(0x00) // 1 = pin is an input, else output
#define MCP23S17_P_IODIRB (char)(0x01) // READ: DEFAULTS TO 0xFF
#define MCP23S17_P_IPOLA (char)(0x02) // 1 = automatically NOT the input
#define MCP23S17_P_IPOLB (char)(0x03) // 0 = don't change the input
#define MCP23S17_P_GPINTENA (char)(0x04) // 1 = Enable interrupt
#define MCP23S17_P_GPINTENB (char)(0x05)
#define MCP23S17_P_DEFVALA (char)(0x06) // Compare value for interrupts
#define MCP23S17_P_DEFVALB (char)(0x07) // (If matching INTCON enabled)
#define MCP23S17_P_INTCONA (char)(0x08) // 1 = Interrupt if not equal DEFVAL
#define MCP23S17_P_INTCONB (char)(0x09) // 0 = Interrupt if input flips at all
#define MCP23S17_P_IOCONA (char)(0x0A) // Mode bits for the chip
#define MCP23S17_P_IOCONB (char)(0x0B)
#define MCP23S17_P_GPPUA (char)(0x0C) // Enable pull-up resistor on input pins
#define MCP23S17_P_GPPUB (char)(0x0D)
#define MCP23S17_P_INTFA (char)(0x0E) // Read-only!
#define MCP23S17_P_INTFB (char)(0x0F) // Shows which pin tripped last interrupt
#define MCP23S17_P_INTCAPA (char)(0x10) // Read-only!
#define MCP23S17_P_INTCAPB (char)(0x11) // Shows pin values at last interrupt
#define MCP23S17_P_GPIOA (char)(0x12) // Pin values on the port
#define MCP23S17_P_GPIOB (char)(0x13)
#define MCP23S17_P_OLATA (char)(0x14) // The real register written when
#define MCP23S17_P_OLATB (char)(0x15) // matching pins are outputs

// Register addresses in banked mode
#define MCP23S17_B_IODIRA (char)(0x00) // SERIOUSLY WHY WOULD YOU EVER DEFAULT
#define MCP23S17_B_IODIRB (char)(0x10) // THE ONE REGISTER TO 0xFF, WHAT TROLLS
#define MCP23S17_B_IPOLA (char)(0x01)
#define MCP23S17_B_IPOLB (char)(0x11)
#define MCP23S17_B_GPINTENA (char)(0x02)
#define MCP23S17_B_GPINTENB (char)(0x12)
#define MCP23S17_B_DEFVALA (char)(0x03)
#define MCP23S17_B_DEFVALB (char)(0x13)
#define MCP23S17_B_INTCONA (char)(0x04)
#define MCP23S17_B_INTCONB (char)(0x14)
#define MCP23S17_B_IOCONA (char)(0x05)
#define MCP23S17_B_IOCONB (char)(0x15)
#define MCP23S17_B_GPPUA (char)(0x06)
#define MCP23S17_B_GPPUB (char)(0x16)
#define MCP23S17_B_INTFA (char)(0x07)
#define MCP23S17_B_INTFB (char)(0x17)
#define MCP23S17_B_INTCAPA (char)(0x08)
#define MCP23S17_B_INTCAPB (char)(0x18)
#define MCP23S17_B_GPIOA (char)(0x09)
#define MCP23S17_B_GPIOB (char)(0x19)
#define MCP23S17_B_OLATA (char)(0x0A)
#define MCP23S17_B_OLATB (char)(0x1A)

#endif

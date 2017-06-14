#ifndef __MAIN_H
#define __MAIN_H

/* Sets up the ports.
	- Ports 2 and 3 and port 1 pin 7 have pull-down resistors enabled because they
		aren't used. This saves some power because they're not floating.
	- Port 1 pins 1, 2, and 4 are the first SPI bus, and 5 is a slave-select.
	- Port 1 pin 3 is an input that triggers interrupts.
	- Port 1 pins 0 and 6 are outputs. */
void port_setup(void);

/* Sets up universal serial module A.
	- Mode(0,0) SPI interface
	- Clock source is SMCLK, divider is 8, effective clock is 92 kHz */
void spi_setup(void);

/* Enable interrupts from the SPI interface. */
void spi_int_enable(void);

/* Disable interrupts from the SPI interface. */
void spi_int_disable(void);

/* Enable interrupts from P1.3. */
void port1_int_enable(void);

/* Disable interrupts from P1.3. */
void port1_int_disable(void);

/* Interrupt Service Routine for P1.3. Clears and disables the interrupt and
resumes main(). */
__attribute__((interrupt(PORT1_VECTOR))) void port1_isr(void);

/* Interrupt Service Routine for the SPI receive buffer. Mainly a wrapper around
spibuf_rx(). Turns on the LED on P1.6 as a status light. */
__attribute__((interrupt(USCIAB0RX_VECTOR))) void spi_rx_isr(void);

/* Interrupt Service Routine for the SPI transmit buffer. Mainly a wrapper around
spibuf_tx(). Turns off the LED when finished. */
__attribute__((interrupt(USCIAB0TX_VECTOR))) void spi_tx_isr(void);

#endif
#include <msp430.h>
#include "main.h"
#include "libspibuf.h"
#include "mcp23s17.h"

/*
	Connects to an MCP23S17 and relays the state of DIP switches on its port B to
	LEDs on its port A. The CPU is turned off at any opportunity and interrupts
	are used to transmit and receive data and be notified when new data is
	available.

	SPI bus A is used via pins 3, 4, and 6 (P1.{1,2,4})
	One interrupt from slave used on pin 5 (P1.3)
	The Launchpad's green LED is used on pin 14 (P1.6)
*/

struct spibuf mybuf;

int main(void) {
	WDTCTL = WDTPW + WDTHOLD; // Disable watchdog

	port_setup();
	spi_setup();

	// Set up the data structure for libspibuf
	spibuf_init(&mybuf, &P1OUT, BIT5, &UCA0TXBUF, &UCA0RXBUF);
	spibuf_reset(&mybuf); // Not exactly a 100% reset, see implementation

	// Configure all MCP23S17 on CS=P1.5 in bank addr. with auto pointer inc.
	spibuf_queue(&mybuf, MCP23S17_W);
	spibuf_queue(&mybuf, MCP23S17_P_IOCONA);
	spibuf_queue(&mybuf, MCP23S17_BANK);
	spi_int_enable();
	_BIS_SR(CPUOFF + GIE);
	_BIC_SR(GIE);
	spibuf_reset(&mybuf);

	// Configure all MCP23S17 port A on CS=P1.5 (All registers at once)
	spibuf_queue(&mybuf, MCP23S17_W);
	spibuf_queue(&mybuf, MCP23S17_B_IODIRA);
	spibuf_queue(&mybuf, 0x00); // All pins outputs
	spibuf_queue(&mybuf, 0x00); // Don't care, not inputs
	spibuf_queue(&mybuf, 0x00); // Don't generate interrupts
	spibuf_queue(&mybuf, 0x00); // Don't care, not generating interrupts
	spibuf_queue(&mybuf, 0x00); // Don't care, not generating interrupts
	spibuf_queue(&mybuf, MCP23S17_BANK); // New mode
	spibuf_queue(&mybuf, 0x00); // Don't care, not inputs
	spibuf_queue(&mybuf, 0x00); // Don't care, read-only register
	spibuf_queue(&mybuf, 0x00); // Don't care, read-only register
	spibuf_queue(&mybuf, 0xAA); // Sentinel value
	spi_int_enable();
	_BIS_SR(CPUOFF + GIE);
	_BIC_SR(GIE);
	spibuf_reset(&mybuf);

	// Configure all MCP23S17 port B on CS=P1.5 (All registers at once)
	spibuf_queue(&mybuf, MCP23S17_W);
	spibuf_queue(&mybuf, MCP23S17_B_IODIRB);
	spibuf_queue(&mybuf, 0xFF); // All pins inputs
	spibuf_queue(&mybuf, 0xFF); // NOT the inputs
//	spibuf_queue(&mybuf, 0x00); // Inputs do not generate interrupts
	spibuf_queue(&mybuf, 0xFF); // Inputs generate interrupts
	spibuf_queue(&mybuf, 0x00); // Don't care, not using comparison to interrupt
	spibuf_queue(&mybuf, 0x00); // Interrupt whenever inputs transition
	spibuf_queue(&mybuf, MCP23S17_BANK); // New mode
	spibuf_queue(&mybuf, 0xFF); // Enable pull-up resistors
	spibuf_queue(&mybuf, 0x00); // Don't care, read-only register
	spibuf_queue(&mybuf, 0x00); // Don't care, read-only register
	spibuf_queue(&mybuf, 0x00); // Probably accomplishes nothing :P
	spi_int_enable();
	_BIS_SR(CPUOFF + GIE);
	_BIC_SR(GIE);
	spibuf_reset(&mybuf);

	char temp = 0;

	loop_forever:
		// Get a byte from MCP23S17 port B
		spibuf_queue(&mybuf, MCP23S17_R);
		spibuf_queue(&mybuf, MCP23S17_B_GPIOB);
		spibuf_expect(&mybuf, 1);
		spi_int_enable();
		_BIS_SR(CPUOFF + GIE);
		_BIC_SR(GIE);
		temp = spibuf_getrecv(&mybuf,2);
		spibuf_reset(&mybuf);

		// Transpose the bits
		temp = ((temp & 0xF0) >> 4) | ((temp & 0x0F) << 4);
		temp = ((temp & 0xCC) >> 2) | ((temp & 0x33) << 2);
		temp = ((temp & 0xAA) >> 1) | ((temp & 0x55) << 1);

		// Set the byte on MCP23S17 port A
		spibuf_queue(&mybuf, MCP23S17_W);
		spibuf_queue(&mybuf, MCP23S17_B_GPIOA);
		spibuf_queue(&mybuf, temp);
		spi_int_enable();
		_BIS_SR(CPUOFF + GIE);
		_BIC_SR(GIE);
		spibuf_reset(&mybuf);

		// Wait for an interrupt from the MCP23S17
		port1_int_enable();
		_BIS_SR(CPUOFF + SCG0 + SCG1 + GIE);
		_BIC_SR(GIE);
	goto loop_forever; // One of the few cases where "goto" is really appropriate

	return 0;
}

void port_setup(void) {
	P1OUT = BIT3 | BIT5; // P1.{3,5} are logical 1 now
	P1DIR = BIT0 | BIT5 | BIT6; // P1.{0,5,6} are GPIO outputs
	P1SEL = BIT1 | BIT2 | BIT4; // Between these two lines...
	P1SEL2 = BIT1 | BIT2 | BIT4; // ... P1.{1,2,4} are now connected to the UCA0 serial bus
	P1IES = BIT3; // Interrupts on P1.3 happen on high-low transition
}

void spi_setup(void) {
	UCA0CTL1 = UCSWRST; // Reset serial module
	UCA0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // Most Significant Bit, Master, Synchronous, Mode(0,0)
	UCA0CTL1 |= UCSSEL_2; // Clock source is the Sub-Main Clock (For peripherals)
	UCA0BR0 |= 0x02; // Divide clock by 4 to get effective SPI bus speed
	UCA0BR1 = 0; // High byte of clock division, not used
	UCA0MCTL = 0; // No modulation, unknown beyond that
	UCA0CTL1 &= ~(UCSWRST); // Start serial module
}

void spi_int_enable(void) {
	IE2 |= (UCA0RXIE + UCA0TXIE);
}

void spi_int_disable(void) {
	IE2 &= ~(UCA0RXIE + UCA0TXIE);
}

void port1_int_enable(void) {
	P1IE |= BIT3;
}

void port1_int_disable(void) {
	P1IE &= ~BIT3;
}

__attribute__((interrupt(PORT1_VECTOR)))
void port1_isr(void) {
	P1IFG = 0; // Clear the interrupt
	port1_int_disable();
	__low_power_mode_off_on_exit();
}

__attribute__((interrupt(USCIAB0RX_VECTOR)))
void spi_rx_isr(void) {
	IFG2 &= ~UCA0RXIFG; // Clear the interrupt
	if(spibuf_rx(&mybuf)) {
		P1OUT &= ~BIT6;
		spi_int_disable();
		__low_power_mode_off_on_exit();
	}
}

__attribute__((interrupt(USCIAB0TX_VECTOR)))
void spi_tx_isr(void) {
	IFG2 &= ~UCA0TXIFG; // Clear the interrupt
	if(spibuf_tx(&mybuf)) {
		P1OUT |= BIT6;
	}
}
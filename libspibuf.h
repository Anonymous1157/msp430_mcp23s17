#ifndef __LIBSPIBUF_H
#define __LIBSPIBUF_H

#define SPIBUF_MAXSIZE 16

struct spibuf {
	volatile unsigned char *cs_port;	/*
		Pointer to the port that has the CS pin on it.
		(ex: P1OUT) */
	char cs_mask; /*
		Bitmask of the correct CS pin on the port.
		(ex: BIT5)*/
	volatile unsigned char *txbuf;
	volatile unsigned char *rxbuf;	/*
		Pointers to RX and TX buffers.
		(ex: UCA0RXBUF and UCA0TXBUF) */
	int size;
	int send_curr;
	int recv_curr;
	char send[SPIBUF_MAXSIZE];
	char recv[SPIBUF_MAXSIZE];/*
		Send and receive buffers. */
};

/* Sets the pointers so we can work with the correct slave chip. */
void spibuf_init(
		struct spibuf *buf,
		typeof(buf->cs_port) cs_port,
		typeof(buf->cs_mask) cs_mask,
		typeof(buf->txbuf) txbuf,
		typeof(buf->rxbuf) rxbuf);

/* Zeroes enough information to RX/TX more data to the same device again. */
void spibuf_reset(struct spibuf *buf);

/* Adds a byte to the send buffer.
		RETURN VALUES:
	0: The byte was added to the buffer.
	1: No action. The buffer is full. */
int spibuf_queue(struct spibuf *buf, char byte);

/* Increases the number of bytes to send/receive to account for extra data to
	be received after any command/data sent.
		RETURN VALUES:
	0: The number of bytes was updated.
	1: No action. The buffer would be full. */
int spibuf_expect(struct spibuf *buf, int numbytes);

/* Jumps to a byte in the receive buffer.
		RETURN VALUES:
	0: Jumped to the byte.
	1: No action. Index out of range. */
int spibuf_seekrecv(struct spibuf *buf, int index);

/* Returns the next byte from the receive buffer.
		RETURN VALUES:
	Nonzero: Valid data.
	0: May be valid zero data or end of buffer reached. */
char spibuf_nextrecv(struct spibuf *buf);

/* Returns a byte from the receive buffer.
		RETURN VALUES:
	Nonzero: Valid data.
	0: May be valid zero data or index was out of bounds. */
char spibuf_getrecv(struct spibuf *buf, int index);

/* Check if the target chip is enabled or disabled.
		RETURN VALUES:
	0: The target chip is selected. This lib is probably busy accessing it.
	1: The target chip is deselected. This lib is probably done working. */
int spibuf_isbusy(struct spibuf *buf);

/* Select the target if necessary, then transmit a byte.
		RETURN VALUES:
	0: Sent a byte.
	1: Selected the chip and sent the first byte. */
int spibuf_tx(struct spibuf *buf);

/* Receive a byte, then deselect the target if finished.
		RETURN VALUES:
	0: Received a byte.
	1: Received the last byte and deselected the chip. */
int spibuf_rx(struct spibuf *buf);

#endif
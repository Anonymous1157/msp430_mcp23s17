#define __need_NULL
#include <stddef.h>
#include "libspibuf.h"

/*
	Sets the pointers so we can work with the correct slave chip.
*/
void spibuf_init(
		struct spibuf *buf,
		typeof(buf->cs_port) cs_port,
		typeof(buf->cs_mask) cs_mask,
		typeof(buf->txbuf) txbuf,
		typeof(buf->rxbuf) rxbuf) {
	buf->cs_port = cs_port;
	buf->cs_mask = cs_mask;
	buf->txbuf = txbuf;
	buf->rxbuf = rxbuf;
}

/*
	Zeroes enough information to RX/TX more data to the same device again.
*/
void spibuf_reset(struct spibuf *buf) {
	int i;
	buf->size = 0;
	buf->send_curr = 0;
	buf->recv_curr = 0;
	for (i = 0; i < SPIBUF_MAXSIZE; i++) {
		buf->send[i] = 0;
		buf->recv[i] = 0;
	}
}

/*
	Adds a byte to the send buffer.
*/
int spibuf_queue(struct spibuf *buf, char byte) {
	if(buf->size < SPIBUF_MAXSIZE) {
		buf->send[buf->size++] = byte;
		return 0;
	}
	return 1;
}

/*
	Increases the number of bytes to send/receive to account for extra data to be
	received after any command/data sent.
*/
int spibuf_expect(struct spibuf *buf, int numbytes) {
	if((buf->size + numbytes) < SPIBUF_MAXSIZE) {
		buf->size += numbytes;
		return 0;
	}
	return 1;
}

/*
	Returns the next byte from the receive buffer (Or 0).
*/
char spibuf_nextrecv(struct spibuf *buf) {
	char retn = 0;
	if(buf->recv_curr < buf->size) {
		retn = buf->recv[buf->recv_curr++];
	}
	return retn;
}

/*
	Jumps to a byte in the receive buffer.
*/
int spibuf_seekrecv(struct spibuf *buf, int index) {
	if(index >= 0 && index < SPIBUF_MAXSIZE) {
		buf->recv_curr = index;
		return 0;
	}
	return 1;
}

/*
	Returns a byte from the receive buffer.
*/
char spibuf_getrecv(struct spibuf *buf, int index) {
	return buf->recv[index];
}

/*
	Check if the target chip is enabled or disabled, which is an indication that
	this lib is still using the bus and struct, so it's not safe to modify them.
*/
int spibuf_isbusy(struct spibuf *buf) {
	return ((*(buf->cs_port) & buf->cs_mask) == 0);
}

/*
	Select the target if necessary, then transmit a byte.
*/
int spibuf_tx(struct spibuf *buf) {
	int retval = 0;
	// Select the target?
	if(buf->send_curr == 0) {
		*(buf->cs_port) &= ~(buf->cs_mask);
		retval = 1;
	}

	// Send a byte
	*(buf->txbuf) = buf->send[buf->send_curr];

	return retval;
} /* RETURN VALUES:
0: Sent a byte.
1: Selected the chip and sent the first byte.
*/

/*
	Receive a byte, then deselect the target if finished.
*/
int spibuf_rx(struct spibuf *buf) {
	int retval = 0;

	// Receive a byte
	buf->recv[buf->send_curr++] = *(buf->rxbuf);

	// Deselect the target?
	if(buf->send_curr == buf->size) {
		*(buf->cs_port) |= buf->cs_mask;
		retval = 1;
	}
	return retval;
} /* RETURN VALUES:
0: Received a byte.
1: Received the last byte and deselected the chip.
*/
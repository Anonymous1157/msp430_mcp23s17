#include "libspibuf.h"

#define __need_NULL
#include <stddef.h>

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

int spibuf_queue(struct spibuf *buf, char byte) {
	if(buf->size < SPIBUF_MAXSIZE) {
		buf->send[buf->size++] = byte;
		return 0;
	}
	return 1;
}

int spibuf_expect(struct spibuf *buf, int numbytes) {
	if((buf->size + numbytes) < SPIBUF_MAXSIZE) {
		buf->size += numbytes;
		return 0;
	}
	return 1;
}

char spibuf_nextrecv(struct spibuf *buf) {
	char retval = 0;

	if(buf->recv_curr < buf->size) {
		retval = buf->recv[buf->recv_curr++];
	}
	return retval;
}

int spibuf_seekrecv(struct spibuf *buf, int index) {
	if(index >= 0 && index < SPIBUF_MAXSIZE) {
		buf->recv_curr = index;
		return 0;
	}
	return 1;
}

char spibuf_getrecv(struct spibuf *buf, int index) {
	if(index >= 0 && index < SPIBUF_MAXSIZE) {
		return buf->recv[index];
	}
	return 0;
}

int spibuf_isbusy(struct spibuf *buf) {
	return ((*(buf->cs_port) & buf->cs_mask) == 0);
}

int spibuf_tx(struct spibuf *buf) {
	int retval = 0;

	if(buf->send_curr == 0) {
		// Select the target
		*(buf->cs_port) &= ~(buf->cs_mask);
		retval = 1;
	}

	// Send a byte
	*(buf->txbuf) = buf->send[buf->send_curr];

	return retval;
}

int spibuf_rx(struct spibuf *buf) {
	int retval = 0;

	// Receive a byte
	buf->recv[buf->send_curr++] = *(buf->rxbuf);

	if(buf->send_curr == buf->size) {
		// Deselect the target
		*(buf->cs_port) |= buf->cs_mask;
		retval = 1;
	}
	return retval;
}
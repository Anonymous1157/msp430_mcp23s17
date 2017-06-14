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
		Send and receive buffers.
		*/
};

void spibuf_init(
		struct spibuf *buf,
		typeof(buf->cs_port) cs_port,
		typeof(buf->cs_mask) cs_mask,
		typeof(buf->txbuf) txbuf,
		typeof(buf->rxbuf) rxbuf);
void spibuf_reset(struct spibuf *buf);

int spibuf_queue(struct spibuf *buf, char byte);
int spibuf_expect(struct spibuf *buf, int numbytes);

int spibuf_seekrecv(struct spibuf *buf, int index);
char spibuf_nextrecv(struct spibuf *buf);
char spibuf_getrecv(struct spibuf *buf, int index);

int spibuf_isbusy(struct spibuf *buf);

int spibuf_tx(struct spibuf *buf);
int spibuf_rx(struct spibuf *buf);
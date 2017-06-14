void port_setup(void);
void spi_setup(void);

void spi_int_enable(void);
void spi_int_disable(void);
void port1_int_enable(void);
void port1_int_disable(void);

void port1_isr(void);
void spi_rx_isr(void);
void spi_tx_isr(void);
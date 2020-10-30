#ifndef SPI_H
#define SPI_H

egl_interface_t *spi(void);
void spi_dma_tx_irq(void);
void spi_exti_irq(void);

#endif // SPI_H

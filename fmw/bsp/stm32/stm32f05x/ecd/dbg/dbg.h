#ifndef DBG_H
#define DBG_H

/* IOCTL operaions */
enum
{
  DBG_WRITE_INTERRUPT_IOCTL,
  DBG_WRITE_POLLING_IOCTL,
  DBG_WRITE_DMA_IOCTL
};

egl_interface_t *dbg(void);
void dbg_irq(void);
void dbg_dma_irq(void);

#endif // ECD_DBG_UART_H

#ifndef DBG_H
#define DBG_H

/* IOCTL operaions */
enum
{
  DBU_WRITE_INTERRUPT_IOCTL,
  DBG_WRITE_POLLING_IOCTL
};

egl_interface_t *dbg(void);
void dbg_irq(void);

#endif // ECD_DBG_UART_H

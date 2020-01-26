#ifndef ECD_DBG_UART_H
#define ECD_DBG_UART_H

/* IOCTL operaions */
enum
  {
    ECD_DBU_UART_WRITE_INTERRUPT_IOCTL,
    ECD_DBG_UART_WRITE_POLLING_IOCTL
  };

egl_interface_t *ecd_dbg_usart(void);

#endif // ECD_DBG_UART_H

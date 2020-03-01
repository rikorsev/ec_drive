#include <stdint.h>
#include <stdlib.h>

#include "egl_crc.h"

/* Calculating XMODEM CRC-16 in 'C'
   ================================
   Reference model for the translated code */

#define poly 0x1021

/* On entry, addr=>start of data
             num = length of data
             crc = incoming CRC     */

uint16_t egl_crc16_xmodem_calc(uint16_t crc, void* data, size_t len)
{
  int crc_int = crc;
  
  for (; len > 0; len--)                               /* Step through bytes in memory */
  {
    crc_int = crc_int ^ (*(uint8_t *)data++ << 8);     /* Fetch byte from memory, XOR into CRC top byte*/
    for (int i = 0; i < 8; i++)                        /* Prepare to rotate 8 bits */
    {
      crc_int = crc_int << 1;                          /* rotate */
      if (crc_int & 0x10000)                           /* bit 15 was set (now bit 16)... */
        crc_int = (crc_int ^ poly) & 0xFFFF;           /* XOR with XMODEM polynomic */
                                                       /* and ensure CRC remains 16-bit value */
    }                                                  /* Loop for 8 bits */
  }                                                    /* Loop until num=0 */
  return (uint16_t)crc_int;                            /* Return updated CRC */
}

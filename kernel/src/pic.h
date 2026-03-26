#ifndef PIC_H
#define PIC_H

#include <stdbool.h>
#include <stdint.h>

#define PIC_MASTER_CMD  0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD   0xA0
#define PIC_SLAVE_DATA  0xA1

void pic_init(void);
void pic_mask_irq(uint8_t irq, bool mask);
void pic_send_eoi(uint8_t irq);

#endif
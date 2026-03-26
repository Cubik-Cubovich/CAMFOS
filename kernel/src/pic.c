#include "pic.h"
#include "io.h"

void pic_init(void) {
    outb(PIC_MASTER_CMD, 0x11); // ICW1: init, ICW4 needed
    outb(PIC_SLAVE_CMD,  0x11);

    outb(PIC_MASTER_DATA, 0x20); // ICW2: master vector offset
    outb(PIC_SLAVE_DATA,  0x28); // slave vector offset

    outb(PIC_MASTER_DATA, 0x04); // ICW3: slave on IRQ2
    outb(PIC_SLAVE_DATA,  0x02); // ICW3: slave ID

    outb(PIC_MASTER_DATA, 0x01); // ICW4: 8086 mode
    outb(PIC_SLAVE_DATA,  0x01);

    // маскируем все прерывания (потом размаскируем нужные)
    outb(PIC_MASTER_DATA, 0xFF);
    outb(PIC_SLAVE_DATA,  0xFF);
}

void pic_mask_irq(uint8_t irq, bool mask) {
    uint16_t port;
    uint8_t mask_byte;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
        mask_byte = inb(port);
        if (mask)
            mask_byte |= (1 << irq);
        else
            mask_byte &= ~(1 << irq);
        outb(port, mask_byte);
    } else if (irq < 16) {
        port = PIC_SLAVE_DATA;
        mask_byte = inb(port);
        irq -= 8;
        if (mask)
            mask_byte |= (1 << irq);
        else
            mask_byte &= ~(1 << irq);
        outb(port, mask_byte);
    }
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC_SLAVE_CMD, 0x20);
    outb(PIC_MASTER_CMD, 0x20);
}
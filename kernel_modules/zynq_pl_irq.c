#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/irq.h>

// SPI Status registers
#define SPI_STATUS_0 0x00001D04
#define SPI_STATUS_1 0x00001D08

// Interrupt used in the module.
// It doesn't seem possible to handle several interrupts in a single UIO
// driver since only one IRQ ID can be declared in the uio_info structure.
#define MOD_IRQ 0

// Interrupt channel description
// TODO Add required type
struct pl_irq_info {
    u32 reference;
    u32 id;
    int status_reg; // 0 for spi_status_0, 1 for spi_status_1
    u32 status_offset;
};

// Used IRQ channel
static struct pl_irq_info *current_irq = NULL;

// IRQ ID# of each PL interrupt
// Reference: Chapter 7
// http://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.pdf
// /!\ Some padding issues
// https://forums.xilinx.com/t5/Embedded-Linux/Zynq-ZC702-Unable-to-detect-interrupt-from-custom-PL-device/td-p/237232
static struct pl_irq_info zynq_pl_irqs[] = {
    {
        .reference = 0,
        .id = 61,
        .status_reg = 0,
        .status_offset = 29
    },
    {
        .reference = 1,
        .id = 62,
        .status_reg = 0,
        .status_offset = 30
    },
    {
        .reference = 2,
        .id = 63,
        .status_reg = 0,
        .status_offset = 31
    },
    {
        .reference = 3,
        .id = 64,
        .status_reg = 1,
        .status_offset = 0
    },
    {
        .reference = 4,
        .id = 65,
        .status_reg = 1,
        .status_offset = 1
    },
    {
        .reference = 5,
        .id = 66,
        .status_reg = 1,
        .status_offset = 2
    },
    {
        .reference = 6,
        .id = 67,
        .status_reg = 1,
        .status_offset = 3
    },
    {
        .reference = 7,
        .id = 68,
        .status_reg = 1,
        .status_offset = 4
    },
    {
        .reference = 8,
        .id = 84,
        .status_reg = 1,
        .status_offset = 20
    },
    {
        .reference = 9,
        .id = 85,
        .status_reg = 1,
        .status_offset = 21
    },
    {
        .reference = 10,
        .id = 86,
        .status_reg = 1,
        .status_offset = 22
    },
    {
        .reference = 11,
        .id = 87,
        .status_reg = 1,
        .status_offset = 23
    },
    {
        .reference = 12,
        .id = 88,
        .status_reg = 1,
        .status_offset = 24
    },
    {
        .reference = 13,
        .id = 89,
        .status_reg = 1,
        .status_offset = 25
    },
    {
        .reference = 14,
        .id = 90,
        .status_reg = 1,
        .status_offset = 26
    },
    {
        .reference = 15,
        .id = 91,
        .status_reg = 1,
        .status_offset = 27
    }
};

static irqreturn_t irq_handler(int irq, struct uio_info *dev_info) {
    if (irq == MOD_IRQ) {
        // TODO Read the spi_status register
        printk(KERN_INFO "Interrupt received\n");
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static struct uio_info zynq_pl_irq_uio_info = {
    .name = "uio_zynq_pl_irq",
    .version = "0.1",
    .irq = UIO_IRQ_NONE,
    .handler = irq_handler,
};

static struct platform_device uio_device = { 
    .name = "uio_pdrv", 
    .id = -1,
    .dev.platform_data = &zynq_pl_irq_uio_info,
};

static int __init zynq_pl_irq_init(void)
{
    int i;
    int ret = platform_device_register(&uio_device);

    for (i=0; i<ARRAY_SIZE(zynq_pl_irqs); i++)
        if (zynq_pl_irqs[i].reference == MOD_IRQ) {
            zynq_pl_irq_uio_info.irq = zynq_pl_irqs[i].id;
            irq_set_irq_type(zynq_pl_irqs[i].reference, IRQ_TYPE_EDGE_RISING);
            current_irq = &zynq_pl_irqs[i];
        }

    if (current_irq == NULL) {
        printk(KERN_ERR "zynq_pl_irq: Invalid interrupt reference %d\n", MOD_IRQ);
        return -EINVAL;
    }

    // IRQs 8 to 15 (ID# 91:84) seem to be high level only.
    // TODO Check this ...
    

    printk(KERN_INFO "zynq_pl_irq: Module loaded\n");
    return 0;
}

static void __exit zynq_pl_irq_exit(void)
{
    platform_device_unregister(&uio_device);
}

module_init(zynq_pl_irq_init);
module_exit(zynq_pl_irq_exit);


MODULE_AUTHOR("");
MODULE_DESCRIPTION("Zynq PL interrupts handler");
MODULE_LICENSE("GPL");

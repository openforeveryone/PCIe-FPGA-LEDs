#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/leds.h>

#define FPGABOARD_VENDOR_ID 0x1172
#define FPGABOARD_DEVICE_ID 0x1234

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OFE");
MODULE_DESCRIPTION("FPGA Board LEDs");

static struct pci_device_id fpgaboard_ids[] = {
        { PCI_DEVICE(FPGABOARD_VENDOR_ID, FPGABOARD_DEVICE_ID) },
        { }
};
MODULE_DEVICE_TABLE(pci, fpgaboard_ids);

struct fpgaboard {
        struct led_classdev userLED0;
        void __iomem *ptr_bar0;
};

static int fpgaboard_led_brightness_set(struct led_classdev *cdev, enum led_brightness brightness) {
        u8 reg_value, mask, new_value;
        struct fpgaboard *board = container_of(cdev, struct fpgaboard, userLED0);
        
        printk(KERN_INFO "FPGA Board LEDs: LED brightness set to:%d\n", (int)brightness);

        mask = ~((u8) 1);
        new_value = (u8) 1 & (u8) brightness;
        reg_value = ioread8(board->ptr_bar0);
        iowrite8((reg_value & mask) | new_value, board->ptr_bar0);
        return 0;
}

static int fpgaboard_probe(struct pci_dev *dev, const struct pci_device_id *id) {
        int status;
        void __iomem *const* bar_map;
        struct fpgaboard *board;

        printk(KERN_INFO "FPGA Board LEDs: Device registered\n");
        
        status = pci_resource_len(dev, 0);
        if (status < 1)
                return 0;
        
        printk(KERN_INFO "FPGA Board LEDs: BAR0 is at %llx\n", pci_resource_start(dev, 0));
        printk(KERN_INFO "FPGA Board LEDs: Size of BAR0: %d\n", status);

        status = pcim_enable_device(dev);
        if (status < 0) {
                printk(KERN_ERR "FPGA Board LEDs: Cannot enable device\n");
                return status;
        }

        status = pcim_iomap_regions(dev, BIT(0), KBUILD_MODNAME);
        if (status < 0) {
                printk(KERN_ERR "FPGA Board LEDs: Cannot map region, maybe region is in use\n");
                return status;
        }

        bar_map = pcim_iomap_table(dev);
        if (bar_map == NULL) {
                printk(KERN_ERR "FPGA Board LEDs: Invald BAR0\n");
                return -1;
        }

        board = devm_kzalloc(&dev->dev, sizeof(struct fpgaboard), GFP_KERNEL);
        board->ptr_bar0 = bar_map[0];

        board->userLED0.name = "fpga_board::user0";
        board->userLED0.max_brightness = 1;
        board->userLED0.brightness_set_blocking = fpgaboard_led_brightness_set;

        status = devm_led_classdev_register(&dev->dev, &board->userLED0);
        if (status < 0) {
                printk(KERN_ERR "FPGA Board LEDs: CAnnot register LED class\n");
                return status;
        }

        return 0;
}

static void fpgaboard_remove(struct pci_dev *dev) {
        printk(KERN_INFO "FPGA Board LEDs: Device unregistered\n");
}

static struct pci_driver fpgaboardio_driver = {
        .name = "fpgaboardleds",
        .id_table = fpgaboard_ids,
        .probe = fpgaboard_probe,
        .remove = fpgaboard_remove,
};

static int __init ModuleInit(void) {
        printk(KERN_INFO "FPGA Board LEDs: Module Init\n");
        return pci_register_driver(&fpgaboardio_driver);
}

static void __exit ModuleExit(void) {
        printk(KERN_INFO "FPGA Board LEDs: Module Exit\n");
        pci_unregister_driver(&fpgaboardio_driver);
}

module_init(ModuleInit);
module_exit(ModuleExit);

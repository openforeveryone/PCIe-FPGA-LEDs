#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/leds.h>

#define FPGABOARD_VENDOR_ID 0x1172
#define FPGABOARD_DEVICE_ID 0x1234

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthew Wellings");
MODULE_DESCRIPTION("FPGA Board LEDs");

static struct pci_device_id fpgaboard_ids[] = {
        { PCI_DEVICE(FPGABOARD_VENDOR_ID, FPGABOARD_DEVICE_ID) },
        { }
};
MODULE_DEVICE_TABLE(pci, fpgaboard_ids);

struct user_led {
        int led_num;
        struct led_classdev led_dev;
        void __iomem *device_reg;
        int bit;
};

struct fpgaboard {
        struct user_led user_leds[4];
        void __iomem *ptr_bar0;
        struct mutex lock;
};

static int fpgaboard_led_brightness_set(struct led_classdev *cdev, enum led_brightness brightness) {
        u8 reg_value, mask, new_value;
        struct user_led *led = container_of(cdev, struct user_led, led_dev);
        struct fpgaboard *board = container_of(led, struct fpgaboard, user_leds[led->led_num]);        

        printk(KERN_INFO "FPGA Board LEDs: LED %d brightness set to: %d\n", led->led_num, (int)brightness);

        mask = ~((u8) 1 << led->bit);
        new_value = ((u8) 1 & (u8) brightness) << led->bit;
        mutex_lock(&board->lock);
        reg_value = ioread8(led->device_reg);
        iowrite8((reg_value & mask) | new_value, led->device_reg);
        mutex_unlock(&board->lock);

        return 0;
}

static enum led_brightness fpgaboard_led_brightness_get(struct led_classdev *cdev) {
        u8 reg_value;
        struct user_led *led = container_of(cdev, struct user_led, led_dev);

        reg_value = ioread8(led->device_reg);
        return (reg_value >> led->bit) & 1;
}

static int fpgaboard_probe(struct pci_dev *dev, const struct pci_device_id *id) {
        int status, i;
        void __iomem *const* bar_map;
        struct fpgaboard *board;
        char *led_name;

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
        printk(KERN_INFO "FPGA Board LEDs: fpgaboard: %p\n", board);

        for (i = 0; i < 4; i++) {
                board->user_leds[i].bit=i;
                board->user_leds[i].led_num=i;
                board->user_leds[i].device_reg=board->ptr_bar0;
                led_name = devm_kzalloc(&dev->dev, 19, GFP_KERNEL);
                snprintf(led_name, 19, "fpga_board::user%d", i);
                board->user_leds[i].led_dev.name = led_name;
                board->user_leds[i].led_dev.max_brightness = 1;
                board->user_leds[i].led_dev.brightness_set_blocking = fpgaboard_led_brightness_set;
                board->user_leds[i].led_dev.brightness_get = fpgaboard_led_brightness_get;
                
                status = devm_led_classdev_register(&dev->dev, &board->user_leds[i].led_dev);
                if (status < 0) {
                        printk(KERN_ERR "FPGA Board LEDs: Cannot register LED %d class\n", i);
                        return status;
                }
        }

        return 0;
}

static struct pci_driver fpgaboardio_driver = {
        .name = "fpgaboardleds",
        .id_table = fpgaboard_ids,
        .probe = fpgaboard_probe,
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

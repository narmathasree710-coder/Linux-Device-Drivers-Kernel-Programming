#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "gpioioctl"
#define GPIO_MAGIC 'G'

#define GPIO_EXPORT      _IOW(GPIO_MAGIC, 1, int)
#define GPIO_SET_DIR     _IOW(GPIO_MAGIC, 2, int)
#define GPIO_WRITE       _IOW(GPIO_MAGIC, 3, int)
#define GPIO_READ        _IOR(GPIO_MAGIC, 4, int)

static int major;

/* Simulated GPIO state */
static int gpio_exported = 0;
static int gpio_direction = 0; // 0=input, 1=output
static int gpio_value = 0;

struct gpio_data {
    int pin;
    int value;
};

/* IOCTL handler */
static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct gpio_data data;

    switch (cmd) {

    case GPIO_EXPORT:
        if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
            return -EFAULT;

        gpio_exported = 1;
        printk(KERN_INFO "GPIO: Exported pin %d\n", data.pin);
        break;

    case GPIO_SET_DIR:
        if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
            return -EFAULT;

        gpio_direction = data.value;
        printk(KERN_INFO "GPIO: Direction set to %s\n",
               gpio_direction ? "OUTPUT" : "INPUT");
        break;

    case GPIO_WRITE:
        if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
            return -EFAULT;

        if (!gpio_exported || gpio_direction != 1) {
            printk(KERN_WARNING "GPIO: Write not allowed\n");
            return -EPERM;
        }

        gpio_value = data.value;
        printk(KERN_INFO "GPIO: Value written = %d\n", gpio_value);
        break;

    case GPIO_READ:
        if (!gpio_exported)
            return -EPERM;

        data.value = gpio_value;

        if (copy_to_user((void __user *)arg, &data, sizeof(data)))
            return -EFAULT;

        printk(KERN_INFO "GPIO: Value read = %d\n", gpio_value);
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

/* file ops */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = gpio_ioctl,
};

/* init */
static int __init gpio_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "GPIO ioctl driver loaded. Major=%d\n", major);
    return 0;
}

/* exit */
static void __exit gpio_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "GPIO ioctl driver unloaded\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");

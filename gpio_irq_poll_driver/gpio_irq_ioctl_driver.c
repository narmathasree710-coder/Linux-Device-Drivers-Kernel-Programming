#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/wait.h>

#define DEVICE_NAME "gpioirqdev"
#define GPIO_MAGIC 'G'

#define GPIO_SET_PIN     _IOW(GPIO_MAGIC, 1, int)
#define GPIO_SET_EDGE    _IOW(GPIO_MAGIC, 2, int)
#define GPIO_ENABLE_IRQ  _IO(GPIO_MAGIC, 3)
#define GPIO_DISABLE_IRQ _IO(GPIO_MAGIC, 4)
#define GPIO_GET_EVENT   _IOR(GPIO_MAGIC, 5, int)

static int major;

/* Simulated GPIO IRQ state */
static int gpio_pin = 17;
static int irq_enabled = 0;
static int event_flag = 0;
static int irq_count = 0;

/* wait queue for poll/select */
static DECLARE_WAIT_QUEUE_HEAD(gpio_wq);

/* ---------------- IRQ Handler (SIMULATED) ---------------- */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    if (!irq_enabled)
        return IRQ_NONE;

    irq_count++;
    event_flag = 1;

    printk(KERN_INFO "GPIO IRQ occurred! count=%d\n", irq_count);

    wake_up_interruptible(&gpio_wq);

    return IRQ_HANDLED;
}

/* ---------------- POLL FUNCTION ---------------- */
static unsigned int gpio_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;

    poll_wait(file, &gpio_wq, wait);

    if (event_flag)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

/* ---------------- IOCTL HANDLER ---------------- */
static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int val;

    switch (cmd) {

    case GPIO_SET_PIN:
        if (copy_from_user(&val, (int __user *)arg, sizeof(val)))
            return -EFAULT;

        gpio_pin = val;
        printk(KERN_INFO "GPIO: pin set to %d\n", gpio_pin);
        break;

    case GPIO_ENABLE_IRQ:

    	irq_enabled = 1;
    	event_flag = 0;
    	printk(KERN_INFO "GPIO IRQ enabled\n");

    /* Simulate interrupt */
    	gpio_irq_handler(0, NULL);

    	break;

    case GPIO_DISABLE_IRQ:
        irq_enabled = 0;
        printk(KERN_INFO "GPIO IRQ disabled\n");
        break;

    case GPIO_GET_EVENT:
        val = event_flag;

        if (copy_to_user((int __user *)arg, &val, sizeof(val)))
            return -EFAULT;

        event_flag = 0;
        printk(KERN_INFO "GPIO event read\n");
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

/* ---------------- FILE OPS ---------------- */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = gpio_ioctl,
    .poll = gpio_poll,
};

/* ---------------- INIT ---------------- */
static int __init gpio_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);

    printk(KERN_INFO "GPIO IRQ ioctl driver loaded. Major=%d\n", major);

    /* NOTE:
     * In real hardware:
     * request_irq(GPIO_IRQ, gpio_irq_handler, ...)
     *
     * Here we simulate IRQ externally or via test trigger.
     */

    return 0;
}

/* ---------------- EXIT ---------------- */
static void __exit gpio_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "GPIO IRQ driver unloaded\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");

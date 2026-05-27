#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

#define KEYBOARD_IRQ 1
#define TIMEOUT_SEC 5

static unsigned long last_activity;
static struct timer_list my_timer;

/* Keyboard Interrupt Handler */
static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    last_activity = jiffies;

    printk(KERN_INFO "Keyboard Activity Detected\n");

    return IRQ_HANDLED;
}

/* Timer Callback Function */
static void timer_callback(struct timer_list *t)
{
    unsigned long elapsed;

    elapsed = jiffies - last_activity;

    /* Convert timeout seconds into jiffies */
    if (elapsed > TIMEOUT_SEC * HZ)
    {
        printk(KERN_INFO "Keyboard Timeout Occurred\n");

        /* Reset activity */
        last_activity = jiffies;
    }

    /* Restart timer after 1 second */
    mod_timer(&my_timer, jiffies + HZ);
}

/* Module Init */
static int __init keyboard_timeout_init(void)
{
    int result;

    printk(KERN_INFO "Keyboard Timeout Module Loaded\n");

    last_activity = jiffies;

    /* Register Keyboard IRQ */
    result = request_irq(KEYBOARD_IRQ,
                         keyboard_irq_handler,
                         IRQF_SHARED,
                         "keyboard_timeout",
                         (void *)&keyboard_irq_handler);

    if (result)
    {
        printk(KERN_ERR "Cannot register keyboard IRQ\n");
        return result;
    }

    printk(KERN_INFO "Keyboard IRQ Registered\n");

    /* Setup timer */
    timer_setup(&my_timer, timer_callback, 0);

    /* Start timer */
    mod_timer(&my_timer, jiffies + HZ);

    return 0;
}

/* Module Exit */
static void __exit keyboard_timeout_exit(void)
{
    del_timer(&my_timer);

    free_irq(KEYBOARD_IRQ, (void *)&keyboard_irq_handler);

    printk(KERN_INFO "Keyboard Timeout Module Removed\n");
}

module_init(keyboard_timeout_init);
module_exit(keyboard_timeout_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Keyboard Interrupt Timeout Using Jiffies");

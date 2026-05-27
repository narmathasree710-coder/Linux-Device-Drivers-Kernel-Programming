#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/jiffies.h>

static int __init jiffies_init(void)
{
    printk(KERN_INFO "Module Loaded\n");
    printk(KERN_INFO "Current Jiffies = %lu\n", jiffies);

    return 0;
}

static void __exit jiffies_exit(void)
{
    printk(KERN_INFO "Module Removed\n");
    printk(KERN_INFO "Current Jiffies = %lu\n", jiffies);
}

module_init(jiffies_init);
module_exit(jiffies_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Jiffies Example");

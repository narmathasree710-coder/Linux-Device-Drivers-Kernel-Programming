#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohan");
MODULE_DESCRIPTION("Simple Hello Kernel Module");
MODULE_VERSION("1.0");

/* Function called when module is loaded */
static int __init hello_init(void)
{
    printk(KERN_INFO "Hello: Kernel Module Loaded\n");
    return 0;
}

/* Function called when module is removed */
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Hello: Kernel Module Removed\n");
}

module_init(hello_init);
module_exit(hello_exit);

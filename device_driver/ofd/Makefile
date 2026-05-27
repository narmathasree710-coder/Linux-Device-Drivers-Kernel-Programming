// our first driver
#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>

static int __init ofd_init(){ // __init will tell the compiler that it is not a normal func
	// constructor call
	printf(KERN_INFO "Welcome : ofd registered");
	return 0;
}

static int __exit ofd_exit(){
	// destructor
	printf(KERN_INFO "Exit : ofd unregistered");
	return 0;

}

module_init(ofd_init); // this is an constructor call
module_exit(ofd_exit); // destructor

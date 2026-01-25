#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(hello_linux_test){
	printk("Hello Linux Test!\n");
	return 0;
}

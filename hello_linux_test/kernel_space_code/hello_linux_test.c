#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(hello){
    printk("Hello Linux Test!\n");
    return 0;
}
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_NAME "hello"

static ssize_t hello_read(struct file *file, char __user *buf,
              size_t count, loff_t *ppos)
{
    char msg[] = "Hello world!\n";
    return simple_read_from_buffer(buf, count, ppos, msg, sizeof(msg));
}

static const struct proc_ops hello_ops = {
    .proc_read = hello_read,
};

static int __init hello_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &hello_ops);
    printk(KERN_INFO "hello module loaded\n");
    return 0;
}

static void __exit hello_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "hello module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

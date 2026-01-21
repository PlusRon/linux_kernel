#include "../include/user_hello_linux_test.h"
// #include <linux/kernel.h>
// #include <unistd.h>
#include <sys/syscall.h>

long int user_hello_linux_test(){
    // return 0;
    return syscall(449);
}
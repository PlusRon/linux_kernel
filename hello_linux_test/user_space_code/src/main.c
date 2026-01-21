#include <stdio.h>
#include "../include/user_hello_linux_test.h"
// #include <linux/kernel.h>
// #include <unistd.h>
// #include <sys/syscall.h>

int main(void){

    printf("sys_hello_linux_test, return %ld\n", user_hello_linux_test());

    return 0;
}
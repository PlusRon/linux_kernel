#include "../include/user_get_physical_addresses.h"
#include <stdlib.h>
#include <stdio.h>

unsigned long user_get_physical_addresses(void* virtual_address){
    unsigned long p_addr = 0;
    unsigned long v_addr = (unsigned long *)virtual_address; 
    if(syscall(450, &v_addr, &p_addr) < 0){
    	perror("ERROR : 'syscall()' failed.\n");
	return -1;
    }

    return p_addr;
}


/*
long user_get_physical_addresses(void* vaddr){
     return syscall(451, vaddr);
}
*/

/*
unsigned long* user_get_physical_addresses(unsigned long* virtual_address){
    unsigned long* res = (unsigned long*)malloc(sizeof(unsigned long));
    syscall(450, virtual_address, res);
    return res;
}
*/

/*
int a = 123;
unsigned long* physical_address = user_get_physical_addresses(&a);
printf("the physical address of global variable a is %lx\n", *physical_address);
free(physical_address;)
*/

#include <linux/syscalls.h>
#include <linux/kernel.h>

SYSCALL_DEFINE1(skprint, char *, data){
    char buffer[256];
  	long copied = strncpy_from_user(buffer, data, sizeof(buffer));
	printk( "sk syscall called with \"%s\"\n", buffer);
  return 0;
}


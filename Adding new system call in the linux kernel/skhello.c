#include <linux/syscalls.h>
#include <linux/kernel.h>
SYSCALL_DEFINE0(skhello)
{
    printk("Hello TA \n");
    return 0;
}


#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/cred.h>
#include <linux/sched.h>

SYSCALL_DEFINE0(skprocess){
    printk("PID of parent process: %d",current->parent->pid);
    printk("PID of current process: %d",current->pid);
    return 0;
}                                                                                                           

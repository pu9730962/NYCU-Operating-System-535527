#include <linux/kernel.h>
#include <linux/syscalls.h>
SYSCALL_DEFINE0(hello)
{
        printk("Hello,world!\n");
        printk("311512039\n");
        return 0;
}
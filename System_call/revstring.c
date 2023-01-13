#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE2(revstring,
                int,len,
                char __user*,ch){
                    char revch;
                    unsigned long lenth =len;
                    char copych[lenth+1];
                    copy_from_user(copych,ch,lenth+1);
                    printk("The original string:%s\n",copych);

                    for(int i=0;i<=lenth/2;i++){
                    revch=copych[i];
                    copych[i]=copych[lenth-i-1];
                    copych[lenth-i-1]=revch;
                                            }
                    printk("The reversed string:%s\n",copych);
                    return 0;
                }
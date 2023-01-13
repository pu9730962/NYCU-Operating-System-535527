#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/printk.h> /* for pr_* */
#include <linux/cpumask.h> /* for num_online_cpus */
#include <linux/smp.h>
#include <asm/cpu.h>
#include <linux/mm.h>
#include<linux/kernel.h>
#include<linux/utsname.h>
#include <linux/sched.h>
#include<linux/pid_namespace.h>
#include <linux/proc_fs.h>
#include <linux/cpu.h>
#include <linux/jiffies.h>

int init_module(void);
void cleanup_module(void);
static int kfetch_open(struct inode *, struct file *);
static int kfetch_release(struct inode *, struct file *);
static ssize_t kfetch_read(struct file *, char *, size_t, loff_t *);
static ssize_t kfetch_write(struct file *, const char *, size_t, loff_t *);
#define DEVICE_NAME "kfetch"
static struct class *cls;
static int Register;
static int mask_info;
unsigned int cpu = 0;
struct cpuinfo_x86 *c;
struct sysinfo si;
unsigned long free_ram,total_ram;
static int thread_count;
char bird0[]="                        ";
char bird1[]="      .-.               ";
char bird2[]="     (.. |              ";  
char bird3[]="     <>  |              ";      
char bird4[]="    / --- \\             ";     
char bird5[]="   ( |   | |            ";
char bird6[]=" |\\\\_)___/\\)/\\          ";
char bird7[]="<__)------(__/          ";
char *string[150];
// inforamation declaration
char hostname[30];
char kernel[50];
char CPU_name[50];
char CPU_core[30];
char Mem[30];
char process_num[30];
char run_time[30];
char *information[50];
char start[150];
char dash[30];
char output[500];


void get_thread_count(void){
    struct task_struct *task;
    for_each_process(task)
        thread_count += task->signal->nr_threads;
}
unsigned long uptime;


static struct file_operations kfetch_ops = {
    .owner   = THIS_MODULE,
    .read    = kfetch_read,
    .write   = kfetch_write,
    .open    = kfetch_open,
    .release = kfetch_release,
};

int init_module(void){

    Register=register_chrdev(0,DEVICE_NAME,&kfetch_ops); //成功返回0，失敗返回-12
    if (Register < 0) {
        pr_alert("Registering char device failed with %d\n", Register);
        return Register;
    }

    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(Register, 0), NULL, DEVICE_NAME);

    return 0;
}

void cleanup_module(void){
    device_destroy(cls, MKDEV(Register, 0));
    class_destroy(cls);
    unregister_chrdev(Register, DEVICE_NAME);
}



static int kfetch_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return 0;
}

static int kfetch_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t kfetch_write(struct file *filp,const char __user *buffer,size_t length,loff_t *offset)
{
    int count=0;
    int flag;
    int temp;
    if (copy_from_user(&mask_info, buffer, length)) {
        pr_alert("Failed to copy data from user");
        return 0;
    }
    sprintf(hostname,"%s\n",utsname()->nodename);                //hostname
    sprintf(kernel,"Kernel:     %s\n",utsname()->release);       //kernel name
    c = &cpu_data(cpu);
    if (c->x86_model_id[0]){                                     //cpu name
        // printk("CPU: %s\n", c->x86_model_id);
        pr_info("CPU: %s\n", c->x86_model_id);
        sprintf(CPU_name,"CPU:        %s\n",c->x86_model_id);}
    else{
        // pr_cont("%d86", c->x86);
        pr_info("%d86", c->x86);
        sprintf(CPU_name,"CPU:        %d86\n",c->x86);}
    // sprintf(CPU_core,"CPUs:       %d/%d\n",num_online_cpus(),nr_cpu_ids);   //cpu cores
    sprintf(CPU_core,"CPUs:       %d/%d\n",num_online_cpus(),num_active_cpus());   //cpu cores
    si_meminfo(&si);                                              //memory
    free_ram = si.freeram * PAGE_SIZE;
    total_ram = si.totalram * PAGE_SIZE;
    sprintf(Mem,"Mem:        %lu Gb/%lu Gb\n",free_ram/1000000000,total_ram/1000000000);
    get_thread_count();                                       //process numbers
    sprintf(process_num,"Procs:      %d\n",thread_count);
    uptime = jiffies_to_msecs(jiffies) / 1000;
    sprintf(run_time,"Untime:     %lu seconds\n",uptime/60);  //run time
    information[0]=kernel;
    information[1]=CPU_name;
    information[2]=CPU_core;
    information[3]=Mem;
    information[4]=process_num;
    information[5]=run_time;
    memset(dash,'-',(strlen(hostname)-1));  //複製字符c（一個無符號字符）到參數所指向的字符串的前n個字符。
    dash[strlen(hostname)]='\0';
    sprintf(start,"%s%s%s%s\n",bird0,hostname,bird1,dash); //start有包含\n
    string[0]=bird2;
    string[1]=bird3;
    string[2]=bird4;
    string[3]=bird5;
    string[4]=bird6;
    string[5]=bird7;
    sprintf(output,start);
    for(int i=0;i<6;i++){
        Again:
            flag=1<<count;
            temp=flag&mask_info;
            if(temp!=0){
                sprintf(output+strlen(output),string[i]);
                sprintf(output+strlen(output),information[count]);
                count++;
            }
            else if(temp==0){
                if(count<6){
                    count++;
                    goto Again; 
                }
                else if(count==6){
                    sprintf(output+strlen(output),string[i]);
                    sprintf(output+strlen(output),"\n");
                }
            }
    }


    /* setting the information mask */
    return 0;
}

static ssize_t kfetch_read(struct file *filp,char __user *buffer,size_t length,loff_t *offset)
{   
    if (copy_to_user(buffer, output, sizeof(output))) {         
        pr_alert("Failed to copy data to user");
        return 0;
    }
    
    return 0;
}
MODULE_LICENSE("GPL");
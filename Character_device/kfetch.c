/*
 * kfetch.c: user-space program for interacting with kfetch kernel module
 * by Cycatz <cycatz@staque.xyz>, 2022
 * License: GPLv3
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include "kfetch.h"

#define err_quit(msg)       \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

void kfetch_set_info(int fd, int mask)
{
    int len;
    len = write(fd, &mask, sizeof(mask)); //write()將mask指向的資料寫入sizeof(mask)個位元組到fd，返回實數際寫入的字元組
    if (len < 0)
        err_quit("write");
}

void kfetch_print_info(int fd)
{

    char buf[KFETCH_BUF_SIZE];
    read(fd, buf, sizeof(buf));
    printf("%s\n",buf);

    
}

void usage(const char *progname)
{
    fprintf(stderr,
            "Usage:\n"
            "\t%s [options]\n",
            progname);
    fprintf(stderr,
            "Options:\n"
            "\t-a  Show all information\n"
            "\t-c  Show CPU model name \n"
            "\t-m  Show memory information\n"
            "\t-n  Show the number of CPU cores\n"
            "\t-p  Show the number of processes\n"
            "\t-r  Show the kernel release information\n"
            "\t-u  Show how long the system has been running\n");
}

int main(int argc, char *argv[])
{
    int fd;
    int opt;
    int mask_info;

    mask_info = -1;
    while ((opt = getopt(argc, argv, "acnmpruh")) != -1) {
        if (mask_info < 0)
            mask_info = 0;
        switch (opt) {
        case 'a': mask_info = KFETCH_FULL_INFO; break;
        case 'c': mask_info |= KFETCH_CPU_MODEL; break;
        case 'm': mask_info |= KFETCH_MEM; break;
        case 'n': mask_info |= KFETCH_NUM_CPUS; break;
        case 'p': mask_info |= KFETCH_NUM_PROCS; break;
        case 'r': mask_info |= KFETCH_RELEASE; break;
        case 'u': mask_info |= KFETCH_UPTIME; break;
        case 'h': usage(*argv); exit(EXIT_SUCCESS);
        case '?':
            fprintf(stderr, "Unknown option: %c\n", optopt);
            usage(*argv);
            exit(EXIT_FAILURE);
        case ':':
            fprintf(stderr, "Missing arg for %c\n", optopt);
            usage(*argv);
            exit(EXIT_FAILURE);
        }
    }

    fd = open(KFETCH_DEV_PATH, O_RDWR); //成功打開文件就返回fd(文件描述符)，有錯誤就返回-1
    if (fd < 0)
        err_quit("open");

    if (mask_info != -1)
        kfetch_set_info(fd, mask_info);
    kfetch_print_info(fd);
}

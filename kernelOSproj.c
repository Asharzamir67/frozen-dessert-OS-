#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <sys/sys.h>
#include <kernel.h>


asmlinkage long sys_icecream_log(int user_id, const char *user_msg){
    printk("System call initiated");
    
    printk("\nIceCreamLog User[%d]: %s\n", user_id, user_msg);
    return 0;
}

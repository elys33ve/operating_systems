#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "lkm_module"
#define EXAMPLE_MSG "Hello from the lkm_module!\n"
#define MSG_BUFFER_LEN 40
#define MAX_READS 10 // MAXIMUM NUMBER OF TIMES THE MESSAGE CAN BE READ PER DEVICE OPENING

MODULE_AUTHOR("Original Author: Robert W. Oliver II - modified by jcg");
MODULE_DESCRIPTION("A simple example character driver that limits message reads");
MODULE_VERSION("0.02");
MODULE_LICENSE("GPL");

// FUNCTION PROTOTYPES
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;

// TRACKING VARIABLES
static int device_open_count = 0; // NUMBER OF TIMES DEVICE HAS BEEN OPENED
static char msg_buffer[MSG_BUFFER_LEN]; // BUFFER TO HOLD MESSAGE
static char *msg_ptr; // POINTER TO CURRENT POSITION IN BUFFER
static int read_count = 0; // TRACKS NUMBER OF READ OPERATIONS PER DEVICE OPEN INSTANCE

// FILE OPERATIONS STRUCTURE
static struct file_operations file_ops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

// DEVICE READ FUNCTION
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;

    // IF MAXIMUM READ COUNT REACHED, RETURN 0 (END OF FILE CONDITION)
    if (read_count >= MAX_READS) {
        return 0;
    }

    if (*msg_ptr == 0) {
        msg_ptr = msg_buffer;
    }

    while (len && *msg_ptr) {
        put_user(*(msg_ptr++), buffer++);
        len--;
        bytes_read++;
    }

    read_count++; // INCREMENT READ COUNT
    return bytes_read;
}

// DEVICE WRITE FUNCTION - WRITE NOT SUPPORTED
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    printk(KERN_ALERT "Write operation is not supported in module lkm_example.\n");
    return -EINVAL;
}

// DEVICE OPEN FUNCTION
static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count) {
        return -EBUSY; // PREVENT MULTIPLE SIMULTANEOUS OPENS
    }
    device_open_count++;
    try_module_get(THIS_MODULE);

    // RESET READ COUNT ON NEW DEVICE OPEN
    read_count = 0;
    return 0;
}

// DEVICE RELEASE FUNCTION
static int device_release(struct inode *inode, struct file *file) {
    device_open_count--; // DECREMENT OPEN COUNT
    module_put(THIS_MODULE);
    return 0;
}

// MODULE INIT FUNCTION
static int __init lkm_example_init(void) {
    strncpy(msg_buffer, EXAMPLE_MSG, MSG_BUFFER_LEN); // COPY MESSAGE TO BUFFER
    msg_ptr = msg_buffer; // RESET MESSAGE POINTER
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);

    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_ALERT "lkm_module loaded with device major number %d\n", major_num);
        return 0;
    }
}

// MODULE EXIT FUNCTION
static void __exit lkm_example_exit(void) {
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_ALERT "the lkm_module has left the building...\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
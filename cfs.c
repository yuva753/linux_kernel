#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "cfs_sched"
#define CLASS_NAME "cfs_class"
#define MAX_PROCESSES 10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohan Kumar");
MODULE_DESCRIPTION("CFS Simulation Driver");

struct cfs_task {
    int pid;
    int nice;
    int weight;
    unsigned int vruntime;  // store as integer (e.g., in milliseconds * 100)
};

static struct cfs_task tasks[MAX_PROCESSES];
static int task_count = 0;

static dev_t dev_num;
static struct cdev cfs_cdev;
static struct class *cfs_class;

int nice_to_weight(int nice) {
    int base_weight = 1024;
    return base_weight - (nice * 100);
}

static ssize_t cfs_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    int pid, nice;
    char kbuf[32];

    if (len >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    if (sscanf(kbuf, "%d %d", &pid, &nice) != 2)
        return -EINVAL;

    if (task_count >= MAX_PROCESSES)
        return -ENOMEM;

    tasks[task_count].pid = pid;
    tasks[task_count].nice = nice;
    tasks[task_count].weight = nice_to_weight(nice);
    tasks[task_count].vruntime = 0;

    printk(KERN_INFO "CFS: Added process PID=%d, NICE=%d, WEIGHT=%d\n",
           pid, nice, tasks[task_count].weight);

    task_count++;

    return len;
}

static ssize_t cfs_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char kbuf[512];
    int i, ret = 0;

    if (*off > 0) return 0;

    ret += snprintf(kbuf, sizeof(kbuf), "PID NICE WEIGHT VRUNTIME\n");

    for (i = 0; i < task_count; i++) {
        int vruntime_int = tasks[i].vruntime / 100;
        int vruntime_frac = tasks[i].vruntime % 100;

        ret += snprintf(kbuf + ret, sizeof(kbuf) - ret,
                        "%d %d %d %d.%02d\n",
                        tasks[i].pid,
                        tasks[i].nice,
                        tasks[i].weight,
                        vruntime_int,
                        vruntime_frac);

        // simulate vruntime growth (multiply by 100 to simulate 2 decimals)
        tasks[i].vruntime += (10 * 100 * 1024) / tasks[i].weight;
    }

    if (copy_to_user(buf, kbuf, ret))
        return -EFAULT;

    *off += ret;
    return ret;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = cfs_write,
    .read = cfs_read,
};

static int __init cfs_init(void) {
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&cfs_cdev, &fops);
    cdev_add(&cfs_cdev, dev_num, 1);

    cfs_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(cfs_class, NULL, dev_num, NULL, DEVICE_NAME);

    printk(KERN_INFO "CFS: Module loaded.\n");
    return 0;
}

static void __exit cfs_exit(void) {
    device_destroy(cfs_class, dev_num);
    class_destroy(cfs_class);
    cdev_del(&cfs_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "CFS: Module unloaded.\n");
}

module_init(cfs_init);
module_exit(cfs_exit);

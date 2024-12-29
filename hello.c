#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Bohdan Bihdash <bogdan.bigdash@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint times = 1;
module_param(times, uint, 0644);
MODULE_PARM_DESC(times, "Number of times to print 'Hello, world!'");

struct hello_entry {
    struct list_head list;
    ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
    int i;
    struct hello_entry *entry;

    BUG_ON(times > 10);

    if (times == 0 || (times >= 5 && times <= 10)) {
        printk(KERN_WARNING "Warning: times = %u, but continuing.\n", times);
    }

    for (i = 0; i < times; i++) {
        if (i == 4) {
            entry = NULL;
        } else {
            entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        }

        if (!entry) {
            printk(KERN_ERR "Simulated kmalloc() failure for entry %d.\n", i + 1);
            continue;
        }

        entry->time = ktime_get();
        INIT_LIST_HEAD(&entry->list);
        list_add_tail(&entry->list, &hello_list);
        printk(KERN_INFO "Hello, world! %d\n", i + 1);
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_entry *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &hello_list, list) {
        printk(KERN_INFO "Time: %llu ns\n", ktime_to_ns(entry->time));
        list_del(&entry->list);
        kfree(entry);
    }

    printk(KERN_INFO "Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);


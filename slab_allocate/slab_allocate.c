#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#define SLAB_NAME       "foo_slab"
#define PROC_DIR        "slab_allocate"
#define INFO_FILE       "slab_info\0"
#define CREATE_FILE     "slab_create\0"
#define DESTROY_FILE    "slab_destroy\0"
#define NUM_SLAB_FILES  3

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("slab_allocate");
MODULE_VERSION("0.01");

/*
   TODO
    - File for showing info
    - File for adding an object to the slab
    - File for removing an object from the slab
*/

static struct kmem_cache *foo_slab_ptr;
static struct proc_dir_entry *proc_parent;
static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset);
static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);
static void create_proc_layout(void);
static void cleanup_proc(void);

typedef struct {
    char name[13];
    const struct file_operations *fops;
    umode_t mode;
} slab_file;

typedef struct {
    int a, b;
} foo;

static const struct file_operations info_file_fops = {
    .read   = read_proc,
};

static const struct file_operations create_file_fops = {
    .write  = write_proc,
};

static const struct file_operations destroy_file_fops = {
    .write  = write_proc,
};

static slab_file slab_files[NUM_SLAB_FILES] = {
    {
        .name = INFO_FILE,
        .fops = &info_file_fops,
        .mode = 0666
    },
    {
        .name = CREATE_FILE,
        .fops = &create_file_fops,
        .mode = 0555
    },
    {
        .name = DESTROY_FILE,
        .fops = &destroy_file_fops,
        .mode = 0555
    }
};

static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset) {
    return 0;
}

static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset) {
    return 0;
}

static void create_proc_layout(void) {
    int i;

    proc_parent = proc_mkdir(PROC_DIR, NULL);
    if (proc_parent == NULL) {
        printk(KERN_ERR "Unable to create proc dir: %s\n", PROC_DIR);
    }

    for(i=0; i<NUM_SLAB_FILES; i++) {
        if (proc_create(slab_files[i].name,
                    slab_files[i].mode,
                    proc_parent,
                    slab_files[i].fops) == NULL)
            printk(KERN_ERR "Unable to create proc file: %s\n", slab_files[i].name);
    }
}

static void cleanup_proc(void) {
    int i;
    for(i=0; i<NUM_SLAB_FILES; i++) {
        remove_proc_entry(slab_files[i].name, proc_parent);
    }
    remove_proc_entry(PROC_DIR, NULL);
}

static int __init slab_allocate_init(void) {
    create_proc_layout();
    foo_slab_ptr = kmem_cache_create(SLAB_NAME,
            sizeof(foo),
            0, 0, NULL);

    if(foo_slab_ptr == NULL) {
        printk(KERN_ERR "Failed to create slab: %s", SLAB_NAME);
        return -ENOMEM;
    }

    return 0;
}

static void __exit slab_allocate_exit(void) {
    if (foo_slab_ptr) {
        kmem_cache_destroy(foo_slab_ptr);
    }
    cleanup_proc();
}

module_init(slab_allocate_init);
module_exit(slab_allocate_exit);

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/slab_def.h>
#include <linux/uaccess.h>

#define SLAB_NAME       "foo_slab"
#define PROC_DIR        "slab_allocate"
#define INFO_FILE       "slab_info"
#define CREATE_FILE     "slab_create"
#define DESTROY_FILE    "slab_destroy"
#define NUM_SLAB_FILES  3

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("slab_allocate");
MODULE_VERSION("0.01");

/*
   TODO
    - File for showing info
*/

static ssize_t get_slab_info(struct file *filp, char *user, size_t count, loff_t *offset);
static ssize_t add_object_to_slab(struct file *filp, const char *user, size_t count, loff_t *offset);
static ssize_t remove_object_from_slab(struct file *filp, const char *user, size_t count, loff_t *offset);
static void create_proc_layout(void);
static void cleanup_proc(void);

static struct kmem_cache *foo_slab_ptr;
static struct proc_dir_entry *proc_parent;

typedef struct {
    char name[13];
    const struct file_operations *fops;
    umode_t mode;
} slab_file;

static slab_file *sf = NULL;

typedef struct {
    int a, b;
} foo;

static const struct file_operations info_file_fops = {
    .read   = get_slab_info
};

static const struct file_operations create_file_fops = {
    .write  = add_object_to_slab
};

static const struct file_operations destroy_file_fops = {
    .write  = remove_object_from_slab,
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

/*
   DOES NOT WORK YET. HOW DO YOU PRINT FIELDS OF A KMEM_CACHE?
*/
static ssize_t get_slab_info(struct file *filp, char *user, size_t count, loff_t *offset) {
    size_t len;
    char *info;

    info = kmalloc(4096, GFP_KERNEL);
    len = snprintf(info, 4096, "Name: %s\nSize: %d\n", foo_slab_ptr->name, foo_slab_ptr->refcount);

    if (*offset > 0) {
        return 0;
    }

    if (copy_to_user(user, info, len)) {
        return -EFAULT;
    }
    *offset += len;
    return len;
}

static ssize_t add_object_to_slab(struct file *filp, const char *user, size_t count, loff_t *offset) {
    if (sf != NULL) {
        pr_err("foo_slab already has object allocated");
        return -EFAULT;
    }

    sf = kmem_cache_alloc(foo_slab_ptr, GFP_KERNEL);
    if (sf == NULL) {
        pr_err("Unable to allocate object from foo_slab!");
        return -ENOMEM;
    }

    return count;
}

static ssize_t remove_object_from_slab(struct file *filp, const char *user, size_t count, loff_t *offset) {
    if (sf == NULL) {
        pr_err("foo_slab empty, unable to free object");
        return -EFAULT;
    }
    kmem_cache_free(foo_slab_ptr, sf);
    sf = NULL;
    return count;
}

static void create_proc_layout(void) {
    int i;

    proc_parent = proc_mkdir(PROC_DIR, NULL);
    if (proc_parent == NULL) {
        pr_err("Unable to create proc dir: %s\n", PROC_DIR);
    }

    for(i=0; i<NUM_SLAB_FILES; i++) {
        if (proc_create(slab_files[i].name,
                    slab_files[i].mode,
                    proc_parent,
                    slab_files[i].fops) == NULL)
            pr_err("Unable to create proc file: %s\n", slab_files[i].name);
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
        pr_err("Failed to create slab: %s", SLAB_NAME);
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

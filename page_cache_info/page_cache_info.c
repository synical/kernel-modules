#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/proc_fs.h>

#define PROC_DIR "page_cache_info"
#define PAGE_CACHE_INFO_GLOBAL "global"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("page_cache_info");
MODULE_VERSION("0.01");

/*
   TODO
    - /proc/page_cache_info/file
        - Read in filename via write op
            - Print info about address_space
            - Print info about buffer page/head
    - /proc/page_cache_info/global
        - Give overall info about page cache
            - Amount used
*/

static long get_cached_memory(void);
static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset);
static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);
static void create_proc_layout(void);
static void cleanup_proc(void);

static struct proc_dir_entry *proc_parent;

static const struct file_operations proc_fops = {
    .read   = read_proc,
    .write  = write_proc
};

static long get_cached_memory(void) {
    long cached;
    struct sysinfo si;
    si_meminfo(&si);

    cached = global_node_page_state(NR_FILE_PAGES) - si.bufferram;
    return cached;
}

static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset) {
    pr_info("Cached memory: %ld", get_cached_memory());
    return 0;
}

static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset) {
    return 0;
}

static void create_proc_layout(void) {
    proc_parent = proc_mkdir(PROC_DIR, NULL);
    if (proc_parent == NULL) {
        pr_err("Unable to create proc dir: %s\n", PROC_DIR);
    }

    if (proc_create(PAGE_CACHE_INFO_GLOBAL,
                0755,
                proc_parent,
                &proc_fops) == NULL)
        pr_err("Unable to create proc file: %s\n", PAGE_CACHE_INFO_GLOBAL);
}

static int __init page_cache_info_init(void) {
    create_proc_layout();
    return 0;
}

static void cleanup_proc(void) {
    remove_proc_entry(PAGE_CACHE_INFO_GLOBAL, proc_parent);
    remove_proc_entry(PROC_DIR, NULL);
}

static void __exit page_cache_info_exit(void) {
    cleanup_proc();
}

module_init(page_cache_info_init);
module_exit(page_cache_info_exit);

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

#define PAGE_INFO_FILE "page_info"
#define RW 0666
#define MSG "Not implemented"
#define MSG_BUFFER_LEN 15

/*
   TODO
    - Make this use seqfile access https://linux.die.net/lkmpg/x861.html
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("page_info");
MODULE_VERSION("0.01");

static void create_proc_file(void);
static void delete_proc_file(void);
static void print_pte_flags(long address);
static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset);
static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;

static const struct file_operations proc_fops = {
    .read   = read_proc,
    .write  = write_proc
};

static ssize_t read_proc(struct file *filep, char *buf, size_t len, loff_t *offset)
{
    ssize_t retval = 0;
    unsigned long ret = 0;

    if(*offset >= MSG_BUFFER_LEN)
        goto out;

    ret = copy_to_user(buf, msg_buffer, MSG_BUFFER_LEN);
    if(ret != 0)
        return -EFAULT;

    *offset += MSG_BUFFER_LEN - ret;
    retval = MSG_BUFFER_LEN;

out:
    return retval;
}

static void print_pte_flags(long address)
{
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct page *page;
    struct vm_area_struct *vma;

    mm = current->mm;
    vma = find_vma(mm, address);
    if(vma == NULL) {
        printk(KERN_INFO "Address %lx not in process address space!", address);
        return;
    }
    pgd = pgd_offset(mm, address);
    p4d = p4d_offset(pgd, address);
    pud = pud_offset(p4d, address);
    pmd = pmd_offset(pud, address);
    pte = pte_offset_map(pmd, address);
    page = pte_page(*pte);

    printk(KERN_INFO "Pte for 0x%lx\n", address);
    printk(KERN_INFO "\tPresent: %d\n", (pte_present(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "\tRW: %d\n", (pte_write(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "\tDirty: %d\n", (pte_dirty(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "\tAccessed: %d\n", (pte_young(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "Page for 0x%lx\n", address);
    printk(KERN_INFO "\tDirty: %d\n", (PageDirty(page)));
    printk(KERN_INFO "\tCount: %d\n", (page_count(page)));
    printk(KERN_INFO "Physical address for page: 0x%lx\n", (long)page_to_phys(page));
}

static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset)
{     
    char *buf;
    long address;
    int ret;

    buf = kzalloc(count+1, GFP_KERNEL);
    if(!buf) return -ENOMEM;

    ret = copy_from_user(buf, user, count);
    if(ret != 0) {
        kfree(buf);
        return ret;
    }

    ret = kstrtol(buf, 0, &address);
    if(ret != 0) {
        kfree(buf);
        return ret;
    };

    print_pte_flags(address);
    kfree(buf);

    return count;
}

static void create_proc_file(void)
{
    proc_create(PAGE_INFO_FILE, RW, NULL, &proc_fops);
}

static void delete_proc_file(void)
{
    remove_proc_entry(PAGE_INFO_FILE, NULL);
}

static int __init page_info_init(void)
{
    strncpy(msg_buffer, MSG, MSG_BUFFER_LEN);
    msg_ptr = msg_buffer;
    create_proc_file();

    return 0;
}

static void __exit page_info_exit(void)
{
    delete_proc_file();
}

module_init(page_info_init);
module_exit(page_info_exit);

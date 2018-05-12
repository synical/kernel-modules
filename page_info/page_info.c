#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("page_info");
MODULE_VERSION("0.01");

#define PAGE_INFO_DIR "page_info"
#define PAGE_INFO_FILE "info"
#define RW 0666

void create_proc_file(void);
void delete_proc_file(void);
ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset);
ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);

static struct proc_dir_entry *procd;

struct file_operations proc_fops = {
    read: read_proc,
    write: write_proc
};

ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset)
{     
    return 0;
}

int check_address_in_current(long address) {
    struct vm_area_struct *ptr;

    ptr = current->mm->mmap;
    if(ptr->vm_next) {
        do {
            if(address >= ptr->vm_start &&
               address <= ptr->vm_end) return 0;
            ptr = ptr->vm_next;
        } while(ptr->vm_next);
    }
    return -EINVAL;
}

void print_pte_flags(long address)
{
    struct vm_area_struct *vma;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    mm = current->mm;
    vma = mm->mmap;
    pgd = pgd_offset(mm, vma->vm_start);
    p4d = p4d_offset(pgd, vma->vm_start);
    pud = pud_offset(p4d, vma->vm_start);
    pmd = pmd_offset(pud, vma->vm_start);
    pte = pte_offset_map(pmd, vma->vm_start);

    printk(KERN_INFO "Pte for 0x%lx\n", address);
    printk(KERN_INFO "\tPresent: %d\n", (pte_present(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "\tRW: %d\n", (pte_write(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "\tDirty: %d\n", (pte_dirty(*pte) > 0 ? 1 : 0));
    printk(KERN_INFO "\tAccessed: %d\n", (pte_young(*pte) > 0 ? 1 : 0));
}

ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset)
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

    ret = check_address_in_current(address);
    if(ret != 0) {
        kfree(buf);
        return ret;
    };

    print_pte_flags(address);
    kfree(buf);

    return count;
}

void create_proc_file(void)
{
    procd = proc_mkdir(PAGE_INFO_DIR, NULL);
    proc_create(PAGE_INFO_FILE, RW, procd, &proc_fops);
}

void delete_proc_file(void) 
{
    remove_proc_entry(PAGE_INFO_FILE, procd);
    remove_proc_entry(PAGE_INFO_DIR, NULL);   
}

static int __init hw_mod_init(void)
{
    create_proc_file();
    return 0;
}

static void __exit hw_mod_exit(void) 
{
    delete_proc_file();
}

module_init(hw_mod_init);
module_exit(hw_mod_exit);

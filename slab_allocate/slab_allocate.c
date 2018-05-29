#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define SLAB_NAME "foo_slab"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("slab_allocate");
MODULE_VERSION("0.01");

static struct kmem_cache *foo_slab_ptr;

struct foo {
    int a, b;
};

static int __init slab_allocate_init(void) {
    foo_slab_ptr = kmem_cache_create(SLAB_NAME,
            sizeof(struct foo),
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
}

module_init(slab_allocate_init);
module_exit(slab_allocate_exit);

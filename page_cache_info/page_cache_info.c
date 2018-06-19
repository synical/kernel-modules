#include <linux/module.h>
#include <linux/kernel.h>

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

static int __init page_cache_info_init(void) {
    return 0;
}

static void __exit page_cache_info_exit(void) {
    ;
}

module_init(page_cache_info_init);
module_exit(page_cache_info_exit);

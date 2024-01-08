#include <linux/init.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>

MODULE_AUTHOR("Yaroslav Latypov <jarlat04@lapuas.xyz>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "A parameter that determines the number of times 'Hello, world!' is printed");

struct hello_time {
  struct list_head list;
  ktime_t time;
};

// Ініціалізація голови списку
static LIST_HEAD(hello_time_list);

// Функція ініціалізації модуля
static int __init hello_init(void) {
  struct hello_time *time_entry;
  unsigned int i;

  // Перевірка на допустиме значення print_count
  BUG_ON(print_count > 10);

  for (i = 0; i < print_count; i++) {
    // Симуляція помилки виділення пам'яті при i == 4
    if (i == 4) {
      time_entry = NULL;
    } else {
      time_entry = kmalloc(sizeof(*time_entry), GFP_KERNEL);
    }

    // Перевірка на успішне виділення пам'яті
    BUG_ON(time_entry == NULL);

    time_entry->time = ktime_get();
    list_add_tail(&time_entry->list, &hello_time_list);
    pr_emerg("Hello, world!\n");
  }

  return 0;
}

// Функція очищення модуля
static void __exit hello_exit(void) {
  struct hello_time *time_entry, *tmp;

  // Прохід по списку та виведення збережених часових міток
  list_for_each_entry_safe(time_entry, tmp, &hello_time_list, list) {
    pr_info("Time: %lld\n", ktime_to_ns(time_entry->time));
    list_del(&time_entry->list);
    kfree(time_entry);
  }
}

module_init(hello_init);
module_exit(hello_exit);

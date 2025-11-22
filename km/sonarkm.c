/* Necessary includes for device drivers */
#include "linux/kern_levels.h"
#include "linux/timer.h"
#include <asm/system_misc.h> /* cli(), *_flags */
#include <asm/uaccess.h>     /* copy_from/to_user */
#include <linux/errno.h>     /* error codes */
#include <linux/fcntl.h>     /* O_ACCMODE */
#include <linux/fs.h>        /* everything... */
#include <linux/init.h>
#include <linux/jiffies.h> /* jiffies */
#include <linux/kernel.h>  /* printk() */
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched/signal.h> // for sig sending
#include <linux/seq_file.h>     // signle_open
#include <linux/signal.h>
#include <linux/slab.h>  /* kmalloc() */
#include <linux/types.h> /* size_t */
#include <linux/uaccess.h>
#include <linux/vmalloc.h> // For vmalloc

#include <linux/gpio.h> // for GPIO control
#include <linux/interrupt.h> // for IRQ handling

#define GPIO_TRIG1 67     
#define GPIO_ECHO1 68
#define GPIO_TRIG2 44
#define GPIO_ECHO2 26
#define GPIO_TRIG3 46
#define GPIO_ECHO3 65

#define BUF_LEN 256
#define MAX_PRINT_LENGTH PAGE_SIZE
#define MAX_TIMERS 2 // one more than Necessary to handle overflow
#define DEVICE_NAME "sonar"

MODULE_LICENSE("Dual BSD/GPL");

// define the timer holder struct
struct my_timer_holder {
  struct timer_list timer;
  int state;
};

static int sonar_open(struct inode *inode, struct file *filp);
static int sonar_release(struct inode *inode, struct file *filp);
static ssize_t sonar_write(struct file *filp, const char *buf, size_t count,
                               loff_t *f_pos);
static ssize_t sonar_read(struct file *filep, char __user *buffer,
                              size_t len, loff_t *offset);
static int sonar_init(void);
static void sonar_exit(void);
static void timer_handler(struct timer_list *);
struct my_timer_holder *find_by_pid(pid_t pid);

/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations sonar_fops = {
    .write = sonar_write,
    .read = sonar_read,
    .open = sonar_open,
    .release = sonar_release,
};

/* Declaration of the init and exit functions */
module_init(sonar_init);
module_exit(sonar_exit);

// all the variables
static int sonar_major = 61; /* be sure to run mknod with this major num! */
static struct my_timer_holder *my_timer;
int echo1_time, echo2_time, echo3_time;
static int irq_echo1 = 0;
static int irq_echo2 = 0;
static int irq_echo3 = 0;

int echo1_state = 0;
int echo2_state = 0;
int echo3_state = 0;


static irqreturn_t echo1_rise(int irq, void *dev_id) {
  //printk(KERN_ALERT "ECHO1 RISE");
  if (!echo1_state){
    echo1_time = jiffies;
  }else{
    echo1_time = jiffies - echo1_time;
    printk(KERN_ALERT "ECHO1 Time jiffies: %d", echo1_time);
  }
  echo1_state = !echo1_state;
  return IRQ_HANDLED;
}

static irqreturn_t echo2_rise(int irq, void *dev_id) {
  //printk(KERN_ALERT "ECHO2 RISE");
if (!echo2_state){
    echo2_time = jiffies;
  }else{
    echo2_time = jiffies - echo2_time;
    printk(KERN_ALERT "ECHO2 Time jiffies: %d", echo2_time);
  }
  echo1_state = !echo1_state;  return IRQ_HANDLED;
}

static irqreturn_t echo3_rise(int irq, void *dev_id) {
  //printk(KERN_ALERT "ECHO3 RISE");
  if (!echo3_state){
    echo3_time = jiffies;
  }else{
    echo3_time = jiffies - echo3_time;
    printk(KERN_ALERT "ECHO3 Time jiffies: %d", echo3_time);
  }
  echo1_state = !echo1_state;  return IRQ_HANDLED;
}


static int sonar_init(void) {
  // First set up the timer
  int result;
  int ret = 0;

  /* Register Timer  */
  my_timer = kzalloc(sizeof(struct my_timer_holder), GFP_KERNEL);
  if (!my_timer) {
    kfree(my_timer);
    return -ENOMEM;
  }

  // set state to default blinking pattern
  // -1 is a hack because you +1 at the start of each loops so I want the first
  // thing to run to be 0, not 1
  my_timer->state = 0;
  // then set up timer and run it at the current frequency
  timer_setup(&my_timer->timer, timer_handler, 0);
  mod_timer(&my_timer->timer,
            jiffies +
                msecs_to_jiffies(10)); // immediately start

  // configure GPIO pins
  result = gpio_request(GPIO_TRIG1, "trigger_1");
  if (result) {
    printk(KERN_ALERT "Failed to request GPIO %d\n", GPIO_TRIG1);
    // goto err_gpio;
    return result;
  }
  gpio_direction_output(GPIO_TRIG1, 0);

  result = gpio_request(GPIO_TRIG2, "trigger_2");
  if (result) {
    printk(KERN_ALERT "Failed to request GPIO %d\n", GPIO_TRIG2);
    // goto err_gpio;
    return result;
  }
  gpio_direction_output(GPIO_TRIG2, 0);

  result = gpio_request(GPIO_TRIG3, "trigger_3");
  if (result) {
    printk(KERN_ALERT "Failed to request GPIO %d\n", GPIO_TRIG3);
    // goto err_gpio;
    return result;
  }
  gpio_direction_output(GPIO_TRIG3, 0);

  result = gpio_request(GPIO_ECHO1, "echo_1");
  if (result) {
    printk(KERN_ALERT "Failed to request GPIO %d\n", GPIO_ECHO1);
    // goto err_gpio;
    return result;
  }
  gpio_direction_input(GPIO_ECHO1);

  result = gpio_request(GPIO_ECHO2, "echo_2");
  if (result) {
    printk(KERN_ALERT "Failed to request GPIO %d\n", GPIO_ECHO2);
    // goto err_gpio;
    return result;
  }
  gpio_direction_input(GPIO_ECHO2);

  result = gpio_request(GPIO_ECHO3, "echo_3");
  if (result) {
    printk(KERN_ALERT "Failed to request GPIO %d\n", GPIO_ECHO3);
    // goto err_gpio;
    return result;
  }
  gpio_direction_input(GPIO_ECHO3);

  // request IRQs for les buttons
  irq_echo1 = gpio_to_irq(GPIO_ECHO1);
  result = request_irq(irq_echo1, echo1_rise, (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING), "echo_1", NULL);
  if (result) {
    printk(KERN_ALERT "Failed to request IRQ for ECHO1 RISING\n");
    return result;
  }

  irq_echo2 = gpio_to_irq(GPIO_ECHO2);
  result = request_irq(irq_echo2, echo2_rise, (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING) , "echo_2", NULL);
  if (result) {
    printk(KERN_ALERT "Failed to request IRQ for ECHO2 RISING\n");
    return result;
  }

  irq_echo3 = gpio_to_irq(GPIO_ECHO3);
  result = request_irq(irq_echo3, echo3_rise, (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING), "echo_3", NULL);
  if (result) {
    printk(KERN_ALERT "Failed to request IRQ for ECHO3 RISING\n");
    return result;
  }

  printk(KERN_ALERT "sonar loaded\n");

  /* Registering device */
  result = register_chrdev(sonar_major, "sonar", &sonar_fops);
  if (result < 0) {
    printk(KERN_ALERT "sonar: cannot obtain major number %d\n", sonar_major);
    return result;
  }

  return ret;
}

static void sonar_exit(void) {
  /* Freeing the major number */
  if (my_timer) {
    del_timer_sync(&my_timer->timer);
    kfree(my_timer);
    my_timer = NULL;
  }

  // freedom
  if (irq_echo1)
    free_irq(echo1_rise, NULL);
  if (irq_echo2)
    free_irq(echo2_rise, NULL);
  if (irq_echo3)
    free_irq(echo3_rise, NULL);

  gpio_free(GPIO_TRIG1);
  gpio_free(GPIO_TRIG2);
  gpio_free(GPIO_TRIG3);
  gpio_free(GPIO_ECHO1);
  gpio_free(GPIO_ECHO2);
  gpio_free(GPIO_ECHO3);


  unregister_chrdev(sonar_major, "sonar");
  printk(KERN_ALERT "Removing sonar module\n");
}

static int sonar_open(struct inode *inode, struct file *filp) { return 0; }

static int sonar_release(struct inode *inode, struct file *filp) {
  return 0;
}

static ssize_t sonar_read(struct file *filep, char __user *buffer,
                              size_t len, loff_t *offset) {
  char buf[512] = {0};
  int count = 0;

  if (*offset)
    return 0;

  // TODO: Print which lights are active
  count += scnprintf(buf + count, sizeof(buf) - count, "ECHO1: %d", echo1_time);
  count += scnprintf(buf + count, sizeof(buf) - count, "ECHO2: %d", echo2_time);
  count += scnprintf(buf + count, sizeof(buf) - count, "ECHO3: %d", echo3_time);
  // then sent that to the user
  if (copy_to_user(buffer, buf, count))
    return -EFAULT;

  *offset = count;
  return count;
}

static ssize_t sonar_write(struct file *filp, const char *buf, size_t count,
                               loff_t *f_pos) {
  return count;
}

static void timer_handler(struct timer_list *t) {
    my_timer->state = !my_timer->state;
    gpio_set_value(GPIO_TRIG1, my_timer->state);
    gpio_set_value(GPIO_TRIG2, my_timer->state);
    gpio_set_value(GPIO_TRIG3, my_timer->state);
   
  mod_timer(&my_timer->timer, jiffies + msecs_to_jiffies(90 - 80*(my_timer->state)));
}
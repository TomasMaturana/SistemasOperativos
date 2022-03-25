/* Necessary includes for device drivers */
#include <linux/init.h>
/* #include <linux/config.h> */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_from/to_user */

MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of cdown.c functions */
static int cdown_open(struct inode *inode, struct file *filp);
static int cdown_release(struct inode *inode, struct file *filp);
static ssize_t cdown_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t cdown_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void cdown_exit(void);
int cdown_init(void);

/* Structure that declares the usual file */
/* access functions */
static struct file_operations cdown_fops = {
  read: cdown_read,
  write: cdown_write,
  open: cdown_open,
  release: cdown_release
};

/* Declaration of the init and exit functions */
module_init(cdown_init);
module_exit(cdown_exit);

/* Global variables of the driver */
/* Major number */
static int cdown_major = 61;
/* Buffer to store data */
#define KBUF_SIZE 20
#define MAX_SIZE 8192
static char *cdown_buffer; //contenido (counter)
static ssize_t curr_size; //tamaño del buffer
static struct semaphore mutex; 
static struct semaphore write_mutex;
static int c;

int cdown_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(cdown_major, "cdown", &cdown_fops);
  if (result < 0) {
    printk(
      "<1>cdown: cannot obtain major number %d\n", cdown_major);
    return result;
  }

  sema_init(&mutex, 1);
  sema_init(&write_mutex, 1);
  c=0;

  /* Allocating cdown for the buffer */
  cdown_buffer = kmalloc(MAX_SIZE, GFP_KERNEL); 
  if (!cdown_buffer) { 
    result = -ENOMEM;
    goto fail; 
  } 
  memset(cdown_buffer, 0, MAX_SIZE);
  curr_size= 0;

  printk("Inserting cdown module\n"); 
  return 0;

  fail: 
    cdown_exit(); 
    return result;
}

void cdown_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(cdown_major, "cdown");

  /* Freeing buffer cdown */
  if (cdown_buffer) {
    kfree(cdown_buffer);
  }

  printk("Removing cdown module\n");

}

static int cdown_open(struct inode *inode, struct file *filp) {
  char *mode=   filp->f_mode & FMODE_WRITE ? "write" :
                filp->f_mode & FMODE_READ ? "read" :
                "unknown";
				
  int rc= down_interruptible(&write_mutex);
  if (filp->f_mode & FMODE_WRITE) {
    if (rc) {
      printk("<1> down interrupted, rc=%d\n", rc);
      return rc;
    }
    curr_size= 0;
  }
  
  printk("<1>open for %s\n", mode);
  /* Success */
  return 0;
}

static int cdown_release(struct inode *inode, struct file *filp) {
  if(filp->f_mode & FMODE_READ){
	  if(c>0){
		  c--;
	  }
  }
  up(&write_mutex);
  printk("<1>close\n");
  /* Success */
  return 0;
}

static ssize_t cdown_read(struct file *filp, char *buf, 
                    size_t count, loff_t *f_pos) { 
  ssize_t rc;
  down(&mutex);

  if (count > curr_size-*f_pos) {
    count= curr_size-*f_pos;
  }
  
  if(c==0){
	  rc=0;
	  goto epilog;
  }
  
  char kbuf[KBUF_SIZE];    
  int len= snprintf(kbuf, KBUF_SIZE, "%d\n", c);    
  if (count>len)      
    count= len;

  printk("<1>read %d bytes at %d\n", (int)count, (int)*f_pos);

  /* Transfering data to user space */
  if (copy_to_user(buf, kbuf, count)!=0) {
    /* el valor de buf es una direccion invalida */
    rc= -EFAULT;
    goto epilog;
  }

  *f_pos+= count;
  rc= count;

epilog:
  up(&mutex);
  return rc;
}

static ssize_t cdown_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {

  ssize_t rc;
  loff_t last;

  down(&mutex);

  last= *f_pos + count;
  if (last>MAX_SIZE) {
    count -= last-MAX_SIZE;
  }
  printk("<1>write %d bytes at %d\n", (int)count, (int)*f_pos);

  int rcc =kstrtoint_from_user(buf, count, 10, &c);
  /* Transfering data from user space */
  if (rcc!=0) {
    /* el valor de buf es una direccion invalida */
	printk("bash: echo: error de escritura: Argumento invalido\n");
    rc= rcc;
    goto epilog;
  }

  *f_pos += count;
  curr_size= *f_pos;
  rc= c;

epilog:
  up(&mutex);
  return rc;
}


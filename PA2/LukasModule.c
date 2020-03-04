#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include</usr/include/asm/errno.h>
#include<linux/string.h>
#define DRIVER_NAME "simple_character_device"

//Global Variables
static int major_Number = 0;
static int open_Counter = 0;
static int close_Counter = 0;
static char *buffer = NULL;

//filp is a file pointer
static int device_open(struct inode *inode, struct file *filp)
{
	open_Counter+=1;
	printk(KERN_INFO "The device has been opened %d time(s)\n",open_Counter);
	return 0;
}
static int device_release(struct inode *inode, struct file *filp)
{
	close_Counter+=1;
	printk(KERN_INFO "The device has been closed %d time(s)\n", close_Counter);
	return 0;
}

loff_t device_llseek(struct file *filp, loff_t offset, int whence)
{	
	if(offset < 0 || offset > 1023)
	{
		return -1;
	}	
	if(whence == 0)
	{
		filp->f_pos = offset; //wherever you are in the file, go straight to the offset
	}
	else if(whence == 1)
	{
		filp->f_pos = filp->f_pos + offset; //Wherever you are in the file, add the offset to that position
	}
	else if(whence == 2)
	{
		filp->f_pos = 1023 - offset; //Go to the end of the file and subtract the offset to get the new position.
	}
	if(whence == EINVAL)
	{
		return -1;
	}
	return 0;	
}

static ssize_t device_read(struct file *filp, char *userBuffer, size_t length, loff_t *offset)
{
	if((*offset + length) < 0 || (*offset + length) > 1023)
	{
		return -1;
	}	
	copy_to_user(userBuffer, buffer+*offset, length);
	offset = offset + length;
	return 0;	
}

static ssize_t device_write(struct file *filp, const char *userBuffer, size_t length, loff_t *offset)
{
	int printLen;	
	if((*offset + length) < 0 || (*offset + length) > 1023)
	{
		return -1;
	}	
	copy_from_user(buffer+*offset,userBuffer, length);
	offset = offset + length;
	printLen = length;
	printk(KERN_ALERT "%d\n", printLen);
	return 0;
}

static struct file_operations ops = 
{
	open: device_open,	
	release: device_release,
	llseek: device_llseek,	
	read: device_read,
	write: device_write,
};

int Lukas_init(void)
{
	buffer = kmalloc(1024,GFP_KERNEL);	
	memset(buffer, 0, 1024);	
	major_Number = register_chrdev(243,DRIVER_NAME,&ops);		
	if(major_Number < 0)
	{
		printk(KERN_ALERT "FAILED TO REGISTER MAJOR NUMBER\n");
	}
	printk(KERN_ALERT "SUCCESSFUL MAJOR NUMBER\n");	
	return 0;
}

void Lukas_exit(void)
{
	printk(KERN_ALERT "CLOSING\n");	
	unregister_chrdev(243,DRIVER_NAME);
	kfree(buffer);
}

module_init(Lukas_init);
module_exit(Lukas_exit);

MODULE_LICENSE("GPL");

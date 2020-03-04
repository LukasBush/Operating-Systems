#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>
asmlinkage long sys_cs3753_add(int num1, int num2, int __user *result)
{	
	int sum = 0;
	printk(KERN_ALERT "num1 = %d num2 = %d\n", num1, num2);

	sum = num1 + num2;
	printk(KERN_ALERT "Sum is %d\n", sum);
	put_user(sum, result);
	printk(KERN_ALERT "Put result pointer = %p\n", result);
	return 0;
}

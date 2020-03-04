#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main()
{
	int num1 = 5;
	int num2 = 10;
	int sum = 0;
	printf("we passed in %d and %d\n", num1, num2);        
	long int amma = syscall(334,num1,num2,&sum);
      	printf("System call sys_cs3753_add returned %ld\n", amma);
	printf("sum=%d\n",sum);
        return 0;
}

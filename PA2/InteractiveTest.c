#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

int main()
{
	char choice = ' ';

	char * readBytes;
	int readLen = 0;

	char * writeBytes;
	char standBytes[1024];
	int len = 0;
	int writeReturn = 0;
	
	int offset;
	int whence;
	int seekReturn = 0;
		
	int testO;
	int testC;
	
	printf("\n");
	printf("\n");
	printf("\n");
	printf("Testing to open....");	
	testO = open("/dev/simple_character_device", O_RDWR);
	if(testO<0)
	{
		perror("FAILED\n");
		return -1;
	}
	printf("SUCCESSFUL\n");
	printf("\n");
	printf("\n");
	printf("\n");

	do
	{
		printf("WELCOME TO THE MAIN MENU\n");
		printf("__________________________\n");
		printf("\n");
		printf("Press r to read from device\n");
		printf("Press w to write to the device\n");
		printf("Press s to seek into the device\n");
		printf("Press e to exit from the device\n");
		printf("Enter Command: ");
		scanf(" %c", &choice);
		
		switch(choice)
		{
			case 'r' :
			printf("Enter the number of bytes you want to read: ");
			scanf(" %d", &readLen);
			readBytes = (char*)malloc(readLen*sizeof(char));
			read(testO, readBytes, readLen);
			printf("%s\n", readBytes);			
			printf("Returning to Main Menu...\n");
			printf("\n");
			break;
			
			case 'w' :
			printf("Enter data you want to write to the device: ");
			scanf(" %[^\n]s",standBytes);
			//printf("%s", standBytes);
			len = strlen(standBytes);
			writeBytes = (char*)malloc(len*sizeof(char));
			strncpy(writeBytes, standBytes, len);
			writeReturn = write(testO, writeBytes, len);
			printf("%d\n", writeReturn);
			printf("Returning to Main Menu...\n");
			printf("\n");
			free(writeBytes);
			break;
				
			case 's' :
			printf("Enter an offset value: ");
			scanf(" %d", &offset);
			printf("Enter a value for whence: ");
			scanf(" %d", &whence);
			seekReturn = lseek(testO,offset,whence);
			printf("%d\n", seekReturn);
			printf("Returning to Main Menu...\n");
			printf("\n");
			break;
			
			case 'e' :
			testC = close(testO);
			printf("Closing....");
			if(testC<0)
			{
				perror("FAILED\n");
				return -1;
			}
			printf("SUCCESSFUL\n");		
			printf("Exiting program now...\n");
			break;
			
			default :
			printf("Returning to menu to continue to read and write\n");
		}
	}while(choice != 'e');
}


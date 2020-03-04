#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include <sys/time.h>

#include"util.c"
#include"multi-lookup.h"
#include"Queue.h"

#define MaxInputFiles 10
#define MaxResolverThreads 10
#define MaxRequesterThreads 5
#define MaxNameLength 1025
#define MaxIpLength INET6_ADDRSTRLEN

void* reqFunc(void* id)
{	
	struct reqThread* thread = id;	
	
	char website[1025];
	
	char* passIn;

	int done = 0;	
	
	while(thread->fileOn < thread->Gspot->numFiles)
	{		
		if(thread->Gspot->fileArray[thread->fileOn] != NULL)
		{						
			thread->filename = thread->Gspot->fileArray[thread->fileOn];
			while(fscanf(thread->filename, "%1024s", website) > 0)
			{								
				while(done == 0)
				{
					//printf("%s\n", website);					
					pthread_mutex_lock(&thread->Gspot->Buffer);
					if(queue_is_full(thread->Gspot->sharedbuff) == 0)
					{
						passIn = malloc(1025);
						strncpy(passIn, website, 1025);
						queue_push(thread->Gspot->sharedbuff, passIn);
						pthread_mutex_unlock(&thread->Gspot->Buffer);						
						done = 1;
					}
					else
					{
						pthread_mutex_unlock(&thread->Gspot->Buffer);
						done = 0;
					}
				}
				done = 0;
			}			
		}
		thread->fileOn += 1;
		thread->count += 1;
	}
	pthread_mutex_unlock(&thread->Gspot->Service);
	if(thread->serviced != NULL)
	{
		if(thread->count != 0)
		{
			fprintf(thread->serviced, "thread %d worked on %d\n", thread->id, thread->count);
		}
		else
		{
			fprintf(thread->serviced, "thread %d worked on 0\n", thread->id);
		}
	}
	pthread_mutex_unlock(&thread->Gspot->Service);
	return NULL;
}
	/*char website[1025];
	char* passIn;

	int done = 0;
	int finish = 0;
	int findFile = 0;
	struct reqThread* thread = id;
	thread->count = 0;
	while(finish == 0)
	{
		if(thread->filename != NULL)
		{					
			thread->count++;
			while(fscanf(thread->filename, "%1024s", website) > 0)
			{				
				while(done == 0)
				{		
					if(queue_is_full(thread->Gspot->sharedbuff) == 0)
					{
						passIn = (char*)malloc(MaxNameLength*sizeof(char));
						strncpy(passIn, website, 1025);
						pthread_mutex_lock(&thread->Gspot->Buffer);
						queue_push(thread->Gspot->sharedbuff, passIn);
						pthread_mutex_unlock(&thread->Gspot->Buffer);
						done = 1;
					}
				}
				done = 0;
			}
			pthread_mutex_lock(&thread->Gspot->varProtect);
			if(thread->Gspot->fileStatusTable[thread->fileOn] == 0)
			{			
				fclose(thread->filename);
				thread->Gspot->fileStatusTable[thread->fileOn] = 1;
			}
			if(thread->Gspot->numFiles != thread->Gspot->fileAssign)
			{
				//printf("%d\n", thread->Gspot->fileAssign);
				thread->filename = thread->Gspot->fileArray[thread->Gspot->fileAssign];
				thread->fileOn = thread->Gspot->fileAssign;
				thread->Gspot->fileAssign += 1;
			}			
			else if(thread->Gspot->helpFiles != 0)
			{						
				findFile = 0;				
				while(findFile == 0)
				{				
					if(thread->Gspot->helpFiles >= 0)
					{
						//fprintf(stderr, "%d\n", thread->Gspot->helpFiles);
						if(thread->Gspot->fileStatusTable[thread->Gspot->helpFiles] == 1)
						{
							//printf("File Done!\n");
							thread->Gspot->helpFiles = thread->Gspot->helpFiles - 1;						
						}
						else if(thread->Gspot->fileStatusTable[thread->Gspot->helpFiles] == 0)
						{
							//printf("I will help you!\n");
							thread->filename = thread->Gspot->fileArray[thread->Gspot->helpFiles];
							thread->Gspot->helpFiles = thread->Gspot->helpFiles-1;
							findFile = 1;
						}						
					}
					else
					{
						findFile = 1;
						finish = 1;
					}
				}
			}
			else
			{
				finish = 1;
			}
			pthread_mutex_unlock(&thread->Gspot->varProtect);
		}
		else
		{
			pthread_mutex_lock(&thread->Gspot->varProtect);			
			thread->Gspot->fileStatusTable[thread->fileOn] = 1;
			if(thread->Gspot->numFiles != thread->Gspot->fileAssign)
			{
				//printf("%d\n", thread->Gspot->fileAssign);
				thread->filename = thread->Gspot->fileArray[thread->Gspot->fileAssign];
				thread->fileOn = thread->Gspot->fileAssign;
				thread->Gspot->fileAssign += 1;	
			}
			else
			{
				findFile = 0;				
				while(findFile == 0)
				{	
					if(thread->Gspot->helpFiles >= 0)
					{					
						if(thread->Gspot->fileStatusTable[thread->Gspot->helpFiles] == 1)
						{
							//printf("Fake File Done!\n");
							thread->Gspot->helpFiles = thread->Gspot->helpFiles - 1;
							findFile = 1;					
						}
						else if(thread->Gspot->fileStatusTable[thread->Gspot->helpFiles] == 0)
						{
							//printf("Fake Need Help!\n");
							thread->filename = thread->Gspot->fileArray[thread->Gspot->helpFiles];
							thread->Gspot->helpFiles = thread->Gspot->helpFiles - 1;
							findFile = 1;	
						}
					}
					else
					{
						findFile = 1;
						finish = 1;
					}
				}
			}
			pthread_mutex_unlock(&thread->Gspot->varProtect);		
		}
	}
	//printf("%d\n", thread->count);
	pthread_mutex_lock(&thread->Gspot->Service);
	fprintf(thread->serviced, "thread %d worked on %d\n", thread->id, thread->count);
	pthread_mutex_unlock(&thread->Gspot->Service);
	return NULL;
}*/

void* resFunc(void* id)
{
	struct resThread* thread = id;
	char website[1025];
	char ipAD[INET6_ADDRSTRLEN];	
	int len = sizeof(ipAD);
	void* passIn;
	while(queue_is_empty(thread->Gspot->sharedbuff) == 0 || *thread->Gspot->noLiveRequester == 1)
	{		
		if(queue_is_empty(thread->Gspot->sharedbuff) == 0)
		{		
			pthread_mutex_lock(&thread->Gspot->Buffer);			
			passIn = queue_pop(thread->Gspot->sharedbuff);
			pthread_mutex_unlock(&thread->Gspot->Buffer);
			strncpy(website, passIn, 1025);
			pthread_mutex_lock(&thread->Gspot->Results);
			fprintf(thread->results, "%s,", website);
			if(dnslookup(website, ipAD, len) == UTIL_FAILURE)
			{
				fprintf(thread->results, "\n");
			}
			else
			{						
				fprintf(thread->results, "%s\n", ipAD);
			}
		}
		pthread_mutex_unlock(&thread->Gspot->Results);					
	}
	free(passIn);
	return NULL;
}

int main(int argc, char* argv[])
{	
	struct timeval start, end;
	gettimeofday(&start,NULL);
	if(argc > (MaxInputFiles + 5))
	{
		fprintf(stderr, "You entered too many files!\n");
		fprintf(stderr, "You've enter %d files to be read\n", (argc-5));
		fprintf(stderr, "You can enter a max of %d files\n", MaxInputFiles);
		return -1;
	}

	FILE* SERVICE;
	FILE* RESULT;
	SERVICE = fopen(argv[3], "w");
	if(SERVICE)
	{
		printf("File exist/created!\n");

	}
	RESULT = fopen(argv[4], "w");
	if(RESULT)
	{
		printf("File exist/created!\n");
	}
	
	int request = atoi(argv[1]);
	int resolve = atoi(argv[2]);
	if(atoi(argv[1]) > MaxRequesterThreads)
	{
		fprintf(stderr, "You requested too many requester threads!\n");
		fprintf(stderr, "You've asked for %d requester threads\n", atoi(argv[1]));
		fprintf(stderr, "Setting number of Requester threads to max of %d\n", MaxRequesterThreads);
		request = MaxRequesterThreads;
	}
	if(atoi(argv[2]) > MaxResolverThreads)
	{
		fprintf(stderr, "You requested too many resolver threads!\n");
		fprintf(stderr, "You've asked for %d resolver threads\n", atoi(argv[2]));
		fprintf(stderr, "Setting number of Resolver threads to max of %d\n", MaxResolverThreads);	
		resolve = MaxResolverThreads;	
	}
			
	queue buffer;
	queue_init(&buffer, 1025);

	int numberFiles = argc - 5;

	FILE* inputFiles[numberFiles]; 
	for(int i = 0; i < numberFiles; i++)
	{
		inputFiles[i] = fopen(argv[i+5], "r");
	}

	/*int* fileStatus;
	fileStatus = (int*)malloc(numberFiles*sizeof(int));
	for(int i = 0; i < (numberFiles); i++)
	{
		fileStatus[i]= 0;
	}*/
	
	int activeReq = 1;
	struct Global Info;
	pthread_mutex_init(&Info.Service, NULL);
	pthread_mutex_init(&Info.Results, NULL);
	pthread_mutex_init(&Info.Buffer, NULL);
	//pthread_mutex_init(&Info.varProtect, NULL);
	//Info.fileStatusTable = fileStatus;	
	Info.sharedbuff = &buffer;
	Info.numFiles = numberFiles;
	Info.fileArray = inputFiles;	
	Info.helpFiles = numberFiles-1;
	Info.noLiveRequester = &activeReq;

	struct reqThread requester[request];
	pthread_t reqArray[request];

	struct resThread resolver[resolve];
	pthread_t resArray[resolve];
	
	/*if((numberFiles) - request >= 0)
	{
		Info.fileAssign = request;	
	}
	else
	{
		Info.fileAssign = (numberFiles);
	}*/
	int Check;
	for(int i = 0; i < request; i++)
	{
		requester[i].Gspot = &Info;
		requester[i].fileOn = i;
		requester[i].id = i;
		requester[i].count = 0;
		requester[i].serviced = SERVICE;
		Check = pthread_create(&(reqArray[i]), NULL, reqFunc, &(requester[i]));

		if(Check != 0)
		{
			fprintf(stderr, "Error creating request thread\n");
		}
	}


	/*
	for(int i = 0; i < request && i < (argc - 5); i++)
	{
		requester[i].count = 0;
		requester[i].filename = Info.fileArray[i];
		requester[i].Gspot = &Info;
		requester[i].fileOn = i;
		requester[i].serviced = SERVICE;
		requester[i].id = i;
		pthread_create(&(reqArray[i]), NULL, reqFunc, &(requester[i]));
	}*/
	for(int i = 0; i < resolve; i++)
	{
		resolver[i].Gspot = &Info;
		resolver[i].results = RESULT;
		pthread_create(&(resArray[i]), NULL, resFunc, &(resolver[i]));	
	}

	for(int i = 0; i < request; i++)
	{
		pthread_join(reqArray[i], NULL);
	}
	activeReq = 0;
	for(int i = 0; i < resolve; i++)
	{
		pthread_join(resArray[i], NULL);	
	}
	pthread_mutex_destroy(&Info.Buffer);
	pthread_mutex_destroy(&Info.Service);
	pthread_mutex_destroy(&Info.Results);
	queue_cleanup(&buffer);
	fclose(SERVICE);
	fclose(RESULT);
	//free(fileStatus);
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Multi-Lookup took %ld seconds and %ld micros to complete\n", seconds, micros);

	return 0;
}
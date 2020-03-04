#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"Queue.h"
struct Global
{
	pthread_mutex_t Buffer;
	pthread_mutex_t Service;
	pthread_mutex_t Results;
	//pthread_mutex_t varProtect;
	queue* sharedbuff;
	int numFiles;
	//int fileAssign;
	FILE** fileArray;
	//int* fileStatusTable;
	int helpFiles;
	int* noLiveRequester;
};
struct reqThread
{
	int count;
	FILE* filename;
	struct Global* Gspot;
	int fileOn;
	FILE* serviced;
	int id;
}; 
struct resThread
{
	struct Global* Gspot;
	FILE* results;
};
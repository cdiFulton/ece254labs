#include <stddef.h>
#include <stdio.h>
#include <sys/wait.h>
#include <spawn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>



int main()
{
	sem_t* BUFFER_MUTEX;
	BUFFER_MUTEX = sem_open("/buffermutex", O_CREAT, S_IRUSR | S_IWUSR, 1);
}
// todo: makefile with gcc -lpthread -o threads threads.c helpers.c
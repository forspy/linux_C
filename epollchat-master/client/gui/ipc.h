#ifndef IPC_H
#define IPC_H


#include <semaphore.h>
#include <sys/sem.h>
#include <gtk/gtk.h>


struct ipc {

	int usockfd[2];
	
	pid_t   pid;
	sem_t * sem;
	
	GtkWidget * userinput;
	GtkWidget * connTxt;
	GtkListStore * store;
	
};

#define OPEN_SESAME "SAMS_SEMAPHORE"

#endif

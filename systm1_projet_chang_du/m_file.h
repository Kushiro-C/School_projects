#ifndef M_FILE_H
#define M_FILE_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <assert.h>
#include <dirent.h>

#define N 10 // max registered processes

/**
 * struct of a message that we send
 **/
typedef struct
{
    long type;
    char mtext[];
} mon_message;

/**
 * struct of a message stocked in the queue
 **/
typedef struct
{
    int size;
    long type;
    char mtext[];
} message_enreg;

/**
 * struct of a message stocked in the queue
 **/
typedef struct
{
    // header
    size_t size;           // writed size
    size_t first;          // index of the first element
    size_t last;           // index of the last element
    size_t len_max;        // max length of a message
    size_t nb_msg;         // max number of message stockable
    mode_t mode;           // mode
    pthread_mutex_t mutex; // mutex
    pthread_cond_t rcond;  // condition variable for read
    pthread_cond_t wcond;  // condition variable for write

    // extension 2
    pid_t registered[N];   // registered processes
    long reg_type[N];      // type wanting by processes
    int signal[N];         // signal required
    int id;                // id of the file

    // saved messages
    message_enreg *messages; // all saved messages
} ma_file;

/**
 * MESSAGE struct
 **/
typedef struct
{
    mode_t type;   // options
    ma_file *file; // ma_file struct pointer

} MESSAGE;

extern int initialiser_mutex(pthread_mutex_t *pmutex);
extern int initialiser_cond(pthread_cond_t *pcond);
extern MESSAGE *m_connexion(const char *nom, int options, ...);
extern int m_deconnexion(MESSAGE *file);
extern int m_destruction(const char *nom);
extern int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag);
extern size_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags);
extern size_t m_message_len(MESSAGE *);
extern size_t m_capacite(MESSAGE *);
extern size_t m_nb(MESSAGE *);
extern void displayAll(MESSAGE *file);
extern size_t m_enregistrer(MESSAGE *message, int signal, long type, void *msg, size_t len);
extern size_t m_desenregistrer(MESSAGE *message);
#endif
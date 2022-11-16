#ifdef __linux__
#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#define handle_error(msg)
#endif

#include "m_file.h"

void thread_error(const char *file, int line, int code, char *txt)
{
    if (txt != NULL)
        fprintf(stderr, "[%s] in file %s in line %d :  %s\n",
                txt, file, line, strerror(code));
    else
        fprintf(stderr, "in file %s in line %d :  %s\n",
                file, line, strerror(code));
    exit(1);
}

/**
 * initialization of mutex
 * @param pmutex the condition
 * @return int 0 if succeeded
 **/
int initialiser_mutex(pthread_mutex_t *pmutex)
{
    pthread_mutexattr_t mutexattr;
    int code;
    if ((code = pthread_mutexattr_init(&mutexattr)) != 0)
        return code;

    if ((code = pthread_mutexattr_setpshared(&mutexattr,
                                             PTHREAD_PROCESS_SHARED)) != 0)
        return code;
    code = pthread_mutex_init(pmutex, &mutexattr);
    return code;
}

/**
 * initialization of mutex's condition
 * @param pcond the condition
 * @return int 0 if succeeded
 **/
int initialiser_cond(pthread_cond_t *pcond)
{
    pthread_condattr_t condattr;
    int code;
    if ((code = pthread_condattr_init(&condattr)) != 0)
        return code;
    if ((code = pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED)) != 0)
        return code;
    code = pthread_cond_init(pcond, &condattr);
    return code;
}

/**
 * convert sh_open's options to mmap's protection
 * @return int
 **/
int oflagToProt(int options)
{
    int res = 0;
    if (options & O_RDWR) // options != NULL &&
        res = res | PROT_READ | PROT_WRITE;
    if (options & O_RDONLY)
        res = res | PROT_READ;
    if (options & O_WRONLY)
        res = res | PROT_WRITE;
    return res;
}

static int id = 0;  // count number of connexion O_CREAT called

/**
 * extension 2
 * create shm file with syntax type_pid
 * @param id of the file
**/
int create_shm(long type, int id) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d_%ld_%d", id, type, getpid());
    int fd = shm_open(buffer, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open() failed");
        return -1;
    }
    return 1;
}

/**
 * extension 2
 * destroy shm file with syntax type_pid
 * @param id of the file
**/
void destroy_shm(long type, int id) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d_%ld_%d", id, type, getpid());
    shm_unlink(buffer);
}

/**
 * extension 2
 * check if shm file with syntax id_type exists
 * @return 1 exists else 0
**/
int check_shm_exists(long type, int id) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d_%ld_", id, type);
    struct dirent *pDirent;
    DIR *pDir;
    pDir = opendir("/dev/shm");
    while ((pDirent = readdir(pDir)) != NULL) {
        if(strncmp(pDirent->d_name, buffer, strlen(buffer)) == 0)
            return 1;
    }

    return 0;
}

/**
 * Either connect to an existing message queue or create (2 parameters)
 * a new message queue and log in. (5 parameters)
 * @param nom name of the message queue
 * @param options
 * @param nb_msg
 * @param len_max
 * @param mode
 * @return MESSAGE or NULL
 **/
MESSAGE *m_connexion(const char *nom, int options, ...)
{
    // check if options has appropriate value
    if (!(options & O_RDWR || options & O_RDONLY || options & O_WRONLY)) // options != NULL &&
    {
        perror("wrong usage of m_connexion");
        return NULL;
    }

    // if options not contains O_CREAT, we have to use function with 2 parameters
    if (nom != NULL && !(options & O_CREAT))
    {

        int fd = shm_open(nom, options, 0);
        if (fd == -1)
        {
            perror("m_connexion's shm_open() failed");
            return NULL;
        }
        struct stat bufStat;
        if (fstat(fd, &bufStat) < 0)
        {
            perror("m_connexion fstat error");
            return NULL;
        }
        ma_file *buffer = mmap(NULL, bufStat.st_size, oflagToProt(options), MAP_SHARED, fd, 0);

        if ((void *)buffer == MAP_FAILED)
        {
            perror("map() failed lul");
            return NULL;
        }

        MESSAGE *message = malloc(sizeof(MESSAGE));
        if (message == NULL)
        {
            perror("malloc MESSAGE");
            return NULL;
        }
        message->file = buffer;
        message->type = options;
        return message;
    }

    // getting 3 others arguments
    va_list ap;
    va_start(ap, options); // va_start(ap, 3);
    size_t nb_msg = va_arg(ap, size_t);
    size_t len_max = va_arg(ap, size_t);
    mode_t mode = va_arg(ap, mode_t);
    va_end(ap);
    printf("creation du fichier partage %s\n", nom);
    printf("nombre message : %ld\n", nb_msg);
    printf("longueur message : %ld\n", len_max);
    printf("mode message : %d\n", mode);

    // maximum file size
    int size = sizeof(ma_file) + (sizeof(message_enreg) + len_max) * nb_msg;
    printf("creation d un message de taille maximale : %d\n\n", size);
    ma_file *buffer;
    MESSAGE *message = malloc(sizeof(MESSAGE));
    if (message == NULL)
    {
        perror("malloc MESSAGE");
        return NULL;
    }

    // anonymous
    if (nom == NULL)
    {
        if (nb_msg >= 1000000 || len_max >= 1000000 || mode >= 100000000)
        {
            perror("need parameter for anonymous MESSAGE!");
            free(message);
            return NULL;
        }
        buffer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

        if ((void *)buffer == MAP_FAILED)
        {
            perror("map() failed");
            free(message);
            return NULL;
        }
    }
    else
    {
        // Not anonymous
        int fd = shm_open(nom, options, mode);
        if (fd == -1)
        {
            if (errno == EEXIST)
                perror("shm_open existing object");
            else
                perror("m_connexion's shm_open() failed");
            free(message);
            return NULL;
        }

        // update message file size
        if (ftruncate(fd, size) == -1)
        {
            perror("ftruncate");
            free(message);
            return NULL;
        }

        buffer = mmap(NULL, size, oflagToProt(options), MAP_SHARED, fd, 0);

        if ((void *)buffer == MAP_FAILED)
        {
            perror("map() failed");
            free(message);
            return NULL;
        }
    }

    // message initialization
    message->type = options;
    buffer->first = 0;
    buffer->last = 0;
    buffer->len_max = len_max;
    buffer->mode = mode;
    buffer->nb_msg = nb_msg;
    int code = initialiser_mutex(&buffer->mutex);
    if (code != 0)
        thread_error(__FILE__, __LINE__, code, "init_mutex");
    code = initialiser_cond(&buffer->rcond);
    if (code != 0)
        thread_error(__FILE__, __LINE__, code, "init_rcond");
    code = initialiser_cond(&buffer->wcond);
    if (code != 0)
        thread_error(__FILE__, __LINE__, code, "init_wcond");

    // extension 2
    for(int i = 0; i < N; i++)
        buffer->reg_type[i] = -1;
    for(int i = 0; i < N; i++)
        buffer->registered[i] = -1;
    for(int i = 0; i < N; i++)
        buffer->signal[i] = -1;
    buffer->id = id;
    id ++;

    buffer->size = sizeof(ma_file);

    message->file = buffer;
    return message;
}

/**
 * message queue unlog
 * @param file
 * @return 0 if succeed , -1 error
 **/
int m_deconnexion(MESSAGE *file)
{
    if (munmap(file->file, sizeof(ma_file)) == -1)
        return -1;
    file->file = NULL; // need **file as parameter
    return 0;
}

/**
 * destory a MESSAGE
 **/
int m_destruction(const char *nom)
{
    /*
    Even if the object continues to exist after the last shm_unlink(),
    reuse of the name shall subsequently cause shm_open() to behave as
    if no shared memory object of this name exists (that is, shm_open()
    will fail if O_CREAT is not set, or will create
    a new shared memory object if O_CREAT is set).
    */
    if (shm_unlink(nom) == -1)
        return -1;
    return 0;
}

/**
 * send a message to the fifo list
 * @param file pointer to a MESSAGE
 * @param msg the message as a struc of mon_message
 * @param len len of the message (not all the struct)
 * @param msgflag 0 if Block and 1 if NONBLOCK
 * @return 0 if succeeded, -1 error failed
 **/
int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag)
{
    if (!(file->type & O_WRONLY || file->type & O_RDWR))
    {
        perror("no permission");
        return -1;
    }

    if (file->file == NULL)
    {
        perror("file NULL");
        return -1;
    }

    // get msg information
    long *msg_type = (long *)msg;
    char *mtext = (char *)msg_type + sizeof(long);

    int code = 0; // all mutex's functions return

    // maximum size of a MESSAGE
    int maxSize = sizeof(ma_file) + (sizeof(message_enreg) + file->file->len_max) * file->file->nb_msg;

    if (len > file->file->len_max)
    {
        perror("you insering a length too big");
        return -1;
    }

    code = pthread_mutex_lock(&file->file->mutex);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_lock");
        return -1;
    }

    // if no spaces available and O_NONBLOCK -> exit
    if ((maxSize < file->file->size + sizeof(message_enreg) + len) && msgflag == O_NONBLOCK)
    {
        code = pthread_mutex_unlock(&file->file->mutex);
        if (code != 0)
        {
            thread_error(__FILE__, __LINE__, code, "mutex_unlock");
            return -1;
        }
        errno = EAGAIN;
        perror("MESSAGE is full");
        return -1;
    }
    // if no spaces available and BLOCK -> wait
    else if (maxSize < file->file->size + sizeof(message_enreg) + len)
    {
        if (msgflag != 0)
        {
            perror("wrong msgflag");
            return -1;
        }

        printf("Attente de l envoi en bloquant de %d..\n", getpid());

        while (maxSize < file->file->size + sizeof(message_enreg) + len)
        {
            code = pthread_cond_wait(&file->file->wcond, &file->file->mutex);
            if (code > 0)
            {
                fprintf(stderr, "process %d error\n", (int)getpid());
                thread_error(__FILE__, __LINE__, code, "pthread_cond_wait");
            }
        }

        printf("Sortie de l'attente de l'envoi en bloquant de %d..\n", getpid());
    }

    // insert new message_enreg
    message_enreg *new_message_index = (message_enreg *)(((char *)file->file) + file->file->size);
    new_message_index->type = *msg_type;
    memmove(new_message_index->mtext, mtext, len);
    new_message_index->size = sizeof(message_enreg) + len;

    // update MESSAGE's header informations
    file->file->size += sizeof(message_enreg) + len;
    file->file->last += 1;
    printf("%d a change la valeur de size, apres envoi size: %ld\n\n", getpid(), file->file->size);

    // unlock mutex
    code = pthread_mutex_unlock(&file->file->mutex);
    if (code != 0)
    {
        printf("erreur on unlock !");
        thread_error(__FILE__, __LINE__, code, "mutex_unlock");
        return -1;
    }

    // signal condition lifted
    code = pthread_cond_signal(&file->file->rcond);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_unlock");
        return -1;
    }

    // extension 2
    for(int i = 0; i < N; i++) {
        if(file->file->reg_type[i] == *msg_type) {

            // if not exists pid
            if ((kill(file->file->registered[i], 0) < 0) && errno == EPERM) {
                file->file->registered[i] = -1;
                file->file->reg_type[i] = -1;
                file->file->signal[i] = -1;
            }
            else {
                kill(file->file->registered[i], file->file->signal[i]);
                return 0;
            }
        }
    }

    return 0;
}

/**
 * receive a message from the fifo list
 * @param file pointer to a MESSAGE
 * @param msg where message is copy
 * @param len len of the message buffer where it is copy
 * @param type what we are looking for
 * @param flags 0 if Block and 1 if NONBLOCK
 * @return 0 if succeeded, -1 error failed
 **/
size_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags)
{
    if (!(file->type & O_RDONLY || file->type & O_RDWR))
    {
        perror("no permission");
        return -1;
    }

    if (file->file == NULL)
    {
        perror("file NULL");
        return -1;
    }

    int code; // all mutex's functions return

    // get msg attributes
    long *msg_type = (long *)msg;
    char *mtext = (char *)msg_type + sizeof(long);

    int current_size = sizeof(ma_file); // current index in MESSAGE scan
    int found = 0;                      // if we found a message_enreg
    message_enreg *new_message_index;   // message to receive

    code = pthread_mutex_lock(&file->file->mutex);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_lock");
        return -1;
    }

    // empty MESSAGE with O_NONBLOCK -> exit
    if (file->file->last == 0 && flags == O_NONBLOCK)
    {
        code = pthread_mutex_unlock(&file->file->mutex);
        if (code != 0)
        {
            thread_error(__FILE__, __LINE__, code, "mutex_unlock");
            return -1;
        }
        printf("not found\n");
        errno = EAGAIN;
        perror("MESSAGE is empty\n");
        return -1;
    }
    // empty MESSAGE with BLOCK -> wait until availablity
    else if (file->file->last == 0)
    {
        if (flags != 0) // flags different than O_NONBLOCK and 0
        {
            perror("wrong flags");
            return -1;
        }

        create_shm(type, file->file->id); // extension 2

        printf("Attente de la reception en bloquant de %d..\n", getpid());

        while (file->file->last == 0)
        {
            code = pthread_cond_wait(&file->file->rcond, &file->file->mutex);
            if (code > 0)
            {
                fprintf(stderr, "process %d error\n", (int)getpid());
                thread_error(__FILE__, __LINE__, code, "pthread_cond_wait");
            }
        }

        destroy_shm(type, file->file->id); // extension 2
        printf("Sortie de l'attente de la reception en bloquant de %d..\n", getpid());
    }

    // search message
    // if we want first message
    if (type == 0)
    {
        new_message_index = (message_enreg *)(((char *)file->file) + current_size);
        found = 1;
    }
    else
    {
        while (current_size < (file->file->size - sizeof(message_enreg)))
        {
            new_message_index = (message_enreg *)(((char *)file->file) + current_size);
            // match the type
            if (type == new_message_index->type ||
                (type < 0 && abs(type) >= new_message_index->type))
            {
                found = 1;
                break;
            }
            current_size += new_message_index->size;
        }
    }

    // not found
    if (!found && flags == O_NONBLOCK)
    {
        code = pthread_mutex_unlock(&file->file->mutex);
        if (code != 0)
        {
            thread_error(__FILE__, __LINE__, code, "mutex_unlock");
            return -1;
        }
        printf("not found\n");
        return -1;
    } else if (! found && flags == 0) {
        printf("Attente de la reception type en bloquant de %d..\n", getpid());
        create_shm(type, file->file->id); // extension 2

        while( !found ) {
            code = pthread_cond_wait(&file->file->rcond, &file->file->mutex);
            if (code > 0)
            {
                fprintf(stderr, "process %d error\n", (int)getpid());
                thread_error(__FILE__, __LINE__, code, "pthread_cond_wait");
            }
            if (type == 0)
            {
                new_message_index = (message_enreg *)(((char *)file->file) + current_size);
                found = 1;
            }
            while (current_size < (file->file->size - sizeof(message_enreg)))
            {
                new_message_index = (message_enreg *)(((char *)file->file) + current_size);
                // match the type
                if (type == new_message_index->type ||
                    (type < 0 && abs(type) >= new_message_index->type))
                {
                    found = 1;
                    break;
                }
                current_size += new_message_index->size;
            }
        }
        destroy_shm(type, file->file->id); // extension 2
        printf("Sortie de l'attente de la reception type en bloquant de %d..\n", getpid());
    }

    // check if buffer is too small
    if (len < new_message_index->size - sizeof(message_enreg))
    {
        code = pthread_mutex_unlock(&file->file->mutex);
        if (code != 0)
        {
            thread_error(__FILE__, __LINE__, code, "mutex_unlock");
            return -1;
        }
        // printf("too small buffer for receiving! len :%ld     message_len :%ld\n", len, new_message_index->size - sizeof(message_enreg));
        perror("too small buffer for receiving!");
        return -1;
    }

    // copy to buffer
    *msg_type = new_message_index->type;
    memmove(mtext, new_message_index->mtext, len);

    // deleting message_enreg in MESSAGE
    int new_message_size = new_message_index->size;
    memmove(((char *)file->file) + current_size, ((char *)file->file) + current_size + new_message_size, file->file->size - current_size - new_message_size);
    // clean the end of the shared memory
    memset(((char *)file->file) + (file->file->size - new_message_size), 0, new_message_size);

    // update header in MESSAGE
    file->file->size -= new_message_size;
    file->file->last -= 1;
    printf("%d a change la valeur de size, apres reception size: %ld\n\n", getpid(), file->file->size);

    // unlock mutex
    code = pthread_mutex_unlock(&file->file->mutex);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_unlock");
        return -1;
    }

    // signal condition lifted
    code = pthread_cond_signal(&file->file->wcond);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_unlock");
        return -1;
    }

    return 0;
}

/**
 * length max of messages that we can stock
 **/
size_t m_message_len(MESSAGE *message)
{
    return message->file->len_max;
}

/**
 * number max of messages that we can stock
 **/
size_t m_capacite(MESSAGE *message)
{
    return message->file->nb_msg;
}

/**
 * current number of stocked messages
 **/
size_t m_nb(MESSAGE *message)
{
    int code;
    int last;

    // lock mutex
    code = pthread_mutex_lock(&message->file->mutex);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_lock");
    }

    last = message->file->last;
    // unlock mutex
    code = pthread_mutex_unlock(&message->file->mutex);
    if (code != 0)
    {
        thread_error(__FILE__, __LINE__, code, "mutex_unlock");
        return -1;
    }

    return last;
}

volatile sig_atomic_t flag = 0; // handler activated

/**
 * handler for m_enregistrer signal parameter
**/
void handler(int sig)
{
    flag = 1;
    write(1, "\nHANDLER ACTIVE\n", strlen("\nHANDLER ACTIVE\n"));
}

void handler2(int sig)
{
}

/**
 * register a proccesses
 * @param signal used to wait
 * @param type type of message
 * @return 0 if succeeded, -1 error or failed
**/
size_t m_enregistrer(MESSAGE *message, int signal, long type, void *msg, size_t len) {
    int index = 0;  // index in array registered
    int code;       // mutex

    // init sigaction
    struct sigaction str;
    str.sa_handler = handler;
    sigfillset(&str.sa_mask);
    str.sa_flags = SA_RESTART;

    // check conditions for register
    for(int i = 0; i < N; i++) {

        // already in registered
        if (message->file->registered[i] == getpid()) {
            printf("\nPlease call m_desenregistrer before\n");
            return -1;
        }

        // place found
        if(message->file->registered[i] == -1) {
            index = i;
            break;
        }

        // full
        if(i == N-1) {
            printf("\nMaximum number of registered processes reached\n");
            return 0;
        }
    }

    if(message->file->registered[index] == -1) {

        // install handler
        if (sigaction(signal, &str, NULL) < 0) {
            printf("\n sigaction error\n");
            return -1;
        }

        str.sa_handler = NULL;
        if (sigaction(SIGIO, &str, NULL) < 0) {
            printf("\n sigaction error\n");
            return -1;
        }

        /* get STDIN_FILENO */
        if( fcntl( STDIN_FILENO, F_SETOWN, getpid() ) == -1 ){
            printf("\nfcntl error\n");
            return -1;
        }

        // NONBLOCK on stdin
        int flags = fcntl( STDIN_FILENO, F_GETFL );
        if( fcntl( STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK ) == -1 ) {
            printf("\nfcntl error\n");
            return -1;
        }

        /* get STDIN_FILENO */
        if( fcntl( STDOUT_FILENO, F_SETOWN, getpid() ) == -1 ){
            printf("\nfcntl error\n");
            return -1;
        }

        // NONBLOCK on stdout
        flags = fcntl( STDOUT_FILENO, F_GETFL );
        if( fcntl( STDOUT_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK ) == -1 ) {
            printf("\nfcntl error\n");
            return -1;
        }

        str.sa_handler = handler2;
        str.sa_flags = SA_NOCLDWAIT;
        if (sigaction(SIGCHLD, &str, NULL) < 0) {
            printf("\n sigaction error\n");
            return -1;
        }

        // lock mutex
        code = pthread_mutex_lock(&message->file->mutex);
        if (code != 0)
        {
            thread_error(__FILE__, __LINE__, code, "mutex_lock");
        }

        // init data in file
        message->file->registered[index] = getpid();
        message->file->reg_type[index] = type;
        message->file->signal[index] = signal;

        // unlock mutex
        code = pthread_mutex_unlock(&message->file->mutex);
        if (code != 0)
        {
            thread_error(__FILE__, __LINE__, code, "mutex_unlock");
            return -1;
        }

        int res;
        while(1) {
            if(flag) {
                flag = 0;
                if(check_shm_exists(type, message->file->id)) {
                    printf("Quelqu'un d'autre attend le meme type de message\n");
                    continue;
                }
                printf("Signal recu pour %d\n", getpid());
                if((res = m_reception(message, msg, len, type, O_NONBLOCK)) < 0)
                    continue;
                m_desenregistrer(message);
                return res;
            }
        }
    }
    return -1;
}

/**
 * unregister
 * @param message
 * @return 0 if succeeded, -1 error or failed
**/
size_t m_desenregistrer(MESSAGE *message) {
    for(int i = 0; i < N; i++) {
        if(message->file->registered[i] == getpid()) {
            message->file->registered[i] = -1;
            message->file->reg_type[i] = -1;
            message->file->signal[i] = -1;
            return 0;
        }
    }
    return -1;
}
/**
 * display all saved message in MESSAGE as a char*
 * @param file MESSAGE
 **/
void displayAll(MESSAGE *file)
{
    printf("\n------Display all saved messages------\n");
    if (file->file->size == sizeof(ma_file))
    {
        printf("EMPTY MESSAGE\n\n");
        return;
    }

    message_enreg *new_message_index;
    int current_size = sizeof(ma_file);
    int cpt = 0;

    while (current_size < (file->file->size - sizeof(message_enreg)))
    {
        new_message_index = (message_enreg *)(((char *)file->file) + current_size);
        printf("message%d   type %ld\n", cpt, new_message_index->type);
        printf("message%d   mtext %s\n\n", cpt, new_message_index->mtext);
        current_size += new_message_index->size;
        cpt++;
    }
}

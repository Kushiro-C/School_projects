#include "m_file.h"

/**
 * Test ext2Test.c
 * Doit parvenir a obtenir les messages de type 56
 * A la fin seul 1 message reste dans la file (type 55)
**/
int main(int argc, char const *argv[])
{
    printf("%s\n", "\n\n---------- Extension 2 processus test----------\n\n");

    int i = -1;
    int len = 100;

    char *t = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem"; /*valeurs Ã  envoyer*/
    mon_message *m1_message = malloc(sizeof(mon_message) + strlen(t) + 1);
    if (m1_message == NULL)
    { /* traiter erreur de malloc */
        perror("malloc");
        return -1;
    }
    m1_message->type = (long)getpid();            // pid as type
    memmove(m1_message->mtext, t, strlen(t) + 1); /* copier les char Ã  envoyer */
    m_destruction("montest3");
    MESSAGE *m7 = m_connexion("montest3", O_CREAT | O_RDWR, 5, len, S_IRUSR | S_IWUSR);
    assert(m7 != NULL);

    MESSAGE *m8 = m_connexion("montest3", O_RDWR);
    assert(m8 != NULL);

    srand(time(NULL));

    for (int j = 0; j < 5; j++)
    {
        int pid = fork();

        if (pid == -1)
        {
            perror("failed fork");
            exit(1);
        }
        if (pid == 0)
        { // children
            if (j == 1)
            {
                sleep(1);
                printf("Moi %d je suis reveille\n", getpid());
                mon_message *receive7 = malloc(sizeof(mon_message) + strlen(t) + 1);
                if (receive7 == NULL)
                {
                    perror("malloc");
                    return -1;
                }
                i = m_reception(m7, receive7, strlen(t) + 1, 56, 0);
                if (i != -1)
                {
                    printf("Je suis le fils %d j ai recu un message de %ld en bloquant:\n%s\n\n", getpid(), receive7->type, receive7->mtext);
                }
                else
                    printf("Je suis le fils %d et je n ai rien recu en bloquant\n", getpid());
                free(receive7);
            }
            if(j == 0 ) {
                m1_message->type = 55;
                i = m_envoi(m8, m1_message, strlen(t) + 1, 0);
            }
            if(j == 2)
            {
                sleep(2);
                m1_message->type = 56;
                i = m_envoi(m8, m1_message, strlen(t) + 1, 0);
            }
            if(j == 3) {
                mon_message *receive7 = malloc(sizeof(mon_message) + strlen(t) + 1);
                int i = m_enregistrer(m7, SIGUSR1, 56, receive7, strlen(t) + 1);
                printf("start enregistrement for %d\n", getpid());

                while(1) {
                    //sleep(1);
                    if(i == 0) {
                        printf("Je suis le fils %d j ai recu un message de %ld en enregistrement:\n%s\n\n", getpid(), receive7->type, receive7->mtext);
                        return 1;
                    }
                }
                printf("Je suis le fils %d j ai recu un message de %ld en enregistrement:\n%s\n\n", getpid(), receive7->type, receive7->mtext);
            }
            if(j == 4) {
                sleep(4);
                printf("\nEnvoi du second message type 56\n");
                m1_message->type = 56;
                i = m_envoi(m8, m1_message, strlen(t) + 1, 0);
            }
            exit(0);
        }
        else
        { // father
            continue;
        }
    }
    sleep(7);
    displayAll(m7);

    free(m1_message);
    m_destruction("montest3");
    return 0;
}

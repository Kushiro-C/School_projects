#include "m_file.h"

int main(int argc, char const *argv[])
{
    m_destruction("montest");

    // tests with uniq processus in O_NONBLOCK
    printf("----------WITHOUT MULTIPLE processus test----------\n\n");
    int len = 100;
    // test first connexion
    printf("---------test first connexion-----------\n");
    MESSAGE *m1 = m_connexion("montest", O_CREAT | O_RDWR, 5, len, 0);
    assert(m1 != NULL);

    // anonyme connexion
    printf("---------anonymous first connexion-----------\n");
    MESSAGE *m2 = m_connexion(NULL, O_CREAT | O_RDWR, 5, len, 0);
    assert(m2 != NULL);

    // O_RDWR, O_RDONLY, O_WRONLY need test
    printf("---------O_RDWR, O_RDONLY, O_WRONLY need test-----------\n");
    MESSAGE *m3 = m_connexion(NULL, O_CREAT);
    assert(m3 == NULL);

    // no file test
    printf("--------no file test-----------\n");
    MESSAGE *m4 = m_connexion("moExists", O_RDWR, 5, len, 0);
    assert(m4 == NULL);

    // perror("shm_open existing object");
    printf("--------shm_open existing object-----------\n");
    MESSAGE *m5 = m_connexion("montest", O_CREAT | O_EXCL | O_RDWR, 5, len, 0);
    assert(m5 == NULL);

    // init first mon_message
    char *t = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem"; /*valeurs Ã  envoyer*/
    mon_message *m1_message = malloc(sizeof(mon_message) + strlen(t) + 1);
    if (m1_message == NULL)
    { /* traiter erreur de malloc */
        perror("malloc");
        return -1;
    }
    m1_message->type = (long)getpid();            // pid as type
    memmove(m1_message->mtext, t, strlen(t) + 1); /* copier les char Ã  envoyer */

    // init second mon_message
    int t2[2] = {-12, 99}; /*valeurs Ã  envoyer*/
    mon_message *m2_message = malloc(sizeof(mon_message) + sizeof(t2));
    if (m2_message == NULL)
    {
        perror("malloc");
        return -1;
    }
    m2_message->type = 55;                      // type
    memmove(m2_message->mtext, t2, sizeof(t2)); /* copier les deux int Ã  envoyer */

    // init thirs mon_message
    char t3[57] = "Ac ne quis a nobis hoc ita dici forte miretur, quod alia"; /*valeurs Ã  envoyer*/
    mon_message *m3_message = malloc(sizeof(mon_message) + sizeof(t3) + 1);
    if (m3_message == NULL)
    {
        perror("malloc");
        return -1;
    }
    m3_message->type = 56;                          // type
    memmove(m3_message->mtext, t3, sizeof(t3) + 1); /* copier les char Ã  envoyer */

    printf("-------try receive empty MESSAGE---------\n");
    mon_message *receive0 = malloc(sizeof(mon_message) + strlen(t) + 1);
    if (receive0 == NULL)
    {
        perror("malloc");
        return -1;
    }
    assert(m_reception(m1, receive0, strlen(t) + 1, 0, O_NONBLOCK) != 0);

    /* envoyer en mode non bloquant */
    printf("-------insering first message---------\n");
    int i = m_envoi(m1, m1_message, strlen(t) + 1, O_NONBLOCK);
    assert(i == 0);
    if (i == 0)
    { /* message envoyÃ© */
        printf("message envoye \n");
    }
    else if ((i == -1) && (errno = EAGAIN))
    { /* file pleine, essayer plus tard */
        printf("message non envoye \n");
    }
    else
    { /* erreur de la fonction */
    }

    // tests getters
    printf("m1 message_len %ld\n", m_message_len(m1));
    printf("m1 message_capacite %ld\n", m_capacite(m1));
    printf("m1 message_nb %ld\n\n", m_nb(m1));
    assert(m_nb(m1) == 1);

    /* envoyer en mode non bloquant avec un len superieur*/
    printf("-------insering a message too bigs---------\n");
    i = m_envoi(m1, m1_message, len + 1, O_NONBLOCK);
    assert(i != 0);
    /* remplis la base jusqu'au max*/
    printf("-------insering 2nd message---------\n");
    i = m_envoi(m1, m1_message, strlen(t) + 1, O_NONBLOCK);
    assert(i == 0);
    printf("-------insering 3rd message---------\n");
    i = m_envoi(m1, m1_message, strlen(t) + 1, O_NONBLOCK);
    assert(i == 0);
    printf("-------insering 4th message---------\n");
    i = m_envoi(m1, m1_message, strlen(t) + 1, O_NONBLOCK);
    assert(i == 0);
    printf("-------insering 5th message---------\n");
    i = m_envoi(m1, m2_message, sizeof(t2), O_NONBLOCK);
    assert(i == 0);
    printf("-------insering 6th message succeded because messages are compacted---------\n");
    i = m_envoi(m1, m3_message, sizeof(t3) + 1, O_NONBLOCK);
    assert(i == 0);
    printf("-------insering 7th message succeded because messages are compacted---------\n");
    i = m_envoi(m1, m2_message, sizeof(t2), O_NONBLOCK);
    assert(i == 0);
    /* envoyer en mode non bloquant avec MESSAGE plein */
    printf("-------insering O_NONBLOCK a message with MESSAGE full---------\n");
    i = m_envoi(m1, m1_message, strlen(t) + 1, O_NONBLOCK);
    assert(i != 0);

    printf("-------insering first message in an anonymous MESSAGE---------\n");
    i = m_envoi(m2, m2_message, sizeof(t2), O_NONBLOCK);
    assert(i == 0);
    ///*printf("capacite : %ld", m_capacite(m1));
    // printf("message len : %ld", m_message_len(m1));
    // printf("nombre actuel : %ld", m_nb(m1));*/
    //// display_last(m1);

    printf("-------receive first message---------\n");
    mon_message *receive1 = malloc(sizeof(mon_message) + strlen(t) + 1);
    if (receive1 == NULL)
    {
        perror("malloc");
        return -1;
    }
    m_reception(m1, receive1, strlen(t) + 1, 0, O_NONBLOCK);
    // receive1->mtext[strlen(t) +1] = '\0';
    printf("main on a recu : %s\n", receive1->mtext);

    printf("-------receive message with type 55 by taking type -56---------\n");
    mon_message *receive2 = malloc(sizeof(mon_message) + sizeof(t2));
    if (receive2 == NULL)
    {
        perror("malloc");
        return -1;
    }
    int receive_t2[2];
    m_reception(m1, receive2, sizeof(t2), -56, O_NONBLOCK);
    memmove(receive_t2, receive2->mtext, sizeof(t2));
    printf("main on a recu : %d\n", receive_t2[0]);
    printf("main on a recu : %d\n\n", receive_t2[1]);

    printf("-------receive message with type 56---------\n");
    mon_message *receive3 = malloc(sizeof(mon_message) + sizeof(t3) + 1);
    if (receive3 == NULL)
    {
        perror("malloc");
        return -1;
    }
    m_reception(m1, receive3, sizeof(t3) + 1, 56, O_NONBLOCK);
    printf("main on a recu : %s\n", receive3->mtext);

    printf("-------receive message with type 55---------\n");
    mon_message *receive4 = malloc(sizeof(mon_message) + sizeof(t2));
    if (receive4 == NULL)
    {
        perror("malloc");
        return -1;
    }
    int receive_t4[2];
    m_reception(m1, receive4, sizeof(t2), 55, O_NONBLOCK);
    memmove(receive_t4, receive4->mtext, sizeof(t2));
    printf("main on a recu : %d\n", receive_t2[0]);
    printf("main on a recu : %d\n", receive_t2[1]);

    displayAll(m1);

    // test deconnexion
    printf("---------------test deconnexion----------\n");
    m_destruction("montest3");
    MESSAGE *m6 = m_connexion("montest3", O_CREAT | O_RDWR, 5, len, 0);
    assert(m6 != NULL);
    printf("deconnexion montest3\n");
    assert(m_deconnexion(m6) == 0);
    /*msync(m6, sizeof(ma_file) + (sizeof(message_enreg) + 100) * 5, 0);
    assert(errno == ENOMEM);*/
    printf("send message to montest3\n");
    i = m_envoi(m6, m3_message, sizeof(t3) + 1, O_NONBLOCK);
    assert(i != 0);

    printf("\n\n---------- FORK MULTIPLE processus test----------\n\n");
    // m_destruction("montest2");
    m_destruction("testFork");
    MESSAGE *m7 = m_connexion(NULL, O_CREAT | O_RDWR, 5, len, 0);
    assert(m7 != NULL);

    srand(time(NULL));

    for (int j = 0; j < 16; j++)
    {
        int pid = fork();

        if (pid == -1)
        {
            perror("failed fork");
            exit(1);
        }
        if (pid == 0)
        { // children
            if (j % 2 == 0)
            {
                sleep(j);
                printf("Moi %d je suis reveille\n", getpid());
                mon_message *receive7 = malloc(sizeof(mon_message) + strlen(t) + 1);
                if (receive7 == NULL)
                {
                    perror("malloc");
                    return -1;
                }
                i = m_reception(m7, receive7, strlen(t) + 1, 0, 0);
                if (i != -1)
                {
                    printf("Je suis le fils %d j ai recu un message de %ld en bloquant:\n%s\n\n", getpid(), receive7->type, receive7->mtext);
                }
                else
                    printf("Je suis le fils %d et je n ai rien recu en bloquant\n", getpid());
                free(receive7);
            }
            else
            {
                if (j == 0)
                {
                    sleep(3);
                }
                m1_message->type = (long)getpid();
                i = m_envoi(m7, m1_message, strlen(t) + 1, 0);
            }
            exit(0);
        }
        else
        { // father
            continue;
        }
    }
    sleep(19);
    displayAll(m7);
    free(receive0);
    free(receive1);
    free(receive2);
    free(receive3);
    free(receive4);
    free(m1_message);
    free(m2_message);
    free(m3_message);

    return 0;
}
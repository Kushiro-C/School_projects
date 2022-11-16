#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "formatter.h"
#include "parser.h"

#define MAX_MESSAGE 140
#define MAX_ID 8
#define TIME_LIMIT 20
#define MAX_DATA 161
#define ADDRESS "127.0.0.1"
#define MAX_IP 15
#define MAX_PORT 4
#define BLOCKSIZE 1024

char addr_ip[MAX_IP + 1];
char txt[3][MAX_MESSAGE + 17]; // MESS ID MAX_MESSAGE
char id[MAX_ID + 1]; // Formatted ID
char ip_address[MAX_IP + 1]; // Unformatted IP

struct diffus
{
    char id[MAX_ID + 1];
    char ip1[MAX_IP + 1];
    char multi_port[MAX_PORT + 1];
    char ip2[MAX_IP + 1];
    char recv_port[MAX_PORT + 1];
};

/**
 * Fill the variable tct with some texts
 * from the file user_msg.txt
 */
void fillTxt()
{
    int fd = open("user_msg.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("Could not open user_msg.txt\n");
        exit(1);
    }

    int re;
    char buf[BLOCKSIZE]; // Small file content (user_msg.txt)
    memset(buf, '\0', BLOCKSIZE);
    re = 0;
    // Reading File
    if ((re = read(fd, buf, BLOCKSIZE)) < 0)
    {
        printf("Could not read user_msg.txt\n");
        close(fd);
        exit(1);
    }
    buf[re] = '\0';
    close(fd);
    int idx = 0;

    for (int i = 0; i < 3; i++) // We store only 3 messages for tests
    {
        char msgUF[MAX_MESSAGE + 1];
        memset(msgUF, '\0', MAX_MESSAGE + 1);
        int k = 0;
        while (k < 140 && idx < re && buf[idx] != '\n')
            msgUF[k++] = buf[idx++];
        msgUF[k] = '\0';
        if (buf[idx] == '\n')
            idx++;
        char *msg = formatMessage(msgUF, 140);
        memcpy(txt[i], "MESS ", 5);
        strncat(txt[i], id, MAX_ID);
        strcat(txt[i], " ");
        strncat(txt[i], msg, MAX_MESSAGE);
        txt[i][MAX_MESSAGE + 14] = '\r';
        txt[i][MAX_MESSAGE + 15] = '\n';
        txt[i][MAX_MESSAGE + 16] = '\0';
        free(msg);
    }
}

// function for receiving multicast
void *recv_multi(void *arg)
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    int ok = 1;
    int r = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &ok, sizeof(ok)); //SO_REUSEADDR
    if (r == -1) {
        printf("Error Set Socket\n");
        return 0;
    }
    struct sockaddr_in address_sock;
    address_sock.sin_family = AF_INET;
    address_sock.sin_port = htons(atoi((char *)((struct diffus *)arg)->multi_port));
    address_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    r = bind(sock, (struct sockaddr *)&address_sock, sizeof(struct sockaddr_in));
    if (r == -1) {
        printf("Error Bind\n");
        return 0;
    }
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(((struct diffus *)arg)->ip1);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    r = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    char buff[MAX_DATA]; //+1 ?
    while (1)
    {
        int rec = recv(sock, buff, MAX_DATA, 0);
        buff[rec] = '\0';
        if (checkDIFF(buff))
            printf("%s\n", unFormatMessage2(buff));
        else
            printf("Wrong Syntax DIFF\r\n");
    }
    return 0;
}

/**
 * Send to diffuser with TCP
 * It will repeatedly send a random request :
 * [LAST 003] : Last 3 messages sent
 * [MESS id mess] : A new message to send
 */
void *sendToDiffus(void *arg)
{
    struct sockaddr_in adress_sock;
    adress_sock.sin_family = AF_INET;
    adress_sock.sin_port = htons(atoi(((struct diffus *)arg)->recv_port));
    inet_aton(((struct diffus *)arg)->ip2, &adress_sock.sin_addr); //server

    while (1)
    {
        sleep(8);
        int descr = socket(PF_INET, SOCK_STREAM, 0);
        int r = connect(descr, (struct sockaddr *)&adress_sock,
                        sizeof(struct sockaddr_in));

        srand(time(NULL));  // Initialization, should only be called once.
        int i = rand() % 2; //if 0 we ask List else we send a new message to diffuser

        if (r != -1)
        {
            if (i == 0)
            { // ask Last
                char buff[MAX_DATA + 1];
                int size_rec;
                printf("\n- - - - - LAST 003 asked - - - - -\n\n");
                if (send(descr, "LAST 003\r\n", sizeof("LAST 003\r\n"), 0) < 0)
                {
                    close(descr);
                    perror("send error");
                    exit(0);
                }

                // get answers
                while ((size_rec = recv(descr, buff, MAX_DATA * sizeof(char), 0)) > 0)
                {
                    buff[size_rec] = '\0';
                    if (checkOLDM(buff) || !memcmp(buff, "ENDM\r\n", size_rec))
                        printf("%s\r\n", unFormatMessage2(buff));
                    else {
                        printf("Wrong Syntax with Received Message [OLDM or ENDM]\r\n");
                        close(descr);
                        break;
                    }
                }
                // close(descr);
                // perror("recv error");
                // exit(0);
            }
            else
            { // send a new message
                char buff[MAX_DATA + 1];
                int size_rec;
                i = rand() % sizeof(txt) / sizeof(txt[0]);

                if (send(descr, txt[i], MAX_MESSAGE + 16, 0) < 0)
                {
                    close(descr);
                    perror("send error");
                    exit(0);
                }
                // get answers
                if ((size_rec = recv(descr, buff, MAX_DATA * sizeof(char), 0)) < 0)
                {
                    close(descr);
                    perror("recv error");
                    exit(0);
                }
                buff[size_rec] = '\0';
                if (memcmp(buff, "ACKM\r\n", size_rec) == 0)
                    printf("\n%s\n", buff);
                else
                    printf("Error recv from diffuser, Received : %s\r\n", buff);
                close(descr);
            }
        }
    }

    return 0;
}

/**
 *transfrom a char* to a struct diffus
 *"[ITEM␣id␣ip1␣port1␣ip2␣port2]"   55 bytes
*/
struct diffus *itemToDiffus(char *item)
{
    struct diffus *diff = malloc(sizeof(struct diffus));
    if (diff == NULL)
    {
        printf("malloc error\n");
        exit(0);
    }
    //printf("%s\n", item+5);
    memset(diff->id, '\0', MAX_ID + 1);
    memset(diff->ip1, '\0', MAX_IP + 1);
    memset(diff->multi_port, '\0', MAX_PORT + 1);
    memset(diff->ip2, '\0', MAX_IP + 1);
    memset(diff->recv_port, '\0', MAX_PORT + 1);

    strncpy(diff->id, item + 5, 8);
    strncpy(diff->ip1, item + 14, 15);
    strncpy(diff->multi_port, item + 30, 4);
    strncpy(diff->ip2, item + 35, 15);
    strncpy(diff->recv_port, item + 51, 4);
    return diff;
}

/**
 * Reads a config file (config_user.txt)
 * Sets the ID, and the IPv4 Address
 * Exits if one data could not be read
 */
void readConfigFile(char *config)
{
    int fd = open(config, O_RDONLY);
    if (fd == -1)
    {
        printf("Could not open user's config file\n");
        exit(1);
    }
    char buf[BLOCKSIZE];
    int re;
    int idx = 0;

    // Read full file
    if ((re = read(fd, buf, BLOCKSIZE - 1)) < 0)
    {
        printf("Could not read user_config.txt\n");
        close(fd);
        exit(1);
    }
    buf[re] = '\0';

    // Get ID (First line)
    int k = 0;
    char myID[MAX_ID + 1];
    memset(myID, '#', MAX_ID + 1);
    while (idx < re && buf[idx] != '\n' && k < MAX_ID)
        myID[k++] = buf[idx++];
    myID[MAX_ID] = '\0';
    memset(id, '\0', MAX_ID + 1);
    memcpy(id, myID, MAX_ID + 1);
    printf("ID: %s\n", id);

    // Get IPv4 (Second line)
    k = 0;
    idx++; // Skip \n
    char myIP[MAX_IP + 1];
    memset(myIP, '\0', MAX_IP + 1);
    while (idx < re && buf[idx] != '\n' && k < MAX_IP)
        myIP[k++] = buf[idx++];
    myIP[k] = '\0';

    printf("IP: %s\n", myIP);
    if (!isValidIPv4Format(myIP))
    {
        printf("Bad Syntax for IPv4 (Line 2) in the user_config.txt");
        close(fd);
        exit(1);
    }
    memset(addr_ip, '\0', MAX_IP + 1);
    memcpy(addr_ip, myIP, MAX_IP + 1);
    close(fd);
}

/**
 * user first connect to manager and then random select a diffuser
 */
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Error: params: PORT number and config_user.txt\n");
        return 0;
    }

    //char *node = "localhost";

    /*struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;*/

    // get first adress
    /*if (getaddrinfo(node, NULL, &hints, &first_info) != 0)
    {
        printf("%s\n", "Error : getaddrinfo");
        exit(1);
    }*/

    int p = atoi(argv[1]); // Manager port
    readConfigFile(argv[2]);
    fillTxt();
    struct sockaddr_in adress_sock;
    adress_sock.sin_family = AF_INET;
    adress_sock.sin_port = htons(p);
    inet_aton(addr_ip, &adress_sock.sin_addr); //server
    //adress_sock.sin_addr = ((struct sockaddr_in *)first_info->ai_addr)->sin_addr; // node

    int descr = socket(PF_INET, SOCK_STREAM, 0);
    int r = connect(descr, (struct sockaddr *)&adress_sock,
                    sizeof(struct sockaddr_in));

    char askList[6] = "LIST\r\n";

    if (r != -1)
    {
        char buff[MAX_DATA + 1];
        int size_rec;

        //send text
        if (send(descr, askList, 6, 0) < 0)
        {
            close(descr);
            perror("send error");
            return -1;
        }

        //get answers
        if ((size_rec = recv(descr, buff, MAX_DATA * sizeof(char), 0)) < 0)
        {
            close(descr);
            perror("recv error");
            return -1;
        }
        buff[size_rec] = '\0';
        if (checkLINB(buff))
        {
            printf("%s\n", buff);
            if (strncmp(buff + 5, "00", 2) == 0)
            {
                printf("%s\n", "no diffuser");
                return 0;
            }
            else
            { //get all diffuser

                //get the number of diffuser
                char *tmp = malloc(3 * sizeof(char));
                strncpy(tmp, buff + 5, 2);
                tmp[2] = '\0';
                int num_diff = atoi(tmp);
                printf("num_diff: %d\r\n", num_diff);
                free(tmp);

                //get the list of diffusers ([ITEM␣id␣ip1␣port1␣ip2␣port2]])
                char list_diff[num_diff][57];
                for (int i = 0; i < num_diff; i++)
                {
                    if ((size_rec = recv(descr, buff, MAX_DATA * sizeof(char), 0)) < 0)
                    {
                        close(descr);
                        perror("recv error");
                        return -1;
                    }
                    buff[size_rec] = '\0';
                    if (checkITEM(buff))
                    {
                        strcpy(list_diff[i], buff);
                        printf("%s", buff);
                    }
                    else
                    {
                        printf("%s\n", "error manager ITEM");
                        return 0;
                    }
                }

                // choose a random diffuser
                srand(time(NULL));               // Initialization, should only be called once.
                int aRandom = rand() % num_diff; //TODO: change with the size of the array
                printf("n'eme diffuseur selectionné :%d\n\n", aRandom + 1);
                struct diffus *aDiffuser = itemToDiffus(list_diff[aRandom]);

                // start threads

                // thread used to receive multi diffusion
                pthread_t th;
                int r2 = pthread_create(&th, NULL, recv_multi, (void *)aDiffuser);
                if (r2 != 0)
                {
                    printf("error : creation thread recv_multi");
                    exit(0);
                }

                // thread used to receive send TCP to diffuser
                pthread_t th2;
                int r3 = pthread_create(&th2, NULL, sendToDiffus, (void *)aDiffuser);
                if (r3 != 0)
                {
                    printf("error : creation thread sendToDiffus");
                    exit(0);
                }

                // no return while
                while (1);
            }
        }
        else
        {
            printf("%s\n", "error manager LINB");
        }
        close(descr);

        return 0;
    } else
        printf("Failed to Connect to Server (Check Port)\n");

}
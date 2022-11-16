#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "formatter.h"
#include "parser.h"

/**
 * Checks if the string is a valid message for OLDM
 * [OLDM␣num-mess␣id␣message] 161 bytes
 */
int checkOLDM(char *str)
{
    // Check spaces
    if (str == NULL || strlen(str) != 161 || str[9] != ' ' || str[18] != ' ')
        return 0;
    // Check keywords
    if (memcmp(str, "OLDM ", 5) || memcmp(str + 159, "\r\n", 2))
        return 0;
    char num[5];
    num[4] = '\0';
    memcpy(num, str + 5, 4);
    char *num_test = unFormatNumber(num);
    if (num_test == NULL)
        return 0;
    free(num_test);
    return 1;
}

/**
 * Checks if the string is a valid message for LINB
 * [LINB␣num-diff] 9 bytes
 */
int checkLINB(char *str)
{
    // Check spaces
    if (str == NULL || strlen(str) != 9)
        return 0;
    // Check keywords
    if (memcmp(str, "LINB ", 5) || memcmp(str + 7, "\r\n", 2))
        return 0;
    char num[3];
    num[2] = '\0';
    memcpy(num, str + 5, 2);
    char *num_test = unFormatNumber(num);
    if (num_test == NULL)
        return 0;
    free(num_test);
    return 1;
}

/**
 * Checks if the string is a valid message for ITEM
 * [ITEM␣id␣ip1␣port1␣ip2␣port2] 57 bytes
 */
int checkITEM(char *str)
{
    // Check spaces
    if (str == NULL || strlen(str) != 57 || str[13] != ' ' || str[29] != ' ' ||
        str[34] != ' ' || str[50] != ' ')
        return 0;
    // Check keywords
    if (memcmp(str, "ITEM ", 5) || memcmp(str + 55, "\r\n", 2))
        return 0;
    // Get IPs and Ports
    char ip1[16];
    ip1[15] = '\0';
    memcpy(ip1, str + 14, 15);
    char port1[5];
    port1[4] = '\0';
    memcpy(port1, str + 30, 4);
    char ip2[16];
    ip2[15] = '\0';
    memcpy(ip2, str + 35, 15);
    char port2[5];
    port2[4] = '\0';
    memcpy(port2, str + 51, 4);

    // Check IPs
    if (!isValidIPv4Format(ip1) || !isValidIPv4Format(ip2))
        return 0;
    // Check Ports
    char *port1_test = unFormatNumber(port1);
    if (port1_test == NULL)
        return 0;
    free(port1_test);
    char *port2_test = unFormatNumber(port2);
    if (port2_test == NULL)
        return 0;
    free(port2_test);

    return 1;
}

/**
 * Checks if the string is a valid message for DIFF
 * [DIFF␣num-mess␣id␣message] 161 bytes
 */
int checkDIFF(char *str)
{
    // Check spaces
    if (str == NULL || strlen(str) != 161 || str[9] != ' ' || str[18] != ' ')
        return 0;
    // Check keywords
    if (memcmp(str, "DIFF ", 5) || memcmp(str + 159, "\r\n", 2))
        return 0;
    char num[5];
    num[4] = '\0';
    memcpy(num, str + 5, 4);
    char *num_test = unFormatNumber(num);
    if (num_test == NULL)
        return 0;
    free(num_test);
    return 1;
}

/**
 * Test purposes (Simple tests functions above)
 * Uncomment the main to test
 * gcc -g3 -Wall parser.c formatter.c formatter.h
 * ./a.out
 */
/*
int main(int argc, char **argv)
{
    // Init Messages Values
    char mess[141];
    mess[140] = '\0';
    memset(mess, 'a', 140);

    char oldmOK[162] = "OLDM 0012 RADIO### ";
    strcat(oldmOK, mess);
    strcat(oldmOK, "\r\n\0");
    char linbOK[10] = "LINB 02\r\n\0";
    char itemOK[58] = "ITEM RADIO### 127.000.000.001 5551 225.001.002.004 5552\r\n\0";
    char diffOK[162] = "DIFF 0012 RADIO### ";
    strcat(diffOK, mess);
    strcat(diffOK, "\r\n\0");

    printf("Test OLDM OK %d\n", checkOLDM(oldmOK));
    printf("Test LINB OK %d\n", checkLINB(linbOK));
    printf("Test ITEM OK %d\n", checkITEM(itemOK));
    printf("Test DIFF OK %d\n", checkDIFF(diffOK));

    char *oldmNO = "OLDM OLDN 0012 RADIO### aaaa\r\n\0"; // Missing big part of mess
    char *linbNO = "LINB 02 \n\0"; // Missing \r
    char *itemNO = "ITEM RRADIO### 127.000.000.001 5551 225.001.002.004 5552\r\n\0"; // Extra char
    char diffNO[162] = "DIFF 0012 RADIO### ";
    strcat(diffNO, mess);
    strcat(diffNO, "\r\0"); // Missing \n

    printf("\nTest OLDM NO %d\n", checkOLDM(oldmNO));
    printf("Test LINB NO %d\n", checkLINB(linbNO));
    printf("Test ITEM NO %d\n", checkITEM(itemNO));
    printf("Test DIFF NO %d\n", checkDIFF(diffNO));
}
 */
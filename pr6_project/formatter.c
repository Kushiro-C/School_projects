#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "formatter.h"

/**
 * Function returning : x power y
 */
int myPow(int x, int y)
{
    int r, i;
    r = 1;
    for (i = 0; i < y; i++)
        r *= x;
    return r;
}

/**
 * Checks if the given string is a valid IPv4 address
 * Returns 1 for true, 0 for false, -1 if IP is NULL
 */
int isValidIPv4Format(char *ip)
{
    if (ip == NULL)
        return 0;
    int d = 0;    // flag for dot
    int dot = 0;  // Counter dots
    int part = 0; // Number of digits per part need : {1,3}
    int len = strlen(ip);
    int i;
    for (i = 0; i < len; i++) {
        if (isdigit(ip[i])) { // Digit
            part++;
            d = 0;
        }
        else if (ip[i] == '.') { // Dot
            if (d) // Consecutive dots
                return 0;
            d = 1;
            dot++;
        }
        else // Not valid in our IPv4 Addresses
            return 0;

        if (d || i == len - 1) { // Need {1,3} digits
            if (part < 1 || part > 3)
                return 0;
            part = 0;
        }
    }
    return dot == 3;
}

/**
 * Formats a message into a given fixed size
 * by adding '#' at the end of the message until
 * the new message has exactly 'size' characters,
 * or cuts the message to 140 chars, if longer than size.
 * For an id (size = 8), "RADIO" becomes "RADIO###"
 *
 * returns NULL if msg is NULL or size < 0
 */
char *formatMessage(char *msg, int size)
{
    if (msg == NULL || size < 0)
        return NULL;
    char *newMsg = malloc(sizeof(char) * (size + 1));
    memcpy(newMsg, msg, size);
    int i;
    for (i = strlen(msg); i < 140; i++)
        newMsg[i] = '#';
    newMsg[size] = '\0';
    return newMsg;
}

/**
 * Formats a number into a fixed size
 * by adding '0' at the beginning until
 * the new number has exactly 'size' characters
 * For num-mess (size = 4), 120 becomes "0120"
 *
 * returns NULL if maxDigits <= 0 or nb < 0
 * or if nb has more digits than maxDigits
 */
char *formatNumber(int nb, int maxDigits)
{
    if (nb < 0 || maxDigits <= 0 || nb >= myPow(10, maxDigits))
        return NULL;
    char *newNb = malloc(sizeof(char) * (maxDigits + 1));
    char strNb[maxDigits];
    sprintf(strNb, "%d", nb);
    int i;
    int n = maxDigits;
    newNb[n--] = '\0';
    for (i = strlen(strNb) - 1; i >= 0; i--)
        newNb[n--] = strNb[i];
    for (; n >= 0; n--)
        newNb[n] = '0';
    return newNb;
}

/**
 * Formats an IPv4 Address into a fixed size of 15 chars
 * by adding '0' at the beginning of each part
 * "127.0.0.1" becomes "127.000.000.001"
 *
 * returns NULL if the IPv4 Address is not valid
 */
char *formatIPv4(char *ip)
{
    if (!isValidIPv4Format(ip))
        return NULL;
    char *newIp = malloc(sizeof(char) * 16);
    int i, j;
    int k = 14;
    int addZeros = 3;
    int len = strlen(ip);
    for (i = len - 1; i >= 0; i--) {
        if (ip[i] == '.' || i == 0) {
            if (ip[i] != '.') // Digit
                addZeros--;
            for (j = 0; j < addZeros; j++)
                newIp[k--] = '0';
            addZeros = 3;
            newIp[k--] = ip[i];
        }
        else { // Digit
            newIp[k--] = ip[i];
            addZeros--;
        }
    }
    newIp[15] = '\0';
    return newIp;
}

/**
 * Unformats a message by removing the extras '#' at the end
 * The msg's length must equal size, returns NULL otherwise
 */
char *unFormatMessage(char *msg, int size)
{
    if (msg == NULL || strlen(msg) != size)
        return NULL;
    int idx = size - 1;
    while (msg[idx] == '#')
        idx--;
    char *newMsg = malloc(sizeof(char) * (idx + 2));
    memcpy(newMsg, msg, idx+1);
    newMsg[idx+1] = '\0';
    return newMsg;
}

/**
 * Unformats a message by removing the extras '#' at the end
 */
char *unFormatMessage2(char *msg)
{
    int idx = strlen(msg) - 1;
    while (msg[idx] == '#' || msg[idx] == '\r' || msg[idx] == '\n')
        idx--;
    char *newMsg = malloc(sizeof(char) * (idx + 2));
    memcpy(newMsg, msg, idx+1);
    newMsg[idx+1] = '\0';
    return newMsg;
}

/**
 * Unformats a number by removing the extras '0' at the beginning
 * Unlike in the Java Formatter, this function returns a char*
 */
char *unFormatNumber(char *nb)
{
    if (nb == NULL)
        return NULL;
    int idx = 0;
    while(nb[idx] == '0')
        idx++;
    int len = strlen(nb) - idx;
    if (len == 0) { // Case only zero(s)
        len++;
        idx--;
    }
    char *newNb = malloc(sizeof(char) * (len + 1));
    memcpy(newNb, nb+idx, len);
    newNb[len] = '\0';
    return newNb;
}

/**
 * Unformats an IPv4 address, by compressing the extras '0' when necessary
 * returns NULL
 */
char *unFormatIPv4(char *ip)
{
    if (!isValidIPv4Format(ip) || strlen(ip) != 15)
        return NULL;
    int i, j;
    int k = 0; // Index for ip
    int idx = 0; // Index for tmp
    char tmp[15];
    for (i = 0; i < 4; i++) { // 4 parts of an IPv4 Address
        j = 0;
        while (j < 3 && ip[k] == '0') { // Skip zeros
            j++;
            k++;
        }
        if (j == 3) { // Case three zeros ("000")
            j--;
            k--;
        }
        while (j < 3) { // Add numbers
            tmp[idx++] = ip[k++];
            j++;
        }
        if (i != 3) {
            tmp[idx++] = '.';
            k++;
        }
    }
    char *newIp = malloc(sizeof(char) * (idx + 1));
    memcpy(newIp, tmp, idx);
    newIp[idx] = '\0';
    return newIp;
}


/**
 * Test purposes (Simple tests functions above)
 * Uncomment the main to test
 */
/*
int main(int argc, char **argv)
{
    printf("Format Functions :\n");
    printf("Message : [%s]\n", formatMessage("RADIO", 8)); // RADIO###
    printf("Message : [%s]\n", formatMessage("RADIO", 5)); // RADIO
    printf("Message : [%s]\n", formatMessage("RADIO", 4)); // RADI
    printf("Message : [%s]\n\n", formatMessage("RADIO", -1)); // (null)

    printf("Number : [%s]\n", formatNumber(1, 3)); // 001
    printf("Number : [%s]\n", formatNumber(1234, 4)); // 5555
    printf("Number : [%s]\n", formatNumber(1, 0)); // (null)
    printf("Number : [%s]\n\n", formatNumber(5555, 3)); // (null)

    printf("isValidIPv4 \"127.0.0.1\" : %d\n", isValidIPv4Format("127.0.0.1")); // 1
    printf("isValidIPv4 \"255.255.255.255\" : %d\n", isValidIPv4Format("255.255.255.255")); // 1
    printf("isValidIPv4 \"127.0..00\" : %d\n", isValidIPv4Format("127.0..00")); // 0
    printf("isValidIPv4 \"127.0.00\" : %d\n", isValidIPv4Format("127.0.00")); // 0
    printf("isValidIPv4 \"127.0.0000.1\" : %d\n\n", isValidIPv4Format("127.0.0000.1")); // 0

    printf("IPv4 : [%s]\n", formatIPv4("127.0.0.1")); // 127.000.000.001
    printf("IPv4 : [%s]\n", formatIPv4("255.255.255.255")); // Same
    printf("IPv4 : [%s]\n", formatIPv4("127.0.0.0001")); // (null)
    printf("IPv4 : [%s]\n", formatIPv4("127.0..01")); // (null)
    printf("IPv4 : [%s]\n\n", formatIPv4("127.0.01")); // (null)

    printf("UnFormat Functions :\n");
    printf("Message : [%s]\n", unFormatMessage("RADIO###", 8)); // RADIO
    printf("Message : [%s]\n", unFormatMessage("RADIO", 5)); // RADIO
    printf("Message : [%s]\n", unFormatMessage("RADIO", 4)); // (null)
    printf("Message : [%s]\n\n", unFormatMessage("RADIO", 6)); // (null)

    printf("Number : [%s]\n", unFormatNumber("0012")); // 12
    printf("Number : [%s]\n", unFormatNumber("1204")); // 1204
    printf("Number : [%s]\n", unFormatNumber("000")); // 0
    printf("Number : [%s]\n\n", unFormatNumber("10")); // 10

    printf("IPv4 : [%s]\n", unFormatIPv4("127.000.000.001")); // 127.000.000.001
    printf("IPv4 : [%s]\n", unFormatIPv4("255.255.255.255")); // Same
    printf("IPv4 : [%s]\n", unFormatIPv4("001.102.030.000")); // 1.102.30.0
    printf("IPv4 : [%s]\n", unFormatIPv4("000.000.000.000")); // 0.0.0.0
    printf("IPv4 : [%s]\n", unFormatIPv4(NULL)); // (null)
    printf("IPv4 : [%s]\n", unFormatIPv4("127.0.0.1")); // (null)
    printf("IPv4 : [%s]\n", unFormatIPv4("127.0.0.0001")); // (null)
    printf("IPv4 : [%s]\n", unFormatIPv4("127.0..01")); // (null)
    printf("IPv4 : [%s]\n\n", unFormatIPv4("127.0.01")); // (null)
}
*/
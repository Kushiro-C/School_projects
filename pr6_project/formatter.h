#ifndef FORMATTER_H
#define FORMATTER_H

extern int isValidIPv4Format(char *ip);
extern char *formatMessage(char *msg, int size);
extern char *formatNumber(int nb, int maxDigits);
extern char *formatIPv4(char *ip);
extern char *unFormatMessage(char *msg, int size);
extern char *unFormatMessage2(char *msg);
extern char *unFormatNumber(char *nb);
extern char *unFormatIPv4(char *ip);

#endif
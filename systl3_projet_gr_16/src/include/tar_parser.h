#ifndef TAR_PARSER_H
#define TAR_PARSER_H

#include "tar.h"
#include <grp.h>
#include <pwd.h>
#include "string_parser.h"
#include <time.h>

extern void skip_contents(int fd, struct header head);
extern struct header nameToHeader(int fd, char* name);
extern int remove_tar(int fd, char *tar);
extern int remove_first_headerAndContents(int fd);
extern int writeOnStdout(int fd, char* s);
extern int writeNOnStdout(int fd, char* s, int n);
extern int write_contents2(int fd, int fd2, char *s, int re);
extern void set_checksum(struct header *hd);
extern int check_checksum(struct header *hd);
extern void unlink_header(int fd, char* s);
extern int is_empty_directory(int fd, struct header head);
extern int exists_header(int fd, char *name);
extern char* getpath(char* path_and_name);
extern int is_valid_path(int fd, char* path, char* path_and_name);
extern int is_valid_path2(int fd, char* path_and_name);
extern off_t AfterLastHeadercontents(int fd);
extern struct header copy_header (struct header source, char* new_name);
extern void change_link(int fd, char* s, char* to);
extern int remove_file_2(int fd, char*name);
extern void rewriteall(int fd);
extern off_t add_header_in_tar(int fd, struct header new_Header);
extern int create_all_directory(int fd, char* source, char* dest);
extern int add_char_in_tar(int fd, char contents[BLOCKSIZE]);
extern struct header create_header (struct stat st , char* new_name);
extern int add_file_in_tar(int fd, char* source, struct header new_header);
extern int isValidOutPath(char* path);
extern int convertDecimalToOctal(int decimalNumber);
extern int countDigits( int value );

#endif
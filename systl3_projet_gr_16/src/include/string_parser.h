#ifndef STRING_PARSER_H
#define STRING_PARSER_H

extern void free_arr(char **arr, int size);
extern int contains_char_only(char *str, char c);
extern char *remove_duplicate_sep(char *pathname, char separator);
extern int get_path_length(char *pathname, char separator);
extern char **str_to_arr(char *str, char separator);
extern int has_tar_in_path(char *pathname);
extern char *get_path_before_tar(char *pathname, char separator);
extern char *get_tar_only(char *pathname, char separator);
extern char *get_path_until_tar(char *pathname);
extern char *get_path_after_tar(char *pathname);
extern char *get_last_of_path(char *pathname, char separator);
extern char *add_slash_at_the_end(char *pathname);
extern char *get_first_after_path(char *pathname, char *pathname2, char separator);
extern char* removeAllOcAtBeginning(char* s, char c);
extern char* convertUnixTime(char mtime[12]);
extern char *clearDots(char *path);
extern void write_cd_err(char *name);
extern int occ_in_str(char *str, char c);
extern int strFirstOccOfChar(char *str, char c);
extern int isValidRedirection(char *cmd);

#endif
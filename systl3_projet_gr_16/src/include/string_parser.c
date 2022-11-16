#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "string_parser.h"

/**
 * Frees an str array
 */
void free_arr(char **arr, int size) {
    for (int i = 0; i < size; i++)
        if (arr[i] != NULL)
            free(arr[i]);
    if (arr != NULL)
        free(arr);
}

/**
 * Checks if a string str contains
 * only the char c parameter
 * Returns 1 if it's the case
 * Else returns 0
 */
int contains_char_only(char *str, char c) {
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        if (str[i] != c)
            return 0;
    }
    return 1;
}

/**
 * Removes the duplicate separators
 * Also removes separators at the beginning and at the end of pathname
 */
char *remove_duplicate_sep(char *pathname, char separator) {
    int i = 0;
    int n = strlen(pathname);
    int length = 0; // We will realloc to the actual size later
    char *new_path;

    if (n == 0)
        return pathname;

    // Error malloc
    if ((new_path = malloc(sizeof(char) * (1 + n))) == NULL) {
        free(new_path);
        return NULL;
    }

    // Remove separators from beginning
    while (i < n && pathname[i] == separator) {
        i++;
    }

    // Remove separators from end
    while (n - 1 > i && pathname[n-1] == separator) {
        n--;
    }

    // Removing next duplicate separators
    for (; i < n; i++) {
        new_path[length++] = pathname[i];
        if (pathname[i] == separator) {
            i++;

            while (i < n && pathname[i] == separator)
                i++;

            if (i < n) // We know it's not a separator
                i--;
        }
    }

    // Realloc correct size
    char *tmp;
    if ((tmp = realloc(new_path, length+1)) != NULL) {
        new_path = tmp;
    } else {
        free(new_path);
        return NULL;
    }

    new_path[length] = '\0';

    return new_path;
}

/**
 * Return length of the path
 * Can also be used with different separators
 * Such as ' ' (space) or '/'
 * Use this before str_to_arr() to get its size
 */
int get_path_length(char *pathname, char separator) {
    if (pathname == NULL || contains_char_only(pathname, separator))
        return 0;

    int i;
    int cpt = 1;
    int n = strlen(pathname);
    int n2;
    char *new_pathname;

    if (n == 0)
        return 0;

    // Also removes the 'separators' at the beginning and ending
    new_pathname = remove_duplicate_sep(pathname, separator);
    n2 = strlen(new_pathname);

    // Counting
    for (i = 0; i < n2; i++) {
        if (new_pathname[i] == separator)
            cpt++;
    }

    free(new_pathname);

    return cpt;
}

/**
 * Transform a char* into a char **
 * Separating each char* with 'separator'
 * Use get_path_length() before to get the array's length
 */
char **str_to_arr(char *str, char separator) {
    if (str == NULL || strlen(str) < 1 || contains_char_only(str, separator))
        return NULL;

    char *new_str;
    int new_len;
    int i;
    int idx = 0;
    int cpt = 0;
    int n;
    char **arr_path;

    new_str = remove_duplicate_sep(str, separator);
    if (new_str == NULL) {
        return NULL;
    }
    new_len = strlen(new_str);
    n = get_path_length(new_str, separator);

    arr_path = malloc(sizeof(char *) * n);
    if (arr_path == NULL) {
        free(new_str);
        return NULL;
    }

    // Parse the str and add into new str array
    for (i = 0; i < new_len; i++) {
        // Separate all strings
        if (new_str[i] == separator || i == new_len - 1) {
            if (i == new_len - 1) {
                i++;
                cpt++;
            }

            arr_path[idx] = malloc(sizeof(char) * (1 + cpt));

            if (arr_path[idx] == NULL) {
                free_arr(arr_path, i + 1);
                return NULL;
            }

            // Copy the str into the next index of the new array
            memcpy(arr_path[idx], &new_str[i - cpt], cpt);
            memset(arr_path[idx] + cpt, '\0', 1);

            // Reset counters
            cpt = 0;
            idx++;

        } else {
            cpt++; // Length of the string
        }
    }

    free(new_str);

    return arr_path;
}

/**
 * Seeks for a file.tar in the given pathname
 * We suppose that the pathname is properly written
 * Return 1 if '.tar' was found, else 0
 */
int has_tar_in_path(char *pathname) {
    int i;
    int n = strlen(pathname);

    if (pathname == NULL || n < 5) // length a.tar = 5 (= minimum)
        return 0;

    for (i = 0; i < n; i++) {
        if (pathname[i] == '.') {
            if (i + 3 > n) { // end of string after '.'
                return 0;

            } else { // look if there is "tar" after '.'
                if (pathname[i + 1] == 't' && pathname[i + 2] == 'a'
                        && pathname[i + 3] == 'r')
                    return 1;

                else
                    i += 3;
            }
        }
    }

    return 0;
}

/**
 * Returns a char* containing the path before the .tar
 * The .tar and what's after is not included
 * Supposing there is a .tar in path
 */
char *get_path_before_tar(char *pathname, char separator) {
    char **arr_path = str_to_arr(pathname, separator);
    int arr_size = get_path_length(pathname, separator);
    char *new_path;
    int n;
    int i;
    int new_path_len = 0;

    for (i = 0; i < arr_size; i++) {
        n = strlen(arr_path[i]);

        // Compare if the name has suffix .tar
        if (n > 4 && !strncmp(&arr_path[i][n - 4], ".tar", 4)) {
            new_path = malloc(sizeof(char) * new_path_len + 1);

            // Malloc error
            if (new_path == NULL) {
                free(new_path);
                return NULL;
            }

            // Copy path
            memcpy(new_path, pathname, new_path_len);
            new_path[new_path_len] = '\0';

            i = arr_size; // Leave 'for' loop

        } else {
            new_path_len += n + 1; // '/' + name
        }
    }

    free_arr(arr_path, arr_size);

    return new_path;
}

/**
 * Returns from a valid path only the name
 * of the .tar file
 * We suppose there is a .tar file
 */
char *get_tar_only(char *pathname, char separator) {
    char **arr_path = str_to_arr(pathname, separator);
    int arr_size = get_path_length(pathname, separator);
    char *tar_name;
    int n;
    int i;

    for (i = 0; i < arr_size; i++) {
        n = strlen(arr_path[i]);

        // Compare if the name has suffix .tar
        if (n > 4 && !strncmp(&arr_path[i][n - 4], ".tar", 4)) {
            tar_name = malloc(sizeof(char) * (n + 1));

            // Malloc error
            if (tar_name == NULL) {
                free_arr(arr_path, arr_size);
                free(tar_name);
                return NULL;
            }

            // Copy name
            memcpy(tar_name, arr_path[i], n);
            tar_name[n] = '\0';

            i = arr_size; // Leave 'for' loop
        }
    }

    free_arr(arr_path, arr_size);

    return tar_name;
}

/**
 * From  path/tarball.tar/path2
 * returns path/tarball.tar
 */
char *get_path_until_tar(char *pathname) {
    char *new_path;
    int i;
    int len;

    if (pathname == NULL || !has_tar_in_path(pathname))
        return NULL;

    len = strlen(pathname);

    // Find the length of the path until tar
    for(i = 0; i < len; i++) {
        if (pathname[i] == '.') {
            // Compare suffix .tar nothing after or separator
            if (i + 3 < len && !strncmp(&pathname[i], ".tar", 4)) {
                i += 4;
                break;
            }
        }
    }

    new_path = malloc(sizeof(char) * (i + 1));
    if (new_path == NULL)
        return NULL;

    // Copy
    strncpy(new_path, pathname, i);
    new_path[i] = '\0';

    return new_path;
}

/**
 * Returns a char* containing the path after the .tar
 * The .tar and what's before is not included
 * Supposing there is a .tar in path
 */
char *get_path_after_tar(char *pathname) {
    char *new_path;
    int len;
    char *path_til_tar;
    int len2;

    if (pathname == NULL || !has_tar_in_path(pathname))
        return NULL;

    path_til_tar = get_path_until_tar(pathname);
    if (path_til_tar == NULL)
        return NULL;

    len = strlen(pathname);
    len2 = strlen(path_til_tar);

    if (len <= len2) { // Nothing after the tarball
        free(path_til_tar);
        return NULL;
    }

    new_path = malloc(sizeof(char) * (len - len2));
    if (new_path == NULL) {
        free(path_til_tar);
        return NULL;
    }

    // Get path after the tarball
    strcpy(new_path, pathname + len2 + 1);

    free(path_til_tar);
    return new_path;
}

/**
 * Returns the last name in the pathname
 */
char *get_last_of_path(char *pathname, char separator) {
    char **arr_path = str_to_arr(pathname, separator);
    int arr_size = get_path_length(pathname, separator);
    int n;

    if (arr_path == NULL)
        return NULL;

    n = strlen(arr_path[arr_size - 1]); // Length of last index
    char *last_of_path = malloc(sizeof(char) * (n + 1));

    // Malloc Error
    if (last_of_path == NULL) {
        free_arr(arr_path, arr_size);
        return NULL;
    }

    // Copy name
    memcpy(last_of_path, arr_path[arr_size - 1], n);
    last_of_path[n] = '\0';

    free_arr(arr_path, arr_size);

    return last_of_path;
}

/**
 * Returns the frist name in the pathname2 after pathname only if pathname2 has (length of pathname) +1
 * char *path1 = "/home.t/chang/Diderot/L3.tar/SY5";
 * char *path2 = "/home.t/chang/Diderot/L3.tar/SY5/popito";
 * return popito
 */
char *get_first_after_path(char *pathname, char *pathname2, char separator) {
    if(strlen(pathname2) > strlen(pathname)) {
        if(!strncmp(pathname2, pathname, strlen(pathname)))
            if(get_path_length(pathname, separator) +1 == get_path_length(pathname2, separator))
                return get_last_of_path(pathname2, separator);
    }
    return NULL;
}

/**
 * Add a '/' at the end of the pathname,
 * return the new char*
 */
char *add_slash_at_the_end(char *pathname) {
    if (pathname == NULL)
        return NULL;

    char *new_name;
    int len = strlen(pathname);
    int i = 1;

    // Already has '/'
    if (pathname[len - 1] == '/')
        i = 0;

    new_name = malloc(sizeof(char) * (len + 1 + i));
    if (new_name == NULL) {
        perror("Malloc");
        return NULL;
    }

    memcpy(new_name, pathname, len);
    if (i == 1)
        new_name[len] = '/';
    new_name[len + i] = '\0';

    return new_name;
}

/**
 * return the directory path where we create the new directory named name
 */
char* getpath(char* path_and_name){
    char* name;
    int size;
    char* path;
    name = get_last_of_path(path_and_name, '/');
    size = (strlen(path_and_name)-strlen(name));
    path = malloc(sizeof(char) * (size+1));

    if (name == NULL)
        exit(EXIT_FAILURE);

    if (path == NULL) {
        free(name);
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    memset(path, '\0', size+1);
    memcpy(path, path_and_name, size);
    path[strlen(path)-1] = '\0';

    free(name);

    return path;
}

/**
 * Returns new char without all char c at the begining 
 * s = 00000015107
 * return 15107
 */
char* removeAllOcAtBeginning(char* s, char c) { //FIXME: not remove but change to a space' ' 
    int i;
    for(i=0; i<strlen(s); i++)
        if(s[i] != c)
            break;
    char *newS = malloc(sizeof(char) * (strlen(s) - i));
    if(newS == NULL) {
        free(newS);
        return NULL;
    }
    strcpy(newS, &s[i]);
    return newS;
}

/**
 * Returns unixtime to normal time
 */

char* convertUnixTime(char mtime[12]) {
    int decimalTime; //decimal time -> int = 16 bits
    sscanf(mtime, "%o", &decimalTime); // Convert Octal to Decimal (char = 8 bits)
    char *s = malloc(sizeof(char) * 17); //date in char*
    if(s == NULL) {
        free(s);
        return NULL;
    }
    long t = (long) decimalTime; //mtime as time_t 
    struct tm my_tm;
    //sscanf(mtime, "%ld", &t);
    my_tm = *localtime(&t);
    strftime(s, sizeof(char) * 17, "%Y-%m-%d %H:%M", &my_tm);
    return s;
}

/**
 * Remove '.' and '..'
 * from an absolute path
 */
char *clearDots(char *path) {
    if (path == NULL)
        return NULL;

    int nb_ref; // Number of 'files' in the path
    char **names; // files in an array

    char *new_path; // to return
    int new_len;


    nb_ref = get_path_length(path, '/');
    names = str_to_arr(path, '/');
    int tab[nb_ref];
    int i, j;

    if (names == NULL)
        return NULL;

    // filter by following "." and ".."
    for (i = 0; i < nb_ref; i++) {
        if (!strcmp(names[i], ".")) // remove "."
            tab[i] = 0;
        else if (!strcmp(names[i], "..")) { // remove previous one
            tab[i] = 0;
            for (j = i; j >= 0; j--) {
                if (tab[j] != 0) {
                    tab[j] = 0;
                    break;
                }
            }
        }
        else
            tab[i] = strlen(names[i]);
    }

    new_len = 0;
    for (i = 0; i < nb_ref; i++)
        if (tab[i] != 0)
            new_len += tab[i] + 1; // 1 for the '/'

    if (new_len == 0) // If empty, we return "/"
        new_len++;

    new_path = malloc(sizeof(char) * (1 + new_len));
    if (new_path == NULL) {
        free_arr(names, nb_ref);
        return NULL;
    }

    if (new_len == 1) { // return "/"
        new_path[0] = '/';
        new_path[1] = '\0';
        free_arr(names, nb_ref);
        return new_path;
    }

    // Copy to new path
    j = 0;
    for (i = 0; i < nb_ref; i++) {
        if (tab[i] != 0) {
            new_path[j++] = '/';
            strcpy(new_path + j, names[i]);
            j += tab[i];
        }
    }
    new_path[j] = '\0';

    free_arr(names, nb_ref);
    return new_path;
}

/**
 * cd error display
 */
void write_cd_err(char *name) {
    write(STDERR_FILENO, "cd: no such directory: ", 23);
    write(STDERR_FILENO, name, strlen(name));
    write(STDERR_FILENO, "\n\0", 2);
}

/**
 * Gives the number of occurences
 * of a char in a string
 */
int occ_in_str(char *str, char c) {
    int occ;
    int len;

    if (str == NULL)
        return -1;

    occ = 0;
    len = strlen(str);

    for (int i = 0; i < len; i++)
        if (str[i] == c)
            occ++;

    return occ;
}

/**
 * Returns the index of the first occurence of a char c in a string str
 */
int strFirstOccOfChar(char *str, char c) {
    int len;

    if (str == NULL)
        return -1;

    len = strlen(str);
    for (int i = 0; i < len; i++)
        if (str[i] == c)
            return i;

    return -1;
}

/**
 * Checks if the command is valid for a redirection
 * We say it is valid if we only have one redirection
 * to a file ('<' OR '>'), only one file associated
 * and if the command cmd is written properly,
 * for instance :
 * 
 * cmd < path_to_file // valid
 * cmd > path_to_file // valid
 * cmd >              // not valid
 * cmd < file > file2 // not valid
 * < path_to_file     // not valid
 * cmd < file1 file2  // not valid
 */
int isValidRedirection(char *cmd) {
    if (cmd == NULL)
        return -1;

    int occ_l = occ_in_str(cmd, '<'); // number occurences '<'
    int occ_r = occ_in_str(cmd, '>'); // number occurences '>'
    int idx; // index of '<' or '>'
    char **files;
    int nbFiles;

    if (occ_l + occ_r != 1) // Only one redirection is allowed
        return 0;

    idx = occ_l ? strFirstOccOfChar(cmd, '<') : strFirstOccOfChar(cmd, '>');

    if (idx <= 0 || idx == strlen(cmd) - 1) // cmd starts with operator,
        return 0; // or ends with operator

    else { // we have something after the operator
        // Check after operator, that we have only one file
        for (int i = 0; i < idx; i++) {
            // We don't have only spaces before the operator
            if (cmd[i] != ' ') {
                if ((files = str_to_arr(cmd + idx + 1, ' ')) == NULL)
                    return 0;

                // Get number of files after the operator
                nbFiles = get_path_length(cmd + idx + 1, ' ');
                free_arr(files, nbFiles);

                // Only one file allowed
                return nbFiles == 1 ? !contains_char_only(cmd + idx + 1, ' ') : 0;
            }
        }
        return 0;
    }
}
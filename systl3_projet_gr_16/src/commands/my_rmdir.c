#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>

#include "../include/tar_parser.h"

/**
 * remove first header, work only if it has no content
 */
int remove_first_header(int fd, char **argv) {
    struct header head2;
    int re;
    int size; // File size
    int n;
    off_t current;

    current = lseek(fd, BLOCKSIZE, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("lseek error seek_cur");
        exit(EXIT_FAILURE);
    }

    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {

        // GO back before the 2 headers
        current = lseek(fd, -BLOCKSIZE*2, SEEK_CUR);
        if (current < 0) {
            close(fd);
            perror("remove_first_header lseek error seek_cur");
            exit(EXIT_FAILURE);
        }

        // overwriting the new header at before header place
        if (write(fd, &head2, BLOCKSIZE) < 0) {
            close(fd);
            perror("remove_first_header write error");
            exit(EXIT_FAILURE);
        }

        current = lseek(fd, BLOCKSIZE, SEEK_CUR);
        if (current < 0) {
            close(fd);
            perror("remove_first_header lseek error seek_cur");
            exit(EXIT_FAILURE);
        }

        // write contents
        sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
        n = (size + 512 - 1) / 512; // size to skip to next header
        int re2;
        for (int i = 0; i < n; i++) {
            if ((re2 = read(fd, &head2, BLOCKSIZE)) < 0) { // overwrite a file
                close(fd);
                perror("Bad read file content");
                exit(EXIT_FAILURE);
            }

            current = lseek(fd, -BLOCKSIZE + re2, SEEK_CUR);
            if (current < 0) {
                close(fd);
                perror("lseek error seek_cur");
                exit(EXIT_FAILURE);
            }

            if ((write(fd, &head2, BLOCKSIZE)) < 0) { // overwrite a file
                close(fd);
                perror("Bad read file content");
                exit(EXIT_FAILURE);
            }

            current = lseek(fd, BLOCKSIZE, SEEK_CUR);
            if (current < 0) {
                close(fd);
                perror("lseek error seek_cur");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (re < 0) {
        close(fd);
        perror("remove_first_header read error");
        exit(EXIT_FAILURE);
    }

    return 1;
}

/**
 * remove empty directory
 */
int remove_empty_directory(char ** argv) {
    char* tar = get_path_until_tar(argv[1]);
    char* name = get_path_after_tar(argv[1]);

    // add a slash
    char name_and_slash[strlen(name) + 1];
    strcpy(name_and_slash, name);
    name_and_slash[strlen(name)] = '/';
    name_and_slash[strlen(name) +1] = '\0';

    int fd = open(tar, O_RDWR);
    if (fd < 0) {
        close(fd);
        write(STDERR_FILENO, "rmdir: No such file or directory\n\0", 34);
        exit(EXIT_FAILURE);
    }

    struct header head2;
    int re;

    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (strcmp(name, head2.name) == 0) {
            if (head2.typeflag != '5') {
                close(fd);
                write(STDERR_FILENO, "rmdir: target is not a directory\n\0", 34);
                exit(EXIT_FAILURE);
            }
        }
        if (strcmp(name_and_slash, head2.name) == 0) {
            if (head2.typeflag != '5') {
                close(fd);
                write(STDERR_FILENO, "rmdir: target is not a directory\n\0", 34);
                exit(EXIT_FAILURE);
            } else {
                if (is_empty_directory(fd, head2) == 1) {
                    unlink_header(fd, head2.name);
                    if (lseek(fd, -BLOCKSIZE, SEEK_CUR) < 0) {
                        close(fd);
                        perror("lseek error seek_cur");
                        exit(EXIT_FAILURE);
                    }

                    remove_first_header(fd, argv);

                    off_t end;
                    if ((end = lseek(fd, 0L, SEEK_END)) < 0) {
                        close(fd);
                        perror("lseek error seek_cur");
                        exit(EXIT_FAILURE);
                    }
                    if (ftruncate(fd, end - BLOCKSIZE) < 0) { // remove useless spaces
                        close(fd);
                        perror("ftruncate error");
                        exit(EXIT_FAILURE);
                    }
                    close(fd);
                    return 1;
                } else {
                    close(fd);
                    write(STDERR_FILENO, "rmdir: target is not an empty directory\n\0", 41);
                    exit(EXIT_FAILURE);
                }
            }
        }
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("remove_empty_directory read error");
        exit(EXIT_FAILURE);
    }

    close(fd);
    write(STDERR_FILENO, "rmdir: No such file or directory\n\0", 34);
    exit(EXIT_FAILURE);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && has_tar_in_path(argv[1])) {
        if (strlen(argv[1]) > strlen(get_tar_only(argv[1], '/')) +1)
        return remove_empty_directory(argv);
    }


    write(STDERR_FILENO, "rmdir: can not execute the command\n\0", 36);
    return 0;
}

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
 * remove 1 file
 */
int remove_file(char **argv, int argc) {
    char* tar = get_path_until_tar(argv[1]);
    char* name = get_path_after_tar(argv[1]);
    int fd = open(tar, O_RDWR);

    //add a slash
    char name_and_slash[strlen(name) + 1];
    strcpy(name_and_slash, name);
    name_and_slash[strlen(name)] = '/';
    name_and_slash[strlen(name) +1] = '\0';

    if (fd < 0) {
        close(fd);
        write(STDERR_FILENO, "rm: No such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }
    struct header head2;
    int re;
    int cpt; // number of BLOCKSIZE that we removed

    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (strcmp(name_and_slash, head2.name) == 0) {
            if (head2.typeflag == '5') {
                close(fd);
                write(STDERR_FILENO, "rm: can not remove a directory\n\0", 32);
                exit(EXIT_FAILURE);
            }
        }
        if (strcmp(name, head2.name) == 0) {
            if (argc == 2 && head2.typeflag == '5') {
                close(fd);
                write(STDERR_FILENO, "rm: can not remove a directory\n\0", 32);
                exit(EXIT_FAILURE);
            } else {
                if (lseek(fd, -BLOCKSIZE, SEEK_CUR) < 0) {
                    close(fd);
                    perror("lseek error seek_cur");
                    exit(EXIT_FAILURE);
                }

                cpt = remove_first_headerAndContents(fd);

                off_t end;
                if ((end = lseek(fd, 0L, SEEK_END)) < 0) {
                    close(fd);
                    perror("lseek error seek_cur");
                    exit(EXIT_FAILURE);
                }
                if (ftruncate(fd, end - cpt) < 0) { // remove useless spaces
                    close(fd);
                    perror("ftruncate error");
                    exit(EXIT_FAILURE);
                }
                close(fd);
                return 1;
            }
        }
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("remove_file read error");
        exit(EXIT_FAILURE);
    }

    close(fd);
    write(STDERR_FILENO, "rm: No such file or directory\n\0", 31);
    exit(EXIT_FAILURE);
    return 0;
}

/**
 * remove recursive (for directies)
 */
int remove_rec(char ** argv) {
    char* tar = get_path_until_tar(argv[2]);
    char* name = get_path_after_tar(argv[2]);

    int fd = open(tar, O_RDWR);
    if (fd < 0) {
        close(fd);
        write(STDERR_FILENO, "rm: No such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }
    struct header head2;
    int re;
    int cpt; // number of BLOCKSIZE that we removed
    int find = 0;
    struct header argHead = nameToHeader(fd, name);

    off_t current;

    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if ((argHead.typeflag == '5' &&
            strncmp(argHead.name, head2.name, strlen(argHead.name)) == 0)
            || strcmp(argHead.name, head2.name) == 0) {
                if (lseek(fd, -sizeof(struct header), SEEK_CUR) < 0) {
                    close(fd);
                    perror("lseek error seek_cur");
                    exit(EXIT_FAILURE);
                }

                cpt = remove_first_headerAndContents(fd);

                current = lseek(fd, 0L, SEEK_CUR);
                if (current < 0) {
                    close(fd);
                    perror("lseek error seek_cur");
                    exit(EXIT_FAILURE);
                }

                off_t end;
                if ((end = lseek(fd, 0L, SEEK_END)) < 0) {
                    close(fd);
                    perror("lseek error seek_cur");
                    exit(EXIT_FAILURE);
                }
                if (ftruncate(fd, end - cpt) < 0) { // remove useless spaces
                    close(fd);
                    perror("ftruncate error");
                    exit(EXIT_FAILURE);
                }

                if (lseek(fd, current, SEEK_SET) < 0) {
                    close(fd);
                    perror("exists_header seek4");
                    exit(EXIT_FAILURE);
                }
                find ++;
                if (argHead.typeflag != '5')
                    return 1;

        } else
            skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("remove_empty_directory read error");
        exit(EXIT_FAILURE);
    }

    if (find == 0) {
        close(fd);
        write(STDERR_FILENO, "rm: No such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
        return 0;
    }
    close(fd);
    return 1;
}


int main(int argc, char ** argv)
{
    char *afterTar;

    if (argc == 2 && has_tar_in_path(argv[1])) {
        afterTar = get_path_after_tar(argv[1]);
        if (afterTar == NULL) {
            write(STDERR_FILENO, "rm: can not remove a directory\n\0", 32);
            return 0;
        } else {
            free(afterTar);
            return remove_file(argv, argc);
        }
    } else if (argc == 3 && strcmp(argv[1], "-r") == 0 && has_tar_in_path(argv[2])) {
        afterTar = get_path_after_tar(argv[2]);
        if ((afterTar = get_path_after_tar(argv[2])) == NULL) {
            unlink(argv[2]);
            return 1;
        }
        else {
            free(afterTar);
            return remove_rec(argv);
        }
    }

    write(STDERR_FILENO, "rm: can not execute the command\n\0", 33);
    return 0;
}

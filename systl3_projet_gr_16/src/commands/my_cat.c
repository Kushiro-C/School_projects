#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "../include/tar_parser.h"

int cat_alone() {
    char str[BLOCKSIZE];
    int r;
    int w;
    while((r = read(STDIN_FILENO, str, BLOCKSIZE)) > 0) {
        if ((w = write(STDOUT_FILENO, str, r)) < 0)
            return -1;
    }
    return 0;
}


/**
 * write on STDOUT_FILENO the contents and check writing error
 * *s is what we write
 * re is que length of what we read
 */
int write_contents(int fd, char *s, int re) {
    int w;
    int writed = 0;;
    while ((w = write(STDOUT_FILENO, s + writed, re - writed)) > 0)
        writed += w;
    if (w  == -1)
        return -1;
    return re;
}

/**
 * after reading a header
 * call it to read contents and go to next header
 */
void read_contents(int fd, struct header head) {
    int size; // File size
    char s[BLOCKSIZE];
    sscanf(head.size, "%o", &size); // Convert Octal to Decimal
    int re; // size of what we read
    int n = (size + 512 - 1) / 512; // size to skip to next header
    for (int i = 0; i < n; i++) {
        memset(s, 0, BLOCKSIZE);
        if ((re = (read(fd, &s, BLOCKSIZE))) < 0) {
            close(fd);
            perror("Bad read file content");
            exit(EXIT_FAILURE);
        }
        write_contents(fd, s, strlen(s));
    }
}


/**
 * cat a tar and inside
 */
int cat_argv2(int argc, char **argv) {
    char* tar = get_path_until_tar(argv[1]);
    int fd = open(tar, O_RDONLY);
    if (fd < 0) {
        perror("ls : open fail");
        exit(EXIT_FAILURE);
    }

    char* path = get_path_after_tar(argv[1]);

    struct header head;
    int re; // length of what we read

    char *dot = strchr(argv[1],'.');
    if (argc == 2 && (dot && !strcmp(dot, ".tar"))) {
        writeOnStdout(fd, argv[1]);
        writeOnStdout(fd, " est la racine du fichier tar..\n");
        exit(EXIT_SUCCESS);
    } else { // if it's not a tar, search the file
          // Reading all headers of tar ball
        while ((re = read(fd, &head, BLOCKSIZE)) > 0) {
            // Name matches
            if (strcmp(path, head.name) == 0) {
                if (head.typeflag == '5') {
                    writeOnStdout(fd, path);
                    writeOnStdout(fd, " est un dossier..\n");
                    close(fd);
                    exit(EXIT_SUCCESS);
                }
                // write its contains
                read_contents(fd, head);
                close(fd);
                exit(EXIT_SUCCESS);
            } else {
                skip_contents(fd, head);
            }
        }
        // file does not exist
        close(fd);
        write(STDERR_FILENO, "cp: this file does not exist\n\0", 30);
        exit(EXIT_FAILURE);
    }

        return 0;
        // Closing file
        close(fd);
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return cat_alone();
    else if (argc == 2 && has_tar_in_path(argv[1])
    && strcmp(argv[1], get_path_until_tar(argv[1])) != 0)
        return cat_argv2(argc, argv);
    else {
        write(STDOUT_FILENO, "cat: target is a directory\n\0", 28);
        exit(EXIT_FAILURE);
    }

    return -1;
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../include/tar_parser.h"

char db_space[2] = "  ";

/**
 * after reading a header
 * print its rights
 */
void print_rights(int fd, char mode[8]) {
    for(int i = 4; i < strlen(mode); i++) {
        switch (mode[i])
        {
        case '0':
            writeOnStdout(fd, "---");
            break;
        case '1':
            writeOnStdout(fd, "--x");
            break;
        case '2':
            writeOnStdout(fd, "-w-");
            break;
        case '3':
            writeOnStdout(fd, "-wx");
            break;
        case '4':
            writeOnStdout(fd, "r--");
            break;
        case '5':
            writeOnStdout(fd, "r-x");
            break;
        case '6':
            writeOnStdout(fd, "rw-");
            break;
        case '7':
            writeOnStdout(fd, "rwx");
            break;
        default:
            break;
        }
    }
    writeOnStdout(fd, " ");
}

/**
 * print file type
 */
void print_fileType(int fd, char typeflag) {
    switch (typeflag)
    {
    case '0':
        writeOnStdout(fd, "-");
        break;
    case '1': // harlink
        writeOnStdout(fd, "h");
        break;
    case '2': // symbolic link
        writeOnStdout(fd, "l");
        break;
    case '3': // character special
        writeOnStdout(fd, "c");
        break;
    case '4': // block special
        writeOnStdout(fd, "b");
        break;
    case '5': // directory
        writeOnStdout(fd, "d");
        break;
    case '6': // FIFO
        writeOnStdout(fd, "p");
        break;
    default:
        break;
    }
}

/**
 * print size without useless space and 0
 * int n = number of spaces we print
 */
void print_size(int fd, struct header head, int n) {
    if (head.typeflag != '0') {
        if (n > 0)
            for(int j = 0; j < n-1; j++)
                writeOnStdout(fd, " ");
        writeOnStdout(fd, "0");
    } else {
        int end = n - strlen(removeAllOcAtBeginning(head.size, '0'));
        if (n > 0) {
            for(int i = 0; i < end; i++)
                writeOnStdout(fd, " ");
            writeOnStdout(fd, removeAllOcAtBeginning(head.size, '0'));
        } else
            writeOnStdout(fd, "0");
    }
}

/**
 * return the length of the maximum size (without 0 )in tar
 */
int maxSize(int fd, int root, char **argv) {
    int max = 0;
    int len_of_head_name;
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        perror("current lseek error");
        exit(EXIT_FAILURE);
    }
    struct header head2;
    int re;
    if (lseek(fd, 0L, SEEK_SET) < 0) {
        perror("seek1");
        exit(EXIT_FAILURE);
    }

    if (root == 0) { // if we are not in root
        char* path = get_path_after_tar(argv[2]);
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
            if (get_first_after_path(path, head2.name, '/') != NULL) { // argv[3] is the directory where we ls
                len_of_head_name = strlen(removeAllOcAtBeginning(head2.size, '0'));
                if (max < len_of_head_name)
                    max = len_of_head_name;
            }
            skip_contents(fd, head2);
        }
    }
    else if (root == 1) { // if we are in root
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
            if (get_path_length(head2.name, '/') == 1) { // argv[2] = the tar
                len_of_head_name = strlen(removeAllOcAtBeginning(head2.size, '0'));
                if (max < len_of_head_name)
                    max = len_of_head_name;
            }
            skip_contents(fd, head2);
        }
    }
    if (re < 0) {
        perror("read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        perror("seek2");
        exit(EXIT_FAILURE);
    }
    return max;
}

/**
 * return number of hard link
 */
void hard_link(int fd, struct header head) {
    if (head.typeflag == '5') { // if it's a directory
        int cpt = 1;
        off_t current = lseek(fd, 0L, SEEK_CUR);
        if (current < 0) {
            perror("current lseek error");
            exit(EXIT_FAILURE);
        }
        struct header head2;
        int re;
        if (lseek(fd, 0L, SEEK_SET) < 0) {
            perror("seek1");
            exit(EXIT_FAILURE);
        }
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
            if (get_first_after_path(head.name, head2.name, '/') != NULL)
                cpt ++;
            skip_contents(fd, head2);
        }
        if (re < 0) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd, current, SEEK_SET) < 0) {
            perror("seek2");
            exit(EXIT_FAILURE);
        }
        char c[100];
        sprintf(c,"%d", cpt);
        writeOnStdout(fd, c);
    } else // if it's not a directory there's only 1 hardlink
        writeOnStdout(fd, "1"); // because the tar does not save the hard link before its creation
}

/**
 * return the header that the head is linked with
 * work only if head.linkname is not only the name but the full path
 * Plus, there can have several files with same name
 */
struct header linkedWith(int fd, struct header head) { // change head to char* s
    struct header head2;                               // call head = nameToHeader(int fd, char* name)
    if (head.typeflag == '2') { // if it's a link
        off_t current = lseek(fd, 0L, SEEK_CUR);
        if (current < 0) {
            perror("current lseek error");
            exit(EXIT_FAILURE);
        }
        int re;
        if (lseek(fd, 0L, SEEK_SET) < 0) {
            perror("seek1");
            exit(EXIT_FAILURE);
        }
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
            if (head2.typeflag == '5' && (strlen(head.linkname)+1 == strlen(head2.name)) // directory
            && strncmp(head.linkname, head2.name, strlen(head.linkname)) == 0) {
                if (lseek(fd, current, SEEK_SET) < 0) {
                    perror("seek");
                    exit(EXIT_FAILURE);
                }
                return head2;
            }
            else if (head2.typeflag == '2' &&(strlen(head.linkname) == strlen(head2.name)) // a link of a link
            && strcmp(head.linkname, head2.name) == 0) {
                if (lseek(fd, current, SEEK_SET) < 0) {
                    perror("seek");
                    exit(EXIT_FAILURE);
                }
                return linkedWith(fd, head2);
            }

            skip_contents(fd, head2);
        }
        if (re < 0) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
    }
    return head; // himself
}

/**
 * print long format (for ls -l)
 */
void long_format(int fd, struct header head, int root, char **argv, int maxSiz) { // int maxSize
    print_fileType(fd, head.typeflag); // filetype
    print_rights(fd, head.mode); // mode
    hard_link(fd, head);
    writeOnStdout(fd, " ");
    writeOnStdout(fd, head.uname); // Owner user name
    writeOnStdout(fd, " ");
    writeOnStdout(fd, head.gname); // group name
    writeOnStdout(fd, " ");
    print_size(fd, head, (maxSiz));
    writeOnStdout(fd, " ");
    writeOnStdout(fd, convertUnixTime(head.mtime)); // date
    writeOnStdout(fd, " ");
    if (root == 1)
        writeOnStdout(fd, get_last_of_path(head.name, '/')); // file name
    else
        writeOnStdout(fd, get_first_after_path(get_path_after_tar(argv[2]), head.name, '/'));
    writeOnStdout(fd, "\n");
}

/**
 * ls a tar only
 */
int ls_tar(int argc, char **argv) {
    int fd = open(get_path_until_tar(argv[1]), O_RDONLY);
    if (fd < 0) {
        write(STDERR_FILENO, "No such file or directory\n\0", 27);
        exit(EXIT_FAILURE);
    }

    struct header head;
    int re; // length of what we read

    //there's no nested tar ( ex : toto.tar/directory/toto2.tar )
    if (argc == 2) {
        // Reading all headers of tar ball
        while ((re = read(fd, &head, BLOCKSIZE)) > 0) {
            // Write filename
            if (get_path_length(head.name, '/') == 1) {
                writeOnStdout(fd, get_last_of_path(head.name, '/'));
                writeOnStdout(fd, "  "); // Add spaces
            }
            skip_contents(fd, head); // Skipping file contents (non header)
        }
        if (re < 0) {
            perror("read error");
            exit(EXIT_FAILURE);
        }

    }

    // Closing file
    close(fd);

    // Add '\n'
    if (write(STDOUT_FILENO, "\n", 1) < 0) {
        perror("Write '\n'");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * ls a directory in tar
 */
int ls_tar_directory(int argc, char **argv) { // ./my_ls name_of_the_file localisation
    int fd = open(get_path_until_tar(argv[1]), O_RDONLY);
    if (fd < 0) {
        write(STDERR_FILENO, "No such file or directory\n\0", 27);
        exit(EXIT_FAILURE);
    }

    struct header head;
    int re; // length of what we read
    char *s; // stock the name in the header

    char* path = get_path_after_tar(argv[1]);
    // use is for symbolic link
    struct header argHead = nameToHeader(fd, path);
    // struct header linkedWithHeader = linkedWith(fd,argHead);
    if (argHead.typeflag == '2') {
        path = argHead.linkname;
        argHead = nameToHeader(fd, path);
        strcpy(argv[1], path);
    }
    if (argHead.typeflag != '5') {
        writeOnStdout(fd, path);
        writeOnStdout(fd, "\n");
        close(fd);
        exit(EXIT_SUCCESS);
    }

    // there's no nested tar ( ex : toto.tar/directory/toto2.tar )
    while ((re = read(fd, &head, BLOCKSIZE)) > 0) { // Reading all headers of tar ball
        // found the directory where we can start reading
        if (argHead.typeflag == '5')
            s = get_first_after_path(path, head.name, '/'); // get name after argv[2]/
        if (argHead.typeflag == '5' && s != NULL) {
            writeOnStdout(fd, s);
            skip_contents(fd, head); // Skipping file contents (non header)
            writeOnStdout(fd, "  ");  // Add spaces
        } else {
            skip_contents(fd, head);
        }
    }
    if (re < 0) {
        perror("read error");
        exit(EXIT_FAILURE);
    }

    // Closing file
    close(fd);

    // Add '\n'
    if (argHead.typeflag == '5' && write(STDOUT_FILENO, "\n", 1) < 0) {
        perror("Write '\n'");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * ls -l a tar
 */
int ls_tar_longF(int argc, char **argv) { // ./my_ls -l my_tar
    int fd = open(get_path_until_tar(argv[2]), O_RDONLY);
    if (fd < 0) {
        write(STDERR_FILENO, "No such file or directory\n\0", 27);
        exit(EXIT_FAILURE);
    }

    struct header head;
    int re; // length of what we read
    int maxSiz = maxSize(fd, 1, argv); // the length of the maximum size file in root
    while ((re = read(fd, &head, BLOCKSIZE)) > 0) {
            // Write filename
            if (get_path_length(head.name, '/') == 1)
                long_format(fd, head, 1, argv, maxSiz);
            skip_contents(fd, head); // Skipping file contents (non header)
    }
    if (re < 0) {
        perror("read error");
        exit(EXIT_FAILURE);
    }

    // Closing file
    close(fd);
    return 0;
}

/**
 * ls -l a directory in tar
 */
int ls_tar_directory_longF(int argc, char **argv) {  // ./my_ls -l my_tar dirToto (without the /)
    int fd = open(get_path_until_tar(argv[2]), O_RDONLY);
    if (fd < 0) {
        write(STDERR_FILENO, "No such file or directory\n\0", 27);
        exit(EXIT_FAILURE);
    }

    struct header head;
    int re; // length of what we read
    char *s; // stock the name in the header

    char* path = get_path_after_tar(argv[2]);
    struct header argHead = nameToHeader(fd, path);
    if (argHead.typeflag == '2') {
        path = argHead.linkname;
        argHead = nameToHeader(fd, path);
        strcpy(argv[1], path);
    }

    if (argHead.typeflag != '5') {
        writeOnStdout(fd, path);
        writeOnStdout(fd, "\n");
        close(fd);
        exit(EXIT_SUCCESS);
    }

    int maxSiz = maxSize(fd, 0, argv); // the length of the maximum size file in the directory

    // there's no nested tar ( ex : toto.tar/directory/toto2.tar )
    if (argc == 3) { // if it's not a tar, search the directory
        while ((re = read(fd, &head, BLOCKSIZE)) > 0) { // Reading all headers of tar ball
            // found the directory where we can start reading
            if (argHead.typeflag == '5')
                s = get_first_after_path(path, head.name, '/'); // get name after argv[2]/
            if (argHead.typeflag == '5' && s != NULL)
                long_format(fd, head, 0, argv, maxSiz);
            skip_contents(fd, head);
        }
        if (re < 0) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
    }

    // Closing file
    close(fd);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1) { // normal ls
        if (execlp("ls","ls", NULL) < 0) {
            perror("ls normal fail");
            exit(EXIT_FAILURE);
        }
    }

    if (argc == 2 && has_tar_in_path(argv[1])) {
        char *tmp = get_path_until_tar(argv[1]);
        if (tmp == NULL) {
            perror("ls: getting path");
            return -1;
        }

        int len = strlen(argv[1]);
        int len2 = strlen(tmp);
        free(tmp);

        if (len == len2)
            return ls_tar(argc, argv); // path/to/tarball
        return ls_tar_directory(argc, argv); // path/to/tarball/dirA...
    }

    // ls -l long format a tar or a directory in tar
    else if (argc == 3) {
        if (has_tar_in_path(argv[2]) && strcmp(argv[1], "-l") == 0) {
            char *tmp = get_path_until_tar(argv[2]);
            if (tmp == NULL) {
                perror("ls: getting path");
                return -1;
            }

            int len = strlen(argv[2]);
            int len2 = strlen(tmp);
            free(tmp);

            if (len == len2) // path/to/tarball
                return ls_tar_longF(argc, argv);
            return ls_tar_directory_longF(argc, argv); // path/to/tarball/dirA...
        }
    }
    write(STDERR_FILENO, "No such file or directory\n\0", 27);
    exit(EXIT_FAILURE);
    return -1;
}
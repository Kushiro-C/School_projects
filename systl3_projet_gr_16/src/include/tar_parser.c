#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "tar_parser.h"

/**
 * after reading a header
 * call it to skip useless contents and go to next header
 */
void skip_contents(int fd, struct header head) {
    int size; // File size
    sscanf(head.size, "%o", &size); // Convert Octal to Decimal
    int n = (size + 512 - 1) / 512; // size to skip to next header
    for (int i = 0; i < n; i++) {
        if ((read(fd, &head, BLOCKSIZE)) < 0) {
            close(fd);
            perror("Bad read file content");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * write on STDOUT_FILENO and check writing error
 */
int writeOnStdout(int fd, char* s){
    if (write(STDOUT_FILENO, s, strlen(s)) < 0) {
        close(fd);
        perror("Write spaces");
        exit(EXIT_FAILURE);
    }
    return 1;
}

/**
 * write on STDOUT_FILENO and check writing error
 */
int writeNOnStdout(int fd, char* s, int n){
    if (write(STDOUT_FILENO, s, n) < 0) {
        close(fd);
        perror("Write spaces");
        exit(EXIT_FAILURE);
    }
    return 1;
}

/**
 * write on fd2 the contents and check writing error
 * *s is what we write
 * re is que length of what we read
 */
int write_contents2(int fd, int fd2, char *s, int re) {
    int w;
    int writed = 0;;
    while ((w = write(fd2, s + writed, re - writed)) > 0)
        writed += w;
    if (w  == -1)
        return -1;
    return re;
}

/**
 * return the header that matches with the name
 * if we not find a header, it print a error message
 * and rise exit failure
 */
struct header nameToHeader(int fd, char* name) {
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("nameToHeader seek");
        exit(EXIT_FAILURE);
    }
    struct header head2; // stock the header that we read
    int re; // read
    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("nameToHeader seek1");
        exit(EXIT_FAILURE);
    }
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (head2.typeflag == '5' && (strlen(name)+1 == strlen(head2.name)) // directory
        && strncmp(name, head2.name, strlen(name)) == 0) {
            if (lseek(fd, current, SEEK_SET) < 0) {
                close(fd);
                perror("seek");
                exit(EXIT_FAILURE);
            }
            return head2;
        } else if (head2.typeflag != '5' &&(strlen(name) == strlen(head2.name)) // a link
        && strcmp(name, head2.name) == 0) {
            if (lseek(fd, current, SEEK_SET) < 0) {
                close(fd);
                perror("seek");
                exit(EXIT_FAILURE);
            }
            return head2;
        }
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("seek2");
        exit(EXIT_FAILURE);
    }
    close(fd);
    write(STDERR_FILENO, "No such file or directory\n\0", 27);

    exit(EXIT_FAILURE);
}

/**
 * check if a header has already the same name,
 * or if the header already exits and return
 * 1 if it exists and 0 toherwise
 */
int exists_header(int fd, char *name) {
    off_t current; // current seek
    if ((current = lseek(fd, 0L, SEEK_CUR)) < 0) {
        close(fd);
        perror("exists_header seek");
        exit(EXIT_FAILURE);
    }
    struct header head2;
    int re;
    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("exists_header seek1");
        exit(EXIT_FAILURE);
    }
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (head2.typeflag == '5' && (strlen(name)+1 == strlen(head2.name)) // directory
        && strncmp(name, head2.name, strlen(name)) == 0) {
            if (lseek(fd, current, SEEK_SET) < 0) {
                close(fd);
                perror("exists_header seek2");
                exit(EXIT_FAILURE);
            }
            return 1;
        }
        else if (strcmp(name, head2.name) == 0) {
            if (lseek(fd, current, SEEK_SET) < 0) {
                close(fd);
                perror("exists_header seek3");
                exit(EXIT_FAILURE);
            }
            return 1;
        }
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("exists_header read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("exists_header seek4");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/**
 * /!\ NOT USE
 * check if the tar is empty and delete it
 * if it is
 */
int remove_tar(int fd, char *tar) {
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("remove_tar seek");
        exit(EXIT_FAILURE);
    }
    struct header head;
    int re;
    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("remove_tar seek1");
        exit(EXIT_FAILURE);
    }
    if ((re = read(fd, &head, BLOCKSIZE)) > 0) {
        // if first header is empty, the tar is empty too
        if (strcmp(head.mode, "\0") == 0) {
            close(fd);
            remove(tar);
            return 1;
        }
    }
    if (re < 0) {
        close(fd);
        perror("remove_tar read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("remove_tar seek2");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/**
 * unlink all headers which have param s as
 * linkname 
 */
void unlink_header(int fd, char* s) {
    char name[100];
    memset(name, '\0', 100);

    // the linkname dos not finish with "/"
    if (s[strlen(s)-1] == '/') // if it is a directory
        strncpy(name, s, strlen(s)-1);
    else {
        strcpy(name, s);
    }
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("unlink_header seek");
        exit(EXIT_FAILURE);
    }
    struct header head2;
    int re;
    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("unlink_header seek1");
        exit(EXIT_FAILURE);
    }
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (strcmp(head2.linkname, name) == 0) {
            memset(head2.linkname, '\0', 100);

            if (lseek(fd, -BLOCKSIZE, SEEK_CUR) < 0) {
                close(fd);
                perror("unlink seek error");
                exit(EXIT_FAILURE);
            }

            // rewrite the header with no linkname
            if (write(fd, &head2, BLOCKSIZE) < 0) {
                close(fd);
                perror("unlink write error");
                exit(EXIT_FAILURE);
            }
        }
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("unlink_header read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("unlink_header seek2");
        exit(EXIT_FAILURE);
    }
}

/**
 * check if the header is a empty directory
 * we supposed that head is indeed a directory
 */
int is_empty_directory(int fd, struct header head) {
    off_t current = lseek(fd, 0L, SEEK_CUR);
    struct header head2;
    int re;
    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("is_empty_directory seek1");
        exit(EXIT_FAILURE);
    }
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (get_first_after_path(head.name, head2.name, '/') != NULL) // get name after argv[2]/ {
            return -1;
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("is_empty_directory read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("is_empty_directory seek2");
        exit(EXIT_FAILURE);
    }
    return 1;
}

/**
 * check if the path is valid for creating a new header
 * param path is the directory path without the last element (name of the file)
 * param path_and_name is th total path
 */
int is_valid_path(int fd, char* path, char* path_and_name) {
    // in root
    if (strcmp(path, "\0") == 0) {
        if (exists_header(fd, path_and_name) == 1)
            return 0;
        return 1;
    }

    // not root
    // check where we create the new header, also check if the path is valid
    // by checking if we can find a header with this path
    struct header from = nameToHeader(fd, path);
    if (from.typeflag == '5')
        return 1;

    return 0;
}

/**
 * check if the path is valid for creating a new header
 * param path_and_name (path is the directory)
 * (name is the name of the file or directory)
 */
int is_valid_path2(int fd, char* path_and_name) {
    // in root
    char *path =  getpath(path_and_name);
    if (strcmp(path, "\0") == 0) {
        if (exists_header(fd, path_and_name) == 1)
            return 0;
        return 1;
    }

    // not root
    // where we create the new directory, also check if the path is valid
    // by checking if we can find a header with this path
    struct header from = nameToHeader(fd, path);
    if (from.typeflag == '5')
        return 1;

    return 0;
}


/**
 * remove first header and its contetns
 * from where we are (pointer)
 */
int remove_first_headerAndContents(int fd) {
    struct header head2;
    int re;
    int size; // File size
    int n; // number of Blocksize needed to read contents
    int re2;
    off_t current;
    off_t save;

    save = lseek(fd, 0L, SEEK_CUR);
    if (save < 0) {
        close(fd);
        perror("lseek error seek_cur");
        exit(EXIT_FAILURE);
    }

    // skip header
    if ((re = read(fd, &head2, BLOCKSIZE)) < 0) {
        close(fd);
        perror("Bad read file content");
        exit(EXIT_FAILURE);
    }
    unlink_header(fd, head2.name);

    // skip contents
    sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
    n = (size + 512 - 1) / 512; // size to skip to next header
    for (int i = 0; i < n; i++) {
        current = lseek(fd, BLOCKSIZE, SEEK_CUR);
        if (current < 0) {
        close(fd);
        perror("lseek error seek_cur");
        exit(EXIT_FAILURE);
        }
    }
    int cpt = 2 + n ;


    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {

        // Go back before the 2 headers
        current = lseek(fd, -BLOCKSIZE*cpt, SEEK_CUR);
        if (current < 0) {
            close(fd);
            perror("remove_first_headerAndContents lseek error seek_cur");
            exit(EXIT_FAILURE);
        }

        // overwriting the new at before place
        if (write(fd, &head2, BLOCKSIZE) < 0) {
            close(fd);
            perror("remove_first_headerAndContents write error");
            exit(EXIT_FAILURE);
        }

        // return to the contents of head2
        current = lseek(fd, BLOCKSIZE*(cpt-1), SEEK_CUR);
        if (current < 0) {
            close(fd);
            perror("remove_first_headerAndContents lseek error seek_cur");
            exit(EXIT_FAILURE);
        }

        // write contents
        sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
        n = (size + 512 - 1) / 512; // size to skip to next header
        for (int i = 0; i < n; i++) {
            if ((re2 = read(fd, &head2, BLOCKSIZE)) < 0) { // overwrite a file
                close(fd);
                perror("Bad read file content");
                exit(EXIT_FAILURE);
            }

            current = lseek(fd, -((BLOCKSIZE*(cpt-1)) + re2), SEEK_CUR);
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

            current = lseek(fd, (BLOCKSIZE*(cpt-1)), SEEK_CUR);
            if (current < 0) {
                close(fd);
                perror("lseek error seek_cur");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (re < 0) {
        close(fd);
        perror("remove_first_headerAndContents read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, save, SEEK_SET) < 0) {
        close(fd);
        perror("exists_header seek4");
        exit(EXIT_FAILURE);
    }

    return cpt -1;
}

/**
 * return off_t, the pointer after the last header and its contents
 */
off_t AfterLastHeadercontents(int fd) {
    off_t current;
    if ((current = lseek(fd, 0L, SEEK_CUR)) < 0) {
        close(fd);
        perror("AfterLastHeadercontents seek cur");
        exit(EXIT_FAILURE);
    }
    struct header head2;
    int re;
    off_t after_lastHeader_contents = lseek(fd, 0L, SEEK_SET);
    if (after_lastHeader_contents < 0) {
        close(fd);
        perror("AfterLastHeadercontents seek1");
        exit(EXIT_FAILURE);
    }
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (strncmp(head2.name, "\0", 1) == 0) {
            if (lseek(fd, current, SEEK_SET) < 0) {
                close(fd);
                perror("AfterLastHeadercontents seek2");
                exit(EXIT_FAILURE);
            }
            return after_lastHeader_contents;
        }
        skip_contents(fd, head2);
        after_lastHeader_contents = lseek(fd, 0L, SEEK_CUR);
        if (after_lastHeader_contents < 0) {
            close(fd);
            perror("seek after_lastHeader_contents");
            exit(EXIT_FAILURE);
        }

    }
    if (re < 0) {
        close(fd);
        perror("AfterLastHeadercontents read error");
        exit(EXIT_FAILURE);
    }
    return after_lastHeader_contents;
}

// source :tp
/* Compute and write the checksum of a header, by adding all
   (unsigned) bytes in it (while hd->chksum is initially all ' ').
   Then hd->chksum is set to contain the octal encoding of this
   sum (on 6 bytes), followed by '\0' and ' '.
*/
void set_checksum(struct header *hd) {
  memset(hd->chksum,' ',8);
  unsigned int sum = 0;
  char *p = (char *)hd;
  for (int i=0; i < BLOCKSIZE; i++) { sum += p[i]; }
  sprintf(hd->chksum,"%06o",sum);
}

// source : tp
/* Check that the checksum of a header is correct */
int check_checksum(struct header *hd) {
  unsigned int checksum;
  sscanf(hd->chksum,"%o ", &checksum);
  unsigned int sum = 0;
  char *p = (char *)hd;
  for (int i=0; i < BLOCKSIZE; i++) { sum += p[i]; }
  for (int i=0;i<8;i++) { sum += ' ' - hd->chksum[i]; }
  return (checksum == sum);
}

/** /!\ no used yet
 * rewrite all from where we are
 */
void rewriteall(int fd) {
    int re;
    struct header head2;

    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("rewriteall seek");
        exit(EXIT_FAILURE);
    }

    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        printf("\n rewrite header  : %s\n", head2.name);
        if (lseek(fd, -BLOCKSIZE, SEEK_CUR) < 0) {
            close(fd);
            perror("rewriteall seek error");
            exit(EXIT_FAILURE);
        }
        if (write(fd, &head2, BLOCKSIZE) < 0) {
                close(fd);
                perror("rewriteall write error");
                exit(EXIT_FAILURE);
        }
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("rewriteall seek2");
        exit(EXIT_FAILURE);
    }
}

/**
 * copy header source and return new header with name new_name
 */
struct header copy_header (struct header source, char* new_name) {
    struct header new_Header;

    memset(new_Header.name,'\0',100);
    memcpy(new_Header.name, new_name, strlen(new_name));

    memset(new_Header.mode,'\0',8);
    memcpy(new_Header.mode, source.mode, strlen(source.mode));

    memset(new_Header.uid,'\0',8);
    memcpy(new_Header.uid, source.uid, strlen(source.uid));

    memset(new_Header.gid,'\0',8);
    memcpy(new_Header.gid, source.gid, strlen(source.gid));

    memset(new_Header.size,'\0',12);
    memcpy(new_Header.size, source.size, strlen(source.size));

    time_t tim;
    time(&tim);
    memset(new_Header.mtime,'\0',12);
    sprintf(new_Header.mtime, "%lo", tim);

    new_Header.typeflag =  source.typeflag;

    memset(new_Header.linkname,'\0',100);
    memcpy(new_Header.linkname, source.linkname, strlen(source.linkname));

    strcpy(new_Header.magic, TMAGIC);

    memcpy(new_Header.version, TVERSION, 2);

    memset(new_Header.uname,'\0',32);
    memcpy(new_Header.uname, source.uname, strlen(source.uname));

    memset(new_Header.gname,'\0',32);
    memcpy(new_Header.gname, source.gname, strlen(source.gname));

    memset(new_Header.devminor,'\0',8);
    memcpy(new_Header.devminor, source.devminor, strlen(source.devminor));

    memset(new_Header.devmajor,'\0',8);
    memcpy(new_Header.devmajor, source.devmajor, strlen(source.devmajor));

    memset(new_Header.prefix,'\0',155);
    memcpy(new_Header.prefix, source.prefix, strlen(source.prefix));

    memset(new_Header.junk,'\0',12);
    memcpy(new_Header.junk, source.junk, strlen(source.junk));

    set_checksum(&new_Header);

    return new_Header;
}


/**
 * change headers with linkname "s" to "to"
 */
void change_link(int fd, char* s, char* to) {
    struct header head2;
    int re;

    // remove '/' for directories
    char name[100];
    memset(name, '\0', 100);
    if (s[strlen(s)-1] == '/')
        strncpy(name, s, strlen(s)-1);
    else
        strcpy(name, s);

    char name2[100];
    memset(name2, '\0', 100);
    if (to[strlen(to)-1] == '/')
        strncpy(name2, to, strlen(to)-1);
    else
        strcpy(name2, to);

    // save seek_cur
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("change_link seek");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("change_link seek1");
        exit(EXIT_FAILURE);
    }

    // rewrite the headers
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if (strcmp(head2.linkname, name) == 0) {

            strcpy(head2.linkname, name2);
            check_checksum(&head2);

            if (lseek(fd, -BLOCKSIZE, SEEK_CUR) < 0) {
                close(fd);
                perror("change_link seek error");
                exit(EXIT_FAILURE);
            }
            if (write(fd, &head2, BLOCKSIZE) < 0) {
                close(fd);
                perror("change_link write error");
                exit(EXIT_FAILURE);
            }
        }
        skip_contents(fd, head2);
    }
    if (re < 0) {
        close(fd);
        perror("change_link read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("change_link seek2");
        exit(EXIT_FAILURE);
    }
}

/**
 * remove file named param name
 * into tar
 */
int remove_file_2(int fd, char*name) {
    struct header head2;
    int re;
    int cpt; // number of BLOCKSIZE that we removed

    off_t save = lseek(fd, 0L, SEEK_CUR);
    if (save < 0) {
        close(fd);
        perror("remove_file_2 seek");
        exit(EXIT_FAILURE);
    }

    // header that we remove
    struct header argHead = nameToHeader(fd, name);

    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("lseek error seek_cur");
        exit(EXIT_FAILURE);
    }

    // if it's not a directory
    if (argHead.typeflag != '5') {
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
            if (strcmp(name, head2.name) == 0) {

                    if (lseek(fd, -BLOCKSIZE, SEEK_CUR) < 0) {
                        close(fd);
                        perror("lseek error seek_cur");
                        exit(EXIT_FAILURE);
                    }

                    cpt = remove_first_headerAndContents(fd);

                    off_t end;
                    if ((end = lseek(fd, 0L, SEEK_END)) < 0) {
                        close(fd);
                        perror("lseek error seek_end");
                        exit(EXIT_FAILURE);
                    }
                    if (ftruncate(fd, end - cpt) < 0) { // remove useless spaces
                        close(fd);
                        perror("ftruncate error");
                        exit(EXIT_FAILURE);
                    }
                    if (lseek(fd, save, SEEK_SET) < 0) {
                        close(fd);
                        perror("remove_file_2 seek2");
                        exit(EXIT_FAILURE);
                    }
                    return 1;
            }
            skip_contents(fd, head2);
        }
    } else { // we remove a directory
        int cpt; // number of BLOCKSIZE that we removed
        int find = 0;
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

                    // actualize the size
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
            perror("remove_file_2 read error");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd, save, SEEK_SET) < 0) {
            close(fd);
            perror("remove_file_2 seek2");
            exit(EXIT_FAILURE);
        }
        return find; // number of files removed
    }
    if (re < 0) {
            close(fd);
            perror("remove_file_2 read error");
            exit(EXIT_FAILURE);
    }
    if (lseek(fd, save, SEEK_SET) < 0) {
        close(fd);
        perror("remove_file_2 seek2");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/**
 * add a new_Header into the tar
 */
off_t add_header_in_tar(int fd, struct header new_Header) {
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("add_header_in_tar seek");
        exit(EXIT_FAILURE);
    }

    off_t end = lseek(fd, 0L, SEEK_END);
    if (end < 0) {
        close(fd);
        perror("add_header_in_tar seek end1");
        exit(EXIT_FAILURE);
    }

    // seek after the last header and its contents
    off_t after_last_header =  AfterLastHeadercontents(fd);

    if (lseek(fd, after_last_header, SEEK_SET) < 0) {
        close(fd);
        perror("add_header_in_tar seek after_last_header");
        exit(EXIT_FAILURE);
    }

    // write the new header in the tar
    if (write(fd, &new_Header, BLOCKSIZE) < 0) {
        close(fd);
        perror("add_header_in_tar write new header error");
        exit(EXIT_FAILURE);
    }

    off_t after_wr = lseek(fd, 0L, SEEK_CUR);
    if (after_wr < 0) {
        close(fd);
        perror("add_header_in_tar seek");
        exit(EXIT_FAILURE);
    }

    //-----Add a new block fills with 0--------
    if (lseek(fd, 0L, SEEK_END) < 0) {
        close(fd);
        perror("add_header_in_tar seek end2");
        exit(EXIT_FAILURE);
    }

    int zero = 0;
    int i = 0;
    while( i< BLOCKSIZE ) {
        i++;
        if (write(fd, &zero, sizeof(int)) < 0) {
            close(fd);
            perror("add_header_in_tar write error");
            exit(EXIT_FAILURE);
        }
    }
    //----------------------------------------


    //----------edit the size of the tar-------------
    if (ftruncate(fd, end + BLOCKSIZE) < 0) { // extends ok
        close(fd);
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("move_noDirectory seek2");
        exit(EXIT_FAILURE);
    }

    return after_wr;
}

/**
 * create all directory of source into directory dest in fd
 * but not create the root directory dest
 */
int create_all_directory(int fd, char* source, char* dest) {
    struct header head2;
    int re;
    int cpt = 1;
    int find = 1;

    off_t save = lseek(fd, 0L, SEEK_CUR);
    if (save < 0) {
        close(fd);
        perror("create_all_directory seek");
        exit(EXIT_FAILURE);
    }

    struct header source_header = nameToHeader(fd, source);

    // write in the new directory
    while (find >= 1) {
        find = 0;
        if (lseek(fd, 0L, SEEK_SET) < 0) {
                close(fd);
                perror("create_all_directory seek");
                exit(EXIT_FAILURE);
        }

        // write in the new directory
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {

            if (head2.typeflag == '5' &&
            strncmp(head2.name, source_header.name, strlen(source_header.name)) == 0 &&
            get_path_length(head2.name, '/') == (get_path_length(source_header.name, '/') + cpt)) {
                char tmp[BLOCKSIZE];
                strcpy(tmp, dest);
                strcat(tmp, "/");
                strcat(tmp, head2.name+ strlen(source_header.name));
                if (mkdir(tmp, 0700) < 0) { // atoi(head2.mode+3)
                    close(fd);
                    perror("chemin exterieur non valide");
                    exit(EXIT_FAILURE);
                }
                find ++;
            }
            skip_contents(fd, head2);
        }
        if (re < 0) {
            close(fd);
            perror("create_all_directory read error");
            exit(EXIT_FAILURE);
        }
        cpt ++;
    }


    if (lseek(fd, save, SEEK_SET) < 0) {
        close(fd);
        perror("create_all_directory seek2");
        exit(EXIT_FAILURE);
    }
    return cpt;
}

/**
 * add char in tar at SEEK_CUR and write immediatly a block of zero after
 * this function should be used when we adding a new header at the
 * end of the tar
 */
int add_char_in_tar(int fd, char contents[BLOCKSIZE]) {
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("add_char_in_tar seek");
        exit(EXIT_FAILURE);
    }

    off_t end = lseek(fd, 0L, SEEK_END);
    if (end < 0) {
        close(fd);
        perror("add_char_in_tar seek end1");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("add_char_in_tar seek after_last_header");
        exit(EXIT_FAILURE);
    }

    char tmp[BLOCKSIZE];
    memset(tmp, '\0', BLOCKSIZE);
    memcpy(tmp, contents, strlen(contents));

    // write the new header in the tar
    if (write(fd, &tmp, BLOCKSIZE) < 0) {
        close(fd);
        perror("add_char_in_tar write new header error");
        exit(EXIT_FAILURE);
    }

    off_t after_wr = lseek(fd, 0L, SEEK_CUR);
    if (after_wr < 0) {
        close(fd);
        perror("add_char_in_tar seek");
        exit(EXIT_FAILURE);
    }

    //-----Add a new block fills with 0--------
    if (lseek(fd, 0L, SEEK_END) < 0) {
        close(fd);
        perror("add_char_in_tar seek end2");
        exit(EXIT_FAILURE);
    }

    int zero = 0;
    int i = 0;
    while( i< BLOCKSIZE ) {
        i++;
        if (write(fd, &zero, sizeof(int)) < 0) {
            close(fd);
            perror("add_char_in_tar write error");
            exit(EXIT_FAILURE);
        }
    }
    //----------------------------------------


    //----------edit the size of the tar-------------
    if (ftruncate(fd, end + BLOCKSIZE) < 0) { // extends ok
        close(fd);
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("add_char_in_tar seek2");
        exit(EXIT_FAILURE);
    }

    return after_wr;
}


/**
 * create a header from struct stat st
 * and give him param new_name as name
 */
struct header create_header (struct stat st , char* new_name) {
    struct header new_Header;

    memset(new_Header.name,'\0',100);
    memcpy(new_Header.name, new_name, strlen(new_name));

    memset(new_Header.mode,'\0',8);
    char mode[8];
    memset(mode,'\0',8);
    sprintf(mode, "%o",st.st_mode);
    for(int i = 0; i < (7-strlen(mode)); i++) {
            new_Header.mode[i] = '0';
    }
    int j0 = 0;
    for(int i = (7-strlen(mode)); i < 7; i++) {
        new_Header.mode[i] = mode[j0];
        j0++;
    }

    memset(new_Header.uid,'\0',8);
    sprintf(new_Header.uid, "%u", st.st_uid);

    memset(new_Header.gid,'\0',8);
    sprintf(new_Header.gid, "%u",st.st_gid);

    memset(new_Header.size,'\0',12);
    if (S_ISDIR(st.st_mode) == 1)
        memcpy(new_Header.size, "00000000000", 12);
    else {
        char size[12];
        memset(size,'\0',12);
        sprintf(size, "%ld", st.st_size);
        int octal_size = convertDecimalToOctal(atoi (size));
        int number_digits = countDigits(octal_size);

        char tmp[number_digits];
        sprintf(tmp, "%d", octal_size);

        for( int i = 0; i < (11-number_digits); i++) {
            new_Header.size[i] = '0';
        }
        int j = 0;
        for( int i = (11-number_digits); i < 11; i++) {
            new_Header.size[i] = tmp[j];
            j++;
        }

    }

    memset(new_Header.mtime,'\0',12);
    sprintf(new_Header.mtime, "%lo", st.st_mtim.tv_sec);

    if (S_ISREG(st.st_mode) == 1)
        new_Header.typeflag = '0';
    else if (S_ISDIR(st.st_mode) == 1)
        new_Header.typeflag = '5';
    else if (S_ISCHR(st.st_mode) == 1)
        new_Header.typeflag = '3';
    else if (S_ISBLK(st.st_mode) == 1)
        new_Header.typeflag = '4';
    else if (S_ISFIFO(st.st_mode) == 1)
        new_Header.typeflag = '6';
    else if (S_ISLNK(st.st_mode) == 1)
        new_Header.typeflag = '2';
    else if (S_ISSOCK(st.st_mode) ==1)
        new_Header.typeflag = '7';
    else
        new_Header.typeflag = '1';

    memset(new_Header.linkname,'\0',100);

    strcpy(new_Header.magic, TMAGIC);

    memcpy(new_Header.version, TVERSION, 2);

    uid_t uid = getuid();
    struct passwd *user = getpwuid(uid);

    memset(new_Header.uname,'\0',32);
    memcpy(new_Header.uname, user->pw_name, strlen(user->pw_name));

    struct group *gr = getgrgid(user->pw_gid);
    memset(new_Header.gname,'\0',32);
    memcpy(new_Header.gname, gr->gr_name, strlen(gr->gr_name));

    memset(new_Header.devmajor,'\0', 8);
    memset(new_Header.devminor,'\0',8);
    memset(new_Header.prefix,'\0',155);
    memset(new_Header.junk,'\0',12);

    set_checksum(&new_Header);

    return new_Header;
}


/**
 * create a header from struct stat st (not directory)
 * and give him param new_name as name
 * (please create all directories before calling this function)
 */
int add_file_in_tar(int fd, char* source, struct header new_header) {
    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("add_file_in_tar seek current");
        exit(EXIT_FAILURE);
    }

    // add the new_header into the tar
    off_t after_wr = add_header_in_tar(fd, new_header);
    after_wr = lseek(fd, after_wr, SEEK_SET);
    if (after_wr < 0) {
        close(fd);
        perror("add_file_in_tar seek");
        exit(EXIT_FAILURE);
    }

    // add contents
    if (new_header.typeflag != '5') {
        int fd2 = open(source, O_RDWR);
        if (fd2 < 0) {
            close(fd);
            write(STDERR_FILENO, "No such file or directory\n\0", 27);
            exit(EXIT_FAILURE);
        }

        char contents[BLOCKSIZE];
        memset(contents,'\0',BLOCKSIZE);
        int re;
        while ((re = read(fd2, &contents, BLOCKSIZE)) > 0) {
            after_wr = add_char_in_tar(fd, contents);
            after_wr = lseek(fd, after_wr, SEEK_SET);
            if (after_wr < 0) {
                close(fd);
                perror("add_file_in_tar seek");
                exit(EXIT_FAILURE);
            }
            memset(contents,'\0',BLOCKSIZE);
        }
        if (re < 0) {
            close(fd);
            close(fd2);
            perror("add_file_in_tar read error");
            exit(EXIT_FAILURE);
        }
        close(fd2);
    }

    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("add_file_in_tar seek2");
        exit(EXIT_FAILURE);
    }
    return after_wr;
}


/**
 * check if out path is valid
 */
int isValidOutPath(char* path) {
    if (path == NULL)
        return 0;
    if (get_path_length(path, '/') == 1)
        return 1;
    struct stat stat_dest;
    int len = strlen(path) - strlen(get_last_of_path(path, '/'));
    char pathWithOutLasts[len];
    strncpy(pathWithOutLasts, path, len);
    if (lstat(pathWithOutLasts, &stat_dest) < 0) {
        return 0;
    }
    return 1;
}

//www.programiz.com
int convertDecimalToOctal(int decimalNumber) {
    int octalNumber = 0, i = 1;

    while (decimalNumber != 0) {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8;
        i *= 10;
    }

    return octalNumber;
}

//stackoverflow.com
int countDigits(int value) {
    int result = 0;
    while (value != 0) {
        value /= 10;
        result++;
    }
    return result;
}
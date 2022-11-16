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
 * testing another method
 */
struct header create_new_header_directory(int fd, char* path_and_name) {
    struct header new_Header;

    uid_t uid = getuid();
    struct passwd *user = getpwuid(uid);

    memset(new_Header.name,'\0',100);
    memcpy(new_Header.name, path_and_name, strlen(path_and_name));
    new_Header.name[strlen(path_and_name)] = '/';

    strcpy(new_Header.mode, "0000745");

    memset(new_Header.uid,'\0',8);
    sprintf(new_Header.uid, "%d", user->pw_uid);

    memset(new_Header.gid,'\0',8);
    sprintf(new_Header.gid, "%d", user->pw_gid);

    strcpy(new_Header.size, "00000000000");

    time_t tim;
    time(&tim);
    memset(new_Header.mtime,'\0',12);
    sprintf(new_Header.mtime, "%lo", tim);

    new_Header.typeflag = '5';

    memset(new_Header.linkname,'\0',100);

    strcpy(new_Header.magic, TMAGIC);

    memcpy(new_Header.version, TVERSION, 2);

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
 * create the new header directory and place it in tar
 */
int create_directory(char** argv) {
    char* path_and_name = get_path_after_tar(argv[1]);
    char* path = getpath(path_and_name); // the directory where we create the new directory

    //----CHECK IF THE NAME AND PATH ARE VALID ------
    char* tar = get_path_until_tar(argv[1]);

    int fd = open(tar, O_RDWR);
    if (fd < 0) {
        close(fd);
        perror("Aucun fichier ou dossier de ce type");
        exit(EXIT_FAILURE);
    }

    if (is_valid_path(fd, path, path_and_name) == 0) {
        close(fd);
        write(STDERR_FILENO, "mkdir: can not the create directory\n\0", 37);
        exit(EXIT_FAILURE);
    }

    // create a new header
    struct header new_Header = create_new_header_directory(fd, path_and_name);


    //----------------------PLACE THE NEW DIRECTORY IN THE TAR ----------------------------

    off_t end = lseek(fd, 0L, SEEK_END);
    if (end < 0) {
        close(fd);
        perror("create_directory seek end1");
        exit(EXIT_FAILURE);
    }

    // seek after the last header
    off_t after_last_header =  AfterLastHeadercontents(fd);

    if (lseek(fd, after_last_header, SEEK_SET) < 0) {
        close(fd);
        perror("create_directory seek after_last_header");
        exit(EXIT_FAILURE);
    }

    // write the new header in the tar
    if (write(fd, &new_Header, BLOCKSIZE) < 0) {
        close(fd);
        perror("create_directory write new header error");
        exit(EXIT_FAILURE);
    }

    //-----Add a new block fills with 0--------
    if (lseek(fd, 0L, SEEK_END) < 0) {
        close(fd);
        perror("create_directory seek end2");
        exit(EXIT_FAILURE);
    }

    int zero = 0;
    int i = 0;
    while( i< BLOCKSIZE ) {
        i++;
        if (write(fd, &zero, sizeof(int)) < 0) {
            close(fd);
            perror("create_directory write error");
            exit(EXIT_FAILURE);
        }
    }

    //----------edit the size of the tar-------------
    if (ftruncate(fd, end + BLOCKSIZE) < 0) { // extends ok
        close(fd);
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    // Closing file
    close(fd);

    return -1;
}

int main(int argc, char** argv)
{

    if (argc == 2 && has_tar_in_path(argv[1])) {
        if (strlen(argv[1]) > strlen(get_tar_only(argv[1], '/')) +1)
        return create_directory(argv);
    }

    return -1;
}

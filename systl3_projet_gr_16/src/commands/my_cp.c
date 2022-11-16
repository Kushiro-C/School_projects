#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "../include/tar_parser.h"

/**
 * check if a file exists out of tar
 */
int file_exist(char *name) {
    struct stat stat_name;
    return (lstat(name, &stat_name) == 0);
}

/**
 * cp a directory "source" (already in the tar)
 * at the destination "dest"
 */
int cp_directory(int fd, char* source, char* dest) {
    if (strcmp(dest, source) == 0)
        return 1;

    // supposed 1 if dest already exist, 0 if not
    // if dest exists we write the header into a other directory
    // else we change the name too
    int inDirectory = 1;

    char s[100];
    memset(s, '\0', 100);
    struct header head_dest;
    struct header head_source = nameToHeader(fd, source);

    // if dest exist check if conditions are valid
    if (exists_header(fd, dest) == 1) {
        head_dest = nameToHeader(fd, dest);
        if (head_dest.typeflag != '5') {
            close(fd);
            perror("impossible d'écraser le non répertoire");
            exit(EXIT_FAILURE);
        }

        // determinae a new name
        strcpy(s, head_dest.name);
        strcat(s, get_last_of_path(source, '/'));

        if (exists_header(fd, s) == 1) {
            struct header s_header = nameToHeader(fd, s);
            if (s_header.typeflag != '5') {
                close(fd);
                perror("impossible d'écraser le non répertoire");
                exit(EXIT_FAILURE);
            }
            if (is_empty_directory(fd, s_header) == 0)
                remove_file_2(fd, s_header.name);
            else {
                close(fd);
                perror("impossible d'écraser le répertoire non vide");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        inDirectory = 0;
    }

    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("cp_directory seek");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("cp_directory seek1");
        exit(EXIT_FAILURE);
    }

    int re;
    struct header head2;
    int cpt = 0;
    char content_name[100];
    char tmp[100];
    off_t seek_after_header;

    // cp files with approriate names
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {
        if ((seek_after_header = lseek(fd, 0L, SEEK_CUR)) < 0) {
            close(fd);
            perror("cp_directory seek2");
            exit(EXIT_FAILURE);
        }
        if (strncmp(source, head2.name, strlen(source)) == 0) {
            struct header new_header;
            memset(tmp, '\0', 100);
            memset(content_name, '\0', 100);

            if (strcmp(head_source.name, head2.name) == 0) {
                if (inDirectory == 0) {
                    new_header = copy_header(head2, add_slash_at_the_end(dest));
                }
                else {
                    strcpy(tmp, s);
                    strcat(tmp, "/");
                    new_header = copy_header(head2, tmp);
                }
            }
            else if (inDirectory == 0) { //dest is not a directory we change the name
                strcpy(tmp, dest);
                strncpy(content_name, head2.name + strlen(source), strlen(head2.name));
                strcat(tmp, content_name);
                new_header = copy_header(head2, tmp);
            }
            else { //dest is a directory
                strcpy(tmp, s);
                strncpy(content_name, head2.name + strlen(source), strlen(head2.name));
                strcat(tmp, content_name);
                new_header = copy_header(head2, tmp);
            }

            off_t after_wr = add_header_in_tar(fd, new_header);

            if (new_header.typeflag != '5') {
                char contents[BLOCKSIZE];
                memset(contents,'\0',BLOCKSIZE);
                int size;
                sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
                int n = (size + 512 - 1) / 512; // number of blocks of the contents
                int re2;
                off_t seek_content;

                for (int i = 0; i < n; i++) {

                    if ((re2 = read(fd, &contents, BLOCKSIZE)) < 0) {
                        close(fd);
                        perror("cp_noDirectory read in error");
                        exit(EXIT_FAILURE);
                    }

                    if ((seek_content = lseek(fd, 0L, SEEK_CUR)) < 0) {
                        close(fd);
                        perror("cp_directory seek2");
                        exit(EXIT_FAILURE);
                    }

                    after_wr = lseek(fd, after_wr, SEEK_SET);
                    if (after_wr < 0) {
                        close(fd);
                        perror("cp_noDirectory seek");
                        exit(EXIT_FAILURE);
                    }

                    after_wr = add_char_in_tar(fd, contents);

                    if (lseek(fd, seek_content, SEEK_SET) < 0) {
                        close(fd);
                        perror("cp_directory seek2");
                        exit(EXIT_FAILURE);
                    }

                    memset(contents,'\0',BLOCKSIZE);
                }
            }

            if (lseek(fd, seek_after_header, SEEK_SET) < 0) {
                close(fd);
                perror("cp_directory seek2");
                exit(EXIT_FAILURE);
            }

            cpt ++;
        } else {
            skip_contents(fd, head2);
        }
    }

    if (re < 0) {
        close(fd);
        perror("cp_directory read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("cp_directory seek2");
        exit(EXIT_FAILURE);
    }
    if (cpt == 0) {
        writeOnStdout(fd, "erreur dossier");
        close(fd);
        return 0;
    }
    close(fd);
    return 1;
}


/**
 * cp no directory (already in the tar)
 * source header to dest header
 */
int cp_noDirectory(int fd, char* source, char* dest) {
    if (strcmp(dest, source) == 0)
        return 1;

    // supposed 1 if dest already exist, 0 if not
    // if dest exists we write the header into a other directory
    // else we change the name too
    int inDirectory = 0;
    char s[100];
    memset(s, '\0', 100);

    //change conditions and appropriate name
    if (exists_header(fd, dest) == 1) {
        struct header head_dest = nameToHeader(fd, dest);

        // if it's not a directory we remove the header with the same name
        if (head_dest.typeflag != '5')
            remove_file_2(fd, dest);
        else {
            inDirectory = 1;
            strcpy(s, head_dest.name);
            strcat(s, get_last_of_path(source, '/'));

            if (strcmp (get_last_of_path(source, '/'), get_last_of_path(dest, '/')) == 0) {
                close(fd);
                perror("impossible d'écraser le répertoire par un non répertoire");
                exit(EXIT_FAILURE);
            }
        }
    }

    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("cp_noDirectory seek");
        exit(EXIT_FAILURE);
    }
    struct header head2 = nameToHeader(fd, source);
    struct header new_header;
    if (inDirectory == 0)
        new_header = copy_header(head2, dest);
    else
        new_header = copy_header(head2, s);

    // add the new_header into the tar
    off_t after_wr = add_header_in_tar(fd, new_header);

    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("cp_noDirectory seek");
        exit(EXIT_FAILURE);
    }

    int re;
    // write headers
    while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {

        if (strcmp(head2.name, source) == 0) {

            char contents[BLOCKSIZE];
            int size;
            sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
            int n = (size + 512 - 1) / 512; // number of blocks of the contents
            int re2;
            off_t seek_after_header;

            for (int i = 0; i < n; i++) {
                memset(contents,'\0',BLOCKSIZE);
                if ((re2 = read(fd, &contents, BLOCKSIZE)) < 0) {
                    close(fd);
                    perror("cp_noDirectory read in error");
                    exit(EXIT_FAILURE);
                }
                if ((seek_after_header = lseek(fd, 0L, SEEK_CUR)) < 0) {
                    close(fd);
                    perror("cp_directory seek2");
                    exit(EXIT_FAILURE);
                }

                after_wr = lseek(fd, after_wr, SEEK_SET);
                if (after_wr < 0) {
                    close(fd);
                    perror("cp_noDirectory seek");
                    exit(EXIT_FAILURE);
                }

                after_wr = add_char_in_tar(fd, contents);

                if (lseek(fd, seek_after_header, SEEK_SET) < 0) {
                    close(fd);
                    perror("cp_directory seek2");
                    exit(EXIT_FAILURE);
                }

            }
            if (re < 0) {
                close(fd);
                perror("cp_noDirectory read error");
                exit(EXIT_FAILURE);
            }

            if (lseek(fd, current, SEEK_SET) < 0) {
                close(fd);
                perror("cp_noDirectory seek2");
                exit(EXIT_FAILURE);
            }

            close(fd);
            return 1;
        }
        skip_contents(fd, head2);
    }

    if (re < 0) {
        close(fd);
        perror("cp_noDirectory read error");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("cp_noDirectory seek2");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return 0;
}

/**
 * cp files and folders into the tar to the tar
 */
int cp_TarToTar(int argc, char ** argv) {

    char* source;
    char* tar;
    char* dest;

    if (argc == 3) {
        tar = get_path_until_tar(argv[1]);
        source = get_path_after_tar(argv[1]);

        // if dest is the root, choose the appropriate destination
        if (get_path_after_tar(argv[2]) != NULL)
            dest = get_path_after_tar(argv[2]);
        else
            dest = get_last_of_path(source, '/');
    } else {
        tar = get_path_until_tar(argv[2]);
        source = get_path_after_tar(argv[2]);

        // if dest is the root, choose the appropriate destination
        if (get_path_after_tar(argv[3]) != NULL)
            dest = get_path_after_tar(argv[3]);
        else
            dest = get_last_of_path(source, '/');
    }

    int fd = open(tar, O_RDWR);
    if (fd < 0) {
        close(fd);
        write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }

    /*------ check if the 2 paths are valid --------------*/
    struct header argHead = nameToHeader(fd, source);

    if (argc == 3 && argHead.typeflag == '5') {
        close(fd);
        perror("cp: -r non spécifié ; omission du répertoire ");
        exit(EXIT_FAILURE);
    }


    if (is_valid_path2(fd, dest) == 0 && exists_header(fd, dest) == 0) {
        close(fd);
        write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }
    /*---------------------------------------------------*/

    if (argHead.typeflag != '5')
        return cp_noDirectory(fd, source, dest);
    return cp_directory(fd, source, dest);

    return 0;
}

/**
 * cp no directory (already in the tar) out the tar
 */
int cp_no_directory_out(int fd, char* source, char* dest) {
    struct header head2;
    int re;
    int size;

    if (file_exist(dest) == 0) {
        // open dest
        int fd2 = open(dest, O_RDWR | O_CREAT, 0666);
        if (fd2 < 0) {
            close(fd);
            close(fd2);
            write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
            exit(EXIT_FAILURE);
        }

        // find the header and writes its contents
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {

            if (strcmp(head2.name, source) == 0) {

                sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
                int n = (size + 512 - 1) / 512; // number of blocks of the contents
                char contents[BLOCKSIZE];
                int re2;
                for (int i = 0; i < n; i++) {
                    memset(contents, 0, BLOCKSIZE);

                    if ((re2 = read(fd, &contents, BLOCKSIZE)) < 0) {
                        close(fd2);
                        close(fd);
                        perror("cp_no_directory_out read in error");
                        exit(EXIT_FAILURE);
                    }

                    char tmp[strlen(contents)];
                    strncpy(tmp, contents, strlen(contents));

                    write_contents2(fd, fd2, tmp, strlen(contents));
                }

                close(fd2);
                close(fd);
                return 1;
            }
            skip_contents(fd, head2);
        }

        if (re < 0) {
            close(fd);
            perror("cp_no_directory_out read error");
            exit(EXIT_FAILURE);
        }
    } else { // if destination already exists, we remove it if we can
        struct stat stat_dest;
        if (lstat(dest, &stat_dest) < 0) {
            close(fd);
            perror("cp_no_directory_out lstat error");
            exit(EXIT_FAILURE);
        }

        if (! S_ISDIR(stat_dest.st_mode)) {
            remove(dest);
            return cp_no_directory_out(fd, source, dest);
        } else {
            char tmp[BLOCKSIZE];
            strcpy(tmp, dest);
            strcat(tmp, "/");
            strcat(tmp, get_last_of_path(source, '/'));
            return cp_no_directory_out(fd, source, tmp);
        }
    }

    return 0;
}

/**
 * cp no directory (already in the tar) out
 * "allow" -> if we can create a directory in dest with
 * a dest directory already existing
 * (first call the function with allow = 0 )
 */
int cp_directory_out(int fd, char* source, char* dest, int allow) {
    struct header head2;
    int re;
    int size;
    struct header head_source = nameToHeader(fd, source);
    if (head_source.typeflag != '5')
        return cp_no_directory_out(fd, source, dest);

    if (allow == 1 || file_exist(dest) == 0) {
        // create the root directory (source) and inside directories
        if (allow == 1) {
            char new_directory[BLOCKSIZE];
            strcpy(new_directory, dest);
            strcat(new_directory, "/");
            strcat(new_directory, get_last_of_path(source, '/'));
            if (mkdir(new_directory, 0700) < 0) { // atoi(head_source.mode +3)
                close(fd);
                perror("chemin exterieur non valide");
                exit(EXIT_FAILURE);
            }
            create_all_directory(fd, source, new_directory);

        } else {
            if (mkdir(dest, 0700) < 0) { // atoi(head_source.mode +3)
                close(fd);
                perror("chemin exterieur non valide");
                exit(EXIT_FAILURE);
            }
            create_all_directory(fd, source, dest);
        }

        off_t seek_after_header;

        if (lseek(fd, 0L, SEEK_SET) < 0) {
            close(fd);
            perror("cp_directory seek2");
            exit(EXIT_FAILURE);
        }

        // write in new directories
        while ((re = read(fd, &head2, BLOCKSIZE)) > 0) {

            if (head2.typeflag == '5')
                continue;

            if ((seek_after_header = lseek(fd, 0L, SEEK_CUR)) < 0) {
                close(fd);
                perror("cp_directory seek2");
                exit(EXIT_FAILURE);
            }

            if (strncmp(source, head2.name, strlen(source)) == 0) {
                char moveTo[BLOCKSIZE];
                strcpy(moveTo, dest);
                strcat(moveTo, "/");
                if (allow == 1)
                    strcat(moveTo, head2.name+strlen(source)-strlen(get_last_of_path(source, '/')));
                else
                    strcat(moveTo, head2.name+strlen(source));
                int fd2 = open(moveTo, O_RDWR | O_CREAT, 0700);
                if (fd2 < 0) {
                    close(fd);
                    close(fd2);
                    write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
                    exit(EXIT_FAILURE);
                }

                sscanf(head2.size, "%o", &size); // Convert Octal to Decimal
                int n = (size + 512 - 1) / 512; // number of blocks of the contents
                char contents[BLOCKSIZE];
                int re2;
                for (int i = 0; i < n; i++) {
                    memset(contents, 0, BLOCKSIZE);

                    if ((re2 = read(fd, &contents, BLOCKSIZE)) < 0) {
                        close(fd);
                        perror("cp_directory_out read in error");
                        exit(EXIT_FAILURE);
                    }

                    char tmp[strlen(contents)];
                    strncpy(tmp, contents, strlen(contents));

                    write_contents2(fd, fd2, tmp, strlen(contents));
                }
                close(fd2);

                if (lseek(fd, seek_after_header, SEEK_SET) < 0) {
                    close(fd);
                    perror("cp_directory seek2");
                    exit(EXIT_FAILURE);
                }

            }
            skip_contents(fd, head2);
        }


        if (re < 0) {
            close(fd);
            perror("cp_directory_out read error");
            exit(EXIT_FAILURE);
        }

        close(fd);
        return 1;
    } else { // if there's already a file with the same name
        struct stat stat_dest;
        if (lstat(dest, &stat_dest) < 0) {
            close(fd);
            perror("cp_directory_out lstat error");
            exit(EXIT_FAILURE);
        }

        if (! S_ISDIR(stat_dest.st_mode)) {
            close(fd);
            perror("impossible de déplacer  : Le fichier existe");
            exit(EXIT_FAILURE);
        } else {
            char tmp[BLOCKSIZE];
            strcpy(tmp, dest);
            strcat(tmp, "/");
            strcat(tmp, get_last_of_path(source, '/'));
            DIR* dir = opendir(tmp);

            // if it is not empty
            int nb_files = 0;
            if (file_exist(tmp) == 1) {
                while (readdir(dir) != NULL) {
                    if (++nb_files > 2) {
                        close(fd);
                        perror("impossible de déplacer  : Le dossier n'est pas vide");
                        exit(EXIT_FAILURE);
                    }
                }

                // if it is empty
                rmdir(dest);
                return cp_directory_out(fd, source, dest, 1);
            }
        }
    }

    return 0;
}

/**
 * cp folders and files in tar to tar
 */
int tarToOut(int argc, char ** argv) {

    char* tar;
    char* source;
    char* dest;

    if (argc == 3) {
        tar = get_path_until_tar(argv[1]);
        source = get_path_after_tar(argv[1]);
        dest = argv[2];
    } else {
        tar = get_path_until_tar(argv[2]);
        source = get_path_after_tar(argv[2]);
        dest = argv[3];
    }


    int fd = open(tar, O_RDWR);
    if (fd < 0) {
        close(fd);
        write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }

    struct header argHead = nameToHeader(fd, source);

    if (argc == 3 && argHead.typeflag == '5') {
        close(fd);
        perror("cp: -r non spécifié ; omission du répertoire ");
        exit(EXIT_FAILURE);
    }

    if (argHead.typeflag != '5') {
        return cp_no_directory_out(fd, source, dest);
    }
    else {
        return cp_directory_out(fd, source, dest, 0);
    }

    return 0;
}

/**
 * add out no directory into the tar
 */
int out_no_directory_to_tar(int fd, char* source, char* dest, struct stat stat_source) {

    // supposed 1 if dest already exist, 0 if not
    // if dest exists we write the header into a other directory
    // else we change the name too
    int inDirectory = 0;

    char s[100];
    memset(s, '\0', 100);

    // check condiitons and choose appropriate name
    if (exists_header(fd, dest) == 1) {
        struct header head_dest = nameToHeader(fd, dest);
        if (head_dest.typeflag != '5')
            remove_file_2(fd, dest);
        else {
            inDirectory = 1;
            strcpy(s, head_dest.name);
            strcat(s, get_last_of_path(source, '/'));

            if (strcmp (get_last_of_path(source, '/'), get_last_of_path(dest, '/')) == 0) {
                close(fd);
                perror("impossible d'écraser le répertoire par un non répertoire");
                exit(EXIT_FAILURE);
            }
        }
    }

    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("out_no_directory_to_tar seek");
        exit(EXIT_FAILURE);
    }
    struct header new_header;

    if (inDirectory == 0) {
        new_header = create_header(stat_source, dest);
    }
    else {
        new_header = create_header(stat_source, s);
    }

    add_file_in_tar(fd, source, new_header);
    close(fd);
    return 1;
}

/**
 * cp a directory (already in the tar) in the tar
 */
int out_directory_to_tar(int fd, char* source, char* dest, struct stat stat_source) {

    // supposed 1 if dest already exist, 0 if not
    // if dest exists we write the header into a other directory
    // else we change the name too
    int inDirectory = 1;

    char s[100];
    memset(s, '\0', 100);
    struct header head_dest;

    // check conditions and choose appropriate name
    if (exists_header(fd, dest) == 1) {
        head_dest = nameToHeader(fd, dest);
        if (head_dest.typeflag != '5') {
            close(fd);
            perror("impossible d'écraser le non répertoire");
            exit(EXIT_FAILURE);
        }

        strcpy(s, head_dest.name);
        strcat(s, get_last_of_path(source, '/'));

        if (exists_header(fd, s) == 1) {
            struct header s_header = nameToHeader(fd, s);
            if (s_header.typeflag != '5') {
                close(fd);
                perror("impossible d'écraser le non répertoire");
                exit(EXIT_FAILURE);
            }
            if (is_empty_directory(fd, s_header) == 0)
                remove_file_2(fd, s_header.name);
            else {
                close(fd);
                perror("impossible d'écraser le répertoire non vide");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        inDirectory = 0;
    }

    off_t current = lseek(fd, 0L, SEEK_CUR);
    if (current < 0) {
        close(fd);
        perror("out_directory_to_tar seek");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 0L, SEEK_SET) < 0) {
        close(fd);
        perror("out_directory_to_tar seek1");
        exit(EXIT_FAILURE);
    }

    struct header new_header;


    // create the source directory in the tar
    if (inDirectory == 0) {
        new_header = create_header(stat_source, add_slash_at_the_end(dest));
        add_file_in_tar(fd, source, new_header);
    }
    else {
        new_header = create_header(stat_source, s);
        strcat(new_header.name, "/");
        add_file_in_tar(fd, source, new_header);
    }

    struct dirent *files;
    char content_name[100];
    char tmp[100];

    // read all files and folders in source directory
    // and add it into tar
    DIR *dir = opendir(source);
    if (dir != NULL) {
        while ((files = readdir(dir)) != NULL) {
            memset(content_name, '\0', 100);
            memset(tmp, '\0', 100);
            struct stat stat_source2;
            struct header head2;
            if (strcmp(files->d_name, ".") == 0)
                continue;
            if (strcmp(files->d_name, "..") == 0)
                continue;
            char file_name[BLOCKSIZE];
            strcpy(file_name, source);
            strcat(file_name, "/");
            strcat(file_name, files->d_name);

            if (lstat(file_name, &stat_source2) < 0) {
                close(fd);
                write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
                exit(EXIT_FAILURE);
            }
            else if (inDirectory == 0) {
                strcpy(tmp, dest);
                strncpy(content_name, file_name + strlen(source), strlen(file_name));
                strcat(tmp, content_name);
                if (S_ISDIR(stat_source2.st_mode) == 1) {
                    out_directory_to_tar(fd, file_name, tmp, stat_source2);
                } else {
                    head2 = create_header(stat_source2, tmp);
                    add_file_in_tar(fd, file_name, head2);
                }
            }
            else { //dest is a directory
                strcpy(tmp, s);
                strncpy(content_name, file_name + strlen(source), strlen(file_name));
                strcat(tmp, content_name);
                if (S_ISDIR(stat_source2.st_mode) == 1) {
                    out_directory_to_tar(fd, file_name, tmp, stat_source2);
                } else {
                    head2 = create_header(stat_source2, tmp);
                    add_file_in_tar(fd, file_name, head2);
                }
            }
        }
    } else {
        close(fd);
        closedir(dir);
        perror("opendir fail");
        exit(EXIT_FAILURE);
    }
    closedir(dir);

    if (lseek(fd, current, SEEK_SET) < 0) {
        close(fd);
        perror("out_directory_to_tar seek2");
        exit(EXIT_FAILURE);
    }

    return 1;
}


/**
 * cp out folders and out files into tar
 */
int outToTar(int argc, char ** argv) {
    char* tar;
    char* source;
    char* dest;

    if (argc == 3) {
        tar = get_path_until_tar(argv[2]);
        source = argv[1];

        if (get_path_after_tar(argv[2]) != NULL)
            dest = get_path_after_tar(argv[2]);
        else
            dest = get_last_of_path(source, '/');
    } else {
        tar = get_path_until_tar(argv[3]);
        source = argv[2];

        if (get_path_after_tar(argv[3]) != NULL)
            dest = get_path_after_tar(argv[3]);
        else
            dest = get_last_of_path(source, '/');
    }

    int fd = open(tar, O_RDWR);
    if (fd < 0) {
        close(fd);
        write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }

    struct stat stat_source;
    if (lstat(source, &stat_source) < 0) {
        close(fd);
        write(STDERR_FILENO, "cp: no such file or directory\n\0", 31);
        exit(EXIT_FAILURE);
    }

    if (argc == 3 && S_ISDIR(stat_source.st_mode)) {
        close(fd);
        perror("cp: -r non spécifié ; omission du répertoire ");
        exit(EXIT_FAILURE);
    }

    if (! S_ISDIR(stat_source.st_mode)) {
        out_no_directory_to_tar(fd, source, dest,  stat_source);
    } else {
        out_directory_to_tar(fd, source, dest, stat_source);
        close(fd);
    }

    return 1;
}

int main(int argc, char ** argv)
{

    if ((argc == 3 && has_tar_in_path(argv[1]) && has_tar_in_path(argv[2])) ||
    (argc == 4 && (strcmp(argv[1], "-r") == 0) && has_tar_in_path(argv[2]) && has_tar_in_path(argv[3]))) {
        return cp_TarToTar(argc, argv);
    }
    else if ((argc == 3 && has_tar_in_path(argv[1])) ||
    (argc == 4 && (strcmp(argv[1], "-r") == 0) && has_tar_in_path(argv[2]))) {
        return tarToOut(argc, argv);
    }
    else if ((argc == 3  && has_tar_in_path(argv[2])) ||
    (argc == 4 && (strcmp(argv[1], "-r") == 0) && has_tar_in_path(argv[3]))){
        return outToTar(argc, argv);
    }

    write(STDERR_FILENO, "cp: can not execute the command\n\0", 33);
    return 0;
}
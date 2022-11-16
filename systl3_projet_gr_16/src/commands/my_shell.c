#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

#include "../include/tar_parser.h"

#define R_BUFFSIZE 1024

char *home;              // $HOME
char cwd[R_BUFFSIZE];    // $PWD
char old_wd[R_BUFFSIZE]; // $OLDPWD

int inTar = 0;

// Absolute paths to executable
char p_ls[R_BUFFSIZE];
char p_cat[R_BUFFSIZE];
char p_mkdir[R_BUFFSIZE];
char p_rmdir[R_BUFFSIZE];
char p_rm[R_BUFFSIZE];
char p_cp[R_BUFFSIZE];
char p_mv[R_BUFFSIZE];

void pipe_handler(char *cmd);

/**
 * Display basic tsh data
 */
void displayBasicTsh() {
    write(STDOUT_FILENO, "tsh > ", 6);
    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, " # \0", 4);
}

/**
 * Innitiate absolute paths
 * to tsh executables (commands)
 * It requires to launch tsh
 * in the root of the project directory :
 * "systl3_projet_gr_16"
 */
void initExec() {
    int len = strlen(cwd);

    strcpy(p_ls, cwd);
    strcpy(p_ls + len, "/src/exec/./my_ls");

    strcpy(p_cat, cwd);
    strcpy(p_cat + len, "/src/exec/./my_cat");

    strcpy(p_mkdir, cwd);
    strcpy(p_mkdir + len, "/src/exec/./my_mkdir");

    strcpy(p_rmdir, cwd);
    strcpy(p_rmdir + len, "/src/exec/./my_rmdir");

    strcpy(p_rm, cwd);
    strcpy(p_rm + len, "/src/exec/./my_rm");

    strcpy(p_cp, cwd);
    strcpy(p_cp + len, "/src/exec/./my_cp");

    strcpy(p_mv, cwd);
    strcpy(p_mv + len, "/src/exec/./my_mv");
}

/**
 * Remove '.' and '..'
 * from the path
 * Returns an absolute path
 * Supposing that all files do exist
 * The absolute path has the extra '/',
 * and the '/' at the end removed
 */
char *get_abs_path_no_dots(char *path) {
    if (path == NULL)
        return NULL;

    char abs_path[R_BUFFSIZE]; // Absolute path to use
    int abs_len;

    // Concat cwd and path
    if (path[0] != '/') {
        strcpy(abs_path, cwd);
        abs_len = strlen(abs_path);
        abs_path[abs_len++] = '/';
        strcpy(abs_path + abs_len, path);
    }
    else // path is already an absolute path
        strcpy(abs_path, path);

    return clearDots(abs_path);
}

/**
 * Checks if a path is valid in a tarall,
 * if isDir == 1, we check in out tarball
 * the name with a '/' at the end
 * else we don't add anything to the name.
 * returns -1 if the path is NOT valid,
 * else returns typeflag (from header tar)
 *
 * see https://en.wikipedia.org/wiki/Tar_%28computing%29
 * for typeflag values
 */
int validPathType(char *path, int isDir) {
    struct header head;
    int fd;
    char *path_til_tar;
    int len1;
    char path_after_tar[R_BUFFSIZE];
    int len2;
    int size;

    // No tarball in path
    if (!has_tar_in_path(path))
        return -1;

    // Nothing after the tarball (path/to/toto.tar)
    if (has_tar_in_path(get_last_of_path(path, '/')) && access(path, F_OK) != -1)
        return 5; // 5 == DIR

    // Could not get path until tar
    if ((path_til_tar = get_path_until_tar(path)) == NULL)
        return -1;

    // Creating path_after_tar
    len1 = strlen(path_til_tar);
    strcpy(path_after_tar, path + len1 + 1);
    len2 = strlen(path_after_tar);
    if (isDir) {
        path_after_tar[len2] = '/'; // directories in a tar are as : dirA/
        len2++;
        path_after_tar[len2] = '\0';
    }

    // Open path/to/toto.tar
    if ((fd = open(path_til_tar, O_RDONLY)) == -1) {
        free(path_til_tar);
        return -1;
    }

    // Look for the name in tar
    while (read(fd, &head, BLOCKSIZE) > 0) {
        if (!strcmp(head.name, path_after_tar)) { // Found name
            free(path_til_tar);
            close(fd);
            return head.typeflag - '0';
        }

        else { // Skip file contents
            sscanf(head.size, "%o", &size); // Convert octal size to decimal
            size = (size + 512 - 1) / 512;

            for (int i = 0; i < size; i++) {
                if (read(fd, &head, BLOCKSIZE) < 0) {
                    perror("Error skipping tar files' content");
                    free(path_til_tar);
                    close(fd);
                    return -1;
                }
            }
        }
    }

    free(path_til_tar);
    close(fd);

    return -1;
}

/**
 * Executes pwd command
 * Displays in STDOUT the current
 * working directory
 */
void do_pwd(int len) {
    if (len > 1)
        write(STDERR_FILENO, "pwd: too many arguments\n\0", 25);
    else {
        write(STDOUT_FILENO, cwd, strlen(cwd));
        write(STDOUT_FILENO, "\n\0", 2);
    }
}

/**
 * Executes cd command
 * It uses tarballs as normal directories
 * and will work whether we use or not a tarball
 */
void do_cd(int argc, char **cmd) {
    char *args[2];
    args[0] = "cd";

    if (argc == 1) { // cd no arguments
        args[1] = home;
        do_cd(2, args);
        inTar = 0;
    }

    else if (argc != 2)
        write(STDOUT_FILENO, "cd: too many arguments\n\0", 24);

    else if (!strcmp(cmd[1], "-")) { // cd -
        args[1] = old_wd;
        do_cd(2, args);
    }

    // Normal cd one argument OR cd /absolute/path/no/tar
    else if (((!inTar || cmd[1][0] == '/') && !has_tar_in_path(cmd[1]))) {
        if (chdir(cmd[1]) == -1) // do cd arg (not tar)
            perror("cd");

        else { // change pwds
            strcpy(old_wd, cwd); // Set old wd
            strcpy(cwd, getcwd(NULL, 0)); // Set cwd
            inTar = has_tar_in_path(cwd) ? 1 : 0;
        }
    }

    // cd concerning a tarball
    else if (inTar || has_tar_in_path(cmd[1])) {
        char *new_abs_path = get_abs_path_no_dots(cmd[1]);
        if (new_abs_path == NULL) {
            perror("cd: clear dots");
            return;
        }

        // Actually not a cd concerning a tarball
        if (!has_tar_in_path(new_abs_path)) {
            args[1] = new_abs_path;
            do_cd(2, args);
        }

        else if (validPathType(new_abs_path, 1) == 5) {
            strcpy(old_wd, cwd); // Set old wd
            strcpy(cwd, new_abs_path); // Set cwd
            inTar = 1;
        }

        else
            write_cd_err(cmd[1]);
        free(new_abs_path);
    }
}

/**
 * Executes one of the following command :
 * ls | ls -r | cat | mkdir | rmdir | rm | rm -r
 * Uses tarballs as directories
 */
void do_cmd(char **cmd, int len, char *cmd_name) {
    pid_t fd;
    fd = fork();

    switch (fd) {
        case -1 :
            perror("fork");
        case 0 : // CHILD
            signal(SIGINT, SIG_DFL); // Restore SIGINT for child

            // tsh command (except cp and mv)
            if ((len == 2 || len == 3)
                && strcmp(cmd_name, "mv") && strcmp(cmd_name, "cp")
                && (inTar || has_tar_in_path(cmd[len - 1]))) {

                // Absolute path for command
                char *new_abs_path = get_abs_path_no_dots(cmd[len - 1]);
                if (new_abs_path == NULL) {
                    perror("clear dots");
                    exit(EXIT_FAILURE);
                }

                // Set new path to command
                cmd[len - 1] = new_abs_path;

                // Re-check command for tarballs
                if (!has_tar_in_path(new_abs_path))
                    execvp(cmd[0], cmd);

                else {
                    if (!strcmp(cmd_name, "ls"))
                        cmd[0] = p_ls;
                    else if (!strcmp(cmd_name, "cat"))
                        cmd[0] = p_cat;
                    else if (!strcmp(cmd_name, "rmdir"))
                        cmd[0] = p_rmdir;
                    else if (!strcmp(cmd_name, "mkdir"))
                        cmd[0] = p_mkdir;
                    else if (!strcmp(cmd_name, "rm"))
                        cmd[0] = p_rm;

                    execvp(cmd[0], cmd);
                }
                free(new_abs_path);
            }

            // In tar, mv or cp
            else if ((len == 3 || len == 4) && (inTar
                || has_tar_in_path(cmd[len - 1])
                || has_tar_in_path(cmd[len - 2]))) {

                // Absolute path for command
                char *abs_path_src = get_abs_path_no_dots(cmd[len - 2]);
                if (abs_path_src == NULL) {
                    perror("clear dots");
                    exit(EXIT_FAILURE);
                }

                char *abs_path_dest = get_abs_path_no_dots(cmd[len - 1]);
                if (abs_path_dest == NULL) {
                    free(abs_path_src);
                    perror("clear dots");
                    exit(EXIT_FAILURE);
                }

                // Set new paths to command
                cmd[len - 1] = abs_path_dest;
                cmd[len - 2] = abs_path_src;

                // Re-check command for tarballs
                if (!has_tar_in_path(abs_path_src)
                && !has_tar_in_path(abs_path_dest))
                    execvp(cmd[0], cmd);

                else {
                    if (!strcmp(cmd_name, "cp"))
                        cmd[0] = p_cp;
                    else if (!strcmp(cmd_name, "mv"))
                        cmd[0] = p_mv;

                    execvp(cmd[0], cmd);
                }

                free(abs_path_src);
                free(abs_path_dest);
            }

            else // Normal command
                execvp(cmd[0], cmd);
            exit(EXIT_SUCCESS);
        default : // PARENT
            wait(NULL);
    }
}

/**
 * Executing normal commands
 * of a shell
 * (not concerning tarballs)
 */
void do_normal_command(char **cmd) {
    pid_t fd;
    fd = fork();

    switch (fd) {
        case -1 :
            perror("fork");
        case 0 : // CHILD
            signal(SIGINT, SIG_DFL); // Restore SIGINT for child

            if (execvp(cmd[0], cmd) < 0) {
                perror("tsh : command not found");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        default : // PARENT
            wait(NULL);
    }
}

/**
 * Calls do_cmd multiple times when multiple
 * file arguments are present (for ls)
 */
void my_loop_ls(char **full_cmd, int len, int option) {
    char *loop_cmd[3 + option]; // usage : ls (-l) dirA
    int i;

    // ls with at most one file as an argument (normal display)
    if (len <= 2 + option) {
        if (len == 1 || !strcmp(full_cmd[len - 1], "-l")) { // ls or ls -l (no file arg)
            loop_cmd[0] = full_cmd[0];
            if (option)
                loop_cmd[option] = full_cmd[option];
            loop_cmd[1 + option] = ".";
            loop_cmd[2 + option] = NULL;

            do_cmd(loop_cmd, 2 + option, loop_cmd[0]);
        }

        else
            do_cmd(full_cmd, len, full_cmd[0]);
    }

    else { // ls with multiple files as arguments (special display)
        loop_cmd[0] = full_cmd[0]; // ls
        if (option)
            loop_cmd[1] = full_cmd[1]; // -l

        loop_cmd[2 + option] = NULL;

        // Looping for every arguments
        for (i = 1 + option; i < len; i++) {
            loop_cmd[1 + option] = full_cmd[i]; // Pick new file

            write(STDOUT_FILENO, loop_cmd[1 + option], strlen(loop_cmd[1 + option]));
            write(STDOUT_FILENO, ":\n\0", 3);

            do_cmd(loop_cmd, 2 + option, loop_cmd[0]);
            if (i < len - 1)
                write(STDOUT_FILENO, "\n\0", 2);
        }
    }
}

/**
 * Handles the command to execute
 * Using tsh commands
 * or normal shell commands
 */
void command_handler(char *command) {
    char **cmd = str_to_arr(command, ' ');
    int len = get_path_length(command, ' '); // cmd length (NOT NULL)
    char *full_cmd[len + 1]; // NULL terminated cmd
    int option = 0; // option to command
    int i;

    // Error handling
    if (cmd == NULL) {
        return;
    }

    // Copy cmd -> full_cmd (except first argument)
    for (i = 0; i < len; i++)
        full_cmd[i] = cmd[i];
    full_cmd[len] = NULL;

    // Compare the command name with our tsh commands
    if (!strcmp(cmd[0], "cd"))
        do_cd(len, cmd);

    else if (!strcmp(cmd[0], "pwd"))
       do_pwd(len);

    else if (!strcmp(cmd[0], "exit")) {
        if (len == 1)
            exit(EXIT_SUCCESS);
        else if (len == 2)
            exit(atoi(cmd[1]));
        else
            write(STDOUT_FILENO, "exit: too many arguments\n\0", 26);
    }

    else if (!strcmp(cmd[0], "ls")) {
        if (len >= 2 && !strcmp(full_cmd[1], "-l"))
            option = 1; // Option -l

        my_loop_ls(full_cmd, len, option);
    }

    else if (!strcmp(cmd[0], "cat") || !strcmp(cmd[0], "mkdir")
        || !strcmp(cmd[0], "rmdir") || !strcmp(cmd[0], "rm")) {

        if (len == 1)
            do_cmd(full_cmd, len, cmd[0]);
        else {
            if (len >= 2 && !strcmp(full_cmd[1], "-r"))
                option = 1; // for rm -r

            char *loop_cmd[3 + option]; // cmd (-r) arg
            loop_cmd[0] = full_cmd[0]; // cmd

            if (option)
                loop_cmd[1] = full_cmd[1]; // -r

            loop_cmd[2 + option] = NULL;

            // Looping for every arguments
            for (i = 1 + option; i < len; i++) {
                loop_cmd[1 + option] = full_cmd[i]; // Pick new file (arg)

                do_cmd(loop_cmd, 2 + option, loop_cmd[0]);
            }
        }
    }

    else if (!strcmp(cmd[0], "cp") || !strcmp(cmd[0], "mv")) {
        if (len >= 4 && !strcmp(full_cmd[1], "-r"))
            option = 1; // for cp -r src dest

        char *loop_cmd[4 + option]; // cmd (-r) src dest
        loop_cmd[0] = full_cmd[0]; // cmd

        if (option)
            loop_cmd[1] = full_cmd[1]; // -r

        loop_cmd[2 + option] = full_cmd[len - 1];
        loop_cmd[3 + option] = NULL;

        // Looping for every arguments
        for (i = 1 + option; i < len - 1; i++) {
            loop_cmd[1 + option] = full_cmd[i]; // Pick new file (src)

            do_cmd(loop_cmd, 3 + option, loop_cmd[0]);
        }
    }

    // Executing other normal commands (non - tsh)
    else
        do_normal_command(full_cmd);

    if (cmd != NULL)
        free_arr(cmd, len);
}

/**
 * Handles a part of the redirection <
 * when the file is in a tarball
 * We simulate the redirection
 * with a pipe :
 * cmd < file == cat file | cmd
 */
void redirection_tar_in(char *name, char *cmd) {
    if (name == NULL || cmd == NULL)
        exit(EXIT_FAILURE);

    // Create cat command (for tarballs)
    int len1 = strlen(name);
    int len2 = strlen(cmd);
    char cmd_in[len1 + len2 + 6];

    // Create a command to replicate the '<' output
    memcpy(cmd_in, "cat ", 4);
    memcpy(cmd_in + 4, name, len1);
    memcpy(cmd_in + 4 + len1, "|", 1);
    memcpy(cmd_in + 5 + len1, cmd, len2);
    memcpy(cmd_in + 5 + len1 + len2, "\0", 1);

    pipe_handler(cmd_in);
}

/**
 * Checks if a path in a tar results in a directory,
 * or if it does not exist
 * Returns 1 if the path is not a directory, or
 * if it does not exist, else 0
 */
int canRedirectToTar(char *name) {
    char *absPath = get_abs_path_no_dots(name);
    if (absPath == NULL)
        return 0;

    char *tarName = get_path_until_tar(absPath);
    if (tarName == NULL) {
        free(absPath);
        return 0;
    }

    char *pathAfterTar = get_path_after_tar(absPath);
    if (pathAfterTar == NULL) {
        free(absPath);
        free(tarName);
        return 0;
    }

    int fd = open(tarName, O_RDONLY);
    if (fd == -1) {
        free(absPath);
        free(tarName);
        free(pathAfterTar);
        return 0;
    }

    if (exists_header(fd, pathAfterTar) < 1) { // Does not exist
        free(absPath);
        free(tarName);
        free(pathAfterTar);
        close(fd);

        return 1;
    } else { // exists in tar
        struct header head = nameToHeader(fd, pathAfterTar);
        free(absPath);
        free(tarName);
        free(pathAfterTar);
        close(fd);

        return head.typeflag != '5'; // We don't want a directory
    }
}

/**
 * Handles a part of the redirection >
 * when the file is in a tarball
 * We create a temporary file to copy,
 * and delete it after
 */
void redirection_tar_out(char *name, char *cmd) {
    if (name == NULL || cmd == NULL)
        exit(EXIT_FAILURE);

    char tmp_file[20] = "/tmp/.temp_file_cpy\0";
    int fd = open(tmp_file, O_WRONLY | O_CREAT , 0644);

    if (fd == -1) {
        write(STDERR_FILENO, "Could not open the file\n\0", 25);
        return;
    }

    int len = strlen(name);
    char cmd_out[len + 24]; // 24 = for cp command, and strlen tmp_file

    // Create cp command (for tarballs)
    memcpy(cmd_out, "cp ", 3);
    memcpy(cmd_out + 3, tmp_file, 19);
    memcpy(cmd_out + 22, " ", 1);
    memcpy(cmd_out + 23, name, len);
    memcpy(cmd_out + 23 + len, "\0", 1);

    // Redirect to tmp_file
    dup2(fd, STDOUT_FILENO);
    close(fd);
    command_handler(cmd);
    close(fd);

    // Execute cp tmp_file name
    command_handler(cmd_out);
    unlink(tmp_file); // remove temporary file
}


/**
 * Handles simple file redirection '<' and '>'
 * We allow only one redirection per command
 */
void redirection_handler(char *cmd) {
    pid_t pid;
    char **cmd_r;
    char *name;
    char **tmp;
    int fd = -1;

    // Handles redirection
    if (isValidRedirection(cmd))
    {
        pid = fork();

        switch(pid) {
            case -1 :
                perror("fork");
                exit(EXIT_FAILURE);

            case 0 : // CHILD
                if (occ_in_str(cmd, '<')) {
                    if ((cmd_r = str_to_arr(cmd, '<')) == NULL)
                        exit(EXIT_FAILURE);

                    // Removes extra spaces from name to open
                    if ((tmp = str_to_arr(cmd_r[1], ' ')) == NULL) {
                        free_arr(cmd_r, 2);
                        exit(EXIT_FAILURE);
                    }

                    name = tmp[0];

                    if (inTar || has_tar_in_path(name))
                    {
                        redirection_tar_in(name, cmd_r[0]);
                        free_arr(tmp, 1);
                        free_arr(cmd_r, 2);
                        exit(EXIT_SUCCESS);
                    }

                    else // Normal redirection
                    {
                        // Open file for redirection
                        fd = open(name, O_RDONLY);
                        if (fd == -1) {
                            write(STDERR_FILENO, "Could not open the file\n\0", 25);
                            free_arr(cmd_r, 2);
                            free_arr(tmp, 1);
                            exit(EXIT_FAILURE);
                        }

                        // Redirect <
                        dup2(fd, STDIN_FILENO);
                        close(fd);
                    }

                } else if (occ_in_str(cmd, '>')) {
                    if ((cmd_r = str_to_arr(cmd, '>')) == NULL)
                        exit(EXIT_FAILURE);

                    // Removes extra spaces from name to open
                    if ((tmp = str_to_arr(cmd_r[1], ' ')) == NULL) {
                        free_arr(cmd_r, 2);
                        exit(EXIT_FAILURE);
                    }

                    name = tmp[0];

                    if ((inTar || has_tar_in_path(name)))
                    {
                        if (canRedirectToTar(name))
                            redirection_tar_out(name, cmd_r[0]);
                        else
                            write(STDERR_FILENO, "Could not open the file\n\0", 25);
                        free_arr(tmp, 1);
                        free_arr(cmd_r, 2);
                        exit(EXIT_SUCCESS);
                    }

                    else
                    {
                        // Open file for redirection
                        fd = open(name, O_WRONLY | O_CREAT , 0644);
                        if (fd == -1) {
                            write(STDERR_FILENO, "Could not open the file\n\0", 25);
                            free_arr(cmd_r, 2);
                            free_arr(tmp, 1);
                            exit(EXIT_FAILURE);
                        }

                        // Redirect >
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                }

                // Execute the command
                free_arr(tmp, 1);
                command_handler(cmd_r[0]);
                if (fd != -1)
                    close(fd);
                free_arr(cmd_r, 2);
                exit(EXIT_SUCCESS);

            default : // PARENT
                wait(NULL);
        }

    }
    else
        write(STDERR_FILENO, "tsh: Syntax error for redirection\n\0", 35);
}

/**
 * Signal handler
 * Ignores SIGINT (^C)
 */
void sig_handler() {
    struct sigaction action;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &action, NULL);
}

/**
 * Closes all fds
 */
void close_fds(int fds[][2], int n) {
    for (int i = 0; i < n; i++) {
        close(fds[i][0]);
        close(fds[i][1]);
    }
}

/**
 * Handles execution of commands with pipes,
 * but also normal execution without pipes
 * if pipes are not in the command
 * These commands are equivalent :
 * cmdA| == |cmdA == cmdA
 */
void pipe_handler(char *cmd) {
    int cmd_len = strlen(cmd);
    if (cmd == NULL || cmd_len <= 0)
        return;

    char **cmds = str_to_arr(cmd, '|');
    int len = get_path_length(cmd, '|');

    if (cmds == NULL)
        return;

    pid_t pid;
    int n = len - 1;
    int fds[n][2];

    // Creating pipes
    for (int i = 0; i < n; i++) {
        if (pipe(fds[i]) == -1) {
            perror("pipe");
            free_arr(cmds, len);
            return;
        }
    }

    // Normal execution, no pipes '|' in command
    if (len == 1) {
        if (!contains_char_only(cmd, ' '))
        {
            if (occ_in_str(cmds[0], '<') > 0 ||
                occ_in_str(cmds[0], '>') > 0)
                redirection_handler(cmds[0]); // Redirection
            else
                command_handler(cmds[0]); // No redirection
        }
    }

    // pipes included
    else if (len > 1)
    {
        // Looping for all commands and pipes
        for (int i = 0; i < len; i++)
        {
            pid = fork();

            switch (pid)
            {
                case -1 :
                    perror("fork");
                    free_arr(cmds, len);
                    close_fds(fds, n);
                    exit(EXIT_FAILURE);
                    return;

                case 0 : // CHILD PROCESS
                    if (i == 0) {
                        dup2(fds[0][1], STDOUT_FILENO); // write pipe
                        close_fds(fds, n);
                    } else if (i == len - 1) {
                        dup2(fds[i-1][0], STDIN_FILENO); // read prev pipe
                        close_fds(fds, n);
                    } else {
                        dup2(fds[i-1][0], STDIN_FILENO); // read prev pipe
                        dup2(fds[i][1], STDOUT_FILENO); // write cur pipe
                        close_fds(fds, n);
                    }

                    if (occ_in_str(cmds[i], '<') > 0 ||
                        occ_in_str(cmds[i], '>') > 0)
                        redirection_handler(cmds[i]); // Redirection
                    else
                        command_handler(cmds[i]); // No redirection
                    exit(EXIT_SUCCESS);

                default : // PARENT PROCESS
                    wait(NULL);

                    if (i == 0)
                        close(fds[0][1]);
                    else if (i == len - 1)
                        close(fds[i-1][0]);
                    else {
                        close(fds[i-1][0]);
                        close(fds[i][1]);
                    }
            }
        }
    }

    // Freeing memory
    free_arr(cmds, len);
}

/**
 * Shell, waits for the user to input commands
 * Then executes the commands if possible
 */
int main(int argc, char const *argv[])
{
    if (argc != 1) {
        write(STDERR_FILENO, "Wrong number of arguments\n\0", 27);
        return -1;
    }

    int re;
    char buffer[R_BUFFSIZE]; // Command input stored

    // Storing cwd for the first time
    if (getcwd(cwd, sizeof(cwd)) == NULL || getcwd(old_wd, sizeof(old_wd)) == NULL) {
        perror("cwd");
        return -1;
    }

    // Storing $HOME value
    if ((home = getenv("HOME")) == NULL) {
        perror("getenv HOME");
        return -1;
    }

    // Ignore ^C signal (SIGINT)
    sig_handler();

    initExec(); // Setting absolute paths for our executables
    displayBasicTsh();

    // Read input: (tsh) Shell loop
    while((re = read(STDIN_FILENO, buffer, R_BUFFSIZE)) > 0) {
        buffer[re - 1] = '\0'; // Replace '\n' by '\0'

        pipe_handler(buffer); // executes command, and handles pipes

        displayBasicTsh();
    }

    return 0;
}

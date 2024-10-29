#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>


int main(int argc, char* argv[])
{
    if (argc < 4) {
        fprintf(stderr,
            "Usage: %s <inputFile> <command> <outputFile>[<arg1> <arg2> ...]\n", argv[0]);
        return 1;
    }
    int input_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    
    if (input_fd == -1) {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 1;
    }

    if (output_fd == -1) {
        fprintf(stderr, "Failed to open %s\n", argv[3]);
        return 1;
    }

    char** newargv = (char**)malloc(sizeof(char*) * (1 + argc - 2));

    for (int ix = 2; ix < argc; ix++) {
        newargv[ix - 2] = (char*)argv[ix];
    }
    newargv[argc - 2] = NULL;

    int child_pid = fork();
    if (child_pid == 0) {
        if (strcmp(argv[1], "-") != 0) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (strcmp(argv[3], "-") != 0) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        execve(newargv[0], newargv, NULL);

    }
    close(input_fd);
    const char *words[] = {"pear", "peach", "apple", "orange", "banana"};
    size_t numwords = sizeof(words)/sizeof(words[0]);
    for (size_t i = 0; i < numwords; i++) {
        dprintf(output_fd, "%s\n", words[i]);
    }
    close(output_fd);

    int status;
    pid_t wpid = waitpid(child_pid, &status, 0);
    printf("%s pid is %d. forked %d. "
           "Parent exiting\n",
        argv[0], getpid(), child_pid);
    return wpid == child_pid && WIFEXITED(status) ? WIFEXITED(status) : -1;
}
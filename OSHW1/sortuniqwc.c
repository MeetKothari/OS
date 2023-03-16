// I worked alone 

// References: Figures 1-3 


/* insert #include directives for all needed header files here */
#include <stdio.h>
#include <unistd.h>

// Main structure, provided in Figure 1 of PDF
int main(int argc, char *argv[])
{
    pid_t pid;
    // create first pipe fd1
    int fd1[2];
    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe failed");
        return 1;
    }
    //  fork first child
    pid = fork(); // create first child for sort
    if (pid < 0)
    {
        // fork error
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid == 0)
    {
        printf("I am the child %d\n", getpid()); // first child process, run sort
        dup2(fd1[1], 1); // tie write end of pipe fd1 to standard output (file descriptor 1)
        close(fd1[0]); // close read end of pipe fd1
        execlp("/usr/bin/sort", "sort", NULL); // start the sort command using execlp
        printf("Should not be here after execlp to <sort>\n");
        exit(1);
        // should not get here
    }
    // create second pipe fd2
    int fd2[2];
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    //  fork second child
    pid = fork(); // create second child for uniq
    if (pid < 0)
    {
        // fork error
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid == 0)
    { 

        printf("I am the child %d\n", getpid()); // second child process, run uniq
        dup2(fd1[0], 0); // tie read end of fd1 to standard input (file descriptor 0)
        dup2(fd2[1], 1); // tie write end of fd2 to standard output (file descriptor 1)
        close(fd1[1]);   // close write end of pipe fd1
        close(fd2[0]); // close read end of pipe fd2
        execlp("/usr/bin/uniq", "uniq", NULL); // start the uniq command using execlp
        printf("Should not be here after execlp to <uniq>\n");
        exit(1);
        // should not get here
    }
    // fork third child
    pid = fork(); // create third child for wc -l
        if (pid < 0)
    {
        // fork error
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid == 0)
    {
        printf("I am the child %d\n", getpid()); // third child process, run wc -l
        dup2(fd2[0], 0); // tie read end of fd2 to standard input (file descriptor 0)
        close(fd2[1]); // close write end of pipe fd2
        close(fd1[0]); // close read end of pipe fd1
        close(fd1[1]); // close write end of pipe fd1
        execlp("/usr/bin/wc", "wc", "-l", NULL); // start the wc -l command using execlp
        printf("Should not be here after execlp to <wc -l>\n");
        // should not get here
    }
    // parent process code
    // close both ends of pipes fd1 and fd2
    // wait for third process to end.
    printf("I am the parent %d\n", getpid());
    waitpid(NULL);
    printf("Child Complete\n");
    return 0;
}
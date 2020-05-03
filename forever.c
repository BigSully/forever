// https://linux.die.net/man/2/waitpid
// cc forever.c -O -o forever
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2) exit(1);

    // detach from the terminal parent
    setsid();
    signal (SIGHUP, SIG_IGN);

    int status;

    for(;;){
        time_t forkTime=time(NULL);

        pid_t pid = -1;
        if ( 0 == (pid = fork()) ) {  //child
            fprintf(stderr, "child pid: %d, spawning...\n", getpid());
            char *cwd = getenv("CWD");
            if(cwd != NULL && strlen(cwd) > 0) {
                chdir(cwd);
            }
            char **args=&argv[1];  // start from the second parameter
            execve(args[0], args, envp);  // exec binary with arguments and environment variables in child process
            int errsv = errno;
            fprintf(stderr, "failed to execute, error no: %d \n", errsv);
            exit(1); // break loop in the monitored process in case it forks itself
        }else{ //parent
            fprintf(stderr, "start monitoring %d\n", pid);
            pid_t w = waitpid(pid, &status, WUNTRACED);   // block the main thread

            if (WIFEXITED(status)) {
                fprintf(stderr, "exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(stderr, "killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                fprintf(stderr, "stopped by signal %d\n", WSTOPSIG(status));
            }
            time_t now=time(NULL);
            if( 180 > (now - forkTime) ){  // If the child process keep exit within 180 seconds, stop resume it anymore.
                fprintf(stderr, "abnormally resume, please check manually.\n");
                exit(1);
            }

        }
    }

}


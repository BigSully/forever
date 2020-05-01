// https://developer.apple.com/documentation/corefoundation/cffiledescriptor-ru3#2556086
// cc forever-osx.c -framework CoreFoundation -O
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <sys/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define FOREVER ((CFTimeInterval) 1e20)

static void noteProcDeath(CFFileDescriptorRef fdref, CFOptionFlags callBackTypes, void *info) {
    struct kevent kev;
    int fd = CFFileDescriptorGetNativeDescriptor(fdref);
    kevent(fd, NULL, 0, &kev, 1, NULL);
    // take action on death of process here
    printf("process with pid '%u' died\n", (unsigned int)kev.ident);
    CFFileDescriptorInvalidate(fdref);
    CFRelease(fdref); // the CFFileDescriptorRef is no longer of any use in this example
}

static void monitor(int pid){
    int fd = kqueue();
    struct kevent kev;
    EV_SET(&kev, pid, EVFILT_PROC, EV_ADD|EV_ENABLE, NOTE_EXIT, 0, NULL);
    kevent(fd, &kev, 1, NULL, 0, NULL);
    CFFileDescriptorRef fdref = CFFileDescriptorCreate(kCFAllocatorDefault, fd, true, noteProcDeath, NULL);
    CFFileDescriptorEnableCallBacks(fdref, kCFFileDescriptorReadCallBack);
    CFRunLoopSourceRef source = CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, fdref, 0);
    CFRunLoopAddSource(CFRunLoopGetMain(), source, kCFRunLoopDefaultMode);
    CFRelease(source);
    // run the run loop forever seconds
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, FOREVER, true);
}


int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2) exit(1);

    // detach from the terminal parent
    setsid();
    signal (SIGHUP, SIG_IGN);

    for(;;){
        time_t forkTime=time(NULL);

        pid_t pid = -1;
        if ( 0 == (pid = fork()) ) {  //child
            printf("child pid: %d, spawning...\n", getpid());
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
            printf("start monitoring %d\n", pid);
            monitor(pid);  // block the main thread
            printf("child exit %d\n", pid);
            time_t now=time(NULL);
            if( 180 > (now - forkTime) ){  // If the child process keep exit within 180 seconds, stop resume it anymore.
                fprintf(stderr, "abnormally resume, please check manually.\n");
                exit(1);
            }

        }
    }

    return 0;
}


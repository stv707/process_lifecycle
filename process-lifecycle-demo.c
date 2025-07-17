#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

void sig_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Child received SIGUSR1, doing CPU-intensive work...\n");

        // CPU-burning loop for ~5 seconds
        time_t start = time(NULL);
        while (time(NULL) - start < 5) {
            for (volatile int i = 0; i < 1000000; i++); // just keep CPU busy
        }

        printf("Child done with CPU work, now sleeping 10s...\n");
        sleep(10); // move to S (sleeping)
    }
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        signal(SIGUSR1, sig_handler);
        printf("Child: PID %d, waiting for SIGUSR1...\n", getpid());
        pause(); // Wait for signal
        printf("Child exiting.\n");
        exit(0);
    } else {
        // Parent process
        printf("Parent: PID %d, Child PID %d\n", getpid(), pid);
        sleep(2);

        printf("Sending SIGUSR1 to child (start work)...\n");
        kill(pid, SIGUSR1);
        sleep(2);

        printf("Sending SIGSTOP to child...\n");
        kill(pid, SIGSTOP);
        sleep(2);

        printf("Sending SIGCONT to child...\n");
        kill(pid, SIGCONT);
        sleep(2);

        printf("Waiting for child to exit...\n");
        sleep(5); // Let child exit, becomes zombie

        printf("Zombie should be visible now. Use: ps -eo pid,ppid,stat,cmd | grep %d\n", pid);
        sleep(10);

        printf("Reaping zombie with wait()...\n");
        wait(NULL);
        printf("Done.\n");
    }

    return 0;
}

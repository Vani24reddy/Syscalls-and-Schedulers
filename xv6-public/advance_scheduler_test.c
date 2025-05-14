#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int p1, p2, p3;
    int pids[3];
#ifdef PRIORITY
    const char *process_names[] = {"first", "second", "third"};
#else
#endif

    p1 = fork();
    if (p1 == 0)
    {
        goto process_A; // If p1 is the child process, go to process_A
    }
    else
    {
        p2 = fork();
        if (p2 == 0)
        {
            goto process_B; // If p2 is the child process, go to process_B
        }
        else
        {
            p3 = fork();
            if (p3 == 0)
            {
                goto process_C; // If p3 is the child process, go to process_C
            }
            else
            {
                goto parent_block; // If in the parent process, go to parent_block
            }
        }
    }

process_A:
    printf(1, "first process A started at %d\n", uptime());
    setPriority(getpid(), -1); // Set priority to keep process sleep
    printf(1, "first process A\n");
    printf(1, "first process A execution completed in %d\n", ticks_running(getpid()));
    exit();

process_B:
    printf(1, "second process B started at %d\n", uptime());
    setPriority(getpid(), -1);
    printf(1, "second process B\n");
    printf(1, "second process B execution completed in %d\n", ticks_running(getpid()));
    exit();

process_C:
    printf(1, "third process C started at %d\n", uptime());
    setPriority(getpid(), -1);
    printf(1, "third process C\n");
    printf(1, "third process C execution completed in %d\n", ticks_running(getpid()));
    exit();

parent_block:
    pids[0] = p1;
    pids[1] = p2;
    pids[2] = p3;

    sleep(50); // Delay before setting priorities

#ifdef PRIORITY
    int priorities[] = {3, 3, 5}; // Specific priorities if PRIORITY is defined
#else
    int priorities[] = {2, 2, 2}; // Default priorities if PRIORITY is not defined
#endif

    // Loop through processes and set priorities
    for (int i = 0; i < 3; i++)
    {
        setPriority(pids[i], priorities[i]);
#ifdef PRIORITY
        printf(1, "%s process priority %d\n", process_names[i], getPriority(pids[i]));
#endif
    }

    sleep(50); // Give some time before waiting for processes to finish

    // Wait for all child processes to finish
    for (int i = 0; i < 3; i++)
    {
        wait();
    }

    exit();
}

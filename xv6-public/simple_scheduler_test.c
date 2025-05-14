

#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int p1, p2, p3;
    int pids[3];
    const char *process_names[] = {"first", "second", "third"};

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
    printf(1, "first process %s started at %d\n", process_names[0], uptime());
    toggle_process_state(getpid(), -1); // Set priority to keep process sleep
    printf(1, "process %s is running\n", process_names[0]);
    printf(1, "process %s execution completed in %d\n", process_names[0], ticks_running(getpid()));
    exit();

process_B:
    printf(1, "first process %s started at %d\n", process_names[1], uptime());
    toggle_process_state(getpid(), -1);
    printf(1, "process %s is running\n", process_names[1]);
    printf(1, "process %s execution completed in %d\n", process_names[1], ticks_running(getpid()));
    exit();

process_C:
    printf(1, "first process %s started at %d\n", process_names[2], uptime());
    toggle_process_state(getpid(), -1);
    printf(1, "process %s is running\n", process_names[2]);
    printf(1, "process %s execution completed in %d\n", process_names[2], ticks_running(getpid()));
    exit();

parent_block:
    pids[0] = p1;
    pids[1] = p2;
    pids[2] = p3;

    sleep(50); // Delay before setting priorities

    // Loop through processes and set priorities
    for (int i = 0; i < 3; i++)
    {
        toggle_process_state(pids[i], 0);
#ifdef SJF
        printf(1, "%s process lenght %d\n", process_names[i], sjf_job_length(pids[i]));
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


#include "types.h"
#include "stat.h"
#include "user.h"
#define SIZE 1024
#define MAX_LINE_LEN 100 // Adjust this if needed
int uniq_c(int f, char opr);
int getstr(int f, char *str, int len);
int strcmp_ignore_chars(const char *str1, const char *str2, int num_chars);
int strcmp_ignore_chars(const char *str1, const char *str2, int num_chars)
{
    if (num_chars == 0)
    {
        return strcmp(str1, str2); // Let strcmp do the full comparison
    }
    for (int i = 0; i < num_chars; i++)
    {
        if (str1[i] != str2[i])
        {
            return (unsigned char)str1[i] - (unsigned char)str2[i]; // Return the difference if characters mismatch
        }
        // Stop comparison if we reach the end of one of the strings before num_chars
        if (str1[i] == '\0' || str2[i] == '\0')
        {
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        }
    }

    return 0; // If the first num_chars characters are equal
}

int getstr(int f, char *str, int len)
{
    int i = 0;
    char current;
    while (i < len - 1)
    {
        int r = read(f, &current, 1);
        if (r < 0)
            return -1;
        if (r == 0)
            break;
        str[i++] = current;
        if (current == '\n' || current == '\r')
            break;
    }
    str[i] = '\0';

    return i; // Return the number of characters read (excluding the 0 terminator)
}

int operation_w(int f, int cha, char opr)
{
    char current[SIZE];
    char x[20][50];
    int i = 0;
    for (int line_num = 0; getstr(f, current, SIZE) > 0; line_num++)
    {
        if (i == 0)
        {
            strcpy(x[i++], current);
            printf(1, "%s", current);
        }
        else
        {
            int is_unique = (opr == 'i') ? (strcmp(x[i - 1], current) != 0)
                                         : (strcmp_ignore_chars(x[i - 1], current, cha) != 0);

            (is_unique) ? (strcpy(x[i++], current), printf(1, "%s", current)) : 0;
        }

        memset(current, 0, SIZE);
    }

    return 0;
}

// Function to read all lines from the file into an array
int read_lines(int f, char x[20][50])
{
    char current[SIZE];
    int lineCount = -1;

    while (getstr(f, current, SIZE) > 0)
    {
        strcpy(x[++lineCount], current);
        memset(current, 0, SIZE);
    }

    return lineCount; // Return the total number of lines read
}

// Function to count occurrences of identical lines
int count_occurrences(char x[20][50], int z, int lineCount)
{
    int count = 1;
    for (int q = z + 1; q <= lineCount; q++)
    {
        if (strcmp(x[z], x[q]) == 0)
        {
            count++;
        }
        else
        {
            break;
        }
    }
    return count;
}

// Function to print the results based on the operation and count
void print_result(char opr, int count, char line[])
{
    (opr == 'c') ? printf(1, "\t%d %s", count, line) : (opr == 'u' && count == 1) ? printf(1, "%s", line)
                                                                                  : (void)0;
}

// Main uniq_c function
int uniq_c(int f, char opr)
{
    char x[20][50];
    int lineCount = read_lines(f, x); // Read all lines into x[] array

    if (lineCount >= 0)
    {
        for (int z = 0; z <= lineCount;)
        {
            int count = count_occurrences(x, z, lineCount); // Count identical lines
            print_result(opr, count, x[z]);                 // Print the result based on the operation
            z += count;                                     // Move to the next distinct line
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    printf(1, "process started at %d time\n", uptime());
    if (argc == 1)
    {
        int f = 0;
        operation_w(f, 0, 'w');
        close(f);
        printf(1, "process %d completed its execution in %d ticks\n", getpid(), ticks_running(getpid()));
        exit();
    }

    if (argc == 2)
    {
        int o = open(argv[1], 0);
        operation_w(o, 0, 'w');
        close(o);
         printf(1, "process %d completed its execution in %d ticks\n", getpid(), ticks_running(getpid()));
        exit();
    }

    // Flags for different options
    int i = 0, c = 0, d = 0;

    // Check command-line options using ternary operators
    (argc > 1 && strcmp(argv[1], "-u") == 0) ? (i = 1) : (argc > 1 && strcmp(argv[1], "-c") == 0) ? (c = 1)
                                                     : (argc > 1 && strcmp(argv[1], "-w") == 0)   ? (d = 1)
                                                                                                  : 0;

    int x = 1, y = 0;

    while (x < argc)
    {

        if (argv[x][0] >= 48 && argv[x][0] <= 57)
        {
            // Handle numeric arguments
            y = atoi(argv[x]);
            x++;
            continue;
        }
        // printf(1, "data:: %s  -- %d\n",argv[x],x);
        // Open the file
        int f = open(argv[x], 0);
        (f == -1) ? printf(1, "") : 0;

        // If the file was successfully opened, perform the operation based on flags
        (d == 1) ? operation_w(f, y, 'w') : 0;
        (i == 1) ? uniq_c(f, 'u') : 0;
        (c == 1) ? uniq_c(f, 'c') : 0;

        // Close the file after processing
        close(f);

        x++;
    }
     printf(1, "process %d completed its execution in %d ticks\n", getpid(), ticks_running(getpid()));
    exit();
}


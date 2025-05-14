

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

// Define constants for options
#define OPTION_NAME 1
#define OPTION_TYPE 2
#define OPTION_PRINTI 3
#define OPTION_INUM 4

// Structure to hold parsed options
struct find_options
{
    char *dir;
    uint file_type;
    char *name;
    int isInum;
    int printi;
    int inum;
    int inumFlag;
};

// Function to get the option index
struct option_map
{
    char *option_name;
    int option_value;
};

// Array of option mappings
struct option_map options[] = {
    {"-name", OPTION_NAME},
    {"-type", OPTION_TYPE},
    {"-printi", OPTION_PRINTI},
    {"-inum", OPTION_INUM}};
void recursive_search(struct find_options *opts, char *current_path);
void find(struct find_options *opts);
// Function to get option index
int get_option_index(char *arg)
{
    int num_options = sizeof(options) / sizeof(options[0]);

    for (int i = 0; i < num_options; i++)
    {
        if (strcmp(arg, options[i].option_name) == 0)
        {
            return options[i].option_value;
        }
    }

    return -1; // Invalid option
}

// Function to print usage instructions
void print_usage()
{
    printf(1, "Usage: find [path] [-name name] [-type f|d] [-printi] [-inum <number>]\n");
}
int is_inode_match(struct find_options *opts, struct dirent *de)
{
    if (!opts->inumFlag)
        return 1;
    if (opts->isInum == 1 && de->inum <= opts->inum)
        return 0;
    if (opts->isInum == 0 && de->inum >= opts->inum)
        return 0;
    if (opts->isInum == -1 && de->inum != opts->inum)
        return 0;
    return 1;
}
int is_name_match(struct find_options *opts, char *entry_name)
{
    if (!opts->name)
        return 1;
    return strcmp(entry_name, opts->name) == 0;
}

int is_type_match(struct find_options *opts, int file_type)
{
    return opts->file_type == 0 || opts->file_type == file_type;
}
void recursive_search(struct find_options *opts, char *current_path)
{
    struct find_options new_opts = *opts;
    new_opts.dir = current_path;
    find(&new_opts);
}
int firstcheck(struct dirent *de)
{
    if (de->inum == 0)
    {
        return 0; // Ignore if inode number is 0
    }

    if (strcmp(de->name, ".") == 0)
    {
        return 0; // Ignore if the entry is the current directory
    }

    if (strcmp(de->name, "..") == 0)
    {
        return 0; // Ignore if the entry is the parent directory
    }
    return 1;
}
// Function to handle finding files and directories
void find(struct find_options *opts)
{
    char dir_path[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(opts->dir, 0)) < 0)
    {
        printf(1, "find: cannot open %s\n", opts->dir);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        printf(1, "find: cannot stat %s\n", opts->dir);
        close(fd);
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (!firstcheck(&de))
            continue;

        // Reset dir_path for each iteration
        strcpy(dir_path, opts->dir);
        p = dir_path + strlen(opts->dir);
        *p++ = '/';
        strcpy(p, de.name);

        // Get stats of the new path
        if (stat(dir_path, &st) < 0)
        {
            printf(1, "find: cannot stat %s\n", dir_path);
            continue;
        }

        if (st.type == T_DIR)
            recursive_search(opts, dir_path);

        if (!is_inode_match(opts, &de) || !is_name_match(opts, de.name))
            continue;

        if (is_type_match(opts, st.type))
        {

            if (opts->printi)
                printf(1, "%d  %s \n", de.inum, dir_path);
            else
                printf(1, "%s\n", dir_path);
        }

        // Recursive call for directories
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    struct find_options opts = {".", 0, '\0', -2, 0, 0, 0};
    int i = 2;
    printf(1, "process started at %d time\n", uptime());
    // Parse command-line arguments
    while (i < argc)
    {
        int option_index = get_option_index(argv[i]);

        if (option_index == -1)
        {
            goto invalid_option; // Jump to invalid option handler
        }

        if (option_index == OPTION_NAME)
        {
            goto handle_name;
        }
        else if (option_index == OPTION_TYPE)
        {
            goto handle_type;
        }
        else if (option_index == OPTION_PRINTI)
        {
            goto handle_printi;
        }
        else if (option_index == OPTION_INUM)
        {
            goto handle_inum;
        }

    handle_name:
        if (i + 1 < argc)
        {
            opts.name = argv[i + 1];
            i += 2;
            continue; // Skip to next argument
        }
        else
        {
            goto usage_error;
        }

    handle_type:
        if (i + 1 < argc && (strcmp(argv[i + 1], "f") == 0 || strcmp(argv[i + 1], "d") == 0))
        {
            opts.file_type = (strcmp(argv[i + 1], "d") == 0) ? T_DIR : T_FILE;
            i += 2;
            continue; // Skip to next argument
        }
        else
        {
            goto usage_error;
        }

    handle_printi:
        opts.printi = 1;
        i++;
        continue; // Skip to next argument

    handle_inum:
        if (i + 1 < argc)
        {
            char *inum_arg = argv[i + 1];
            opts.inum = atoi(inum_arg[0] == '+' || inum_arg[0] == '-' ? inum_arg + 1 : inum_arg);
            opts.isInum = (inum_arg[0] == '+') ? 1 : (inum_arg[0] == '-') ? 0
                                                                          : -1;
            opts.inumFlag = 1;
            i += 2;
            continue; // Skip to next argument
        }
        else
        {
            goto usage_error;
        }

    invalid_option:
        print_usage();
        printf(1, "process %d completed its execution in %d ticks\n", getpid(), ticks_running(getpid()));
        exit();

    usage_error:
        print_usage();
        printf(1, "process %d completed its execution in %d ticks\n", getpid(), ticks_running(getpid()));
        exit();
    }

    // Call the find function with the parsed options
    find(&opts);
     printf(1, "process %d completed its execution in %d ticks\n", getpid(), ticks_running(getpid()));
    exit();
}

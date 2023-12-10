#include <stdio.h>
#include <string.h>
#include <terminal.h>
#include <kernel.h>

#define MEM_SIZE 1024

// void get_args(char *prompt, char **argv, int *argc)
// {
//     int c = 0;
//     char *p = strtok(prompt, " ");
//     while (p != NULL)
//     {
//         argv[c] = malloc(strlen(p));
//         strcpy(argv[c++], p);
//         p = strtok(NULL, " ");
//     }
//     *argc = c;
// }

static int setargs(char *args, char **argv)
{
    int count = 0;

    while (isspace(*args))
        ++args;
    while (*args)
    {
        if (argv)
            argv[count] = args;
        while (*args && !isspace(*args))
            ++args;
        if (argv && *args)
            *args++ = '\0';
        while (isspace(*args))
            ++args;
        count++;
    }
    return count;
}

char **parsedargs(char *args, int *argc)
{
    char **argv = NULL;
    int argn = 0;

    if (args && *args && (args = strdup(args)) && (argn = setargs(args, NULL)) && (argv = malloc((argn + 1) * sizeof(char *))))
    {
        *argv++ = args;
        argn = setargs(args, argv);
    }

    if (args && !argv)
        free(args);

    *argc = argn;
    return argv;
}

void freeparsedargs(char **argv)
{
    if (argv)
    {
        free(argv[-1]);
        free(argv - 1);
    }
}

int run()
{
    int is_running = 1;
    char prompt[MEM_SIZE];
    char **argv;
    int argc;

    while (is_running)
    {
        // int c;
        // while ((c = getchar()) != '\n' && c != EOF)
        //     ;
        printf("\nТаков Путь: ");
        fgets(prompt, MEM_SIZE, stdin);
        argv = parsedargs(prompt, &argc);
        if (argc < 1) {
            continue;
        }
        if (strcmp(argv[0], "mkfile") == 0)
        {
            mkfile(argc, argv);
        }
        else if (strcmp(argv[0], "sb") == 0)
        {
            sb(argc, argv);
        }
        
        
        freeparsedargs(argv);

    }
}

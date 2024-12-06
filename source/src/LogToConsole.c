#include "../include/LogToConsole.h"

void open_console(void)
{
    pid_t pid = fork();
    if(pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if(pid == 0)
    {
        // Child process: open a new terminal
        execlp("gnome-terminal", "gnome-terminal", "--", "sh", "-c", "tail -f /tmp/my_program.log", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process: open a log file for writing
        FILE *log_file = fopen("/tmp/my_program.log", "we");
        if(log_file == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        if(setvbuf(log_file, NULL, _IOLBF, 0) != 0)
        {
            perror("setvbuf");
            fclose(log_file);
            exit(EXIT_FAILURE);
        }

        fclose(log_file);    // Close the file pointer to avoid leaks
    }
}

void log_msg(const char *format, ...)
{
    char  log_buf[BUFSIZE];
    FILE *log_file;
    snprintf(log_buf, sizeof(log_buf), "%s", format);
    log_file = fopen("/tmp/my_program.log", "ae");
    if(log_file != NULL)
    {
        fprintf(log_file, "%s\n", log_buf);
        fclose(log_file);
    }
}

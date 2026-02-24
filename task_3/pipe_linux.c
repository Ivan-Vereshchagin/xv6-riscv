#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 4096

static void
error_msg(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
}

int
main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t pid;
    int status;
    
    if (pipe(pipefd) < 0) {
        error_msg("Pipe error\n");
        exit(EXIT_FAILURE);
    }
    
    pid = fork();
    
    if (pid < 0) {
        error_msg("Fork error\n");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        if (close(pipefd[1]) < 0) {
            error_msg("Close write end error\n");
            exit(EXIT_FAILURE);
        }
        
        if (close(STDIN_FILENO) < 0) {
            error_msg("Close stdin error\n");
            exit(EXIT_FAILURE);
        }
        if (dup(pipefd[0]) != STDIN_FILENO) {
            error_msg("Dup error\n");
            exit(EXIT_FAILURE);
        }
        if (close(pipefd[0]) < 0) {
            error_msg("Close read end error\n");
            exit(EXIT_FAILURE);
        }
        
        char buf[BUF_SIZE];
        ssize_t n;
        
        while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
            ssize_t written = 0;
            
            while (written < n) {
                ssize_t w = write(STDOUT_FILENO, buf + written, n - written);
                if (w < 0) {
                    error_msg("Write to stdout error\n");
                    exit(EXIT_FAILURE);
                }
                written += w;
            }
        }
        
        if (n < 0) {
            error_msg("Read from pipe error\n");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (close(pipefd[0]) < 0) {
            error_msg("Close read end error\n");
            exit(EXIT_FAILURE);
        }
        
        int i;
        for (i = 1; i < argc; i++) {
            char *arg = argv[i];
            size_t len = strlen(arg);
            size_t written = 0;
            
            while (written < len) {
                ssize_t w = write(pipefd[1], arg + written, len - written);
                if (w < 0) {
                    error_msg("Write error\n");
                    exit(EXIT_FAILURE);
                }
                written += w;
            }
            
            if (write(pipefd[1], "\n", 1) != 1) {
                error_msg("Write newline error\n");
                exit(EXIT_FAILURE);
            }
        }
        
        if (close(pipefd[1]) < 0) {
            error_msg("Close write end error\n");
            exit(EXIT_FAILURE);
        }
        
        if (waitpid(pid, &status, 0) < 0) {
            error_msg("Waitpid error\n");
            exit(EXIT_FAILURE);
        }
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) exit(exit_code);
        } else if (WIFSIGNALED(status)) {
            error_msg("Child terminated by signal\n");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_SUCCESS);
    }
}
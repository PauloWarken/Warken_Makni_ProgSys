#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

// definition of constants
#define BUFFER_SIZE 128
#define ERROR_MSG_SIZE 100
#define FORK_FAILED_LEN 12
#define READ_EOF_OR_ERROR -1
#define READ_EXIT_COMMAND 0
#define READ_EMPTY_COMMAND 1
#define READ_VALID_COMMAND 2
#define MAX_ARGS 64
#define MAX_REDIRECTIONS 2

// pointers to strings
char *welcome = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
char *prompt = "\nenseash % ";
char *bye = "Bye bye...\n";

// struct to manage redirections to stdin/stdout
typedef struct {
        char **args;
        char *input_file;
        char *output_file;
    } command_t;

// function prototypes
void print_welcome_message(void);
void print_prompt(int last_status, long execution_time_ms);
void print_bye_message(void);
int read_user_command(char *buffer, int buffer_size);
int execute_simple_command(char *command, long *execution_time_ms);
command_t *parse_command_arguments(char *command);
int execute_pipe_command(char *command, long *execution_time_ms);

int main(int argc, char *argv[]) {
    // definition of variables
    char buffer[BUFFER_SIZE];
    int command_result;
    int last_status = -1;
    long execution_time_ms = 0;

    print_welcome_message();

    // main REPL loop
	while (1) {
        print_prompt(last_status, execution_time_ms);

		command_result = read_user_command(buffer, BUFFER_SIZE);

		// check result of command reading
		if (command_result == READ_EOF_OR_ERROR) {      // ctrl+D or read error
			print_bye_message();
			break;
		} else if (command_result == READ_EXIT_COMMAND) { // exit command
			print_bye_message();
			break;
		} else if (command_result == READ_EMPTY_COMMAND) { // empty command
				   continue;  // skip and show prompt again
		}

        // check if command contains a single pipe
        if (strchr(buffer, '|') != NULL) {
            last_status = execute_pipe_command(buffer, &execution_time_ms);
        } else {
            last_status = execute_simple_command(buffer, &execution_time_ms);
        }
	}

	return 0;
}

void print_welcome_message(void) {
   write(STDOUT_FILENO, welcome, strlen(welcome));
}

void print_prompt(int last_status, long execution_time_ms) {
    char prompt_buffer[BUFFER_SIZE];

    if (last_status == -1) {
        // first prompt, no command executed
        snprintf(prompt_buffer, BUFFER_SIZE, "enseash %% ");
    } else if (WIFEXITED(last_status)) {
        // command finished normaly
        int exit_code = WEXITSTATUS(last_status);
        snprintf(prompt_buffer, BUFFER_SIZE,
                "enseash [exit:%d|%ldms] %% ", exit_code, execution_time_ms);
	} else if (WIFSIGNALED(last_status)) {
        // command finished by a signal
        int signal_num = WTERMSIG(last_status);
        snprintf(prompt_buffer, BUFFER_SIZE,
                "enseash [sign:%d|%ldms] %% ", signal_num, execution_time_ms);
    } else {
        // default outcome
        snprintf(prompt_buffer, BUFFER_SIZE, "enseash %% ");
    }

    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, prompt_buffer, strlen(prompt_buffer));
}

void print_bye_message(void) {
	write(STDOUT_FILENO, bye, strlen(bye));
}

// returns: -1 = EOF/error, 0 = exit command, 1 = empty command, 2 = valid command
int read_user_command(char *buffer, int buffer_size) {
   ssize_t bytes_read;

   bytes_read = read(STDIN_FILENO, buffer, buffer_size - 1);

   // check for EOF (ctrl+D) or read error
   if (bytes_read <= 0) {
	   write(STDOUT_FILENO, "\n", 1);
	   return READ_EOF_OR_ERROR;
   }

   // null-terminate the string
   buffer[bytes_read] = '\0';

   // remove newline character if present
   if (buffer[bytes_read - 1] == '\n') {
	   buffer[bytes_read - 1] = '\0';
	   bytes_read--;
   }

   // check for exit command
   if (strcmp(buffer, "exit") == 0) {
	   return READ_EXIT_COMMAND;
   }

   // check for empty command
   if (bytes_read == 0) {
	   return READ_EMPTY_COMMAND;
   }

   return READ_VALID_COMMAND;  // valid command
}

// returns: 0 on success, -1 on failure
int execute_simple_command(char *command, long *execution_time_ms) {
	pid_t pid;
	int status;
	struct timespec start, end;
    command_t *cmd;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // parse command with redirections
    cmd = parse_command_arguments(command);
    if (!cmd || !cmd->args[0]) {
        if (cmd) {
            // free allocated memory
            for (int i = 0; cmd->args[i]; i++) free(cmd->args[i]);
            free(cmd->args);
            free(cmd->input_file);
            free(cmd->output_file);
            free(cmd);
        }
        write(STDERR_FILENO, "parse error\n", 12);
        return -1;
    }

	// fork a child process
	pid = fork();

	if (pid == 0) {
	// child process

	// handle input redirection
	if (cmd->input_file) {
		int fd_in = open(cmd->input_file, O_RDONLY);
		if (fd_in < 0) {
			char error_msg[ERROR_MSG_SIZE];
			snprintf(error_msg, sizeof(error_msg),
					"cannot open %s for reading\n", cmd->input_file);
			write(STDERR_FILENO, error_msg, strlen(error_msg));
			_exit(1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}

	// handle output redirection
	if (cmd->output_file) {
		int fd_out = open(cmd->output_file,
						 O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_out < 0) {
			char error_msg[ERROR_MSG_SIZE];
			snprintf(error_msg, sizeof(error_msg),
					"cannot open %s for writing\n", cmd->output_file);
			write(STDERR_FILENO, error_msg, strlen(error_msg));
			_exit(1);
		}
		 dup2(fd_out, STDOUT_FILENO);
		 close(fd_out);
	}

	// execute the command
	execvp(cmd->args[0], cmd->args);

	// if execvp returns, there was an error
	char error_msg[ERROR_MSG_SIZE];
	snprintf(error_msg, sizeof(error_msg),
			"command not found: %s\n", cmd->args[0]);
	write(STDERR_FILENO, error_msg, strlen(error_msg));

	// free memory in child
	for (int i = 0; cmd->args[i]; i++) free(cmd->args[i]);
	free(cmd->args);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd);

	_exit(127);

	} else if (pid > 0) {
	// parent process

	// wait for the child to finish
	waitpid(pid, &status, 0);

	// free memory in parent
	for (int i = 0; cmd->args[i]; i++) free(cmd->args[i]);
	free(cmd->args);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd);

	// calculate execution time
	clock_gettime(CLOCK_MONOTONIC, &end);
	*execution_time_ms = (end.tv_sec - start.tv_sec) * 1000;
	*execution_time_ms += (end.tv_nsec - start.tv_nsec) / 1000000;

    return status;

	} else {
		// fork failed
		write(STDERR_FILENO, "fork failed\n", FORK_FAILED_LEN);

		// free memory
		for (int i = 0; cmd->args[i]; i++) free(cmd->args[i]);
		free(cmd->args);
		free(cmd->input_file);
		free(cmd->output_file);
		free(cmd);

		*execution_time_ms = 0;
		return -1;
	}
}

int execute_pipe_command(char *command, long *execution_time_ms) {
    struct timespec start, end;
    char *cmd1, *cmd2;
    char *argv1[MAX_ARGS], *argv2[MAX_ARGS];
    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // find pipe position
    char *pipe_pos = strchr(command, '|');
    if (!pipe_pos) return -1;

    // split string
   *pipe_pos = '\0';
   cmd1 = command;
   cmd2 = pipe_pos + 1;

   // parse first command
   int i = 0;
   char *token = strtok(cmd1, " ");
   while (token && i < MAX_ARGS-1) {
	   argv1[i++] = token;
	   token = strtok(NULL, " ");
   }
   argv1[i] = NULL;

    // parse second command
	i = 0;
	token = strtok(cmd2, " ");
	while (token && i < MAX_ARGS-1) {
	  argv2[i++] = token;
	  token = strtok(NULL, " ");
	}
	argv2[i] = NULL;

	// restore pipe (optional)
	*pipe_pos = '|';

	// create pipe
	if (pipe(pipefd) < 0) {
	  write(STDERR_FILENO, "pipe failed\n", 12);
	  return -1;
	}

	// fork first command
	pid1 = fork();
	if (pid1 == 0) {
	  close(pipefd[0]);
	  dup2(pipefd[1], STDOUT_FILENO);
	  close(pipefd[1]);

	  execvp(argv1[0], argv1);
	  write(STDERR_FILENO, "command not found\n", 18);
	  _exit(1);
	}

	// fork second command
	pid2 = fork();
	if (pid2 == 0) {
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);

		execvp(argv2[0], argv2);
		write(STDERR_FILENO, "command not found\n", 18);
		_exit(1);
	}

	// parent
	close(pipefd[0]);
	close(pipefd[1]);

	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);

	clock_gettime(CLOCK_MONOTONIC, &end);
	*execution_time_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;

	return status;
}

// divides command in arguments
command_t *parse_command_arguments(char *command) {
    command_t *cmd = malloc(sizeof(command_t));
    if (!cmd) return NULL;

    cmd->args = malloc(MAX_ARGS * sizeof(char *));
    cmd->input_file = NULL;
    cmd->output_file = NULL;

    if (!cmd->args) {
        free(cmd);
        return NULL;
    }

    char *tokens[MAX_ARGS];
    int token_count = 0;

    // make a copy to tokenize
	char *command_copy = strdup(command);
	if (!command_copy) {
		free(cmd->args);
		free(cmd);
		return NULL;
	}

	// first pass: collect all tokens
	char *token = strtok(command_copy, " ");
	while (token && token_count < MAX_ARGS - 1) {
		tokens[token_count++] = token;
		token = strtok(NULL, " ");
	}
	tokens[token_count] = NULL;

	// second pass: handle redirections
	int arg_index = 0;
	int i = 0;

	while (i < token_count) {
		if (strcmp(tokens[i], "<") == 0) {
			// input redirection
			if (i + 1 < token_count) {
				cmd->input_file = strdup(tokens[i + 1]);
				i += 2;  // skip '<' and filename
			} else {
				// syntax error: < without filename
				free(command_copy);
				free(cmd->args);
				free(cmd);
				return NULL;
			}
		} else if (strcmp(tokens[i], ">") == 0) {
			// output redirection
			if (i + 1 < token_count) {
				cmd->output_file = strdup(tokens[i + 1]);
				i += 2;  // skip '>' and filename
			} else {
				// syntax error: > without filename
				free(command_copy);
				free(cmd->args);
				free(cmd);
				return NULL;
			}
		} else {
		            // regular argument
		            cmd->args[arg_index++] = strdup(tokens[i]);
		            i++;
		        }
	}
	cmd->args[arg_index] = NULL;

	free(command_copy);
	return cmd;
}

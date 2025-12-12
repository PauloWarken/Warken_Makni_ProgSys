#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

// definition of constants
#define BUFFER_SIZE 128
#define ERROR_MSG_SIZE 100
#define FORK_FAILED_LEN 12
#define READ_EOF_OR_ERROR -1
#define READ_EXIT_COMMAND 0
#define READ_EMPTY_COMMAND 1
#define READ_VALID_COMMAND 2
#define EXEC_SUCCESS 0
#define EXEC_FAILURE -1

// pointers to strings
char *welcome = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
char *prompt = "\nenseash % ";

// function prototypes
void print_welcome_message(void);
void print_prompt(void);
void print_bye_message(void);
int read_user_command(char *buffer, int buffer_size);
int execute_simple_command(char *command);

int main(int argc, char *argv[]) {
    // definition of variables
    char buffer[BUFFER_SIZE];
    int command_result;

    print_welcome_message();

    // main REPL loop
	while (1) {
		print_prompt();

		command_result = read_user_command(buffer, BUFFER_SIZE);

		// check result of command reading
		if (command_result == READ_EOF_OR_ERROR) {      // ctrl+D or read error
			break;
		} else if (command_result == READ_EXIT_COMMAND) { // exit command
			break;
		} else if (command_result == READ_EMPTY_COMMAND) { // empty command
				   continue;  // skip and show prompt again
		}

		execute_simple_command(buffer);
	}

	return 0;
}

void print_welcome_message(void) {
   write(STDOUT_FILENO, welcome, strlen(welcome));
}

void print_prompt(void) {
	write(STDOUT_FILENO, prompt, strlen(prompt));
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
int execute_simple_command(char *command) {
	pid_t pid;
	int status;

	// fork a child process
	pid = fork();

	if (pid == 0) {
	// child process

	// execlp executes the command
	execlp(command, command, NULL);

	// if execlp returns, there was an error
	char error_msg[ERROR_MSG_SIZE];
	snprintf(error_msg, sizeof(error_msg), "Command not found: %s\n", command);
	write(STDERR_FILENO, error_msg, strlen(error_msg));
	_exit(EXIT_FAILURE);

	} else if (pid > 0) {
	// parent process

	// wait for the child to finish
	waitpid(pid, &status, 0);

	// check if child terminated normally
	if (WIFEXITED(status)) {
		return EXEC_SUCCESS;  // success
	} else {
		return EXEC_FAILURE; // child terminated abnormally
	}

	} else {
	  // fork failed
		write(STDERR_FILENO, "Fork failed\n", FORK_FAILED_LEN);
		return EXEC_FAILURE;
	}
}

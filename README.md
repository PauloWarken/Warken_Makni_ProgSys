# Warken_Makni_ProgSys

#QUESTION 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// definition of constants
#define BUFFER_SIZE 128
#define welcome "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'."
#define prompt "\nenseash % "



int main(int argc, char *argv[]) {
    // print welcome message and prompt
    write(STDOUT_FILENO, welcome, strlen(welcome));
    write(STDOUT_FILENO, prompt, strlen(prompt));
}

#QUESTION 2

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

#QUESTION 3

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
char *bye = "Bye bye...\n";

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
			print_bye_message();
			break;
		} else if (command_result == READ_EXIT_COMMAND) { // exit command
			print_bye_message();
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

#QUESTION 4

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

// pointers to strings
char *welcome = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
char *prompt = "\nenseash % ";
char *bye = "Bye bye...\n";

// function prototypes
void print_welcome_message(void);
void print_prompt(int last_status);
void print_bye_message(void);
int read_user_command(char *buffer, int buffer_size);
int execute_simple_command(char *command);

int main(int argc, char *argv[]) {
    // definition of variables
    char buffer[BUFFER_SIZE];
    int command_result;
    int last_status = -1;

    print_welcome_message();

    // main REPL loop
	while (1) {
		print_prompt(last_status);

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

        last_status = execute_simple_command(buffer);
	}

	return 0;
}

void print_welcome_message(void) {
   write(STDOUT_FILENO, welcome, strlen(welcome));
}

void print_prompt(int last_status) {
    char prompt_buffer[BUFFER_SIZE];
    int prompt_length;

    if (last_status == -1) {
        // first prompt, no command executed
        snprintf(prompt_buffer, BUFFER_SIZE, "enseash %% ");
    } else if (WIFEXITED(last_status)) {
        // command finished regularly
        int exit_code = WEXITSTATUS(last_status);
        snprintf(prompt_buffer, BUFFER_SIZE, "enseash [exit:%d] %% ", exit_code);
    } else if (WIFSIGNALED(last_status)) {
    	// command finished by a signal
    	int signal_num = WTERMSIG(last_status);
    	snprintf(prompt_buffer, BUFFER_SIZE, "enseash [sign:%d] %% ", signal_num);
    } else {
    	// default outcome (shouldnt happen)
    	snprintf(prompt_buffer, BUFFER_SIZE, "enseash %% ");
    }

    // puts a return to separate the prompts
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
	_exit(127);

	} else if (pid > 0) {
	// parent process

	// wait for the child to finish
	waitpid(pid, &status, 0);

	// check if child terminated normally
	if (WIFEXITED(status)) {
		return status;  // success
	} else {
		return -1; // child terminated abnormally
	}

	} else {
	  // fork failed
		write(STDERR_FILENO, "Fork failed\n", FORK_FAILED_LEN);
		return -1;
	}
}


#QUESTION 5

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

// definition of constants
#define BUFFER_SIZE 128
#define ERROR_MSG_SIZE 100
#define FORK_FAILED_LEN 12
#define READ_EOF_OR_ERROR -1
#define READ_EXIT_COMMAND 0
#define READ_EMPTY_COMMAND 1
#define READ_VALID_COMMAND 2

// pointers to strings
char *welcome = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
char *prompt = "\nenseash % ";
char *bye = "Bye bye...\n";

// function prototypes
void print_welcome_message(void);
void print_prompt(int last_status, long execution_time_ms);
void print_bye_message(void);
int read_user_command(char *buffer, int buffer_size);
int execute_simple_command(char *command, long *execution_time_ms);

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

        last_status = execute_simple_command(buffer, &execution_time_ms);
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

    clock_gettime(CLOCK_MONOTONIC, &start);	

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
	_exit(127);

	} else if (pid > 0) {
	// parent process

	// wait for the child to finish
	waitpid(pid, &status, 0);

	// ends timer
	clock_gettime(CLOCK_MONOTONIC, &end);
        
	// calculate time in miliseconds
	*execution_time_ms = (end.tv_sec - start.tv_sec) * 1000;
    *execution_time_ms += (end.tv_nsec - start.tv_nsec) / 1000000;
        
	return status;

	} else {
	  // fork failed
		write(STDERR_FILENO, "Fork failed\n", FORK_FAILED_LEN);
		*execution_time_ms = 0;
		return -1;
	}
}


#QUESTION 6

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

// definition of constants
#define BUFFER_SIZE 128
#define ERROR_MSG_SIZE 100
#define FORK_FAILED_LEN 12
#define READ_EOF_OR_ERROR -1
#define READ_EXIT_COMMAND 0
#define READ_EMPTY_COMMAND 1
#define READ_VALID_COMMAND 2

// pointers to strings
char *welcome = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
char *prompt = "\nenseash % ";
char *bye = "Bye bye...\n";

// function prototypes
void print_welcome_message(void);
void print_prompt(int last_status, long execution_time_ms);
void print_bye_message(void);
int read_user_command(char *buffer, int buffer_size);
int execute_simple_command(char *command, long *execution_time_ms);
char **parse_command_arguments(char *command);

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

        last_status = execute_simple_command(buffer, &execution_time_ms);
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
    char **args;

    clock_gettime(CLOCK_MONOTONIC, &start);

	// fork a child process
	pid = fork();

	if (pid == 0) {
	// child process

	args = parse_command_arguments(command);

	// execlp executes the command
    execvp(args[0], args);

	// if execlp returns, there was an error
	char error_msg[ERROR_MSG_SIZE];
	snprintf(error_msg, sizeof(error_msg), "Command not found: %s\n", command);
	write(STDERR_FILENO, error_msg, strlen(error_msg));
    // free allocated memory in child before exiting
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
	_exit(127);

	} else if (pid > 0) {
	// parent process

	// wait for the child to finish
	waitpid(pid, &status, 0);

	// ends timer
	clock_gettime(CLOCK_MONOTONIC, &end);

	// calculate time in miliseconds
	*execution_time_ms = (end.tv_sec - start.tv_sec) * 1000;
    *execution_time_ms += (end.tv_nsec - start.tv_nsec) / 1000000;

	return status;

	} else {
	  // fork failed
		write(STDERR_FILENO, "Fork failed\n", FORK_FAILED_LEN);
		*execution_time_ms = 0;
		return -1;
	}
}

// new function to divide command in arguments
char **parse_command_arguments(char *command) {
    static char *args[BUFFER_SIZE / 2];
    char *token;
    int i = 0;

    // use a copy to not modify the original
    char command_copy[BUFFER_SIZE];
    strncpy(command_copy, command, BUFFER_SIZE - 1);
    command_copy[BUFFER_SIZE - 1] = '\0';

    // divide by spaces
    token = strtok(command_copy, " ");
    while (token != NULL && i < (BUFFER_SIZE / 2) - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  // finish array with NULL

    // copy strings to a static array
   static char *final_args[BUFFER_SIZE / 2];
   for (int j = 0; j < i; j++) {
	   final_args[j] = strdup(args[j]);
   }
   final_args[i] = NULL;

   return final_args;
}


#QUESTION 7

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

        last_status = execute_simple_command(buffer, &execution_time_ms);
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

#QUESTION 8

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

#QUESTION 9

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>

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
#define MAX_BG_JOBS 10

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

// background job structure
typedef struct {
	pid_t pid;
	char command[50];
	int job_id;
} bg_job_t;

// global variables for background jobs
bg_job_t bg_jobs[MAX_BG_JOBS];
int bg_job_count = 0;
int next_job_id = 1;

// function prototypes
void print_welcome_message(void);
void print_prompt(int last_status, long execution_time_ms);
void print_bye_message(void);
int read_user_command(char *buffer, int buffer_size);
int execute_simple_command(char *command, long *execution_time_ms);
command_t *parse_command_arguments(char *command);
int execute_pipe_command(char *command, long *execution_time_ms);
void check_background_jobs(void);
void add_background_job(pid_t pid, char *command);
void remove_background_job(pid_t pid);
void execute_background_command(char *command);
void print_bg_job_start(int job_id, pid_t pid);
void print_bg_job_end(int job_id, pid_t pid, int status, long exec_time_ms);

int main(int argc, char *argv[]) {
	char buffer[BUFFER_SIZE];
	int command_result;
	int last_status = -1;
	long execution_time_ms = 0;

	print_welcome_message();

	// main REPL loop
	while (1) {
		// check for finished background jobs (non-blocking wait)
		check_background_jobs();

		print_prompt(last_status, execution_time_ms);

		command_result = read_user_command(buffer, BUFFER_SIZE);

		if (command_result == READ_EOF_OR_ERROR) {
			print_bye_message();
			break;
		} else if (command_result == READ_EXIT_COMMAND) {
			print_bye_message();
			break;
		} else if (command_result == READ_EMPTY_COMMAND) {
			continue;
		}

		// check if command ends with &
		int len = strlen(buffer);
		int is_background = 0;

		if (len > 0 && buffer[len-1] == '&') {
			is_background = 1;
			buffer[len-1] = '\0';
			// trim trailing spaces
			while (len > 1 && (buffer[len-2] == ' ' || buffer[len-2] == '\t')) {
				buffer[len-2] = '\0';
				len--;
			}
		}

		if (strchr(buffer, '|') != NULL) {
			last_status = execute_pipe_command(buffer, &execution_time_ms);
		} else {
			if (is_background) {
				execute_background_command(buffer);
				last_status = 0;
				execution_time_ms = 0;
			} else {
				last_status = execute_simple_command(buffer, &execution_time_ms);
			}
		}
	}

	return 0;
}

void print_welcome_message(void) {
   write(STDOUT_FILENO, welcome, strlen(welcome));
}

void print_prompt(int last_status, long execution_time_ms) {
    char prompt_buffer[BUFFER_SIZE];
    char bg_info[20] = "";

    // add background job count to prompt if any jobs are running
    int running_jobs = 0;
    for (int i = 0; i < bg_job_count; i++) {
        if (bg_jobs[i].pid > 0) {
            running_jobs++;
        }
    }
    if (running_jobs > 0) {
        snprintf(bg_info, sizeof(bg_info), "[%d&] ", running_jobs);
    }

    if (running_jobs > 0 && execution_time_ms == 0) {
            // Comando background: mostra apenas contador de jobs
            snprintf(prompt_buffer, BUFFER_SIZE, "enseash %s%% ", bg_info);
        } else if (last_status == -1) {
            // first prompt, no command executed
            snprintf(prompt_buffer, BUFFER_SIZE, "enseash %s%% ", bg_info);
        } else if (WIFEXITED(last_status)) {
            int exit_code = WEXITSTATUS(last_status);
            snprintf(prompt_buffer, BUFFER_SIZE, "enseash %s[exit:%d|%ldms] %% ", bg_info, exit_code, execution_time_ms);
        } else if (WIFSIGNALED(last_status)) {
            int signal_num = WTERMSIG(last_status);
            snprintf(prompt_buffer, BUFFER_SIZE,
                    "enseash %s[sign:%d|%ldms] %% ", bg_info, signal_num, execution_time_ms);
        } else {
            snprintf(prompt_buffer, BUFFER_SIZE, "enseash %s%% ", bg_info);
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
    struct rusage ru;

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
	wait4(pid, &status, 0, &ru);

    // calculate execution time FROM RESOURCE USAGE, not wall clock
    *execution_time_ms = (ru.ru_utime.tv_sec + ru.ru_stime.tv_sec) * 1000;
    *execution_time_ms += (ru.ru_utime.tv_usec + ru.ru_stime.tv_usec) / 1000;

	// free memory in parent
	for (int i = 0; cmd->args[i]; i++) free(cmd->args[i]);
	free(cmd->args);
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd);

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

// check for finished background jobs using wait4 with WNOHANG (non-blocking)
void check_background_jobs(void) {
    int status;
    pid_t pid;
    struct rusage ru;

    // non-blocking wait for any child process
    while ((pid = wait4(-1, &status, WNOHANG, &ru)) > 0) {
        // find this pid in background jobs
        for (int i = 0; i < bg_job_count; i++) {
            if (bg_jobs[i].pid == pid) {
                // calculate execution time from resource usage
                long exec_time_ms = (ru.ru_utime.tv_sec + ru.ru_stime.tv_sec) * 1000;
                exec_time_ms += (ru.ru_utime.tv_usec + ru.ru_stime.tv_usec) / 1000;

                // print job completion message
                print_bg_job_end(bg_jobs[i].job_id, pid, status, exec_time_ms);

                // remove job from array
                remove_background_job(pid);
                break;
            }
        }
    }
}

// add a background job to the array
void add_background_job(pid_t pid, char *command) {
    if (bg_job_count < MAX_BG_JOBS) {
        bg_jobs[bg_job_count].pid = pid;
        strncpy(bg_jobs[bg_job_count].command, command, 49);
        bg_jobs[bg_job_count].command[49] = '\0';
        bg_jobs[bg_job_count].job_id = next_job_id++;
        bg_job_count++;
    }
}

// remove a background job from the array
void remove_background_job(pid_t pid) {
    for (int i = 0; i < bg_job_count; i++) {
        if (bg_jobs[i].pid == pid) {
            // shift remaining jobs
            for (int j = i; j < bg_job_count - 1; j++) {
                bg_jobs[j] = bg_jobs[j+1];
            }
            bg_job_count--;
            break;
        }
    }
}

// execute a command in background (with &)
void execute_background_command(char *command) {
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        command_t *cmd = parse_command_arguments(command);
        if (cmd && cmd->args[0]) {
            execvp(cmd->args[0], cmd->args);
        }
        write(STDERR_FILENO, "command not found\n", 18);
        _exit(1);
    } else if (pid > 0) {
           // parent process - add to background jobs
           add_background_job(pid, command);
           print_bg_job_start(bg_jobs[bg_job_count-1].job_id, pid);
       } else {
           write(STDERR_FILENO, "fork failed\n", 12);
       }
   }

// print background job start message
void print_bg_job_start(int job_id, pid_t pid) {
    char msg[50];
    snprintf(msg, sizeof(msg), "[%d] %d\n", job_id, pid);
    write(STDOUT_FILENO, msg, strlen(msg));
}

// print background job end message
void print_bg_job_end(int job_id, pid_t pid, int status, long exec_time_ms) {
    char msg[100];

    char *command_text = "unknown";
    for (int i = 0; i < bg_job_count; i++) {
        if (bg_jobs[i].job_id == job_id) {
            command_text = bg_jobs[i].command;

    snprintf(msg, sizeof(msg), "[%d]+ Ended: %s &\n", job_id, command_text);

    write(STDOUT_FILENO, msg, strlen(msg));
}


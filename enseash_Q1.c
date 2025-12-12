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



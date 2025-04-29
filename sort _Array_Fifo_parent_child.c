/*

Objective of Code:
This program will as the parent process which will write an array to fifo and via that the child process will read the
array and will sort them

*/


#include <stdio.h>   // For standard input/output functions like printf(), perror(), etc.
#include <unistd.h>  // For POSIX operating system API functions like read(), write(), close(), fork(), etc.
#include <stdlib.h>  // For general utilities like exit(), malloc(), free(), atoi(), etc.
#include <fcntl.h>   // For file control options like open(), O_RDONLY, O_WRONLY flags, fcntl() function, etc.
#include <sys/stat.h> // For data structures and macros related to file information (e.g., file permissions, file modes, mkdir())
#include <sys/types.h> // For data types used in system calls like pid_t, mode_t, off_t, etc.
#include <errno.h>    // For handling error codes set by system calls and library functions (e.g., errno, EACCES, ENOENT)


void sort_array(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    const char *fifopath = "/tmp/myfifo";
   
    // Check if the FIFO exists, and remove it if necessary
    if (access(fifopath, F_OK) == 0) {
        // FIFO exists, so we remove it
        if (unlink(fifopath) == -1) {
            perror("Error removing existing FIFO");
            exit(1);
        }
    }

    // Create FIFO
    if (mkfifo(fifopath, 0666) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        // Error in fork
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0) {
        // Parent process (writing to FIFO)
        int fd = open(fifopath, O_WRONLY); // open file for writing
        if (fd == -1) {
            perror("Opening FIFO for writing failed");
            exit(1);
        }

        int arr[] = {5, 3, 8, 6, 2};
        int n = sizeof(arr) / sizeof(arr[0]);

        // Write array size first
        write(fd, &n, sizeof(int)); // Write the size of the array
        write(fd, arr, n * sizeof(int)); // Write the array

        printf("Parent: Sent array to FIFO\n");

        close(fd); // Close FIFO after writing
    }
    else {
        // Child process (reading from FIFO and sorting the array)
        int fd = open(fifopath, O_RDONLY); // open file for reading
        if (fd == -1) {
            perror("Opening FIFO for reading failed");
            exit(1);
        }

        int n;
        read(fd, &n, sizeof(int)); // Read the size of the array

        int arr[n];
        read(fd, arr, n * sizeof(int)); // Read the array from FIFO

        printf("Child: Received array from FIFO\n");

        // Sort the array
        sort_array(arr, n);

        printf("Child: Sorted array: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");

        close(fd); // Close FIFO after reading
    }

    // Clean up the FIFO
    unlink(fifopath);

    return 0;
}



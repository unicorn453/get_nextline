#include "get_next_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Colors for terminal output
#define BMAG "\x1B[35m"
#define NC "\x1B[0m"

// Global variables for test status
static int res = 1; // To store overall test results
static const char *_title = NULL; // Test title
int next_read_error = 0; // To simulate read errors in tests

// Forward declarations for required functions
void setup_framework(int argn, char **argv);
int test_gnl_func(int fd, const char *expected, const char *title);
void populate_expected(char *buffer, int n);

// Helper macro for running each test and updating global `res`
#define test_gnl(fd, expected) res = test_gnl_func(fd, expected, _title) && res;

// Macro for defining and running test cases
#define TEST(title, code_block) do { \
    _title = title; \
    printf(BMAG "Running test: %s\n" NC, title); \
    code_block \
} while(0)

// Framework setup function to initialize tests
void setup_framework(int argn, char **argv) {
    printf(BMAG "Starting tests...\n" NC);
}

// Populate expected buffer with a pattern for testing
void populate_expected(char *buffer, int n) {
    int i = 0;
    while (i < n) {
        i += sprintf(buffer + i, "0123456789");
    }
    buffer[n] = 0;
}

// Function to test get_next_line against expected output
int test_gnl_func(int fd, const char *expected, const char *title) {
    char *line = get_next_line(fd); // Call to your get_next_line function

    // Compare output line with expected result
    int result = 1;
    if ((line == NULL && expected != NULL) || (line != NULL && expected == NULL) || (line && strcmp(line, expected) != 0)) {
        result = 0;
        printf("%sTest failed: %s\n" NC, BMAG, title);
        printf("  Expected: %s\n", expected ? expected : "(null)");
        printf("  Got     : %s\n", line ? line : "(null)");
    } else {
        printf("%sTest passed: %s\n" NC, BMAG, title);
    }

    // Free line memory if allocated
    free(line);
    return result;
}


// Main function with test cases
int main(int argn, char **argv) {
    setup_framework(argn, argv);
    printf(BMAG "BUFFER_SIZE" NC ": %i\n", BUFFER_SIZE);

    // Test Case 1: "open, error, open"
    TEST("open, error, open", {
        char *name = "read_error.txt";
        char *other = "lines_around_10.txt";
        int fd = open(name, O_RDONLY);
        int fd2 = open(other, O_RDONLY);

        test_gnl(fd, "aaaaaaaaaa\n");
        test_gnl(fd2, "0123456789\n");
        test_gnl(fd, "bbbbbbbbbb\n");
        test_gnl(fd2, "012345678\n");

        // Simulate a read error in the next call
        next_read_error = 1;
        if (BUFFER_SIZE > 100) {
            char *temp;
            do {
                temp = get_next_line(fd);
                free(temp);
            } while (temp != NULL);
        }
        test_gnl(fd, NULL);
        next_read_error = 0;
        
        close(fd);
        test_gnl(fd2, "90123456789\n");
        fd = open(name, O_RDONLY);

        test_gnl(fd, "aaaaaaaaaa\n");
        test_gnl(fd2, "0123456789\n");
        test_gnl(fd, "bbbbbbbbbb\n");
        test_gnl(fd, "cccccccccc\n");
        test_gnl(fd2, "xxxx\n");
        test_gnl(fd2, NULL);
        test_gnl(fd, "dddddddddd\n");
        test_gnl(fd, NULL);
    });

    // Test Case 2: "2 file descriptors"
    TEST("2 file descriptors", {
        char *name = "lines_around_10.txt";
        int fd_1 = open(name, O_RDONLY);
        int fd_2 = open(name, O_RDONLY);

        test_gnl(fd_1, "0123456789\n");
        test_gnl(fd_2, "0123456789\n");
        test_gnl(fd_1, "012345678\n");
        test_gnl(fd_2, "012345678\n");
        test_gnl(fd_2, "90123456789\n");
        test_gnl(fd_2, "0123456789\n");
        test_gnl(fd_2, "xxxx\n");
        test_gnl(fd_2, NULL);
        test_gnl(fd_1, "90123456789\n");
        test_gnl(fd_1, "0123456789\n");
        test_gnl(fd_1, "xxxx\n");
        test_gnl(fd_1, NULL);
    });

    // Test Case 3: "multiple fds"
    TEST("multiple fds", {
        char *name = "lines_around_10.txt";
        char expected[20002];
        populate_expected(expected, 20000);
        expected[20000] = '\n';
        expected[20001] = '\0';

        int fd_1 = open(name, O_RDONLY);
        int fd_2 = open(name, O_RDONLY);
        int fd_3 = open(name, O_RDONLY);

        test_gnl(fd_1, "0123456789\n");
        test_gnl(fd_2, "0123456789\n");
        test_gnl(fd_3, "0123456789\n");
        test_gnl(fd_1, "012345678\n");
        test_gnl(fd_2, "012345678\n");
        test_gnl(fd_2, "90123456789\n");

        int fd_4 = open("giant_line_nl.txt", O_RDONLY);
        test_gnl(fd_2, "0123456789\n");
        test_gnl(fd_3, "012345678\n");
        test_gnl(fd_4, expected);
        test_gnl(fd_2, "xxxx\n");
        test_gnl(fd_2, NULL);
        test_gnl(fd_1, "90123456789\n");
        test_gnl(fd_4, "another line!!!");
        test_gnl(fd_1, "0123456789\n");
        test_gnl(fd_4, NULL);
        test_gnl(fd_1, "xxxx\n");
        test_gnl(fd_4, NULL);
        test_gnl(fd_3, "90123456789\n");
        test_gnl(fd_3, "0123456789\n");
        test_gnl(fd_1, NULL);
        test_gnl(fd_3, "xxxx\n");
        test_gnl(fd_3, NULL);
    });

    return res ? 0 : 1;
}

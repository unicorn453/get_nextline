#include "get_next_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Colors for terminal output
#define BMAG "\033[35m"
#define NC "\033[0m"

int test_gnl_func(int fd, const char *expected, const char *title);

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

int next_read_error = 0;

int main(int argn, char **argv)
{
    const char *_title;
    int res = 1;
    setup_framework(argn, argv);
    printf(BMAG "BUFFER_SIZE" NC ": %i\n", BUFFER_SIZE);

    // Test 1: Invalid file descriptor handling
    TEST("Invalid fd", {
        /* 1 */ test_gnl(-1, NULL);
        /* 2 */ test_gnl(100, NULL);
        int fd = open("empty.txt", O_RDONLY);
        close(fd);
        /* 3 */ test_gnl(fd, NULL);
    });

    // Test 2: Empty file
    TEST("empty.txt", {
        int fd = open("empty.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, NULL);
        /* 2 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 3: Single character file
    TEST("1char.txt", {
        int fd = open("1char.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, "0");
        /* 2 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 4: One line, no newline
    TEST("one_line_no_nl.txt", {
        int fd = open("one_line_no_nl.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, "abcdefghijklmnopqrstuvwxyz");
        /* 2 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 5: File with only newline character
    TEST("only_nl.txt", {
        int fd = open("only_nl.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, "\n");
        /* 2 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 6: Multiple newline characters
    TEST("multiple_nl.txt", {
        int fd = open("multiple_nl.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, "\n");
        /* 2 */ test_gnl(fd, "\n");
        /* 3 */ test_gnl(fd, "\n");
        /* 4 */ test_gnl(fd, "\n");
        /* 5 */ test_gnl(fd, "\n");
        /* 6 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 7: Variable newlines with different contents
    TEST("variable_nls.txt", {
        int fd = open("variable_nls.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, "\n");
        /* 2 */ test_gnl(fd, "0123456789012345678901234567890123456789x2\n");
        /* 3 */ test_gnl(fd, "0123456789012345678901234567890123456789x3\n");
        /* 4 */ test_gnl(fd, "\n");
        /* 5 */ test_gnl(fd, "0123456789012345678901234567890123456789x5\n");
        /* 6 */ test_gnl(fd, "\n");
        /* 7 */ test_gnl(fd, "\n");
        /* 8 */ test_gnl(fd, "0123456789012345678901234567890123456789x8\n");
        /* 9 */ test_gnl(fd, "\n");
        /* 10 */ test_gnl(fd, "\n");
        /* 11 */ test_gnl(fd, "\n");
        /* 12 */ test_gnl(fd, "0123456789012345678901234567890123456789x12");
        /* 13 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 8: Lines around 10 characters
    TEST("lines_around_10.txt", {
        int fd = open("lines_around_10.txt", O_RDONLY);
        /* 1 */ test_gnl(fd, "0123456789\n");
        /* 2 */ test_gnl(fd, "012345678\n");
        /* 3 */ test_gnl(fd, "90123456789\n");
        /* 4 */ test_gnl(fd, "0123456789\n");
        /* 5 */ test_gnl(fd, "xxxx\n");
        /* 6 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Optional: Large lines, only when BUFFER_SIZE is greater than 1
    if (BUFFER_SIZE != 1) {
        TEST("giant_line.txt", {
            int fd = open("giant_line.txt", O_RDONLY);
            char expected[20001];
            populate_expected(expected, 20000);
            expected[20000] = '\0';
            /* 1 */ test_gnl(fd, expected);
            /* 2 */ test_gnl(fd, NULL);
            close(fd);
        });

        TEST("giant_line_nl.txt", {
            int fd = open("giant_line_nl.txt", O_RDONLY);
            char expected[20002];
            populate_expected(expected, 20000);
            expected[20000] = '\n';
            expected[20001] = '\0';
            /* 1 */ test_gnl(fd, expected);
            /* 2 */ test_gnl(fd, "another line!!!");
            /* 3 */ test_gnl(fd, NULL);
            close(fd);
        });
    }

    // Test 9: Read error handling
    TEST("read_error.txt", {
        char *name = "read_error.txt";
        int fd = open(name, O_RDONLY);
        /* 1 */ test_gnl(fd, "aaaaaaaaaa\n");
        /* 2 */ test_gnl(fd, "bbbbbbbbbb\n");
        // Trigger next read error
        next_read_error = 1;
        if (BUFFER_SIZE > 100) {
            char *temp;
            do {
                temp = get_next_line(fd);
                free(temp);
            } while (temp != NULL);
        }
        /* 3 */ test_gnl(fd, NULL);
        next_read_error = 0;
        close(fd);

        // Verify correct behavior after read error is cleared
        fd = open(name, O_RDONLY);
        /* 4 */ test_gnl(fd, "aaaaaaaaaa\n");
        /* 5 */ test_gnl(fd, "bbbbbbbbbb\n");
        /* 6 */ test_gnl(fd, "cccccccccc\n");
        /* 7 */ test_gnl(fd, "dddddddddd\n");
        /* 8 */ test_gnl(fd, NULL);
        close(fd);
    });

    // Test 10: Reading from stdin (assumes content matches lines_around_10.txt format)
    TEST("stdin", {
        int fd = STDIN_FILENO;
        /* 1 */ test_gnl(fd, "0123456789\n");
        /* 2 */ test_gnl(fd, "012345678\n");
        /* 3 */ test_gnl(fd, "90123456789\n");
        /* 4 */ test_gnl(fd, "0123456789\n");
        /* 5 */ test_gnl(fd, "xxxx\n");
        /* 6 */ test_gnl(fd, NULL);
    });

    printf("\nAll tests completed.\n");
    return 0;
}

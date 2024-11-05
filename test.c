# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <ctype.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif

char	*get_next_line(int fd);
int		check_bytes(int read_bytes, char *buffer, char **saved);
char	*str_join(char *s1, char *s2);
char	*str_chr(const char *s, int c);
size_t	str_len(const char *s);
char	*read_from_fd(int fd, char **saved);
char	*read_and_save(int fd, char **saved, int *stop_calls);


size_t	str_len(const char *s)
{
	size_t	len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

char	*str_chr(const char *s, int c)
{
	int	i;
	i = 0;

	if (s == NULL)
	{
		return (NULL);
	}
	while (s[i] != '\0')
	{
		if (s[i] == (unsigned char)c)
			return ((char *)&s[i]);
		i++;
	}
	if ((unsigned char)c == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

char	*str_join(char *saved, char *buffer)
{
	size_t	saved_len;
	size_t	buffer_len;
	char	*joined;

	if (!saved && !buffer)
		return (NULL);
	if (saved)
		saved_len = str_len(saved);
	else
		saved_len = 0;
	if (buffer)
		buffer_len = str_len(buffer);
	else
		buffer_len = 0;
	joined = malloc(saved_len + buffer_len + 1);
	if (!joined)
		return (NULL);
	if (saved)
		strcpy(joined, saved);
	if (buffer)
		strcpy(joined + saved_len, buffer);
	joined[saved_len + buffer_len] = '\0';
	return (joined);
}

int	check_bytes(int read_bytes, char *buffer, char **saved)
{
	if (read_bytes < 0)
	{
		free(buffer);
		if (*saved)
		{
			free(*saved);
			*saved = NULL;
		}
		*saved = NULL;
		return (0);
	}
	if (read_bytes == 0)
	{
		free(buffer);
		return (0);
	}
	return (1);
}

char	*read_and_save(int fd, char **saved, int *stop_calls)
{
	while (!str_chr(*saved, '\n'))
	{
		if (!read_from_fd(fd, saved))
		{
			*stop_calls = 1;
			break ;
		}
	}
	return (*saved);
}


char	*read_from_fd(int fd, char **saved)
{
	char	*buffer;
	int		read_bytes;
	char	*temp;

	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	read_bytes = read(fd, buffer, BUFFER_SIZE);
	if (check_bytes(read_bytes, buffer, saved) == 0)
		return (NULL);
	buffer[read_bytes] = '\0';
	if (*saved)
	{
		temp = str_join(*saved, buffer);
		free(buffer);
		if (!temp)
			return (NULL);
		free(*saved);
		*saved = temp;
	}
	else
		*saved = buffer;
	return (*saved);
}

static char	*extract_line(char **saved, int newline_index)
{
	char	*line;

	line = malloc(newline_index + 1);
	if (!line)
		return (NULL);
	memcpy(line, *saved, newline_index);
	line[newline_index] = '\0';
	return (line);
}

static char	*update_saved(char **saved, int newline_index)
{
	char	*new_saved;

	new_saved = strdup((*saved) + newline_index);
	if (!new_saved)
		return (NULL);
	free(*saved);
	*saved = new_saved;
	return (new_saved);
}

static char	*get_line(char **saved)
{
	char	*line;
	int		newline_index;

	newline_index = 0;
	if (!saved || !*saved || !**saved)
		return (NULL);
	while ((*saved)[newline_index] && (*saved)[newline_index] != '\n')
		newline_index++;
	if ((*saved)[newline_index] == '\n')
		newline_index++;
	line = extract_line(saved, newline_index);
	if (!line)
		return (free(*saved), *saved = NULL, NULL);
	if ((*saved)[newline_index] == '\0')
		return (free(*saved), *saved = NULL, line);
	if (!update_saved(saved, newline_index))
		return (free(line), NULL);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*saved;
	static int	stop_calls;
	char		*line;

	if (stop_calls)
		return (NULL);
	if ((fd < 0 || fd > 1024) || BUFFER_SIZE < 1 || read(fd, &saved, 0) < 0)
		return (free(saved), saved = NULL, NULL);
	saved = read_and_save(fd, &saved, &stop_calls);
	if (saved && str_chr(saved, '\n'))
		return (get_line(&saved));
	line = get_line(&saved);
	if (!line && saved && *saved)
	{
		line = strdup(saved);
		free(saved);
		saved = NULL;
	}
	if (stop_calls == 1 && line)
	{
	stop_calls = 0;
		return (line);
	}
	stop_calls = 0;

	return (line);
}


// Helper function to create strings for testing around BUFFER_SIZE
char *create_test_string(const char *encoding) {
    int len = 0;
    for (int i = 0; encoding[i] != '\0'; ) {
        if (isdigit(encoding[i])) {
            int num = 0;
            while (isdigit(encoding[i])) { // Handle multi-digit numbers
                num = num * 10 + (encoding[i] - '0');
                i++;
            }
            len += num;
        } else if (encoding[i] == '\n') {
            len++;
            i++;
        } else {
            i++; // Skip any unexpected characters
        }
    }

    char *content = malloc(len + 1);
    if (!content) return NULL;

    int offset = 0;
    for (int i = 0; encoding[i] != '\0'; ) {
        if (isdigit(encoding[i])) {
            int count = 0;
            while (isdigit(encoding[i])) { // Handle multi-digit numbers
                count = count * 10 + (encoding[i] - '0');
                i++;
            }
            for (int j = 0; j < count; j++) {
                content[offset] = (offset % 26) + 'a';
                offset++;
            }
        } else if (encoding[i] == '\n') {
            content[offset] = '\n';
            offset++;
            i++;
        } else {
            i++; // Skip any unexpected characters
        }
    }
    content[offset] = '\0';
    return content;
}





// Function to perform the limit tests
void test_limit(const char *encoding) {
    char *content = create_test_string(encoding);
    if (!content) {
        printf("Failed to create test string for encoding: %s\n", encoding);
        return;
    }

    // Print generated content for reference
    printf("Generated content for encoding '%s':\n%s\n", encoding, content);

    // Write content to temporary file
    FILE *file = fopen("limits_test.txt", "w");
    if (!file) {
        perror("Failed to open test file");
        free(content);
        return;
    }
    fprintf(file, "%s", content);
    fclose(file);

    int fd = open("limits_test.txt", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open test file for reading");
        free(content);
        return;
    }

    printf("\nTesting encoding: %s\n", encoding);
    char *line;
    int line_num = 1;
    int passed = 1;

    // Expected result is based on the encoding - print the lines read
    int offset = 0;
    while ((line = get_next_line(fd)) != NULL) {
        int expected_len = 0;
        while (content[offset + expected_len] && content[offset + expected_len] != '\n')
            expected_len++;
        expected_len += (content[offset + expected_len] == '\n'); // Include newline if present

        char *expected = strndup(content + offset, expected_len);
        offset += expected_len;

        // Print the actual vs. expected output for comparison
        printf("Line %d: Expected: \"%s\", Got: \"%s\"\n", line_num++, expected, line);
        
        if (strcmp(expected, line) != 0) {
            passed = 0;
        }

        free(line);
        free(expected);
    }
    close(fd);
    free(content);
    remove("limits_test.txt"); // Clean up temporary file

    // Print pass/fail result
    if (passed) {
        printf("Test for encoding '%s' PASSED\n", encoding);
    } else {
        printf("Test for encoding '%s' FAILED\n", encoding);
    }
}

int main(void) {
    // Define encoded test cases around BUFFER_SIZE boundary
    const char *tests[] = {
        "9", "9\n", "10", "10\n", "11", "11\n",
        "19", "19\n", "20", "20\n", "21", "21\n",
        "9\n9\n", "9\n10", "9\n10\n",
        "10\n8\n", "10\n9", "10\n9\n"
    };

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        test_limit(tests[i]);
        printf("\n");
    }

    return 0;
}
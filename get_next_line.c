/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kruseva <kruseva@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 11:00:43 by kruseva           #+#    #+#             */
/*   Updated: 2024/11/05 18:59:48 by kruseva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

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
		// stop_calls = 0;
		return (line);
	}
	stop_calls = 0;
	return (line);
}

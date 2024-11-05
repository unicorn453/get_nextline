/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kruseva <kruseva@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 11:00:51 by kruseva           #+#    #+#             */
/*   Updated: 2024/11/05 13:56:27 by kruseva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

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

	if (s == NULL)
	{
		return (NULL);
	}
	i = 0;
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

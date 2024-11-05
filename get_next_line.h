/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kruseva <kruseva@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:37:48 by kruseva           #+#    #+#             */
/*   Updated: 2024/11/05 13:50:20 by kruseva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

# define MAX_FD_LIMIT 1024
# define MIN_FD_LIMIT 0

char	*get_next_line(int fd);
int		check_bytes(int read_bytes, char *buffer, char **saved);
char	*str_join(char *s1, char *s2);
char	*str_chr(const char *s, int c);
size_t	str_len(const char *s);
char	*read_from_fd(int fd, char **saved);
char	*read_and_save(int fd, char **saved, int *stop_calls);

#endif

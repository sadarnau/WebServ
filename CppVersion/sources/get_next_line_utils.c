/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpalhol <tpalhol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/14 18:55:33 by tpalhol           #+#    #+#             */
/*   Updated: 2019/10/25 16:42:48 by tpalhol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t			ft_len_p(const char *str)
{
	size_t i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}

char			*ft_strjoin(char const *s1, char const *s2)
{
	int		i;
	char	*dest;
	char	*startpoint;
	int		lens1;
	int		lens2;

	lens1 = (int)ft_len_p(s1);
	lens2 = (int)ft_len_p(s2);
	i = -1;
	if (!(dest = malloc(sizeof(char) * (lens1 + lens2 + 1))))
		return (NULL);
	startpoint = dest;
	while (++i < lens1)
	{
		*dest = s1[i];
		dest++;
	}
	i = -1;
	while (++i < lens2)
	{
		*dest = s2[i];
		dest++;
	}
	*dest = 0;
	return (startpoint);
}

int				ft_strchr(const char *s, int c)
{
	size_t			i;
	char			*p;

	i = 0;
	p = (char *)s;
	while (p[i])
	{
		if (p[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

char			*ft_strdup(const char *s1)
{
	char	*p;
	char	*start;
	size_t	len;

	len = ft_len_p(s1);
	if (!(p = malloc(sizeof(char) * len + 1)))
		return (NULL);
	start = p;
	while (len > 0)
	{
		*p = *s1;
		p++;
		s1++;
		len--;
	}
	*p = 0;
	return (start);
}

char			*ft_substr(char const *s, unsigned int start, size_t len)
{
	unsigned int	lensrc;
	char			*ptr;
	int				i;

	i = 0;
	if (!s)
		return (NULL);
	lensrc = (int)ft_len_p(s);
	if (start > lensrc)
	{
		if (!(ptr = malloc(sizeof(char))))
			return (NULL);
		ptr[i] = 0;
		return (ptr);
	}
	if (!(ptr = malloc(sizeof(char) * (len + 1))))
		return (NULL);
	while (i < (int)len && s[i + start])
	{
		ptr[i] = s[i + start];
		i++;
	}
	ptr[i] = 0;
	return (ptr);
}

#include "get_next_line.h"

char		*init_or_error(char *tmp)
{
	if (!tmp)
	{
		if (!(tmp = (char*)malloc(sizeof(char) * (BUFFER_SIZE + 1))))
			return (NULL);
		tmp[0] = '\0';
	}
	return (tmp);
}

char		*fill_tmp(char *tmp, int fd, char *buf)
{
	int				ret;
	char			*oldtmp;

	while ((ft_strchr(tmp, '\n') == -1) && (ret = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		buf[ret] = 0;
		oldtmp = tmp;
		tmp = ft_strjoin(tmp, buf);
		free(oldtmp);
	}
	return (tmp);
}

char		*ft_substrandfree(char *tmp, int i, char **line)
{
	char	*oldtmp;

	if (i == 0)
		*line = ft_strdup("");
	else
		*line = ft_substr(tmp, 0, i);
	oldtmp = tmp;
	tmp = ft_substr(tmp, i + 1, ft_len_p(tmp));
	free(oldtmp);
	return (tmp);
}

int			get_next_line(int fd, char **line)
{
	static char		*tmp[1024];
	char			buf[BUFFER_SIZE + 1];
	int				i;
	int				ret;

	i = 0;
	if (fd < 0 || BUFFER_SIZE == 0 || !(line) || read(fd, buf, 0) < 0)
		return (-1);
	if (!(tmp[fd] = init_or_error(tmp[fd])))
		return (-1);
	tmp[fd] = fill_tmp(tmp[fd], fd, buf);
	if (tmp[fd])
	{
		while (tmp[fd][i] && tmp[fd][i] != '\n')
			i++;
		if (ft_strchr(tmp[fd], '\n') != -1)
			ret = 1;
		else
			ret = 0;
		tmp[fd] = ft_substrandfree(tmp[fd], i, line);
		return (ret);
	}
	else
	{
		*line = ft_strdup("");
	}
	return (0);
}

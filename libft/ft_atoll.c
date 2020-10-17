/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_stroll.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: samkortekaas <samkortekaas@student.codam.nl> +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/04/29 19:49:58 by samkortekaas  #+#    #+#                 */
/*   Updated: 2020/05/02 16:16:25 by samkortekaas  ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "libft.h"

static void					preprocess(int *i, const char *str, t_info *info)
{
	while (str[*i] < 32)
		(*i)++;
	if (str[*i] == '-')
	{
		info->neg = 1;
		(*i)++;
	}
	else if (str[*i] == '+')
	{
		info->neg = 0;
		(*i)++;
	}
	if ((info->base == 0 || info->base == 16) && str[*i] == '0' &&
		(str[*i + 1] == 'x' || str[*i + 1] == 'X') && ((str[*i + 2] >= '0' &&
														str[*i + 2] <= '9') || (str[*i + 2] >= 'a' && str[*i + 2] <= 'f') ||
													   (str[*i + 2] >= 'A' && str[*i + 2] <= 'F')))
	{
		info->base = 16;
		*i += 2;
	}
	if (info->base == 0)
		info->base = (str[*i] == '0' ? 8 : 10);
}

static t_info				get_limits(t_info *info)
{
	info->cutoff = info->neg ? (unsigned long long)-(LLONG_MIN + LLONG_MAX) +
							   LLONG_MAX : LLONG_MAX;
	info->cutlim = info->cutoff % info->base;
	info->cutoff /= info->base;
	return (*info);
}

static unsigned long long	convert(unsigned long long res, const char *str,
									 int i, t_info *info)
{
	char				c;

	while (str[i])
	{
		if (str[i] >= '0' && str[i] < '0' + info->base)
			c = str[i] - '0';
		else if (str[i] >= 'a' && str[i] < 'a' + (info->base - 10))
			c = str[i] - 'a' + 10;
		else if (str[i] >= 'A' && str[i] < 'A' + (info->base - 10))
			c = str[i] - 'A' + 10;
		else
			break ;
		if (info->any < 0 || res > info->cutoff || (res == info->cutoff &&
													c > (char)info->cutlim))
			info->any = -1;
		else
		{
			info->any = 1;
			res *= info->base;
			res += c;
		}
		i++;
	}
	return (res);
}

long long					ft_atoll(const char *str, int base)
{
	int					i;
	unsigned long long	res;
	t_info				info;

	info.any = 0;
	info.neg = 0;
	info.base = base;
	res = 0;
	i = 0;
	preprocess(&i, str, &info);
	info = get_limits(&info);
	res = convert(res, str, i, &info);
	if (info.any < 0)
	{
		res = info.neg ? LLONG_MIN : LLONG_MAX;
		errno = ERANGE;
	}
	else if (!info.any)
	{
		errno = EINVAL;
	}
	else if (info.neg)
		res = -res;
	return (res);
}

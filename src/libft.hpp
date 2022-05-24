/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:42:44 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 16:38:31 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <string>

std::vector<std::string> ft_split(char *str, char *sep) {
	char *substr;
	std::vector<std::string> res;

	substr = strtok(str, sep);
	while (substr) {
		res.push_back(std::string(substr));
		substr = strtok(str, sep);
	}
	return (res);
}

std::string	ft_strtrim(std::string s, std::string set) {
	int	i;
	//strtrim
	return (s);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionTemplate.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 14:45:14 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/05 20:23:58 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONTEMPLATE_HPP
# define EXCEPTIONTEMPLATE_HPP

#define EXC_CLASS(exc) \
class exc : public std::exception \
{ \
	std::string _str; \
	public : \
	exc(); \
	exc(const std::string &s); \
	~exc() throw(); \
	const char* what() const throw(); \
};

#define EXC_FUNC(base, exc, str) \
base::exc::exc() : _str(str) {} \
base::exc::exc(const std::string &s) : _str(s) {} \
base::exc::~exc() throw() {} \
const char *base::exc::what() const throw() { return _str.c_str(); }

#endif

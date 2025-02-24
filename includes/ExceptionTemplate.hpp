/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionTemplate.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 14:45:14 by glaguyon          #+#    #+#             */
/*   Updated: 2025/01/10 15:15:06 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONTEMPLATE_HPP
# define EXCEPTIONTEMPLATE_HPP

#define EXC_CLASS(exc) \
class exc : public std::exception \
{ \
	public : \
	const char* what() const throw(); \
};

#define EXC_FUNC(base, exc, str) \
const char *base::exc::what() const throw() { return str; }

#endif

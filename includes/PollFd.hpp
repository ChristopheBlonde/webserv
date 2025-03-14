/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollFd.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/14 18:39:08 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLFD_HPP
# define POLLFD_HPP

#include <poll.h>

class PollFd : public pollfd
{
	public:
	PollFd();
	PollFd(int fd);
	~PollFd();
	bool	operator==(const PollFd &p);
};

#endif // POLLFD_HPP

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGetDelete.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 22:20:52 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/07 23:50:45 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Client.hpp"

void	Response::deleteFile(void)
{
	if (_fileName == "")
		return createError(403);
	
	std::string	path = _path + "/" + _fileName;

	if (!testAccess(path, EXIST))
		return createError(404);
	getStatFile(path);
	if (!std::remove(path.data()))
	{
		_status = 204;
		createResponseHeader();
	}
	else
		createError(403);
}

void	Response::getFileOrIndex()
{
	if (!testAccess(_path, DIRACCESS))
		return createError(404);
	if (_fileName != "")
	{
		if (!testAccess(_path + _fileName, EXIST))
			return createError((!_autoIndex ? 404 : 200));
		_fileFd = getFile(_path + _fileName);
		if (_fileFd == -1)
			return createError((!_autoIndex ? 403 : 200));
		getStatFile(_path + "/" + _fileName);
		addFdToCluster(_fileFd, POLLIN);
		return;
	}

	std::vector<std::string>	index = _conf->getIndex();
	int				error = 404;

	for (std::vector<std::string>::iterator it = index.begin();
		it < index.end(); ++it)
	{
		if (!testAccess(_path + *it, EXIST))
			continue;
		_fileFd = getFile(_path + *it);
		if (_fileFd == -1)
			error = 403;
		else
		{
			getStatFile(_path + "/" + _fileName);
			return addFdToCluster(_fileFd, POLLIN);
		}
	}
	return createError((!_autoIndex ? error : 200));
}

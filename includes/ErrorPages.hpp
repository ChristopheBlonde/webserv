/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPages.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 08:07:50 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/05 09:39:24 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGES_HPP
# define ERRORPAGES_HPP

# define ERROR_PAGE(name, page) ("<html lang=\"en\"><head><meta charset=\"UTF-8\">"\
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"\
"<title>" + (name) + "</title><style>body{font-family:serif;display: flex;"\
"flex-direction: column;align-items: center;}h1{color: darkgray;font-size: 3em;"\
"font-weight: bold;}h2{color:orange;font-size:2em;}p{color:darkblue;font-size:"\
"1.5em;}hr{width:100vw;}</style></head><body><h1>Error: " + (page) + "</h1><p>"\
"the server would not process the request due to something the server "\
"considered to be a client error</p><hr/><h2>Webserv 1.0</h2></body></html>")

#endif

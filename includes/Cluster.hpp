/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/22 19:43:08 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

class Cluster
{
	//vector servers
	//liste des fds de merde

	//liste servers default pour host/port ? les trier par ca ? leur donner une cle ?
	public:
	Cluster();
	Cluster(const Cluster &c);
	Cluster &operator=(const Cluster &c);
	~Cluster();
	//try add clients
	//get requests (poll avec clients)
		//renvoie un fd mais aussi des infos sur le client
};

#endif // CLUSTER_HPP

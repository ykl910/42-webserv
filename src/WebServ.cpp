/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: algadea <algadea@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:08:56 by algadea           #+#    #+#             */
/*   Updated: 2025/06/20 18:36:08 by algadea          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServ.hpp"

// Construction
WebServ::WebServ() {
    bzero(this, sizeof(WebServ));
}

WebServ::WebServ(t_config& config){
    bzero(this, sizeof(WebServ));
}

// Destruction
WebServ::~WebServ() {

}
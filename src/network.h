/* netToe Version 1.5.1
 *
 * Copyright 2000,2001 Gabriele Giorgetti <ggdev@users.sourceforge.net>
 *           2009-2014 Mats Erik Andersson <meand@users.sourceforge.net>
 *		
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef	_NETWORK_H
# define _NETWORK_H 1

extern int establish_listening_socket ( unsigned short port_number, char *peer_ip_address, int bytes);
extern int connect_to_socket ( char *host_ip_number, unsigned short port_number );
extern int write_to_socket ( int connected_socket, char *buffer, int bytes );
extern int read_from_socket ( int connected_socket, char *buffer, int bytes );

extern void give_local_IP ( char * local_ip_address, int bytes );

/* extern const char *give_local_hostname ( void ); */ /* NOT USED */

#endif	/* _NETWORK_H */

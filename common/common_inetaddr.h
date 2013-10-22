/*
 * common_inetaddr.h
 *
 *  Created on: 2013Äê8ÔÂ22ÈÕ
 *      Author: 1854
 */

#ifndef COMMON_INETADDR_H_
#define COMMON_INETADDR_H_

#include <string>
#include <vector>
#include <iostream>
#include <netdb.h>

using namespace std;

class Inet_Addr
{
public:
	Inet_Addr();
	Inet_Addr(const Inet_Addr& addr);
	Inet_Addr(const string& hostname, uint16_t port);
	Inet_Addr(uint32_t ip, uint16_t port);
	Inet_Addr(const sockaddr_in& addr);

	virtual ~Inet_Addr();

	bool			is_null() const;
	Inet_Addr&		operator=(const Inet_Addr& addr);
	Inet_Addr&		operator=(const sockaddr_in& addr);
	Inet_Addr&		operator=(const string& addr_str);

	void			set_ip(uint32_t ip);
	void			set_ip(const string& hostname);
	void			set_port(uint16_t port);

	uint32_t		get_ip() const;
	uint16_t		get_port() const;
	const sockaddr_in& ipaddr() const;
	sockaddr_in*	get_addr();
	const sockaddr_in*	get_addr() const;
	string			to_string() const;

	bool			operator==(const Inet_Addr& addr) const;
	bool			operator!=(const Inet_Addr& addr) const;
	bool			operator>(const Inet_Addr& addr) const;
	bool			operator<(const Inet_Addr& addr) const;
private:
	sockaddr_in		addr_;
};


#endif /* COMMON_INETADDR_H_ */

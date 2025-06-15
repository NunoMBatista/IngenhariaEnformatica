### What is it?
Network Address Translation (NAT) was developed to address the growing scarcity of IPv4 addresses by allowing machines in private networks to communicate with external networks. It does this by replacing the private IP address of an internal machine with a public address at the router or firewall that connects to the outside.

#### Inside vs. Outside:

- The 'inside' refers to the network using private addresses.
- The 'outside' typically corresponds to the network with public addresses, though NAT can also be configured between two private networks.
- **Inside local address:** The address used by internal machines for communication within the private network.
- **Outside Local Address:** The address an internal machine's 'inside local' address is translated to when it communicates externally.
- **Inside global address:** An address used by external machines to communicate with an internal machine.
- **Outside global address:** Addresses external machines use to communicate among themselves.

##### Mapping Types:
- NAT mappings can be one-to-one, N-to-one, N-to-M, static, or dynamic

## Configuration (CISCO)

#####  General NAT Configuration (Takes precedence over dynamic NAT

**1 - Designate interfaces as 'inside' or 'outside':** 
- For the inside interface:
	- `interface [interface-type/number]`
	- `ip address [ip-address] [netmask]`
	- `ip nat inside`
	- Example:
		- `interface FastEthernet0` 
		- `ip address 192.168.100.1 255.255.255.0` 
		- `ip nat inside` 
- For the outside interface:
	- `interface [interface-type/number]`
	- `ip address [ip-address] [netmask]`
	- `ip nat outside`

**2 - Configure the NAT rule (Source NAT):** `ip nat inside source list [access-list-number] interface [interface-type/number] overload`

- This command specifies that traffic originating from the 'inside' (as defined by the access list) will have its source address translated.
- The `interface [interface-type/number]` part indicates that the external address to be used for translation is the IP address of that specific interface.
- The `overload` keyword enables **Port Address Translation (PAT)**, meaning multiple 'inside local' addresses can be translated to a single 'outside local' address by distinguishing them using different port numbers.
- Example: `ip nat inside source list 25 interface Ethernet0 overload`

**2 - Define which internal networks are permitted to use NAT using an Access List:** `access-list [access-list-number] permit [source-network-address] [wildcard-mask]`

- This controls which internal machines will have their addresses translated
- Example: `access-list 25 permit 192.168.100.0 0.0.0.255`
	- This allows all machines in the 192.168.100.0/24 network to be translated


#####  Static NAT Configuration (Takes precedence over dynamic NAT)

- Static NAT creates a one-to-one, permanent mapping between a specific 'inside local' address and an 'outside local' address.

- **Command:** `ip nat inside source static [inside-local-ip] [outside-local-ip]` 
- Example: `ip nat inside source static 192.168.100.35 172.16.1.50` maps the internal host 192.168.100.35 to the external address 172.16.1.503.

#### Dynamic NAT Configuration with Pools

Dynamic NAT uses a pool of public IP addresses to translate multiple private IP addresses. The `overload` keyword is often used with pools to enable PAT, allowing the router to distinguish internal machines using different port numbers when the pool's addresses are exhausted.

**1 - Define a NAT pool:** `ip nat pool [pool-name] [start-ip] [end-ip] netmask [netmask]`

-  Example: `ip nat pool POOLB 172.16.1.96 172.16.1.99 netmask 255.255.255.0` defines a pool named `POOLB` with a range of IP addresses from `172.16.1.96` to `172.16.1.99` (inclusive).

**2 - Apply the NAT rule using the pool:** `ip nat inside source list [access-list-number] pool [pool-name] overload`

- Example: `ip nat inside source list 25 pool POOLB overload`
### What is it?
The Routing Information Protocol (RIP) is a simple distance-vector routing protocol used in smaller networks. Cisco routers can be configured to use RIP, specifically version 2 (RIP-2), which is more advanced than RIP-1.

#### Key Characteristics of RIP

- **Distance-vector protocol:** Routers send a vector of distances to their neighbors, indicating the number of hops to known destinations.
- **Metric:** RIP uses hop count as its metric, with a maximum of 15 hops. A value of 16 indicates an unreachable destination.
- **Updates:** RIP-1 sends full routing tables every 30 seconds. RIP-2 also sends periodic updates, but they can be triggered by network changes as well.
- **Underlying Protocol:** RIP operates over User Datagram Protocol (UDP).
- **Limitations:** RIP has several limitations, including a maximum hop count of 15, a simplistic hop-count metric that doesn't consider bandwidth or delay, slow convergence, and lack of strong security.
- **RIP-2 improvements:** RIP-2 is compatible with RIP-1 and adds support for subnet masks (Classless Inter-Domain Routing - CIDR), a simple authentication scheme, and the ability to function in broadcast or multicast environments (using address 224.0.0.9).


### Configuration (CISCO)

**1 - Enter RIP router configuration mode:** `router rip`
- This command initiates the RIP routing process on the router.
**2 - Specify RIP version 2:** `version 2`
- This command ensures the router sends and receives RIP version 2 packets. By default, Cisco routers receive and process both versions but only send RIP-1 packets if this command is not used.
**3 - Declare directly connected networks to be advertised by RIP:** 
- The `network` command is used to indicate the networks that should be announced by the router in its RIP packets. This command enables RIP on interfaces belonging to these networks. `network [network-address]`
- Example:
	- `network 192.168.31.0`
	- `network 192.168.32.0`
	- You would repeat this command for all directly connected networks that you want RIP to advertise.
**4 - Configure passive interfaces (optional but recommended):** 
- The `passive-interface` command tells the router not to send RIP updates through a specific interface. This is useful for interfaces connected to end-user networks where no other routers are present, saving bandwidth. `passive-interface [interface-type/number]`
- Example:
	- `passive-interface interface1`
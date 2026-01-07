
### What is it?
The Border Gateway Protocol (BGP) is the preferred exterior gateway protocol (EGP) used for routing information between different autonomous systems (AS) on the Internet. Unlike Interior Gateway Protocols (IGPs) like RIP or OSPF, which primarily focus on path calculation based on metrics, BGP is a path-vector protocol that transports information about the sequence of autonomous systems (AS-Path) that need to be traversed to reach a destination. This allows for policy-based routing, enabling network administrators to make decisions on which AS to traverse or avoid.

BGP messages are sent using TCP, which ensures reliable and segmented transmission, and BGP sessions are point-to-point connections established between pairs of routers. The current version in widespread use is BGP-4.


### Key Characteristics of BGP

- **Inter-AS Routing:** BGP's primary role is to exchange routing information between different Autonomous Systems.
- **Path-Vector Protocol:** Instead of just advertising distances or link states, BGP advertises the complete AS path (the sequence of AS numbers) that packets traverse to reach a destination. This helps prevent routing loops and allows for policy-based routing.
- **TCP as Transport:** BGP operates over TCP (port 179) for reliable message exchange, unlike RIP which uses UDP or OSPF which runs directly over IP.
- **Scalability:** BGP is designed to be highly scalable, supporting routing tables with over 100,000 routes and utilizing Classless Inter-Domain Routing (CIDR) to help reduce table size.
- **Policy-Based Routing:** BGP uses a rich set of attributes associated with routes to implement diverse routing policies.
- **Types of BGP:**
	- **External BGP (eBGP):** Used for BGP communication between routers in different autonomous systems.
	- **Internal BGP (iBGP):** Used for BGP communication between routers within the same autonomous system.

### Configuration (CISCO)

**1 - Start the BGP Routing Process:** 
- Use the `router bgp` command followed by the Autonomous System (AS) number. This AS number is globally significant, unlike the local process ID used in OSPF. `router bgp [autonomous-system-number]`
- Example: `router bgp 3000` (for AS 3000).

**2 - Disable Synchronization (Common Practice):** 
- Synchronization requires that BGP not advertise routes to other BGP routers until an Interior Gateway Protocol (IGP) also learns them. This mechanism is rarely used and typically disabled to prevent routing instability or delays. For iBGP, it must always be deactivated. `no synchronization` 

**3 - Disable Auto-Summary (Common Practice for CIDR):** 
- By default, BGP performs classful summarization of routes. With CIDR's widespread use, this is usually undesirable and should be disabled. `no auto-summary`

**4 - Declare Networks to Advertise:** 
- Use the `network` command to specify directly connected networks that BGP should advertise. You must include the mask keyword for classless addressing. `network [network-address] mask [subnet-mask]`
- Example: `network 192.168.33.0 mask 255.255.255.0`

**5 - Define BGP Neighbors (Peers):** 
- Use the `neighbor` command to establish BGP peering relationships. The remote-as argument specifies the AS number of the neighbor. `neighbor [neighbor-IP-address] remote-as [remote-AS-number]`
	- If `remote-as` is the same as the local AS, it's an iBGP neighbor.
	- If `remote-as` is different, it's an eBGP neighbor.

**6 - Configure Next-Hop-Self for iBGP (Optional):** 
- By default, iBGP peers advertise routes with the original next-hop received from an eBGP peer. This might be an IP address not reachable by other iBGP peers in the same AS. `next-hop-self` forces the advertising iBGP router to set itself as the next-hop. `neighbor [iBGP-neighbor-IP] next-hop-self`

**7 - Advertise a Default Route (Optional):** 
- To advertise a default route `(0.0.0.0/0)` to a BGP neighbor, you need to configure a static default route on the router first, and then use the `default-originate` keyword under the neighbor configuration. 
	- `ip route 0.0.0.0 0.0.0.0 [next-hop-IP]`
	- `neighbor [neighbor-IP-address] default-originate`

**8 - Enable Route Dampening (Optional but Recommended for Stability):** 
- BGP dampening aims to ensure route stability by suppressing unstable routes. If a route's state changes too frequently within a certain period, it's put into a 'hold-down' state and not used for a defined interval. `bgp dampening`

**9 - Redistribute Routes from Other Protocols (Optional):** 
- To inject routes learned from other routing protocols (like OSPF or RIP) into BGP, use the redistribute command. `redistribute ospf [process-ID]`
	- Example: `redistribute ospf 90 [Ficha 5-99]`

**10 - Route Filtering (Advanced, Optional):** 
- BGP supports various mechanisms for filtering routes. One method uses filter-list with an `AS-path access-list` to permit or deny routes based on the AS path. 
	- `neighbor [neighbor-IP-address] filter-list [access-list-number] [in|out]` 
	- `ip as-path access-list [access-list-number] permit [regex]`
		- Example: 
			- `ip as-path access-list 1 permit _(400|500|600)_` (permits routes containing AS 400, 500, or 600).
			- `ip as-path access-list 1 permit ^$` (this router will only advertise its own originated routes and those redistributed from OSPF to its eBGP neighbors).

**Example Configuration Snippet**

This example illustrates a basic eBGP and iBGP configuration for Router R1 in AS 3000, peering with ISP1 (AS 1000) and R2 (also in AS 3000), and advertising networks and redistributing OSPF routes:
```
Router>enable
Router#config terminal
Router(config)#router bgp 3000
Router(config-router)#no synchronization
Router(config-router)#no auto-summary
Router(config-router)#bgp dampening
Router(config-router)#network 192.168.(X+1).0 mask 255.255.255.0  (Example network in AS 3000) [Ficha 5-103]
Router(config-router)#redistribute ospf 100  (Assuming OSPF process ID 100 is running) [Ficha 5-103]

! eBGP neighbor to ISP1 (AS 1000)
Router(config-router)#neighbor 172.16.X.Y remote-as 1000

! iBGP neighbor to R2 (within AS 3000)
Router(config-router)#neighbor 10.X.1.Z remote-as 3000
Router(config-router)#neighbor 10.X.1.Z next-hop-self  (Important for iBGP) [Ficha 5-103]
Router(config-router)#end
Router#
```

### BGP Route Selection Process

BGP determines the "best" path for each destination based on a sequence of attributes. The router typically prefers routes with:

1. The highest Weight (Cisco-specific attribute, local to the router).
2. The highest LOCAL-PREF (propagated within the AS).
3. Routes originated by the local BGP process.
4. The shortest AS-PATH.
5. The lowest ORIGIN code (IGP < EGP < Incomplete).
6. The lowest MED (Multi-Exit Discriminator, a suggestion for external AS).
7. Preference for external routes over internal routes.
8. The closest IGP next-hop.
9. The lowest BGP router ID (lowest IP address).

Only the best route for each destination is installed in the main routing table, while other alternative routes are maintained in the BGP table

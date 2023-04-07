# Network Connectivity and Devices connected
The Network Connectivity script tests the network connection quality by establishing
TCP connections to a list of well-known DNS servers on port 443 (HTTPS). It calculates 
the success rate of these connections and the average latency for successful connections 
to determine the overall connection quality.

The Devices connected script executes the command "arp -a" to retrieve a list of devices connected 
to the local network, and then prints out the list with the device number and device information. 

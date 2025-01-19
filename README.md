# Packet-Sniffer
> January 19th, 2025
----------------------

![Packet Sniffer](https://github.com/ProfessorJarIO/Packet-Sniffer/blob/main/PacketCapture.jpg)

My packet sniffer program allows users to listen to packets coming to their machine based on an interface they bind to. It's a similar program to Wireshark or Tshark, but with less capabilities for now. 

This is a simple project made by me to learn about networking, packets, linuxAPI, and c-programming. The program doesn't have many features, it only captures the basics from the data-link, ip, and transport layer of the network stack. It also doesn't rely on any 3rd party libraries; it only uses the linux api. The reasoning for this is so my program will be able to work on virtually all linux platforms natively.

## How to use program

1. Compile the program
2. With your executable, you need to use the appropriate arguments to the program:
```
sudo ./a.out [INTERFACE NAME]
```

The interface name will be different on everyone's machine, so if you want to know what interfaces you can use, run this command:

```
ip a
```

## What I learned in this project

UTILIZE MAN PAGES. They are the holy grail of your learning: https://man7.org/linux/man-pages/

Google is also your friend, so use google to find out what headers you need to import and what functions you need to use to accomplish your goals.

I am using a raw socket with the `AF_PACKET` address family, `SOCK_RAW` type, and `htons(ETH_P_ALL)` protocol. Raw sockets operate at the network layer and can capture all packets passing through a network interface. Because of this, they do not require the bind and listen functions that are typically used with stream-oriented sockets, like TCP sockets. When you create a raw socket, you're not establishing a connection like you would with TCP sockets. Instead, you're capturing or sending packets directly at the data link layer.

Instead of using `bind` to associate the socket with a specific address and port, you use `setsockopt` with the `SO_BINDTODEVICE` option to bind the socket to a specific network interface.

With raw sockets, you're not setting up a server to listen for incoming connections. Instead, you are passively receiving all packets that pass through the specified network interface, which is why there's no need for the `listen` function.

I also learned to use `memset` to set the memory block of a structure to all 0's. The reasoning for this is to get rid of any garbage values that could be in the structure before it's used. 

I learned to use the `snprintf` function, which writes formatted output to sized buffer. I believe this gives me more control rather than using the traditional method of putting values into structs.

I also learned that sometimes you have to look at the source  code itself and not always rely solely on the man pages. Sure, the man pages provide excellent details on most functions, structures, and objects, but they may not always have everything in them. 

One thing that was tricky to solve was: pointers to structures.

I was having difficulty trying to assign a buffer to an ethernet header. This was my code before:

```
struct ethhdr eth = (struct ethhdr )buffer;
```

This obviously gave me problems, but I needed to figure out why. I needed to make the data in `buffer` conform into the `ethhdr` structure. I went to do some research, and I came to a conclusion of what was wrong:

-  `(struct ethhdr)buffer` attempts to treat this `buffer` pointer as if it were an actual instance of `struct ethhdr`. However, `buffer` is not an immediate struct of type `struct ethhdr`; it is a memory address pointing to where such data might exist.
-  This operation fails because it tries to directly convert a pointer (a memory address) into a full-fledged structure instance.

In other words, I was trying to forcefully convert a memory address into an entire structure instance.

**My Solution**

```
struct ethhdr *eth = (struct ethhdr *)buffer;
```

Here, I do some type casting, and the biggest difference is that I use a pointer assignment. 

`buffer` is assumed to be a pointer to a block of memory that contains the data matching the structure `ethhdr`.

`eth` now points to the same memory location as buffer, which is where my `ethhdr` structure's data is supposed to be.

This operation is valid because I'm just assigning a pointer to another pointer, and pointers are scalar types.

After I solve this problem, another problem came up when I needed to access IP header information, and I also need to figure out how to access transport layer information such as TCP and UDP

The ethernet header was easy, since the data link header comes first, but how do I access the IP header information? To solve this issue, I simply added the size of the structure of `struct 
ethhdr`. This makes sense because the IP layer comes after the data link layer in TCP/IP stack. Same goes for the transport layer, all I needed to do was add the ethernet and IP header structure size to the buffer to give it a new offset to look from, and I finally could read the data that's in the transport (tcp/udp) layer. 

After this exhausting research and trial and error, I just needed to print out the necessary information to the user from each layer: MAC addresses, IP source and destination, and port numbers. 

## Suggestions and Improvements

If you see any mistakes up above, please let me know and I'll try to get it corrected. I always love learning from my mistakes and trying to do better.

## Resources

This place will contain all the links that I used to help aid me in the development in this project.

> https://stackoverflow.com/questions/51358018/linux-cooked-capture-in-packets
> 
> https://linux.die.net/man/3/htons
> 
> https://stackoverflow.com/questions/14478167/bind-socket-to-network-interface
> 
> https://unix.stackexchange.com/questions/648718/force-programs-bind-to-an-interface-not-ip-address
> 
> https://stackoverflow.com/questions/16528868/c-linux-kernel-module-tcp-header
> 
> https://cplusplus.com/reference/cstring/memset/
> 
> https://cplusplus.com/reference/cstdio/snprintf/
> 
> https://stackoverflow.com/questions/14478167/bind-socket-to-network-interface
> 
> https://man7.org/linux/man-pages/man7/socket.7.html
> 
> https://man7.org/linux/man-pages/man2/socket.2.html
> 
> https://man7.org/linux/man-pages/man2/setsockopt.2.html
> 
> https://man7.org/linux/man-pages/man2/bind.2.html
> 
> https://man7.org/linux/man-pages/man5/protocols.5.html
> 
> https://man7.org/linux/man-pages/man7/packet.7.html
> 
> https://en.wikipedia.org/wiki/C_POSIX_library
> 
> https://en.wikipedia.org/wiki/Unistd.h
> 
> https://sourceware.org/glibc/manual/2.38/html_mono/libc.html#Socket-Concepts
> 
> https://man7.org/linux/man-pages/man3/setsockopt.3p.html
> 
> https://man7.org/linux/man-pages/man0/sys_socket.h.0p.html
> 
> https://man7.org/linux/man-pages/man2/setsockopt.2.html
> 
> https://man7.org/linux/man-pages/man3/recvfrom.3p.html
> 
> https://man7.org/linux/man-pages/man2/socket.2.html
> 
> https://man7.org/linux/man-pages/man2/bind.2.html
> 
> https://man7.org/linux/man-pages/man0/netinet_in.h.0p.html
> 
> https://man7.org/linux/man-pages/man7/raw.7.html
> 
> https://man7.org/linux/man-pages/man3/sockaddr.3type.html
> 
> https://man7.org/linux/man-pages/man7/netdevice.7.html
> 
> https://www.man7.org/linux/man-pages/man3/memset.3.html
> 
> https://man7.org/linux/man-pages/man3/htons.3.html
> 
> https://man7.org/linux/man-pages/man3/probe::netfilter.ip.post_routing.3stap.html
> 
> https://stackoverflow.com/questions/19823797/how-to-get-the-clients-ip-after-receiving-a-udp-packet-with-recvfrom-in-c
> 
> https://www.geeksforgeeks.org/snprintf-c-library/
> 
> https://stackoverflow.com/questions/39023433/recvfrom-is-returning-size-of-buffer-instead-of-number-of-bytes-read
> 
> https://www.cs.cmu.edu/~srini/15-441/F01.full/www/assignments/P2/htmlsim_split/node12.html
> 
> https://www.linuxquestions.org/questions/programming-9/get-source-ip-from-udp-socket-using-recv-835104/
> 
> https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-recvfrom-receive-messages-socket <- ONE OF THE BEST RESOURCES OUT THERE
> 
> https://stackoverflow.com/questions/50652050/what-is-the-difference-between-ether-header-and-ethhdr
>
> https://github.com/torvalds/linux/blob/0512e0134582ef85dee77d51aae77dcd1edec495/include/uapi/linux/if_ether.h#L161
> 
> https://github.com/torvalds/linux/blob/0512e0134582ef85dee77d51aae77dcd1edec495/include/uapi/linux/ip.h
> 
> https://github.com/torvalds/linux/blob/0512e0134582ef85dee77d51aae77dcd1edec495/include/uapi/linux/tcp.h
> 
> https://github.com/torvalds/linux/blob/0512e0134582ef85dee77d51aae77dcd1edec495/include/uapi/linux/udp.h
> 
> https://stackoverflow.com/questions/18438946/format-specifier-02x
> 
> https://www.erg.abdn.ac.uk/users/gorry/course/inet-pages/ip-packet.html

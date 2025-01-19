#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 // I/O operations
#include <unistd.h>
 // Needed for socket operations
#include <sys/socket.h>
 // Will be used to convert data into human-readable ip addresses
#include <arpa/inet.h>
 // Will be used to get IP header information
#include <netinet/ip.h>
 // Used to get Data link layer information
#include <netinet/ether.h>
 // Used to get TCP information for packets
#include <netinet/tcp.h>
 // Will get UDP packet information
#include <netinet/udp.h>
 // Low-level access to Linux network devices
#include <net/if.h>
 // ICMP header information
#include <linux/icmp.h>

int main(int argc, char ** argv) {

    // The 2nd argument will be the interface we are binding to
    if (argc < 2) {
        perror("Specify which interface you want to bind and listen to");
        return 1;
    }

    // Set your interface. For me, this will be enp0s3, but for you, this'll be different.
    char * interface_name = argv[1];

    // Will be used to create our socket object
    int sockfd;

    // Will be used to hold socket information such as family and address
    struct sockaddr saddr;

    // We will attempt to create our socket object, if it fails, give an error.
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("Failed creating socket");
        return 1;
    }

    // Specify the interface we bind to
    // The ifreq structure is used to store information about a network interface; It's a way to get and set network configurations
    struct ifreq ifr;

    // Set every field in ifr struct to 0. This is apparently more secure than the traditional way of setting values in structs
    memset( & ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface_name);
    //ifr.ifr_name[sizeof(ifr.ifr_name)] = *interface_name;

    // Now we set the socket options
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, & ifr, sizeof(ifr)) == -1) {
        perror("setsockopt error");
        close(sockfd);
        return 1;
    }

    // Now it's time we start receiving packets on the interface we bind to

    // The buffer will hold the packet data. Why 65535? Because that is the maximum packet size
    char buffer[65535];

    // Holds the size of the sockaddr information earlier	
    int saddr_len = sizeof(saddr);

    // Count how many packets have been captured so far.
    unsigned int packet_count = 1;

    // Receive packets indefinitely
    while (1) {
        // Receive messages from a connection-mode or connection-less socket. Allows application to retrieve source address of packets and other packet data
        int msg_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, & saddr, & saddr_len);

        // Process the packet to retrieve the source address

        printf("| No. %d | ", packet_count);
        packet_count += 1;

        // Point to the Ethernet header
        // Notice how we just have the buffer here, because the data link layer is first (besides physical), we don't need to do anything else.
        struct ethhdr * eth = (struct ethhdr * ) buffer;

        printf("%02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X | ", eth -> h_source[0], eth -> h_source[1], eth -> h_source[2], eth -> h_source[3], eth -> h_source[4], eth -> h_source[5], eth -> h_dest[0], eth -> h_dest[1], eth -> h_dest[2], eth -> h_dest[3], eth -> h_dest[4], eth -> h_dest[5]);

        // Point to the IP header
        // We are now using buffet + the size of the ethernet header struct. The reason is we need to go to a new offset in the buffer that will have the ip header information
        struct iphdr * ip = (struct iphdr * )(buffer + sizeof(struct ethhdr));

        printf("%s -> ", inet_ntoa( * (struct in_addr * ) & ip -> saddr));
        printf("%s | ", inet_ntoa( * (struct in_addr * ) & ip -> daddr));

        // Transport Layer
        // We will now set the buffer offset to be the data link and ip layer to get to the transport layer where TCP and UDP lies
        if (ip -> protocol == 6) { // TCP Protocol
            struct tcphdr * tcp = (struct tcphdr * )(buffer + sizeof(struct ethhdr) + ip -> ihl * 4);
            printf("TCP: %u -> %u | ", ntohs(tcp -> source), ntohs(tcp -> dest));
        } else if (ip -> protocol == 17) { // UDP Protocol
            struct udphdr * udp = (struct udphdr * )(buffer + sizeof(struct ethhdr) + ip -> ihl * 4);
            printf("UDP: %u -> %u", ntohs(udp -> source), ntohs(udp -> dest));
        } else if (ip -> protocol == 1) { // ICMP Protocol
            struct icmphdr * icmp = (struct icmphdr * )(buffer + sizeof(struct ethhdr) + ip -> ihl * 4);
            printf("ICMP: {Type - %d, Code - %d, Checksum - %d} | ", icmp -> type, icmp -> code, icmp -> checksum);
        }

        printf("\n");

    }

    close(sockfd);
    return 0;

}

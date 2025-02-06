/*
Name:Abraheem Zadron
Date:Febuary 6, 2025
Description: UDP multicasting Server
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MULTICAST_GROUP "239.128.1.1"
#define PORT 7020
#define BUFFER_SIZE 1024

char *findColon(char *str) {
    while (*str) {
        // Return pointer to the first semicolon
        if (*str == ':') return str; 
        str++;
    }
    // If a semi colon is not found
    return NULL; 
}

char *findSpace(char *str) {
    while (*str) {
        // Return pointer to first space
        if (*str == ' ') return str; 
        str++;
    }
    // If there is no space found
    return NULL; 
}

void printAndParseKeyAndValue(char *message) {
    char key[51], value[201];
    char *ptr = message;

    printf("\nFormatted Output:\n");
    printf("-------------------------------------\n");
    printf("%-20s %-20s\n", "Key", "Value");
    printf("-------------------------------------\n");

    while (*ptr) {
        // Extract key
        if (sscanf(ptr, "%50[^:]", key) != 1) break;

        // Move pointer to colon using the function findColon
        ptr = findColon(ptr);
        if (!ptr) break;
        // Move past the semicolon
        ptr++; 

        // Extract value handles quoted and unquoted values
        if (*ptr == '\"') {
            // Skip the first quote
            ptr++; 
            sscanf(ptr, "%200[^\"]", value);
            // Move to closing quote
            ptr = findColon(ptr); 
            if (!ptr) break;
            // Skip closing quote
            ptr++; 
        } else {
            sscanf(ptr, "%200s", value);
        }

        // Print extracted key and value pair
        printf("%-20s %-20s\n", key, value);

        // Move to the next key and value pair using FindSpace
        ptr = findSpace(ptr);
        if (!ptr) break;
        // Move past space
        ptr++; 
    }
}

int main() {
    int sock;
    struct sockaddr_in multicast_addr;
    struct ip_mreq multicast_request;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int reuse = 1;
    int rc;
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char *)&reuse, sizeof(reuse));
    if (rc < 0) {
        perror("Setting SO_REUSEPORT failed");
        exit(1);
    }

    // Bind to the multicast address and port
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    multicast_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Join the multicast group
    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request)) < 0) {
        perror("Multicast group join failed");
        exit(1);
    }

    printf("Listening for multicast messages on %s:%d...\n", MULTICAST_GROUP, PORT);

    while (1) {
        socklen_t addrLen = sizeof(multicast_addr);
        int recvLen = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&multicast_addr, &addrLen);
        if (recvLen < 0) {
            perror("Receive failed");
            exit(1);
        }
        // Null-terminate received data
        buffer[recvLen] = '\0'; 
        printf("\nReceived message:%s\n", buffer);
        printAndParseKeyAndValue(buffer);
    }

    close(sock);
    return 0;
}

/*
Name:Abraheem Zadron
Date:Febuary 6, 2025
Description: UDP multicasting Client
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_GROUP "239.128.1.1"
#define PORT 1818
#define BUFFER_SIZE 1024
#define MESSAGE_FILE "messages.txt"

int main() {
    int sock;
    struct sockaddr_in multicast_addr;
    char message[BUFFER_SIZE];

    // Open the message file
    FILE *file = fopen(MESSAGE_FILE, "r");
    if (file == NULL) {
        perror("Error opening message file");
        return 1;
    }

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure the multicast address
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_addr.sin_port = htons(PORT);

    // Read and send each line separately
    while (fgets(message, BUFFER_SIZE, file)) {
    // Remove newline
        int rc = (int)strlen(message);
        if (rc > 0 && message[rc - 1] == '\n') {
            // Replace newline with null terminator
            message[rc - 1] = '\0'; 
        }
    

        // Debug: Print each message before sending
        printf("Sending: %s\n", message);

        // Send each line as a separate UDP message
        if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
            perror("Send failed");
        }

        // Small delay to prevent message loss
        usleep(100000);  
    }

    fclose(file);
    close(sock);
    return 0;
}

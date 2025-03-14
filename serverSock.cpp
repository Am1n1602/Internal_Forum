#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients; // Store connected clients
std::mutex clients_mutex;    // Prevent concurrent modification

// Function to handle client communication
void handle_client(SOCKET clientSocket) {
    char buffer[200];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected.\n";
            closesocket(clientSocket);

            // Remove client from list
            clients_mutex.lock();
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            clients_mutex.unlock();
            break;
        }

        buffer[bytesReceived] = '\0'; // Null-terminate received message
        std::cout << "Received: " << buffer << std::endl;

        // Broadcast message to all clients (including sender)
        clients_mutex.lock();
        for (SOCKET client : clients) {
            send(client, buffer, bytesReceived, 0);
        }
        clients_mutex.unlock();
    }
}

int main() {
    WSADATA wsaData;
    int wsacheck=WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsacheck != 0)
    {
        std::cout << "Winsock error!!!"<<std::endl;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(55555);

    bind(serverSocket, (SOCKADDR*)&service, sizeof(service));
    listen(serverSocket, 5);

    std::cout << "Server is running...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) continue;

        std::cout << "Client connected!\n";

        // Add new client to list
        clients_mutex.lock();
        clients.push_back(clientSocket);
        clients_mutex.unlock();

        // Start thread for this client
        std::thread clientThread(handle_client, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
    
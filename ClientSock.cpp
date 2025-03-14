#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <tchar.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void receive_messages(SOCKET clientSocket) {
    char buffer[200];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate received message
            std::cout << "\n[Broadcast] " << buffer << std::endl;
        }
    }
}

int main() {
    WSADATA wsaData;
    int check = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (check != 0)
    {
        std::cout << "Winsock error!!" << std::endl;
        WSACleanup();
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;

    std::string serverIP;
    std::cout << "Enter server IP: \n";
    std::cin >> serverIP;


    int connectcheck = InetPtonW(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr); // Server IP

    clientService.sin_port = htons(55555);

    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Connection failed!\n";
        return 1;
    }

    std::cout << "Connected to server!\n";

    // Start receiving thread
    std::thread recvThread(receive_messages, clientSocket);
    recvThread.detach();

    // Sending loop
    char buffer[200];
    while (true) {
        std::cin.getline(buffer, sizeof(buffer));
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

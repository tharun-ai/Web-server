#include <iostream>
#include <thread>
#include <vector>
#include<WS2tcpip.h>
#include<string>
#include<sstream>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void clientFunction(int clientId, const string& serverIp, int port) {
    // Initialize Winsock
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Client " << clientId << " socket creation failed!" << endl;
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &hint.sin_addr);

    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR) {
        cerr << "Client " << clientId << " failed to connect!" << endl;
        closesocket(sock);
        return;
    }

    string message = "Hello from client #" + to_string(clientId);
    send(sock, message.c_str(), message.size() + 1, 0);

    char buffer[4096];
    ZeroMemory(buffer, 4096);
    int bytesReceived = recv(sock, buffer, 4096, 0);
    if (bytesReceived > 0) {
        cout << "[Client #" << clientId << "] Server response: " << string(buffer, 0, bytesReceived) << endl;
    }

    closesocket(sock);
    WSACleanup();
}

int main() {
    while(1){
    int clientCount;
    cout << "Enter number of clients to spawn: ";
    cin >> clientCount;

    const string serverIp = "127.0.0.1";
    const int port = 54000;

    vector<thread> clients;

    for (int i = 0; i < clientCount; ++i) {
        clients.emplace_back(clientFunction, i + 1, serverIp, port);
    }

    for (auto& th : clients) {
        if (th.joinable()) th.join();
    }

    cout << "Press Enter to exit..." << endl;
cin.get(); 
    }
    return 0;
}

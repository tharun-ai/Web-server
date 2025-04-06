#include<iostream>
#include<WS2tcpip.h>
#include<string>
#include<sstream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<thread>
using namespace std;

#pragma comment (lib,"ws2_32.lib");
SOCKET m_Socket;
//IP adress of master
const char* m_ipAddress="127.0.0.1"; 
//Ip address of port
int m_port=54000;
//Both can be mixed to form a URL 😊

class mySemaphore{

    mutex mtx;
    int count;
    condition_variable cv;
    public:   
    explicit mySemaphore(int init = 0) : count(init) {}

    void acquire(){
        unique_lock lock(mtx);
        cv.wait(lock,[&](){return count>0;});
        --count;
    }

    void release(){
        unique_lock lock(mtx);
        ++count;
        cv.notify_one();
    }

};

mySemaphore myS(4);
int createMainSocket(const char* ip_Address,int port){
    WSADATA wsData;
    WORD ver=MAKEWORD(2,2);
    int winSockInit=WSAStartup(ver,&wsData);
    if(winSockInit!=0){
        cout<<"Winsock wasn't Initilazed";
        return -1;
    }
    //let's create a socket for listening
    SOCKET masterSocket=socket(AF_INET,SOCK_STREAM,0);
    if (masterSocket == INVALID_SOCKET)
    {
      cerr << "Socket creation failed! Error: " << WSAGetLastError() << endl;
      WSACleanup();
      return WSAGetLastError();
    }
    //we need to bind this scoket at address and port for listening
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ip_Address, &hint.sin_addr);
  
    if ((bind(masterSocket, (sockaddr *)&hint, sizeof(hint))) == SOCKET_ERROR)
    {
      cerr << "Bind failed! Error: " << WSAGetLastError() << endl;
      closesocket(masterSocket);
      WSACleanup();
      return WSAGetLastError();
    }
  
    if (listen(masterSocket, SOMAXCONN) == SOCKET_ERROR)
    {
      cerr << "Listen failed! Error: " << WSAGetLastError() << endl;
      closesocket(masterSocket);
      WSACleanup();
      return WSAGetLastError();
    }
    m_Socket=masterSocket;
    return 1;
    //FD_SET(m_socket, &m_master);
}
void processEachRequest(SOCKET currentSocket); 
void acceptConnections(){
    while(true){
        SOCKET newlyConnected=accept(m_Socket,nullptr,nullptr);
        if (newlyConnected == INVALID_SOCKET) {
            cerr << "Client accept failed! Error: " << WSAGetLastError() << endl;
            continue;
        }
        cout << "[+] Client connected with socket: " << newlyConnected << endl;
        thread p1(processEachRequest,newlyConnected);
        p1.detach();
    }
}

void processEachRequest(SOCKET currentSocket){
  myS.acquire();
  char buffer[4000];
  ZeroMemory(buffer, 4000);
  int bytesReceived = recv(currentSocket, buffer, 4000, 0);
  if (bytesReceived > 0)
  {
    cout << "Client: " << buffer << endl;
    std::stringstream ss;
    ss << "Hey champ! Message received from thread " << std::this_thread::get_id();
    std::string message = ss.str();
    send(currentSocket, message.c_str(), message.size() + 1, 0);
  }
  closesocket(currentSocket);

  cout << "Client disconnected." << endl;
  myS.release();
}
int main(){
 
    int serverStarted=createMainSocket(m_ipAddress, m_port);
    if(serverStarted!=1){
        cout<<"Server not Started or Main socket wasn't started";
        return 1;
    }
    cout<<"Server Started";
    cout << "Server started and listening on " << m_ipAddress << ":" << m_port << endl;
    std::thread acceptThread(acceptConnections);
   
    acceptThread.join();
}
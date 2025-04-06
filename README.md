# C++ Multi-threaded Socket Server

## Overview
This project implements a multi-threaded socket server in C++ using Winsock. It supports multiple client connections, processes requests concurrently, and demonstrates semaphore-based synchronization.

## Features
- **Multi-threading:** Handles multiple client requests simultaneously.
- **Semaphore:** Uses a custom semaphore for managing threads.
- **Networking:** Implements socket programming for server-client communication.
- **Thread Safety:** Ensures thread-safe operations with mutex and condition variables.

##H How to Run
-->This is for webServer
g++ -o server.exe webServer.cpp -lws2_32 
-->This is for Clients
g++ -o clients.exe clients.cpp -lws2_32
Aftr successfull build,you can double click on the genereated .exe files 

## How It Works
1. The server listens on the specified IP address (`127.0.0.1`) and port (`54000`).
2. Accepts incoming client connections.
3. Processes each client request in a separate thread.
4. Uses a custom semaphore to limit concurrent request processing.
5. Responds to each client with a message acknowledging the thread that handled the request.


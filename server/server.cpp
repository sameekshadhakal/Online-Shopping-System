#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include<sstream>
#include "Server.h"
#define CLIENTS (5)
#pragma comment(lib, "ws2_32.lib")

Server::~Server()
{
    for (SOCKET& fdSock : sockets.fd_array)
        closesocket(fdSock);

    for (HANDLE& handle : handles)
        CloseHandle(handle);

    WSACleanup();
}

bool Server::start()
{
    WORD version = MAKEWORD(2, 2);
    bool WSAok = !WSAStartup(version, &wsaData);

    if (WSAok)
    {
        createThreads();
        Listen();
    }
    return WSAok;
}

void Server::Listen()
{
    // create a socket
    closesocket(server);
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server != INVALID_SOCKET)
    {
        // bind socket
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(m_port);
        addr.sin_addr.S_un.S_addr = inet_addr(m_ipAddress.c_str());
        ZeroMemory(addr.sin_zero, sizeof(addr.sin_zero));

        if (!bind(server, (sockaddr*)&addr, sizeof(addr)))
            listen(server, CLIENTS);
    }
}

void Server::createThreads()
{
    for (size_t i = 0; i < CLIENTS; i++)
    {
        pool[i] = Thread(this, i);
        pool[i].create();

        handles[i] = pool[i].getHandle();
    }
}

void Server::run()
{
    if (server > 0)
    {
        // Initializing the main socket sets to zero
        FD_ZERO(&sockets);

        // including server in our main sets
        FD_SET(server, &sockets);

        // the time interval after which the select function will check
        // the port for connections/requests
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;

        // the SERVER loop
        while (!exit)
        {
            // the copy sets to be refined for connections
            SocketSet sockets_active = sockets;

            // will destroy any inactive socket in copy sets
            int sockCount = select(0, &sockets_active, nullptr, nullptr, &tv);

            // if seletc() is successful : if there is something on the port
            if (sockCount >= 0)
                processReq(sockets_active);
            else
            {
                std::cerr << "\n select() function failed ";
                std::cerr << "\n Error : " << WSAGetLastError << std::endl;
                break;
            }
        }
        WaitForMultipleObjects(CLIENTS, handles, FALSE, INFINITE);
    }
}

void Server::processReq(SocketSet& sockets_active)
{
    // iterating through all available sockets
    for (SOCKET& sock : sockets.fd_array)
    {
        // if a request is made from our active set
        if (FD_ISSET(sock, &sockets_active))
        {
            // if the socket is server
            if (sock == server)
            {
                // accept connection
                SOCKET client = accept(sock, nullptr, nullptr);
                if (client <= 0)
                {
                    std::cerr << "\n Connection with remote client failed";
                    std::cerr << "\n Error : " << WSAGetLastError() << std::endl;
                    closesocket(client);
                    break;
                }
                updateActivity(client);
                // ring a bell
                std::cout << "\a";
                // include the accepted client in our main socket sets
                FD_SET(client, &sockets);
                // pass to the thread pool for further dealings
                threadPool(client);
            }
        }
    }
}

void Server::threadPool(SOCKET& client)
{
    size_t i;
    for (i = 0; i < CLIENTS; i++)
    {
        if (pool[i].available())
        {
            pool[i].enter(client);
            break;
        }
        else if (pool[i].isDestroyed())
        {
            pool[i].create();
            pool[i].enter(client);
            break;
        }
    }

    if (i == CLIENTS)
        std::cout << "\n No space left in the pool for client(" << client << ")\n";
}

// extra function;
void Server::updateActivity(SOCKET& client)
{
    std::ofstream file("clients.txt", std::ios::app);
    if (!file.is_open())
    {
        std::cerr << " An error occured while updating activity\n";
        return;
    }

    SOCKADDR_IN client_info = { 0 };
    int addrsize = sizeof(client_info);
    getpeername(client, (struct sockaddr*)&client_info, &addrsize);
    time_t now = time(0);
    char* dt = ctime(&now);

    file << std::left << " Client " << client << " connected from " << inet_ntoa(client_info.sin_addr)
        << ":" << ntohs(client_info.sin_port) << " - " << dt;
    file.close();
}

std::string Server::getActivity()
{
    std::ifstream file("clients.txt");
    if (!file.is_open())
    {
        std::cerr << " An error occured while opening activity\n";
        return "";
    }

    std::ostringstream ss;
    std::string str;
    while (getline(file, str))
        ss << str << std::endl;
    file.close();
    return ss.str();
}
void Server::closeSocket(SOCKET& sock) {
    closesocket(sock);
}
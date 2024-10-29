#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <thread>
#include <vector>

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

using namespace std;

namespace Server
{
    class Server{
        public: 
            bool logs = false;
            short int worker_count = 1;

        // windows setup
        private:
            #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                void windows_init(){
                    WSADATA wsa_data;
                    WORD w_version_requested = MAKEWORD(2,2);
                    int wsaerr = WSAStartup(w_version_requested, &wsa_data);

                    if(wsaerr != 0){
                        if(logs)cout << "WSAStartup failded with error: " << wsaerr << endl;
                        exit(1);
                    }else if(logs){
                        cout << "Detected Windows OS" << endl;
                        cout << "WSAStartup status :" << wsa_data.szSystemStatus << endl;
                    }
                }

            #endif

            sockaddr_in service;
            long long server_socket;
            short int port = 8080;
            string ip = "127.0.0.1";
            vector<thread> workers;

            short int server_loop(int val){
                while(true){
                    long long accepted_socket = accept(server_socket, NULL, NULL);
                    if(accepted_socket < 0){
                        cout << "Failed to accept connection" << endl;
                        #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                            closesocket(server_socket);
                            WSACleanup();
                        #endif
                        return -1;
                    }else{
                        cout << "Accepted connection " << val << endl;
                    }
                }
            }

        public:

            Server(){
                #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                    windows_init();
                #elif defined(__linux__) && logs == true
                    cout << "Detected Linux OS" << endl;
                #endif

                /**
                 * AF_INET: IPv4
                 * AF_INET6: IPv6
                 * AF_UNIX or AF_LOCAL: Unix Domain Socket local communication
                 * IPPROTO_TCP: TCP = 6
                 */
                server_socket = socket(AF_INET, SOCK_STREAM, 6);

                if(server_socket < 0){
                    if(logs)cout << "Falied to create socket " << endl;
                    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                        WSACleanup();
                    #endif
                    exit(1);
                }else if(logs == true){
                    cout << "Socket status: ACTIVE" << endl;
                }
            }

            ~Server(){
                #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                    closesocket(server_socket);
                    WSACleanup();
                #elif defined(__linux__)
                    close(server_socket);
                #endif

                for(thread &worker: workers){
                    worker.join();
                }
            }

            void set_ip(string ip){ this->ip = ip; }
            void set_logs(bool logs){ this->logs = logs; }
            void set_port(short int port){ this->port = port; }
            void set_worker_counts(short int worker_count){ this->worker_count = worker_count; }
            void set_url(string ip, short int port){
                this->ip = ip;
                this->port = port;
            }

            void bind_server(){
                service.sin_family = AF_INET;
                service.sin_port = htons(port);
                service.sin_addr.s_addr = inet_addr(ip.c_str());

                int bind_status = bind(server_socket, (sockaddr*)&service, sizeof(service));

                if(bind_status < 0){
                    if(logs)cout << "Failed to bind server" << endl;
                    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                        closesocket(server_socket);
                        WSACleanup();
                    #elif defined(__linux__)
                        close(server_socket);
                    #endif
                    exit(1);
                }else if(logs == true){
                    cout << "Server binded to " << ip << ":" << port << endl;
                }
            }

            void listen_server(){
                int listen_status = listen(server_socket, 5); // 5 is the maximum number of connections

                if(listen_status == EADDRINUSE){
                    cout << "Port " << port << " is already in use or socket is not bound. \nRun Server.bind_server() before this function" << endl;
                    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                        closesocket(server_socket);
                        WSACleanup();
                    #elif defined(__linux__)
                        close(server_socket);
                    #endif
                    exit(1);
                }else if(listen_status == EBADF || listen_status == ENOTSOCK){
                    cout << "Socket is not a valid file descriptor or does not refer to it" << endl;
                    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                        closesocket(server_socket);
                        WSACleanup();
                    #elif defined(__linux__)
                        close(server_socket);
                    #endif
                    exit(1);
                }else if(listen_status == EOPNOTSUPP){
                    cout << "The socket is not of a type that supports the listen() call" << endl;
                    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                        closesocket(server_socket);
                        WSACleanup();
                    #elif defined(__linux__)
                        close(server_socket);
                    #endif
                    exit(1);
                }else{
                    cout << "Server is listening on " << ip << ":" << port << endl;
                }

                for(short int i = 0; i < worker_count; i++){
                    workers.push_back(thread(&Server::server_loop, this,i));
                }
                for(thread &worker: workers){
                    worker.join();
                }
            }
            
    };
} // namespace Server


#endif // SERVER_H
#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <regex>
#include "payload.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__linux__)
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

using namespace std;

class HTTPResponse {
    public:
        int status_code;
        string content_type;
        string content;
        string body;

        HTTPResponse(int status_code, string content_type, string body ,string content) {
            this->status_code = status_code;
            this->content_type = content_type;
            this->content = content;
            this->body = body;
        }

        HTTPResponse() {};

        ~HTTPResponse() {};
};

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
bool windows_init()
{
    WSADATA wsa_data;
    WORD w_version_requested = MAKEWORD(2, 2);
    int wsaerr = WSAStartup(w_version_requested, &wsa_data);

    if(wsaerr != 0){
        return false;
    } else {
        return true;
    }
}
#endif

inline int get_status(string data) {
    regex status_regex("HTTP/1.1 (\\d{3})");
    smatch status_match;

    regex_search(data, status_match, status_regex);

    if (status_match.size() > 0){
        return stoi(status_match[1]);
    }else{
        return 500;
    }
}

inline string get_content_type(string data) {
    regex content_type_regex("Content-Type: ([^\\n\\s]*)");
    smatch match;

    regex_search(data, match, content_type_regex);
    if(match.size() > 0) return match[1];
    else return "text/plain";
}

inline string get_body(string data)
{
    regex body_regex("\\r?\\n\\r?\\n([\\s\\S]*)");
    smatch match;

    regex_search(data, match, body_regex);
    if (match.size() >= 2){
        return match[1];
    }else {
        return "";
    }
}

HTTPResponse http_fetch(string url, string route, string method, string data, int port, string content_type){
    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
        windows_init();
    #endif

    long long fetch_socket = socket(AF_INET, SOCK_STREAM, 6);

    if(fetch_socket < 0) {
        #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
            WSACleanup();
        #endif
        return HTTPResponse(500, "text/plain", "Falied to create socket for fetch request", "Falied to create socket for fetch request");
    }

    struct hostent *host = gethostbyname(url.c_str());

    sockaddr_in service;

    service.sin_family = AF_INET;
    if(port != 80){
        service.sin_port = htons(port);
    }else{
        service.sin_port = htons(80);
    }
    service.sin_addr.s_addr = *((unsigned long *)host->h_addr);

    int connection_status = connect(fetch_socket, (SOCKADDR *)(&service), sizeof(service));
    if (connection_status < 0)
    {
        #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
                WSACleanup();
        #endif
        closesocket(fetch_socket);
        return HTTPResponse(500, "text/plain", "Falied to connect socket for fetch request", "Falied to connect socket for fetch request");
    }

    string http_content;
    if(method == "POST"){
        http_content = method + " " + route + " HTTP/1.1\r\nHost: " + url + "\r\nContent-Length: " + to_string(strlen(data.c_str())) + "\r\nContent-Type: " + content_type + "\r\n\r\n" + data;
    } else {
         http_content = method + " " + route + " HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
    }

    send(fetch_socket, http_content.c_str(), strlen(http_content.c_str()), 0);

    char buffer[10000];
    long long data_length = recv(fetch_socket, buffer, 10000, 0);

    string response_data;
    for(int i = 0 ; i < data_length; i++){
        response_data += buffer[i];
    }

    return HTTPResponse(get_status(response_data), get_content_type(response_data), get_body(response_data), response_data);
}

#endif // HTTP_H
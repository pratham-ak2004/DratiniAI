#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <iostream>
#include <regex>
#include <unordered_map>
#include <fstream>
#include "mimes.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#include <winsock2.h>
#elif defined(__linux__)
#include <sys/socket.h>
#include <unistd.h>
#endif

#define buffer_size 1024 * 4

using namespace std;

namespace payload
{
    enum Method{
        GET,
        POST,
        PUT,
        _DELETE,
        UNKNOWN
    };

    inline string get_method_name(Method method){
        switch (method)
        {
        case GET:
            return "GET";
            break;
        case POST:
            return "POST";
            break;
        case PUT:
            return "PUT";
            break;
        case _DELETE:
            return "DELETE";
            break;
        case UNKNOWN:
        default:
            return "UNKNOWN";
            break;
        }
    }

    enum StatusCode{
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        CONFLICT = 409,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503
    };

    class Payload{
        public:
            string method;
            string route;
            string host;
            string query;
            string body;
            string content_type;

            int status_code;

            unordered_map<string, string> params;

        private:
            string url_decode(string value) {
                string result;
                char ch;
                int i, ii;
                for (i = 0; i < value.length(); i++) {
                    if (int(value[i]) == 37) {
                        sscanf(value.substr(i + 1, 2).c_str(), "%x", &ii);
                        ch = static_cast<char>(ii);
                        result += ch;
                        i = i + 2;
                    } else {
                        result += value[i];
                    }
                }
                return result;
            }

        protected:
            char buffer[buffer_size];
            string content;

            void set_method() {
                regex method_regex("GET|POST|PUT|DELETE");
                smatch match;
                Method method_type;

                regex_search(content, match, method_regex);

                if(match[0] == "GET"){ method_type = GET; }
                else if(match[0] == "POST"){ method_type = POST; }
                else if(match[0] == "PUT"){ method_type = PUT; }
                else if(match[0] == "DELETE"){ method_type = _DELETE; }
                else{ method_type = UNKNOWN; }

                method = get_method_name(method_type);
            }

            void set_route() {
                regex route_regex("((GET|POST|PUT|DELETE) (/[^ ]*) (HTTP/1.1))");
                smatch match;

                regex_search(content, match, route_regex);

                route = match[3];
            }

            void set_host() {
                regex host_regex("Host: ([^\\n ]*)");
                smatch match;

                regex_search(content, match, host_regex);

                host = match[1];
            }

            void set_query(){
                regex query_regex("([?][^ ]*)");
                smatch match;

                regex_search(route, match, query_regex);

                query = match[1];
            }

            void set_params(){
                string query_string = url_decode(query);

                for (int i = 0; i < query_string.size(); i++)
                {
                    if (query_string[i] == '?')
                        continue;

                    string key = "";
                    string value = "";
                    bool is_key = true;

                    for (; i < query_string.size(); i++)
                    {
                        if (query_string[i] == '='){
                            is_key = false;
                            continue;
                        }

                        if (query_string[i] == '&'){
                            break;
                        }

                        if(is_key){
                            key += query_string[i];
                        }else{
                            value += query_string[i];
                        }
                    }

                    params[key] = value;
                }
            }

            void set_content_type() {
                regex content_type_regex("Content-Type: ([^\\n ]*)");
                smatch match;

                regex_search(content, match, content_type_regex);

                if(match.size() > 0) content_type = match[0];
            }

            void set_body() {
                regex body_regex("Content-Length: \\d+\\s*\\r?\\n\\r?\\n([\\s\\S]*)");
                smatch match;

                regex_search(content,match,body_regex);
                if(match.size() >= 2) body = match[1];
            }

        public:
            string get_request() { return content; }
    };

    class Request:public Payload{
        public:
            Request(long long *client){
                recv(*client, buffer, buffer_size, 0);
                content = string(buffer);

                set_method();
                set_route();
                set_host();
                set_query();
                set_params();
                set_content_type();
                set_body();
            }
    };

    class Response:public Payload{
        private:
            vector<pair<string, string>> files;

            string response_header = "HTTP/1.1 ";

        public:
            Response(Request request){
                content = request.get_request();

                set_method();
                set_route();
                set_host();
                set_query();
                set_params();
                set_content_type();
                set_body();
                status_code = payload::StatusCode::OK;
            }

            void set_response_body(string body){
                this->body = body;
            }
            void set_response_content_type(string content_type) {
                this->content_type = mime::get_mime(content_type);
            }

            void send_message(long long *client) { 
                string response_content = response_header + to_string(status_code) + "\nContent-Type: " + content_type + "\n\n" + body;

                send(*client, response_content.c_str(), response_content.size(), 0);
            }

            void set_html_content(string file_path){
                fstream file(file_path);
                
                if(!file.is_open()) {
                    cerr << "Error opening the file";
                }else{
                    string line, file_content;

                    while(getline(file,line)){
                        file_content += line;
                    }
                    set_response_body(file_content);
                    set_response_content_type("html");

                    file.close();
                }
            }
    };
}// namespace payload


#endif // PAYLOAD_H
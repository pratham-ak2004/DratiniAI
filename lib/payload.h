#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <iostream>

using namespace std;

namespace payload
{
    enum Method{
        GET,
        POST,
        PUT,
        DELETE
    };

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
            Method method;
            string route;
            string query;
            string body;
            string headers;
            int status_code;
    };

    class Request:public Payload{
        public:
            // Request(char buffer[]){
            //     // parse buffer
            // }
    };
}// namespace payload


#endif // PAYLOAD_H
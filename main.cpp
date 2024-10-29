#include <iostream>
#include "lib/server.h"

int main(){
    Server::Server server;
    server.set_logs(true);
    server.set_worker_counts(3);
    server.bind_server();

    server.listen_server();
    return 0;
}
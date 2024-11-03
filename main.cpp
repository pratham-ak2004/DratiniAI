#include <iostream>
#include "lib/server.h"

int main(){
    Server::Server server;
    Server::Router router;
    server.set_logs(true);
    server.set_worker_counts(3);
    server.bind_server();
    server.set_router(&router);

    router.GET("/", [](Request request, Response response){
        // response.set_status_code(payload::StatusCode::OK);
        // response.set_response_content_type("html");
        // response.set_response_body("<html><head><script src='https://cdn.tailwindcss.com'></script></head><body class='h-screen w-screen bg-red-500'> hello world </body></html>");
        response.set_html_content("./src/index.html");
        return response;
    });

    server.listen_server();
    return 0;
}
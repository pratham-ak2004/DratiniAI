#include <iostream>
#include "lib/server.h"
#include "lib/environment.h"

int main(){
    Server::Server server;
    Server::Router router;
    Envir::Environment env("./.env");
    
    server.set_logs(true);
    server.set_worker_counts(3);
    server.set_ip(env.get_env("HOST").second);
    server.set_port(stoi(env.get_env("PORT").second, nullptr, 10));

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
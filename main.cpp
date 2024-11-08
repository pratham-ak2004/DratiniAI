#include <iostream>
#include "lib/server.h"
#include "lib/environment.h"
#include "lib/nlohmann/json.hpp" // https://github.com/nlohmann/json
#include "lib/http.h"

using json = nlohmann::json;

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

    router.GET("/test", [](Request request, Response response){
        // json json_body;
        // json_body["message"] = "hello world";
        // json_body["status"] = "ok";

        // cout << json_body.dump() << endl;
        // response.set_response_json(json_body);

        // HTTPResponse res = http_fetch("127.0.0.1", "/", "GET", "", 8080);

        return response; 
    });

    router.POST("/api/gemini/prompt", [](Request request, Response response)
                {
        /**
         * Input: prompt, language
         * Output: response text
         */

        json data = request.json_body;

        if(data["prompt"].is_null()){
            response.set_response_status(payload::StatusCode::NO_CONTENT);
            response.set_response_body("Prompt is required");
            response.set_response_content_type("text");
            return response;
        }else{
            string language;
            if(!data["language"].is_null()){
                language = data["language"];
            }

            Envir::Environment env("./.env");
            string url = "generativelanguage.googleapis.com";
            string route = "/v1beta/models/gemini-1.5-flash:generateContent?key=" + env.get_env("GEMINI_API_KEY").second;

            json input_data = json::parse(R"(
            {
                "contents": [{
                    "parts":[{"text": "Write a story about a magic backpack."}]
                    }]
            }
            )");
            HTTPResponse res = http_fetch(url, route, "POST", "", 80, "application/json");

    cout << res.content << endl;

    response.set_html_content("./src/index.html");
    return response;
        }
    });

    server.listen_server();
    return 0;
}
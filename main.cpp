#include <iostream>
#include "lib/server.h"
#include "lib/environment.h"
#include "lib/nlohmann/json.hpp" // https://github.com/nlohmann/json
// #include "lib/http.h"

using json = nlohmann::json;

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

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
            string prompt = data["prompt"];
            string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + env.get_env("GEMINI_API_KEY").second;
            string parts = "{ \"contents\" : [{ \"parts\": [{ \"text\" : \"" +  prompt + "\"}] }]}";
            string command = "curl \"" + url + "\" -X POST -H 'Content-Type: application/json' -d '" + parts + "'";
            std::string output = exec(command.c_str());

            std::regex text_regex(R"(\"text\":\s*\"([^"]+)\")");
            std::smatch match;
            regex_search(output, match, text_regex);

            json response_json;
            response_json["response"] = match[1];
            response.set_response_json(response_json);

            return response;
        }
    });

    server.listen_server();
    return 0;
}
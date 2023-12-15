#include <sys/resource.h>
#include <sys/time.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>

#include "httpmessage.h"
#include "httpserver.h"

using http_server::HttpMethod;
using http_server::HttpRequest;
using http_server::HttpResponse;
using http_server::HttpServer;
using http_server::HttpStatusCode;

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cout << "./server <server IP> <port number> <web object directory path>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    int port = strtol(argv[2], nullptr, 10);
    HttpServer server(host, port);
    std::string web_object_path(argv[3]);

    auto send_html = [&](const HttpRequest& request) -> HttpResponse {
        HttpResponse response(HttpStatusCode::Ok);

        std::ifstream t(web_object_path.c_str()); // html_files/Large_HTML_page_with_images.html
        std::stringstream buffer;
        buffer << t.rdbuf();

        std::string content = buffer.str();

        response.SetHeader("Content-Type", "text/html");
        response.SetContent(content);
        return response;
    };

    server.RegisterHttpRequestHandler("/", HttpMethod::GET, send_html);

    try {
        std::cout << "Starting the web server.." << std::endl;
        server.Start();
        std::cout << "Server listening on " << host << ":" << port << std::endl;

        std::cout << "Enter [quit] to stop the server" << std::endl;
        std::string command;
        while (std::cin >> command, command != "quit") {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "'quit' command entered. Stopping the web server.."
                << std::endl;
        server.Stop();
        std::cout << "Server stopped" << std::endl;
    } catch (std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
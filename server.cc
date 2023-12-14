#include <sys/resource.h>
#include <sys/time.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "httpmessage.h"
#include "httpserver.h"

using http_server::HttpMethod;
using http_server::HttpRequest;
using http_server::HttpResponse;
using http_server::HttpServer;
using http_server::HttpStatusCode;

void ensure_enough_resource(int resource, std::uint32_t soft_limit,
                            std::uint32_t hard_limit) {
    rlimit new_limit, old_limit;

    new_limit.rlim_cur = soft_limit;
    new_limit.rlim_max = hard_limit;
    getrlimit(resource, &old_limit);

    std::cout << "Old limit: " << old_limit.rlim_cur << " (soft limit), "
                << old_limit.rlim_cur << " (hard limit)." << std::endl;
    std::cout << "New limit: " << new_limit.rlim_cur << " (soft limit), "
                << new_limit.rlim_cur << " (hard limit)." << std::endl;

    if (setrlimit(resource, &new_limit)) {
        std::cerr << "Warning: Could not update resource limit ";
        std::cerr << "(" << strerror(errno) << ")." << std::endl;
        std::cerr << "Consider setting the limit manually with ulimit" << std::endl;
        exit(-1);
    }
}

int main(int argc, char **argv) {
    std::string host = "0.0.0.0";
    int port = 8080;
    HttpServer server(host, port);


    auto send_html = [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response(HttpStatusCode::Ok);
        std::string content;
        content += "<!doctype html>\n";
        content += "<html>\n<body>\n\n";
        content += "<h1>Hello, world in an Html page</h1>\n";
        content += "<p>A Paragraph</p>\n\n";
        content += "</body>\n</html>\n";

        response.SetHeader("Content-Type", "text/html");
        response.SetContent(content);
        return response;
    };

    server.RegisterHttpRequestHandler("/", HttpMethod::GET, say_hello);
    server.RegisterHttpRequestHandler("/hello.html", HttpMethod::GET, send_html);

    try {
        // std::cout << "Setting new limits for file descriptor count.." <<
        // std::endl; ensure_enough_resource(RLIMIT_NOFILE, 15000, 15000);

        // std::cout << "Setting new limits for number of threads.." << std::endl;
        // ensure_enough_resource(RLIMIT_NPROC, 60000, 60000);

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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.hpp"


void handleGetRequest(int client_socket) {
    std::ifstream file(file_path1);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path1 << std::endl;
        return;
    }

    std::ostringstream content;
    content << file.rdbuf();
    file.close();

    std::string file_content = content.str();
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + file_content;
    send(client_socket, response.c_str(), response.length(), 0);
}

void handlePostRequest(int client_socket, const std::string& post_data) {
    std::istringstream post_stream(post_data);

    std::string line;
    std::string name;

    // Đọc từng dòng trong dữ liệu POST để tìm tên người nhập
    while (std::getline(post_stream, line)) {
        if (line.find("s_name=") != std::string::npos) {
            // Tìm thấy dòng chứa tên người nhập
            // Trích xuất tên người nhập từ dòng
            std::size_t pos = line.find("=");
            if (pos != std::string::npos) {
                name = line.substr(pos + 1);
                break;
            }
        }
    }

    if (!name.empty()) {
        std::cout << "Tên người nhập: " << name << std::endl;

        // Thực hiện hành động mong muốn dựa trên tên người nhập
        // Ví dụ: chuyển hướng, xử lý dữ liệu, v.v.

        // Sau đó, bạn có thể gửi phản hồi đến client
        std::ifstream file(file_path2);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << file_path2 << std::endl;
            return;
        }

        std::ostringstream content;
        content << file.rdbuf();
        file.close();

        std::string file_content = content.str();
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + file_content;
        send(client_socket, response.c_str(), response.length(), 0);
    } else {
        std::cerr << "Tên người nhập không được tìm thấy trong dữ liệu POST\n";
        // Gửi phản hồi lỗi hoặc thực hiện hành động khác tùy thuộc vào yêu cầu của bạn
    }
}

void server(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    // Thiết lập địa chỉ server
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket\n";
        return;
    }

    // Lắng nghe các kết nối đến
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen\n";
        return;
    }

    std::cout << "Server is running on port " << PORT << std::endl;

    // Chấp nhận kết nối và xử lý yêu cầu
    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        // Đọc yêu cầu từ client
        char buffer[1024] = {0};
        read(client_socket, buffer, 1024);
        std::cout << "Received: " << buffer << std::endl;

        std::string request(buffer);
        if (request.find("GET") == 0) {
            handleGetRequest(client_socket);
        } else if (request.find("POST") == 0) {
            handlePostRequest(client_socket, request);
        } else {
            std::cerr << "Unsupported request\n";
        }

        // Đóng kết nối
        close(client_socket);
    }
}
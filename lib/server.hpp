#include <iostream>
#include <string>
#pragma once

const int PORT = 8080;

const std::string file_path1="../hello_html/index.html";
const std::string file_path2="../hello_html/hello.html";
void server();
void get(int client_socket);
void handleGetRequest(int client_socket);
void handlePostRequest(int client_socket, const std::string& post_data);
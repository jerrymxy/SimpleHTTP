#include "../TCPSocket/tcpSocket.hpp"
#include <string>

const char* IP_ADDR = "127.0.0.1";
const u_short PORT = 12345;

std::string genGetRequest(const std::string&);
std::string genPostRequest(const std::string&, const std::string&);

int main() {
    initSocket();

    std::cout << "Client started..." << std::endl;
    std::string msg;
    // std::getline(std::cin, msg);

    while (true) {
        SOCKET tcp_socket = createClientSocket(IP_ADDR, PORT);
        std::cout << "Enter resource path, enter / to get index.html, enter exit to exit:" << std::endl;
        std::string path;
        std::cin >> path;
        if (path == "exit") {
            send(tcp_socket, "exit", 5, 0);
            closesocket(tcp_socket);
            break;
        }
        if (path[0] != '/') {
            path.insert(0, "/");
        }
        std::string param;
        std::cout << "Enter params or enter / to skip" << std::endl << "Example: data=hello&type=1" << std::endl;
        std::cin >> param;
        if (param == "/") param = "";
        std::cout << "Choose a method, 0 to exit:" << std::endl << "1: GET" << std::endl << "2: POST" << std::endl;
        unsigned short method;
        std::cin >> method;
        if (method == 0) break;
        switch (method) {
        case 1:
            if (param.empty())
                msg = genGetRequest(path);
            else
                msg = genGetRequest(path + "?" + param);
            break;
        case 2:
            msg = genPostRequest(path, param);
            break;
        default:
            std::cerr << "Wrong number, please retry." << std::endl;
            continue;
        }
        // Send data to server
        if (send(tcp_socket, msg.c_str(), msg.size(), 0) == SOCKET_ERROR) {
            err("send");
        }

        // Receive data
        const int LEN = 1024; // Buf size
        char buf[LEN] = { 0 };
        if (recv(tcp_socket, buf, LEN, 0) > 0) {
            std::cout << "Recv: " << std::endl << buf << std::endl;
        }
        closesocket(tcp_socket);
    }

    cleanSocket();

    return 0;
}

std::string genGetRequest(const std::string& url) {
    std::string user_agent = "MyClient/1.0 (Windows NT 10.0; Win64; x64)"; // User-Agent
    std::string first_line = "GET " + url + " HTTP/1.0\r\n"; // First line of header
    // Header
    std::string header =
        "Host: " + std::string(IP_ADDR) + ":" + std::to_string(PORT) + "\r\n" +
        "Connection: keep-alive\r\n" +
        "User-Agent: " + user_agent + "\r\n" +
        "Accept: text/html,application/xhtml+xml\r\n" +
        "Accept-Language: zh-CN,zh\r\n" +
        "\r\n";

    // HTTP request message
    std::string request = first_line + header;
    return request;
}

std::string genPostRequest(const std::string& url, const std::string& param) {
    std::string user_agent = "MyClient/1.0 (Windows NT 10.0; Win64; x64)"; // User-Agent
    std::string first_line = "POST " + url + " HTTP/1.0\r\n"; // First line of header
    std::string content = "data=hello&from=post";
    // Header
    std::string header =
        "Host: " + std::string(IP_ADDR) + ":" + std::to_string(PORT) + "\r\n" +
        "Connection: keep-alive\r\n" +
        "User-Agent: " + user_agent + "\r\n" +
        "Accept: text/html,application/xhtml+xml\r\n" +
        "Accept-Language: zh-CN,zh\r\n" +
        "Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n" +
        "Content-Length: " + std::to_string(param.size()) + "\r\n" +
        "\r\n";

    // HTTP request message
    std::string request = first_line + header + param;
    return request;
}
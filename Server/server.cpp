#include "../TCPSocket/tcpSocket.hpp"
#include "utils.hpp"

std::string genReply(const char*); // Deliver HTTP request string to POST/GET

const std::string http_version = "HTTP/1.0"; // Default HTTP version
const int LEN = 1024 * 1024; // Buf size
char buf[LEN] = { 0 }; // Buffer

int main() {
	initSocket();

	SOCKET tcp_socket = createServerSocket();

	std::cout << "Server started. Waiting for connection." << std::endl;

	size_t fail_count = 0;

	while (true) {
		if (fail_count > 10) {
			break;
		}
		memset(buf, 0, LEN);
		// Receive request from client
		SOCKET client_socket = accept(tcp_socket, nullptr, nullptr);
		if (client_socket == INVALID_SOCKET) {
			err("accept");
			++fail_count;
		}

		std::cout << "Listening..." << std::endl;

		// Receive data from client socket
		if (recv(client_socket, buf, LEN, 0) > 0) {
			if (std::string(buf) == "exit") {
				std::cout << "Client exits." << std::endl;
				closesocket(client_socket);
				break;
			}
			std::cout << "Recv: " << std::endl << buf << std::endl;
		}

		// Reply to client
		std::string re = genReply(buf);
		if (send(client_socket, re.c_str(), re.size(), 0) == SOCKET_ERROR) {
			err("send");
			++fail_count;
		}
		closesocket(client_socket);
	}

	closesocket(tcp_socket);

	cleanSocket();

	return 0;
}

// 
auto processGET(std::map<std::string, std::string>& request_field) {
	// Get file path from URL
	std::string filepath = request_field["url"].substr(0, request_field["url"].find('?'));

	std::string body = "";
	std::string content_type = "text/html; charset=utf-8";
	std::string status_code = "200 OK";
	std::string date = getDate();

	// Return index.html by default
	if (filepath == "/") {
		filepath = "/index.html";
	}
	if (request_field["url"].find('?') < request_field["url"].size()) {
		std::string param_str = request_field["url"].substr(request_field["url"].find('?') + 1);
		std::map<std::string, std::string> request_param = getRequestParam(param_str);
		auto it_param = request_param.begin();
		// Print params and values
		std::cout << std::endl << "Received GET param:" << std::endl;
		while (it_param != request_param.end()) {
			std::cout << it_param->first << ": " << it_param->second << std::endl;
			it_param++;
		}
	}

	// Read html file, return 404 when file not found
	if ((body = readFile(filepath)) == "404") {
		return http_version + " 404 Not Found" + "\r\n" +
			"Date: " + date + "\r\n\r\n";
	}

	// Reply request
	std::string reply = http_version + " " + status_code + "\r\n" +
		"Date: " + date + "\r\n" +
		"Content-Type: " + content_type + "\r\n" +
		"Content-Length: " + std::to_string(body.size()) + "\r\n" +
		"\r\n" +
		body;
	return reply;
}

auto processPOST(std::map<std::string, std::string>& request_field) {
	std::string date = getDate();

	// Current only support Content-Type: application/x-www-form-urlencoded
	if (request_field["Content-Type"].find("application/x-www-form-urlencoded") == std::string::npos) {
		return request_field["http_version"] + " 400 Bad Request" + "\r\n" +
			"Date: " + date + "\r\n\r\n";
	}
	std::string reply = "";

	std::string filepath = request_field["url"];
	std::string body = "";
	std::string content_type = "text/html; charset=utf-8";
	std::string status_code = "200 OK";
	
	if (filepath == "/") {
		filepath = "/index.html"; // Return index.html by default
	}

	std::map<std::string, std::string> request_param = getRequestParam(request_field["Body"]);
	auto it_param = request_param.begin();
	// Print params and values
	if (!request_param.empty()) {
		std::cout << std::endl << "Received POST param:" << std::endl;
		while (it_param != request_param.end()) {
			std::cout << it_param->first << ": " << it_param->second << std::endl;
			it_param++;
		}
	}

	// Read html file, return 404 when file not found
	if ((body = readFile(filepath)) == "404") {
		return request_field["http_version"] + " 404 Not Found" + "\r\n" +
			"Date: " + date + "\r\n\r\n";
	}

	// Reply request
	reply = http_version + " " + status_code + "\r\n" +
		"Date: " + date + "\r\n" +
		"Content-Type: " + content_type + "\r\n" +
		"Content-Length: " + std::to_string(body.size()) + "\r\n" +
		"\r\n" +
		body;

	return reply;
}

std::string genReply(const char* request) {
	std::string str_request = std::string(request);
	auto request_field = getRequestField(splitString(str_request));
	std::string reply = "";
	if (request_field["request_type"] == "GET") {
		reply = processGET(request_field);
	} else if (request_field["request_type"] == "POST") {
		reply = processPOST(request_field);
	} else {
		// Unknown request method, return 400
		reply = http_version + " 400 Bad Request\r\n\r\n";
	}
	return reply;
}


#include "server.h"

namespace {
    bool send_all(int fd, const string& response) {
        size_t sent = 0;

        while (sent < response.size()) {
            ssize_t n = ::send(
                fd,
                response.data() + sent,
                response.size() - sent,
                MSG_NOSIGNAL
            );
            if (n < 0 && errno == EINTR) {
                continue;
            }
            if (n <= 0) {
                return false;
            }
            sent += static_cast<size_t>(n);
        }
        return true;
    }
}

Server::Server(size_t worker_num) : store_(), pool_(worker_num) {}

void Server::start() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) {
        perror("socket");
        return;
    }
    cout << "socket created..." << endl;
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6379);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }
    cout << "bind success" << endl;

    if (listen(server_fd, 10) == -1) {
        perror("lisen");
        close(server_fd);
        return;
    }
    cout << "MiniKv listening on 127.0.0.1:6379" << endl;

    while (true) {

    int client_fd = accept(server_fd, nullptr, nullptr);

    if (client_fd == -1) {
        perror("accept");
        continue;
    }
    
    try
    {
        pool_.equeue([this, client_fd]() {
            handle_client(client_fd);
            
        });
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        close(client_fd);
    }
    

    cout << "client connected..." << endl;


}
    close(server_fd);
    
}

void Server::handle_client(int client_fd) {
    // 局部资源守卫：函数返回或抛出异常时，自动关闭连接。
    struct SocketGuard {
        int fd;

        ~SocketGuard() {
            ::close(fd);
        }
    };

    SocketGuard guard{client_fd};

    char buffer[1024];
    std::string pending;

    // 限制一条命令的长度，避免未结束的命令无限占用内存。
    constexpr size_t max_command_size = 8192;

    while (true) {
        ssize_t n = ::read(client_fd, buffer, sizeof(buffer));

        if (n == 0) {
            // 对端关闭连接。
            return;
        }

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("read");
            return;
        }

        // 按实际读取长度追加，不需要添加 '\0'。
        pending.append(buffer, static_cast<size_t>(n));

        while (true) {
            size_t pos = pending.find('\n');

            if (pos == std::string::npos) {
                break;
            }

            if (pos > max_command_size) {
                send_all(client_fd, "ERROR Command too long\n");
                return;
            }

            std::string line = pending.substr(0, pos);
            pending.erase(0, pos + 1);

            // 兼容客户端发送的 "\r\n"。
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            std::string response = execute_command(line);

            if (!send_all(client_fd, response)) {
                return;
            }
        }

        // 检查尚未遇到换行的那部分数据。
        if (pending.size() > max_command_size) {
            send_all(client_fd, "ERROR Command too long\n");
            return;
        }
    }
}

string Server::execute_command(const string& line) {
    std::istringstream ss(line);
    std::string command;
    ss >> command;

    if (command == "SET") {
        std::string key;
        std::string value;
        std::string extra;

        if (!(ss >> key >> value) || (ss >> extra)) {
            return "ERROR Usage: SET <KEY> <VAL>\n";
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            store_.set(key, value);
        }

        return "OK\n";
    }

    if (command == "GET") {
        std::string key;
        std::string value;
        std::string extra;

        if (!(ss >> key) || (ss >> extra)) {
            return "ERROR Usage: GET <KEY>\n";
        }

        bool found;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            found = store_.get(key, value);
        }

        if (found) {
            return value + "\n";
        }

        return "NOT_FOUND\n";
    }

    if (command == "DEL") {
        std::string key;
        std::string extra;

        if (!(ss >> key) || (ss >> extra)) {
            return "ERROR Usage: DEL <KEY>\n";
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            store_.del(key);
        }

        return "OK\n";
    }

    return "ERROR Unknown command\n";
}
#include "server.h"


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
        char buffer[1024];
        while (true) {
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';

        cout << "client says:" << buffer << endl;

        string line(buffer);
        stringstream ss(line);

        string command;
        ss >> command;

        if (command == "SET") {
            unique_lock<mutex> lock(mtx_);
            string key;
            string val;
            string temp;
            
        if (!(ss >> key >> val)) {
            string response = "Usage SET <KEY> <VAL>\n";
            write(client_fd, response.c_str(), response.size());
        } else  if (ss >> temp) {
            string response = "Usage SET <KEY> <VAL>\n";
            write(client_fd, response.c_str(), response.size());
        }else {
            store_.set(key, val);
            string response = "OK\n";
            write(client_fd, response.c_str(), response.size());
        }
        }

        if (command == "DEL") {
            unique_lock<mutex> lock(mtx_);
            string key;
            string temp;
            if (!(ss >> key)) {
                string response = "Usage SET <KEY> <VAL>\n";
                write(client_fd, response.c_str(), response.size());
            } else if (ss >> temp) {
                string response = "Usage SET <KEY> <VAL>\n";
                write(client_fd, response.c_str(), response.size());
            } else {
                store_.del(key);
                string response = "OK\n";
                write(client_fd, response.c_str(), response.size());
            }
        }

        if (command == "GET") {
            unique_lock<mutex> lock(mtx_);
            string key;
            string val;
            string temp;

            if (!(ss >> key)) {
                string response = "Usage SET <KEY> <VAL>\n";
                write(client_fd, response.c_str(), response.size());
            } else if(ss >> temp) {
                string response = "Usage SET <KEY> <VAL>\n";
                write(client_fd, response.c_str(), response.size());
            } else {
                if (store_.get(key, val)) {

                string response1 = val + "\n";
                write(client_fd, response1.c_str(), response1.size());
                string response = "OK\n";
                write(client_fd, response.c_str(), response.size());
                } else {
                    string response = "no such name\n";
                    write(client_fd, response.c_str(), response.size());
                }
                
            }
        }

    }
}

    //write(client_fd, buffer, n);
    close(client_fd);
}
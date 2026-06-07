#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#define PORT "8080"
#define BACKLOG 10
#define MAX_EVENTS 10

// 1. SHARED BACKEND RESOURCES
std::vector<char> shared_backend_buffer;
int data_notifier_fd; 

// Track connected client FDs so the single eventfd signal knows who to broadcast to
std::vector<int> connected_clients;

// 2. SIMULATED BACKEND THREAD
void backend_worker_thread() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Modify the shared memory buffer
        shared_backend_buffer.push_back('A'); 
        std::cout << "[Backend] Modified buffer data. Ringing doorbell..." << std::endl;

        // Ring the doorbell: Write 8-byte integer '1' to eventfd
        uint64_t signal = 1;
        write(data_notifier_fd, &signal, sizeof(signal));
    }
}

// Signal handler to reap dead/zombie processes
void sigchld_handler(int s) {
    (void)s; 
    int saved_errno = errno;
    while (waitpid(-1, nullptr, WNOHANG) > 0);
    errno = saved_errno;
}

// Get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(reinterpret_cast<struct sockaddr_in*>(sa)->sin_addr);
    }
    return &(reinterpret_cast<struct sockaddr_in6*>(sa)->sin6_addr);
}

int main() {
    addrinfo hints{}, *res, *p;
    int status;
    int sockfd{-1};
    int yes{1};
    int rv{};

    char s[INET6_ADDRSTRLEN]; 
    socklen_t sin_size{};
    struct sigaction sa{};
    struct sockaddr_storage their_addr{}; 

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for(p = res; p != nullptr; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server:socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
            perror("setsockopt SO_REUSEADDR failed");
            exit(1);
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(res); 

    if (p == NULL) {
        std::cerr << "server: failed to bind\n";
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        perror("sigaction");
        exit(1);
    }

    // --- EPOLL AND EVENTFD SETUP ---

    // Create single master epoll instance
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if (epfd == -1) { perror("epoll_create1"); return 1; }

    // Create the shared eventfd
    data_notifier_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (data_notifier_fd == -1) { perror("eventfd"); return 1; }

    // Register eventfd into epoll
    struct epoll_event notifier_ev{};
    notifier_ev.events = EPOLLIN; 
    notifier_ev.data.fd = data_notifier_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, data_notifier_fd, &notifier_ev);

    // Register primary listening socket into epoll
    struct epoll_event server_ev{};
    server_ev.events = EPOLLIN; 
    server_ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &server_ev);

    // Spin up background processing thread
    std::thread background_thread(backend_worker_thread);
    background_thread.detach(); 

    struct epoll_event incoming_events[MAX_EVENTS];
    std::cout << "server: waiting for connections on port " << PORT << "..." << std::endl;

    // --- CENTRAL EVENT LOOP ---
    while(true) {
        // Main thread goes to sleep here natively until an event occurs
        int num_events = epoll_wait(epfd, incoming_events, MAX_EVENTS, -1);
        if (num_events == -1) {
            if (errno == EINTR) continue; // Restart if interrupted by signal handler
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_events; i++) {
            int current_fd = incoming_events[i].data.fd;

            // CASE 1: New connection request arriving on the listening socket
            if (current_fd == sockfd) {
                sin_size = sizeof their_addr;
                int new_fd = accept(sockfd, reinterpret_cast<struct sockaddr*>(&their_addr), &sin_size);
                if (new_fd == -1) {
                    perror("accept");
                    continue;
                }

                inet_ntop(their_addr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr*>(&their_addr)), s, sizeof s);
                std::cout << "server: got connection from " << s << " (FD: " << new_fd << ")" << std::endl;

                // Register the new connection socket into epoll
                struct epoll_event client_ev{};
                client_ev.events = EPOLLIN; 
                client_ev.data.fd = new_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &client_ev) == -1) {
                    perror("epoll_ctl: add client");
                    close(new_fd);
                } else {
                    connected_clients.push_back(new_fd); // Track the active client
                }
            }
            
            // CASE 2: The backend thread modified data and triggered eventfd
            else if (current_fd == data_notifier_fd) {
                uint64_t clear_val;
                read(data_notifier_fd, &clear_val, sizeof(clear_val)); // Reset the doorbell counter

                std::cout << "[Main Loop] Shared buffer updated! Broadcasting data to all connected clients..." << std::endl;

                // Broadcast the data out from the shared RAM vector layout to all connected users
                for (int client_fd : connected_clients) {
                    send(client_fd, shared_backend_buffer.data(), shared_backend_buffer.size(), 0);
                }
            }
            
            // CASE 3: Active connection received inbound network data or hung up
            else {
                char network_buffer[1024];
                int bytes_received = recv(current_fd, network_buffer, sizeof(network_buffer), 0);
                
                if (bytes_received <= 0) {
                    // Connection closed or error encountered
                    std::cout << "server: client on FD " << current_fd << " disconnected." << std::endl;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, current_fd, nullptr);
                    close(current_fd);

                    // Remove from active client tracking vector
                    for (auto it = connected_clients.begin(); it != connected_clients.end(); ++it) {
                        if (*it == current_fd) {
                            connected_clients.erase(it);
                            break;
                        }
                    }
                } else {
                    std::cout << "server: received " << bytes_received << " bytes from client FD " << current_fd << std::endl;
                }
            }
        }
    }

    close(sockfd);
    close(epfd);
    close(data_notifier_fd);
    return 0;
}
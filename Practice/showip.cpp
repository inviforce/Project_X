#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc,char* argv[]){
    addrinfo hints{},*res,*p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    if(argc !=2){
        std::cerr<<"usage:showip hostname";
        return 1;
    }
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    if((status=getaddrinfo(argv[1],"https",&hints,&res))!=0){
        std::cerr << "getaddrinfo: " << gai_strerror(status) << '\n';
        return 2;
    }
    std::cerr<<"IP address for:"<<argv[1]<<"\n";
    p=res;
    while(p!=nullptr){
        void* addr;
        int type;
        const char* ipver;
        if (p->ai_family == AF_INET) {  // IPv4
            auto* ipv4 = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            addr = &(ipv4->sin_addr);
            type = ipv4->sin_port;
            ipver = "IPv4";
        }
        else {                          // IPv6
            auto* ipv6 = reinterpret_cast<sockaddr_in6*>(p->ai_addr);
            addr = &(ipv6->sin6_addr);
            type = ipv6->sin6_port;
            ipver = "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        std::cout << "  " << ipver << ": " << ipstr << ": " << type <<'\n';
        p = p->ai_next;
    }
    freeaddrinfo(res);

    return 0;
}
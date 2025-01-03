#include <fcgiapp.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#define ERROR "\033[1;31m[[ ERROR ]]: \033[0m"

int main(int argc, char*[]) {

    if (FCGX_Init() != 0) {
        std::cerr << ERROR "Unable to initialize FCGX" << std::endl;
        return 1;
    }

    const char *socketPath = "/var/run/input.sock";
    int sock_fd = FCGX_OpenSocket(socketPath, 1024);
    if (sock_fd < 0) {
        std::cerr << ERROR "unable to open UNIX domain socket: " << socketPath << std::endl;
        return 1;
    }

    FCGX_Request request;
    FCGX_InitRequest(&request, sock_fd, 0);

    std::cout << "Successfully init request, start listening\n";

    while (true) {
        if (FCGX_Accept_r(&request) >= 0) {
            std::string str;
            int n;

            do {
                char tmp[256];
                n = FCGX_GetStr(tmp, 256, request.in);
                str.append(tmp, n);
            } while (n == 256);

            std::cout << "Got Request:\n";
            std::cout << str;
            std::cout << "End of Request\n\n";
        } else {
            std::cerr << ERROR "Something went wrong!" << std::endl;
        }
    }

    return 0;
}

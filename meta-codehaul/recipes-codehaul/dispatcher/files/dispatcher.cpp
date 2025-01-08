#include <fcgiapp.h>
#include <getopt.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "dispatcher.grpc.pb.h"

#define ERROR "\033[1;31m[[ ERROR ]]: \033[0m"

void printHelp() {
    std::cout << "Usage: dispather [options]\n"
              << "Options:\n"
              << "  -h, --help              Show this help message\n"
              << "  -l, --log-level LEVEL   Set log level (DEBUG, ERROR, WARNING)\n"
              << "  -g, --grpc-socket SOCKET Specify gRPC socket\n"
              << "  -i, --input-socket SOCKET Specify input socket\n";
}

struct
{
    std::string logLevel;
    std::string grpcSocket;
    std::string inputSocket;
} args;

bool parseArgs(int argc, char* argv[]) {
    int option;

    // Define the long options
    struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"log-level", required_argument, nullptr, 'l'},
        {"grpc-socket", required_argument, nullptr, 'g'},
        {"input-socket", required_argument, nullptr, 'i'},
        {nullptr, 0, nullptr, 0} // End of options
    };

    // Process the command line arguments
    while ((option = getopt_long(argc, argv, "hl:g:i:", long_options, nullptr)) != -1) {
        switch (option) {
        case 'h':
            printHelp();
            return 0;
        case 'l':
            args.logLevel = optarg;
            if (args.logLevel != "DEBUG" && args.logLevel != "ERROR" && args.logLevel != "WARNING") {
                std::cerr << "Invalid log level: " << optarg << "\n";
                return 1;
            }
            break;
        case 'g':
            args.grpcSocket = optarg;
            break;
        case 'i':
            args.inputSocket = optarg;
            break;
        case '?':
            return false;
        default:
            printHelp();
            return false;
        }
    }

    return true;
}

struct UserRequest
{
    const std::string user;
    const std::string token;
    const std::string code;

    template <typename T, typename U, typename V>
    UserRequest(T&& t, U&& u, V&& v) : user(std::forward<T>(t)), token(std::forward<U>(u)), code(std::forward<V>(v)) { }
};

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using ::grpc::Status;

using ::codehaul::compilation::Task;
using ::codehaul::compilation::TaskRequest;
using ::codehaul::compilation::TaskResponce;
using ::codehaul::compilation::TaskResult;

class DispatcherServiceImpl final : public codehaul::compilation::Dispatcher::Service {

    std::mutex              mut;
    std::queue<UserRequest> queue;
    std::condition_variable cv;

    Status GetTask(ServerContext*, const TaskRequest* request, Task* response) {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        std::unique_lock<std::mutex> lock(mut);

        cv.wait(lock, [this] { return !queue.empty(); });

        UserRequest userRequest = queue.front();
        queue.pop();

        response->set_id(0);
        response->set_code(userRequest.code);

        return Status::OK;
    }

    Status SetResult(ServerContext*, const TaskResult* request, TaskResponce* response) { return Status::OK; }

  public:
    template <typename... Args>
    void AddTask(Args&&... request) {
        {
            std::lock_guard<std::mutex> lock(mut);
            queue.emplace(std::forward<Args>(request)...);
        }
        cv.notify_one();
    }
};

void RunServer(DispatcherServiceImpl& service, const std::string& socket) {

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    ServerBuilder builder;
    builder.AddListeningPort(socket, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << socket << std::endl;

    server->Wait();
}

std::string to_string(FCGX_Stream* stream) {
    std::string str;
    int         n;
    do {
        char tmp[256];
        n = FCGX_GetStr(tmp, 256, stream);
        str.append(tmp, n);
    } while (n == 256);

    return str;
}

void processRequests(FCGX_Request& request, DispatcherServiceImpl& service) {
    while (true) {
        if (FCGX_Accept_r(&request) >= 0) {
            std::string user;
            std::string token;
            std::string code;

            std::string str = to_string(request.in);
            service.AddTask(user, token, code);

        } else {
            std::cerr << ERROR "Something went wrong!" << std::endl;
            break;
        }
    }
}

int main(int argc, char** argv) {

    if (not parseArgs(argc, argv)) {
        return 1;
    }

    DispatcherServiceImpl service;

    if (FCGX_Init() != 0) {
        std::cerr << ERROR "Unable to initialize FCGX" << std::endl;
        return 1;
    }

    int sock_fd = FCGX_OpenSocket(args.inputSocket.c_str(), 1024);
    if (sock_fd < 0) {
        std::cerr << ERROR "unable to open UNIX domain socket: " << args.inputSocket << std::endl;
        return 1;
    }

    FCGX_Request request;
    FCGX_InitRequest(&request, sock_fd, 0);

    std::cout << "Successfully init request, start listening\n";

    std::thread producer(processRequests, std::ref(request), std::ref(service));
    RunServer(service, args.grpcSocket);

    producer.join();

    return 0;
}

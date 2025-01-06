#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/strings/str_format.h>

#include <fcgiapp.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <thread>

#include "dispatcher.grpc.pb.h"

#define ERROR "\033[1;31m[[ ERROR ]]: \033[0m"

struct Request {
    const std::string user;
    const std::string token;
    const std::string code;

    template<typename T, typename U, typename V>
    Request(T&& t, U&& u, V&& v) : user(u), token(u), code(v) {}
};

std::string to_string(FCGX_Stream *stream) {
    std::string str;
    int n;
    do {
        char tmp[256];
        n = FCGX_GetStr(tmp, 256, stream);
        str.append(tmp, n);
    } while (n == 256);

    return str;
}

void process_requests(FCGX_Request& request, std::queue<Request>& q, std::mutex& m)
{
    while (true) {
        if (FCGX_Accept_r(&request) >= 0) {
            std::string user;
            std::string token;
            std::string code;

            std::string str = to_string(request.in);
            std::lock_guard lg(m);
            q.emplace(std::move(user), std::move(token), std::move(code));
        } else {
            std::cerr << ERROR "Something went wrong!" << std::endl;
            break;
        }
    }
}

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using ::grpc::Status;

using ::codehaul::compilation::Task;
using ::codehaul::compilation::TaskRequest;
using ::codehaul::compilation::TaskResponce;
using ::codehaul::compilation::TaskResult;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");
// ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

class DispatcherServiceImpl final : public codehaul::compilation::Dispatcher::Service {

    Status GetTask(ServerContext*, const TaskRequest* request, Task* response) { return Status::OK; }
    Status SetResult(ServerContext*, const TaskResult* request, TaskResponce* response) { return Status::OK; }
};

void RunServer(uint16_t port) {
    std::string           server_address = absl::StrFormat("0.0.0.0:%d", port);
    DispatcherServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    std::mutex mut;
    std::queue<Request> queue;

    absl::ParseCommandLine(argc, argv);


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

    std::thread producer(process_requests, std::ref(request), std::ref(queue), std::ref(mut));

    RunServer(absl::GetFlag(FLAGS_port));

    producer.join();
    return 0;
}

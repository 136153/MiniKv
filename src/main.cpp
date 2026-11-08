
#include "kv_store.h"
#include "server.h"
#include <csignal>
#include <atomic>
using namespace std;

namespace {
    volatile std::sig_atomic_t stop_requested = 0;

    void handle_sigint(int) {
        stop_requested = 1;
    }
}
int main() {
    struct sigaction action{};
    action.sa_handler = handle_sigint;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }

    Server server(5);
    server.start(&stop_requested);
    return 0;
}

#include "kv_store.h"
#include "server.h"
using namespace std;



int main() {

    ThreadPool pool(4);

    KVStore store;
    Server server(4);
    server.start();

    return 0;

}
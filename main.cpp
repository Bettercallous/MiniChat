#include "Server.hpp"

int main(int ac, char **av)
{
    Server server;

    try {
        server.parseArgs(ac, av);
        server.init();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

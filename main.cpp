#include "Server.hpp"

int main(int ac, char **av)
{
    Server server;

    try {
        server.parseArgs(ac, av);
        server.init();
        server.run();
    } catch (std::exception& e) {
        server.closeFds();
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cout << ">>> SERVER CLOSED <<<" << std::endl;
    return 0;
}

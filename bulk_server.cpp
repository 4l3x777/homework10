#include <iostream>
#include "bulk_server.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: bulk_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        bulk_server::server server(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }

    return 0;
}
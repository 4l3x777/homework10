#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <cstdlib>
#include <list>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "async.h"

namespace bulk_server
{

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    if (length)
                    {
                        std::vector<std::string> commands;
                        boost::split(commands, std::string{data_, length - 1}, boost::is_any_of("\n "));

                        std::string message;
                        memset(data_, 0x0, max_length);
                        if (commands.size() >= 2)
                        {
                            if (commands[0] == "connect" && commands.size() == 2) 
                            {
                                auto context = connect(stoi(commands[1]));
                                message = "You context is: " + std::to_string(context);
                            }
                            else if (commands[0] == "disconnect" && commands.size() == 2)
                            {
                                auto status = disconnect(std::stoi(commands[1]));
                                if (message::OK == status)
                                {
                                    message = "Context " + commands[1] + " disconnected!";
                                }
                                else if (message::CONTEXT_NOT_FOUND == status)
                                {
                                    message = "Context " + commands[1] + " not found!";
                                }
                            }
                            else if (commands[0] == "receive" && commands.size() >= 2)
                            {
                                auto context = std::stoi(commands[1]);
                                for (auto i = 2; i < commands.size(); i++)
                                {
                                    auto status = receive(commands[i].data(), commands[i].size(), context);
                                    if (message::CONTEXT_NOT_FOUND == status)
                                    {
                                        message = "Context " + commands[1] + " not found!";
                                        break;
                                    }
                                }
                            }
                            else 
                            {
                                message = "Unknown command!";
                            }
                        }
                        else
                        {
                            message = "Unknown command!";
                        }
                        
                        if (!message.empty()) 
                        {
                            strncpy(data_, message.data(), message.size());
                            do_write(message.size());
                        }
                    
                    }
                }
            }
        );
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_, 
            boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t) {}
        );
    }

    tcp::socket socket_;
    enum { max_length = 2048 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket))->start();
                }
                do_accept();
            }
        );
    }
    tcp::acceptor acceptor_;
};

};
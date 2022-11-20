/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class Session : public std::enable_shared_from_this<Session>
{
public:

    Session(const std::string &host,
            const std::string &port,
            std::function<void(const std::string &)> statusHandler,
            boost::asio::io_context &context);

    void run();
    void send(const std::string &json);
    void close();

private:

    void onResolve(boost::beast::error_code ec,
                   boost::asio::ip::tcp::resolver::results_type results);
    void onConnect(boost::beast::error_code ec,
                   boost::asio::ip::tcp::resolver::results_type::endpoint_type);
    void onHandshake(boost::beast::error_code ec);
    void read();
    void onRead(boost::beast::error_code ec, std::size_t);
    void onWrite(boost::beast::error_code ec, std::size_t);
    void onClose(boost::beast::error_code ec);

    std::string mHost;
    std::string mPort;

    std::function<void(const std::string &)> mStatusHandler;

    boost::asio::ip::tcp::resolver mResolver;
    boost::beast::websocket::stream<boost::beast::tcp_stream> mSocket;
    boost::beast::flat_buffer mBuffer;
};

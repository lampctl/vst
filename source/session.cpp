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

#include <chrono>

#include "session.h"

const char *gApiPath = "/api/ws";

Session::Session(const std::string &host,
                 const std::string &port,
                 std::function<void(const std::string &)> errorHandler,
                 boost::asio::io_context &context)
    : mHost(host)
    , mPort(port)
    , mErrorHandler(errorHandler)
    , mResolver(context)
    , mSocket(context)
{}

void Session::run()
{
    mResolver.async_resolve(
        mHost,
        mPort,
        boost::beast::bind_front_handler(
            &Session::onResolve,
            shared_from_this()
        )
    );
}

void Session::send(const std::string &json)
{
    mSocket.async_write(
        boost::asio::buffer(json),
        boost::beast::bind_front_handler(
            &Session::onWrite,
            shared_from_this()
        )
    );
}

void Session::close()
{
    mSocket.async_close(
        boost::beast::websocket::close_code::normal,
        boost::beast::bind_front_handler(
            &Session::onClose,
            shared_from_this()
        )
    );
}

void Session::onResolve(boost::beast::error_code ec,
                        boost::asio::ip::tcp::resolver::results_type results)
{
    if (ec) {
        mErrorHandler(ec.message());
        return;
    }

    boost::beast::get_lowest_layer(mSocket).expires_after(std::chrono::seconds(30));
    boost::beast::get_lowest_layer(mSocket).async_connect(
        results,
        boost::beast::bind_front_handler(
            &Session::onConnect,
            shared_from_this()
        )
    );
}

void Session::onConnect(boost::beast::error_code ec,
                        boost::asio::ip::tcp::resolver::results_type::endpoint_type)
{
    if (ec) {
        mErrorHandler(ec.message());
        return;
    }

    boost::beast::get_lowest_layer(mSocket).expires_never();

    mSocket.async_handshake(
        mHost,
        gApiPath,
        boost::beast::bind_front_handler(
            &Session::onHandshake,
            shared_from_this()
        )
    );
}

void Session::onHandshake(boost::beast::error_code ec)
{
    if (ec) {
        mErrorHandler(ec.message());
        return;
    }

    read();
}

void Session::read()
{
    mSocket.async_read(
        mBuffer,
        boost::beast::bind_front_handler(
            &Session::onRead,
            shared_from_this()
        )
    );
}

void Session::onRead(boost::beast::error_code ec, std::size_t)
{
    if (ec) {
        mErrorHandler(ec.message());
        return;
    }

    read();
}

void Session::onWrite(boost::beast::error_code ec, std::size_t)
{
    if (ec) {
        mErrorHandler(ec.message());
    }
}

void Session::onClose(boost::beast::error_code ec)
{
    if (ec) {
        mErrorHandler(ec.message());
    }
}

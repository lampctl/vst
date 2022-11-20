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

#include <iostream>

#include <boost/bind/bind.hpp>

#include "socket.h"

Socket::Socket(std::function<void(const std::string&)> statusHandler)
    : mStatusHandler(statusHandler)
    , mThread(nullptr)
{}

Socket::~Socket()
{
    disconnect();
}

void Socket::connect(const std::string &host, const std::string &port)
{
    mSession = std::make_shared<Session>(host, port, mStatusHandler, mContext);

    mContext.restart();
    mContext.post(boost::bind(&Session::run, mSession));

    mThread = new boost::thread(
        boost::bind(&boost::asio::io_context::run, &mContext)
    );
}

void Socket::disconnect()
{
    if (mThread) {
        mContext.post(boost::bind(&Session::close, mSession));

        mThread->join();
        delete mThread;
        mThread = nullptr;

        mSession.reset();
    }
}

void Socket::send(const std::string &json)
{
    mContext.post(boost::bind(&Session::send, mSession, json));
}

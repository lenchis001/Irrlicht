#pragma once

#define SERVER_PORT 8081
#define BUFFER_SIZE 4096

#include "boost/thread/mutex.hpp"
#include <boost/asio.hpp>

#include <functional>
#include <string>

#include "boost/beast.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"

#include "TNCTypes/TNCEvent.h"
#include "Types/Event.h"
#include "Utils/Utils.h"

namespace TNC {
class TWebSocket : public boost::enable_shared_from_this<TWebSocket> {
public:
    bool connect();

    void sendData(std::wstring data, Types::Event::BoolEventCallback readHandler);
    void setOnReceiveHandler(Types::Event::WstringWithErrorCallback readHandler);

    TWebSocket(const std::wstring& address);
    ~TWebSocket();

private:
    void onRead(boost::system::error_code ec, std::size_t bytesTransferred);

    void _runContext();

    Types::Event::WstringWithErrorCallback _readHandler;
    boost::asio::io_context _ioc;
    boost::beast::basic_multi_buffer<std::allocator<wchar_t>> _buffer;
    boost::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> _ws;

    boost::thread _contextThread;

    std::wstring _address;
};
}
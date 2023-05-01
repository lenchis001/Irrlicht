#include "TWebSocket.h"
#include "boost/date_time.hpp"
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Utils/TNCUtils.h"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

TNC::TWebSocket::TWebSocket(const std::wstring& address)
{
    _address = address;
}

TNC::TWebSocket::~TWebSocket()
{
    // Close the WebSocket connection
    _ws->close(websocket::close_code::normal);

    _contextThread.join();
}

void TNC::TWebSocket::onRead(boost::system::error_code ec,
    std::size_t bytesTransferred)
{
    if (_readHandler) {

        auto data = ec.value() == NULL ? std::wstring(reinterpret_cast<const wchar_t*>((*(_buffer.data().begin())).data()), bytesTransferred / sizeof(wchar_t)) : std::wstring();
        _readHandler(ec.value() != NULL, data);
    }
}

void TNC::TWebSocket::_runContext()
{
    this->_contextThread = boost::thread([&]() { _ioc.run(); });
}

bool TNC::TWebSocket::connect()
{
    try {
        auto parsedAddress = TUtils::Uri::Parse(_address);

        // Check command line arguments.
        auto const host = TUtils::toString(parsedAddress.Host);
        auto const port = TUtils::toString(parsedAddress.Port);

        // These objects perform our I/O
        tcp::resolver resolver { _ioc };
        _ws = boost::shared_ptr<websocket::stream<tcp::socket>>(new websocket::stream<tcp::socket> { _ioc });

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(_ws->next_layer(), results.begin(), results.end());

        // Perform the websocket handshake
        _ws->handshake(host, TUtils::toString(parsedAddress.Path));

        _ws->async_read(_buffer,
            boost::bind(
                &TWebSocket::onRead,
                shared_from_this(),
                boost::placeholders::_1,
                boost::placeholders::_2));

        _runContext();

        return true;
    } catch (std::exception const&) {
        return false;
    }
}

void TNC::TWebSocket::sendData(std::wstring data, Types::Event::BoolEventCallback readHandler)
{
    // Send the message
    _ws->async_write(boost::asio::buffer(data),
        [=](boost::system::error_code ec, auto b) {
            readHandler(ec.value() == NULL);
        });
}

void TNC::TWebSocket::setOnReceiveHandler(Types::Event::WstringWithErrorCallback readHandler)
{
    _readHandler = readHandler;
}
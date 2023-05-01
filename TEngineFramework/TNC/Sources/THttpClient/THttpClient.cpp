#include "THttpClient.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include "boost/beast.hpp"
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>

#include "Utils/TNCUtils.h"
#include "Utils/Utils.h"

namespace TNC {
void THttpClient::get(const std::wstring& uri, Types::Event::WstringWithErrorCallback callback)
{
    try {
        auto parsedUri = TUtils::Uri::Parse(uri);

        auto const host = TUtils::toString(parsedUri.Host);
        auto const port = TUtils::toString(parsedUri.Port);
        auto const target = TUtils::toString(parsedUri.Path);
        int version = 11; // http 1.1

        // The io_context is required for all I/O
        boost::asio::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver { ioc };
        tcp::socket socket { ioc };

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(socket, results.begin(), results.end());

        // Set up an HTTP GET request message
        http::request<http::string_body> req { http::verb::get, target, version };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(socket, req);

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::string_body> res;

        // Receive the HTTP response
        http::read(socket, buffer, res);

        // Write the message to response stream
        std::stringstream responseStream;
        responseStream << res;

        // Gracefully close the socket
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if (ec && ec != boost::system::errc::not_connected)
            throw boost::system::system_error { ec };

        // If we get here then the connection is closed gracefully

        callback(res.result_int() != 200, TUtils::toWstring(res.body()));
    } catch (std::exception const& e) {
        callback(true, TUtils::toWstring(e.what()));
    }
}
}
#ifndef TWEB_SOCKET_CLIENT
#define TWEB_SOCKET_CLIENT

#include "IWebSocketClient.h"

#include "boost/smart_ptr.hpp"

#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

namespace TNC::WebSocket::Client {
    class TWebSocketClient : public IWebSocketClient {
    public:
        TWebSocketClient();
        
        boost::future<void> connect(const std::string& host, const std::map<std::string, std::string>& headers) override;
        void send(const std::string& data) override;
        void setDataHandler(boost::function<void(const std::string&)> handler) override;
        void disconnect() override;
    private:
        websocketpp::connection_hdl _handler;
        boost::shared_ptr<client> _client;
        boost::function<void(const std::string&)> _messageHandler;
        boost::shared_ptr<boost::thread> _socketThread;
        boost::promise<void> _connectPromise;
    };
}

#endif // TWEB_SOCKET_CLIENT
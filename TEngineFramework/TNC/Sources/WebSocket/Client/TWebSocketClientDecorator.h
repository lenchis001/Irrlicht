#ifndef TWEB_SOCKET_DECORATOR
#define TWEB_SOCKET_DECORATOR

#include "boost/smart_ptr.hpp"
#include "boost/config.hpp"

#include "IWebSocketClient.h"

namespace TNC::WebSocket::Client {
	class TWebSocketClientDecorator : public IWebSocketClient {
	public:
		TWebSocketClientDecorator();

		boost::future<void> connect(const std::string& host, const std::map<std::string, std::string>& headers) override;
		void send(const std::string& data) override;
		void setDataHandler(boost::function<void(const std::string&)> handler) override;
		void disconnect() override;
	private:
		boost::shared_ptr<IWebSocketClient> _webSocket;
	};
}

#endif // TWEB_SOCKET_DECORATOR
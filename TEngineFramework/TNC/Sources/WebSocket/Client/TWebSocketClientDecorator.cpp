#include "TWebSocketClientDecorator.h"

#include "TWebSocketClient.h"

using namespace TNC::WebSocket::Client;

TWebSocketClientDecorator::TWebSocketClientDecorator()
{
	_webSocket = boost::make_shared<TWebSocketClient>();
}

boost::future<void> TWebSocketClientDecorator::connect(const std::string& host, const std::map<std::string, std::string>& headers) {
	return _webSocket->connect(host, headers);
}

void TWebSocketClientDecorator::send(const std::string& data) {
	_webSocket->send(data);
}

void TWebSocketClientDecorator::setDataHandler(boost::function<void(const std::string&)> handler) {
	_webSocket->setDataHandler(handler);
}

void TWebSocketClientDecorator::disconnect() {
	_webSocket->disconnect();
}

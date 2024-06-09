#include "TWebSocketServerDecorator.h"

#include "TWebSocketServer.h"

using namespace TNC::WebSocket::Server;

TWebSocketServerDecorator::TWebSocketServerDecorator() {
	_webSocketServer = boost::make_shared<TWebSocketServer>();
}

void TWebSocketServerDecorator::start(int port) {
	_webSocketServer->start(port);
}

void TWebSocketServerDecorator::stop() {
	_webSocketServer->stop();
}

void TWebSocketServerDecorator::send(const std::string& data) {
	_webSocketServer->send(data);
}

void TWebSocketServerDecorator::setDataHandler(WebSocketDataHandler handler) {
	_webSocketServer->setDataHandler(handler);
}

void TWebSocketServerDecorator::setClientConnectedHandler(ClientConnectedHandler handler) {
	_webSocketServer->setClientConnectedHandler(handler);
}
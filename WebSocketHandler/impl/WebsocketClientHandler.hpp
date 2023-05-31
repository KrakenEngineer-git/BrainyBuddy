#ifndef WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP
#define WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP

#include "ThreadPool/ThreadPool.hpp"
#include "WebsocketClient.hpp"
#include <functional>
#include <memory>
#include <string>

namespace websocket_handler
{

class WebsocketClientHandler : public WebsocketClient
{
  public:
    WebsocketClientHandler();
    ~WebsocketClientHandler();

    void set_client_handlers();
    void set_message_handler(MessageHandler handler) override;
    void set_open_handler(ConnectionHandler handler) override;
    void set_close_handler(CloseHandler handler) override;
    void set_fail_handler(ConnectionHandler handler) override;
    void set_error_handler(ErrorHandler handler) override;
    void connect(const std::string &uri, const std::map<std::string, std::string> &headers);
    void send(const std::string &message);
    bool is_connected();

  private:
    std::mutex m_client_handler_mutex;
    std::unique_ptr<ThreadPool> run_thread_pool_;
};

} // namespace websocket_handler

#endif // WEBSOCKET_HANDLER_IMPL_WEBSOCKETCLIENTHANDLER_HPP

#ifndef WEBSOCKET_HANDLER_WEBSOCKETCLIENTBASE_HPP
#define WEBSOCKET_HANDLER_WEBSOCKETCLIENTBASE_HPP


namespace websocket_handler {


    class WebsocketClientBase {

    public:
        enum class InitializationStatus
        {
            NOT_INITIALIZED,
            INITIALIZATION_SUCCESS,
            INITIALIZATION_FAILED,
        };

        WebsocketClientBase() noexcept = default;
        virtual ~WebsocketClientBase() noexcept = default;

        WebsocketClientBase(WebsocketClientBase &&) = delete;
        WebsocketClientBase(const WebsocketClientBase &) = delete;
        WebsocketClientBase& operator=(WebsocketClientBase&&) & = delete;
        WebsocketClientBase& operator=(const WebsocketClientBase&) & = delete;

        virtual InitializationStatus init() = 0;
    };

}

#endif //WEBSOCKET_HANDLER_WEBSOCKETCLIENTBASE_HPP


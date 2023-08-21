#include <iostream>
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>

int main()
{
    struct PerSocketData {
        /* Fill with user data */
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App().ws<PerSocketData>("/*", {
            /* Settings */
            .idleTimeout = 16,
            /* Handlers */
            .open = [](auto*/*ws*/) {
                /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */

            },
            .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
                 * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
                 * benchmarking of large message sending without compression */
                ws->send(message, opCode, true);
            },
            .close = [](auto*/*ws*/, int /*code*/, std::string_view /*message*/) {
                /* You may access ws->getUserData() here */
            }
            }).listen(9001, [](auto* listen_socket) {
                if (listen_socket) {
                    std::cout << "Listening on port " << 9001 << std::endl;
                }
                }).run();
}

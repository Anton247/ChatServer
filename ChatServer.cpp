#include <iostream>
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

// Публичные сообщения 
// user13 => server: {"command": "public_msg", "text": "Всем приветы в этом чате"}
// server => all users {"command": "public_msg", "text": "...", user_from: 13}
// data, ws

void process_public_msg(auto data, auto* ws) {
    json payload = {
        {"command", data["command"]},
        {"text", data["text"]},
        {"user_from", ws->getUserData()->user_id}
    };
    ws->publish("public_channel", payload.dump());
}

// Приватные сообщения
// user10 => server {"command": "private_msg", "text": "...", user_to: 20}
// server => user20 {"command": "private_msg", "text": "...", user_from: 10}
void process_private_msg(auto data, auto* ws) {
    json payload = {
        {"command", data["command"]},
        {"text", data["text"]},
        {"user_from", ws->getUserData()->user_id},
    };
    int user_to = data["user_to"];
    ws->publish("user" + to_string(user_to), payload.dump());
}

// Возможность указать имя command:set_name
// Оповещение о подключениях command:status (список людей онлайн)

int main()
{
    struct UserData {
        int user_id;
        string name;
    };

    int latest_user_id = 10;


    uWS::App().ws<UserData>("/*", {
        .idleTimeout = 300,
        // Вызывается при новом подключении
        .open = [&latest_user_id](auto* ws) {
            UserData* data = ws->getUserData();
            data->user_id = latest_user_id++;
            data->name = "noname";

            cout << "New user connected: " << data->user_id << endl;
            ws->subscribe("public_channel"); // подписываем всех на канал(топик) паблик
            ws->subscribe("user" + to_string(data->user_id)); // подписываем на персональны топик
        },
        // При получении данных от клиента
        .message = [](auto* ws, std::string_view data, uWS::OpCode opCode) {
            json parsed_data = json::parse(data); // ToDo: check format / handle exception

            if (parsed_data["command"] == "public_msg") {
                process_public_msg(parsed_data, ws);
            }

            if (parsed_data["command"] == "private_msg") {
                process_private_msg(parsed_data, ws);
            }
        },
        .close = [](auto*/*ws*/, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here */
        }
        }).listen(9001, [](auto* listen_socket) {
            if (listen_socket) {
                std::cout << "Listening on port " << 9001 << std::endl;
            } // http://localhost:9001/
            }).run();
}
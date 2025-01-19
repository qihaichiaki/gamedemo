#include <httplib.h>
#include <json/json.h>

static bool checkPost(const httplib::Result& res)
{
    if (!res || res->status != 200) {
        std::cout << "连接不到服务器路由..." << std::endl;
        return false;
    }
    return true;
}

int main()
{
#if defined(_WIN32) || defined(_WIN64)
    // 设置控制台输出编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    // 设置控制台输入编码为 UTF-8
    SetConsoleCP(CP_UTF8);
#endif

    // 读取配置文件中的端口号
    uint16_t port;
    {
        Json::Value config;
        Json::CharReaderBuilder readerBuilder;
        std::ifstream ifs("resources/clientsettings.json");
        std::string err;
        if (!Json::parseFromStream(readerBuilder, ifs, &config, &err)) {
            std::cerr << "`resources/clientsettings.json`配置文件读取失败\n" << err;
            system("pause");
            return -1;
        }
        port = config["port"].asUInt();
    }

    httplib::Client cli("localhost", port);
    int player_id = -1;

    auto res_login = cli.Post("/login");
    if (checkPost(res_login)) {
        player_id = std::stoi(res_login->body);
        if (player_id == -1) {
            std::cout << "当前房间已满" << std::endl;
            system("pause");
            return -1;
        } else {
            std::cout << "登录成功，玩家ID：" << player_id << std::endl;
        }
    }

    auto res_text = cli.Post("/query_text");
    if (checkPost(res_text)) {
        std::cout << "查询到的打字文本：" << res_text->body << std::endl;
    }

    bool stop = false;
    std::cout << "game start ========================>\n";

    Json::StreamWriterBuilder writerBuilder;
    while (!stop) {
        int input;
        std::cout << "player-" << player_id << " 请输入你的更新进度：";
        std::cin >> input;

        Json::Value data;
        data["player_id"] = player_id;
        data["progress"] = input;

        std::stringstream ss;
        auto sw = writerBuilder.newStreamWriter();
        sw->write(data, &ss);

        auto res_update = cli.Post("/update", ss.str(), "text/plain");
        if (checkPost(res_update)) {
            std::cout << "更新成功, 对方进度为: " << std::stoi(res_update->body) << std::endl;
        } else {
            std::cout << "连接好像失败了...";
        }

        std::cout << "是否继续？(y/n)";
        char choice;
        std::cin >> choice;
        if (choice == 'n') {
            stop = true;
        } else if (choice != 'y') {
            std::cout << "输入错误默认继续" << std::endl;
        }
    }

    return 0;
}
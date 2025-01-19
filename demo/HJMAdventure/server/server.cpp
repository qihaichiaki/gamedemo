#include <httplib.h>
#include <json/json.h>

int main()
{
#if defined(_WIN32) || defined(_WIN64)
    // 设置控制台输出编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    // 设置控制台输入编码为 UTF-8
    SetConsoleCP(CP_UTF8);
#endif

    uint16_t port;
    std::string text;
    Json::CharReaderBuilder readerBuilder;
    {
        // 读取配置件
        Json::Value config;
        std::ifstream ifs("resources/settings.json");
        std::string err;
        if (!Json::parseFromStream(readerBuilder, ifs, &config, &err)) {
            std::cerr << "`resources/settings.json`配置文件读取失败\n" << err << std::endl;
            system("pause");
            return -1;
        }
        ifs.close();
        // 读取配置文件中的端口号和文本
        port = config["port"].asUInt();
        text = config["text"].asCString();
    }

    httplib::Server svr;

    // 游戏数据声明处
    int progress_1 = -1;
    int progress_2 = -1;

    // 此互斥锁保护游戏数据
    std::mutex mtx;

    // 处理Post请求

    // 登录路由
    svr.Post("/login", [&](const httplib::Request &req, httplib::Response &res) {
        // WARNING: 是否需要加锁？
        std::lock_guard<std::mutex> lock(mtx);
        if (progress_1 >= 0 && progress_2 >= 0)
            res.set_content("-1", "text/plain");
        else {
            if (progress_1 < 0) {
                progress_1 = 0;
                res.set_content("1", "text/plain");
            } else {
                progress_2 = 0;
                res.set_content("2", "text/plain");
            }
        }
    });

    // 传输文本路由
    svr.Post("/query_text", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_content(text, "text/plain");
    });

    std::string err;
    // 同步进度路由
    svr.Post("/update", [&](const httplib::Request &req, httplib::Response &res) {
        std::lock_guard<std::mutex> lock(mtx);
        Json::Value data;
        auto redner = readerBuilder.newCharReader();

        if (!redner->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &data, &err)) {
            res.set_content(err, "text/plain");
        } else {
            int player_id = data["player_id"].asInt();
            int return_progress = -1;
            if (player_id == 1) {
                progress_1 = data["progress"].asInt();
                return_progress = progress_2;
            } else if (player_id == 2) {
                progress_2 = data["progress"].asInt();
                return_progress = progress_1;
            }
            res.set_content(std::to_string(return_progress), "text/plain");
        }
    });

    // 监听端口, 任意ip
    svr.listen("0.0.0.0", port);
    return 0;
}
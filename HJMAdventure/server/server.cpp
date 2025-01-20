#include <httplib.h>
#include <macro.h>

#include <jsonUtils.hpp>

int main()
{
    WINDOWS_USE_UTF8;

    uint16_t port;
    std::string text;
    JsonUtils::JsonReader json_reader;
    {
        // 读取配置件
        if (!json_reader.readFile("resources/settings.json")) {
            std::cerr << "`resources/settings.json`配置文件读取失败\n" << std::endl;
            system("pause");
            return -1;
        }
        // 读取配置文件中的端口号和文本
        port = json_reader["port"].asUInt();
        std::ifstream ifs("resources/text.txt");
        if (!ifs.good()) {
            std::cerr << "`resources/text.txt`文字读取失败, 将使用默认文本\n" << std::endl;
            text = "hello, world";
        } else {
            std::stringstream ss;
            ss << ifs.rdbuf();
            text = ss.str();
        }
        ifs.close();
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
        if (progress_1 >= 0 && progress_2 >= 0) {
            res.set_content("-1", "text/plain");
            return;
        }

        std::string player_id = (progress_1 >= 0) ? "2" : "1";
        res.set_content(player_id, "text/plain");
        (progress_1 >= 0) ? (progress_2 = 0) : (progress_1 = 0);
        std::cout << "玩家" << player_id << "加入游戏\n";
    });

    // 传输文本路由
    svr.Post("/query_text", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_content(text, "text/plain");
        std::cout << "获取文本信息...\n";
    });

    std::string err;
    // 同步进度路由
    svr.Post("/update", [&](const httplib::Request &req, httplib::Response &res) {
        std::lock_guard<std::mutex> lock(mtx);
        json_reader.clear();

        if (!json_reader.read(req.body)) {
            res.set_content(err, "text/plain");
        } else {
            int player_id = json_reader["player_id"].asInt();
            int return_progress = -1;
            if (player_id == 1) {
                progress_1 = json_reader["progress"].asInt();
                return_progress = progress_2;
            } else if (player_id == 2) {
                progress_2 = json_reader["progress"].asInt();
                return_progress = progress_1;
            }
            res.set_content(std::to_string(return_progress), "text/plain");
            // std::cout << "玩家" << player_id << "同步信息\n";
            // std::cout << "当前进度: " << progress_1 << " vs " << progress_2 << std::endl;
        }
    });

    std::cout << "服务器配置成功......\n";

    // 监听端口, 任意ip
    svr.listen("0.0.0.0", port);
    return 0;
}
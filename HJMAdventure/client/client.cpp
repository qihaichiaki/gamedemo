#include <httplib.h>
#include <macro.h>

#include <jsonUtils.hpp>

#include "path.hpp"

// 游戏状态
enum class Stage {
    Waiting,  // 等待玩家加入
    Ready,    // 准备起跑倒计时
    Racing    // 正在比赛中
};

int val_countdown = 4;         // 起跑倒计时
Stage stage = Stage::Waiting;  // 当前游戏阶段

int id_player = 0;                 // 玩家序号
std::atomic<int> progress_1 = -1;  // 玩家1进度
std::atomic<int> progress_2 = -1;  // 玩家2进度
int num_total_char = 0;            // 文本总字符数

// 根据地图上的关键节点形成路径:
// clang-format off
Path path{
    {
        {842, 842}, {1322, 842}, {1322, 442},
        {2762, 442}, {2762, 842}, {3162, 842},
        {3162, 1722}, {2122, 1722}, {2122, 1562},
        {842, 1562}, {842, 842}
    }
};
// clang-format on

int idx_line = 0;                        // 当前文本行索引
int idx_char = 0;                        // 当前文本字符索引
std::string str_text;                    // 文本内容
std::vector<std::string> str_line_list;  // 行文本列表

int main()
{
    WINDOWS_USE_UTF8;

    // 读取配置文件中的端口号
    uint16_t port;
    {
        JsonUtils::JsonReader json_reader;
        if (!json_reader.readFile("resources/clientsettings.json")) {
            std::cerr << "`resources/clientsettings.json`配置文件读取失败\n";
            system("pause");
            return -1;
        }
        port = json_reader["port"].asUInt();
    }

    httplib::Client cli("localhost", port);

    return 0;
}
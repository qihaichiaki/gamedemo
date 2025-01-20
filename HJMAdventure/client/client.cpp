#include <httplib.h>
#include <macro.h>

#include <chrono>
#include <codecvt>
#include <jsonUtils.hpp>

#include "path.hpp"
#include "player.hpp"

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

Atlas atlas_1p_idle[FACING_NUM] = {};  // 玩家1四个方向的待机动画图集
Atlas atlas_1p_run[FACING_NUM] = {};   // 玩家1四个方向的跑步动画图集
Atlas atlas_2p_idle[FACING_NUM] = {};  // 玩家2四个方向的待机动画图集
Atlas atlas_2p_run[FACING_NUM] = {};   // 玩家2四个方向的跑步动画图集

IMAGE img_ui[3] = {};  // 界面文本
IMAGE img_ui_fight;    // 界面文本FIGHT
IMAGE img_ui_textbox;  // 界面文本框
IMAGE img_background;  // 背景图

std::string str_address;            // 服务器地址
uint16_t port;                      // 服务器端口
httplib::Client* client = nullptr;  // HTTP客户端对象

/// @brief 加载资源
/// @param hwnd 窗口句柄
static void loadResources(HWND hwnd)
{
    // 加载字体
    AddFontResourceEx(_T("resources/zpix.ttf"), FR_PRIVATE, NULL);

    // 加载图集
    // 1P
    atlas_1p_idle[Up].load(_T("resources/hajimi_idle_back_%d.png"), 4);
    atlas_1p_idle[Down].load(_T("resources/hajimi_idle_front_%d.png"), 4);
    atlas_1p_idle[Left].load(_T("resources/hajimi_idle_left_%d.png"), 4);
    atlas_1p_idle[Right].load(_T("resources/hajimi_idle_right_%d.png"), 4);
    atlas_1p_run[Up].load(_T("resources/hajimi_run_back_%d.png"), 4);
    atlas_1p_run[Down].load(_T("resources/hajimi_run_front_%d.png"), 4);
    atlas_1p_run[Left].load(_T("resources/hajimi_run_left_%d.png"), 4);
    atlas_1p_run[Right].load(_T("resources/hajimi_run_right_%d.png"), 4);
    // 2P
    atlas_2p_idle[Up].load(_T("resources/manbo_idle_back_%d.png"), 4);
    atlas_2p_idle[Down].load(_T("resources/manbo_idle_front_%d.png"), 4);
    atlas_2p_idle[Left].load(_T("resources/manbo_idle_left_%d.png"), 4);
    atlas_2p_idle[Right].load(_T("resources/manbo_idle_right_%d.png"), 4);
    atlas_2p_run[Up].load(_T("resources/manbo_run_back_%d.png"), 4);
    atlas_2p_run[Down].load(_T("resources/manbo_run_front_%d.png"), 4);
    atlas_2p_run[Left].load(_T("resources/manbo_run_left_%d.png"), 4);
    atlas_2p_run[Right].load(_T("resources/manbo_run_right_%d.png"), 4);

    // 其他图像资源
    loadimage(img_ui, _T("resources/ui_1.png"));
    loadimage(img_ui + 1, _T("resources/ui_2.png"));
    loadimage(img_ui + 2, _T("resources/ui_3.png"));
    loadimage(&img_ui_fight, _T("resources/ui_fight.png"));
    loadimage(&img_ui_textbox, _T("resources/ui_textbox.png"));
    loadimage(&img_background, _T("resources/background.png"));

    // 加载音频
    loadAudio(_T("resources/bgm.mp3"), _T("bgm"));
    loadAudio(_T("resources/1p_win.mp3"), _T("1p_win"));
    loadAudio(_T("resources/2p_win.mp3"), _T("2p_win"));
    loadAudio(_T("resources/click_1.mp3"), _T("click_1"));
    loadAudio(_T("resources/click_2.mp3"), _T("click_2"));
    loadAudio(_T("resources/click_3.mp3"), _T("click_3"));
    loadAudio(_T("resources/click_4.mp3"), _T("click_4"));
    loadAudio(_T("resources/ui_1.mp3"), _T("ui_1"));
    loadAudio(_T("resources/ui_2.mp3"), _T("ui_2"));
    loadAudio(_T("resources/ui_3.mp3"), _T("ui_3"));
    loadAudio(_T("resources/ui_fight.mp3"), _T("ui_fight"));

    // 读取配置文件, 获得服务器ip和端口
    JsonUtils::JsonReader json_reader;
    if (!json_reader.readFile("resources/clientsettings.json")) {
        MessageBox(hwnd, _T("无法打开配置 resources/clientsettings.json"), _T("启动失败"),
                   MB_OK | MB_ICONERROR);
        exit(-1);
    }
    str_address = json_reader["address"].asCString();
    port = json_reader["port"].asUInt();
}

/// @brief 连接服务器
static void loginToServer(HWND hwnd)
{
    client = new httplib::Client(str_address, port);
    client->set_keep_alive(true);  // 客户端始终保持连接活跃 避免频繁重连带来的性能损耗

    httplib::Result result = client->Post("/login");
    if (!result || result->status != 200) {
        MessageBox(hwnd, _T("无法连接到服务器!"), _T("启动失败"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    id_player = std::stoi(result->body);

    if (id_player <= 0) {
        MessageBox(hwnd, _T("比赛已经开始!"), _T("拒绝加入"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    (id_player == 1) ? (progress_1 = 0) : (progress_2 = 0);
    // TODO: 断线重连处理? ...
    str_text = client->Post("/query_text")->body;

    // 处理多行字符串
    std::stringstream ss(str_text);
    std::string str_line;
    while (std::getline(ss, str_line)) {
        str_line_list.push_back(str_line);
        num_total_char += static_cast<int>(str_line.length());
    }

    // 开辟一个分离线程，在服务器登录后每隔0.1s同步一次进度信息
    std::thread([&]() {
        JsonUtils::JsonWriter js_writer;
        std::cout << "开始同步进度信息...\n";
        while (true) {
            using namespace std::chrono;

            js_writer["player_id"] = id_player;
            js_writer["progress"] = (id_player == 1) ? progress_1.load() : progress_2.load();
            auto result = client->Post("/update", js_writer.str(), "text/plain");

            if (result && result->status == 200) {
                int progress = std::stoi(result->body);
                (id_player == 1) ? (progress_2 = progress) : (progress_1 = progress);
            } else {
                std::cout << "同步进度信息失败\n";
            }

            std::this_thread::sleep_for(nanoseconds(1000000000 / 10));
        }
    }).detach();
}

int main()
{
    WINDOWS_USE_UTF8;

    // ====== 处理数据初始化 ======
    using namespace std::chrono;

    HWND hwnd = initgraph(1280, 720 /*, EW_SHOWCONSOLE*/);
    SetWindowTextW(hwnd, L"哈吉米大冒险!");
    settextstyle(28, 0, _T("zpix"));
    setbkmode(TRANSPARENT);  // 绘制带有透明背景的文本和图形不会遮盖到下面的内容

    loadResources(hwnd);
    loginToServer(hwnd);

    // 初始化游戏中使用的所有对象
    ExMessage msg;
    Timer timer_countdown;
    Camera camera_ui, camera_scene;

    Player player1(atlas_1p_idle, atlas_1p_run);
    Player player2(atlas_2p_idle, atlas_2p_run);

    camera_ui.setSize({1280, 720});
    camera_scene.setSize({1280, 720});

    player1.setPosition({842, 842});
    player2.setPosition({842, 842});

    // 设置起跑倒计时定时器
    timer_countdown.setOneShot(false);
    timer_countdown.setWaitTime(1.0f);
    timer_countdown.setOnTimeout([&]() {
        val_countdown--;

        switch (val_countdown) {
            case 3:
                playAudio(_T("ui_3"));
                break;
            case 2:
                playAudio(_T("ui_2"));
                break;
            case 1:
                playAudio(_T("ui_1"));
                break;
            case 0:
                playAudio(_T("ui_fight"));
                break;
            case -1:
                stage = Stage::Racing;
                playAudio(_T("bgm"), true);
                break;
        }
    });

    const nanoseconds frame_duration(1000000000 / 144);
    steady_clock::time_point last_tick = steady_clock::now();
    // 设置随机数种子
    srand((unsigned int)time(0));

    // 批量绘图避免闪屏
    BeginBatchDraw();

    while (true) {
        // ====== 处理玩家输入 ======
        while (peekmessage(&msg)) {
            if (stage != Stage::Racing) continue;
        }

        if (str_line_list[idx_line][idx_char] == ' ') {
            idx_char++;
            if (idx_char >= str_line_list[idx_line].length()) {
                idx_char = 0;
                idx_line++;
            }
        }

        if (msg.message == WM_CHAR && idx_line < str_line_list.size()) {
            const std::string& str_line = str_line_list[idx_line];
            if (str_line[idx_char] == msg.ch) {
                switch (rand() % 4) {
                    case 0:
                        playAudio(_T("click_1"));
                        break;
                    case 1:
                        playAudio(_T("click_2"));
                        break;
                    case 2:
                        playAudio(_T("click_3"));
                        break;
                    case 3:
                        playAudio(_T("click_4"));
                        break;
                }

                (id_player == 1) ? progress_1++ : progress_2++;

                idx_char++;
                if (idx_char >= str_line.length()) {
                    idx_char = 0;
                    idx_line++;
                }
            }
        }

        // ====== 处理游戏更新 ======
        steady_clock::time_point frame_start = steady_clock::now();
        duration<float> delta = duration<float>(frame_start - last_tick);

        if (stage == Stage::Waiting) {
            if (progress_1 >= 0 && progress_2 >= 0) stage = Stage::Ready;
        } else {
            if (stage == Stage::Ready) timer_countdown.onUpdate(delta.count());

            if ((id_player == 1 && progress_1 >= num_total_char) ||
                (id_player == 2 && progress_2 >= num_total_char)) {
                stopAudio(_T("bgm"));
                playAudio((id_player == 1) ? _T("1p_win") : _T("2p_win"));
                MessageBox(hwnd, _T("赢麻麻!"), _T("游戏结束"), MB_OK | MB_ICONINFORMATION);
                exit(0);
            } else if ((id_player == 1 && progress_2 >= num_total_char) ||
                       (id_player == 2 && progress_1 >= num_total_char)) {
                stopAudio(_T("bgm"));
                MessageBox(hwnd, _T("输光光!"), _T("游戏结束"), MB_OK | MB_ICONINFORMATION);
                exit(0);
            }

            player1.setTarget(
                path.getPositionAtProgress(static_cast<float>(progress_1) / num_total_char));
            player2.setTarget(
                path.getPositionAtProgress(static_cast<float>(progress_2) / num_total_char));

            player1.onUpdate(delta.count());
            player2.onUpdate(delta.count());

            camera_scene.lookAt((id_player == 1) ? player1.getPosition() : player2.getPosition());
        }

        // ====== 处理画面绘制 ======
        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        if (stage == Stage::Waiting) {
            settextcolor(RGB(195, 195, 195));
            outtextxy(15, 675, _T("比赛即将开始, 等待其他玩家加入..."));
        } else {
            // 绘制背景图
            static const Rect rect_bg = {0, 0, img_background.getwidth(),
                                         img_background.getheight()};
            putimageEx(camera_scene, &img_background, &rect_bg);

            // 绘制玩家
            if (player1.getPosition().y > player2.getPosition().y) {
                player2.on_render(camera_scene);
                player1.on_render(camera_scene);
            } else {
                player1.on_render(camera_scene);
                player2.on_render(camera_scene);
            }

            // 绘制倒计时
            switch (val_countdown) {
                case 3: {
                    static const Rect rect_ui_3 = {1280 / 2 - img_ui[2].getwidth() / 2,
                                                   720 / 2 - img_ui[2].getheight() / 2,
                                                   img_ui[2].getwidth(), img_ui[2].getheight()};
                    putimageEx(camera_ui, img_ui + 2, &rect_ui_3);
                } break;
                case 2: {
                    static const Rect rect_ui_2 = {1280 / 2 - img_ui[1].getwidth() / 2,
                                                   720 / 2 - img_ui[1].getheight() / 2,
                                                   img_ui[1].getwidth(), img_ui[1].getheight()};
                    putimageEx(camera_ui, img_ui + 1, &rect_ui_2);
                } break;
                case 1: {
                    static const Rect rect_ui_1 = {1280 / 2 - img_ui[0].getwidth() / 2,
                                                   720 / 2 - img_ui[0].getheight() / 2,
                                                   img_ui[0].getwidth(), img_ui[0].getheight()};
                    putimageEx(camera_ui, img_ui, &rect_ui_1);
                } break;
                case 0: {
                    static const Rect rect_ui_fight = {1280 / 2 - img_ui_fight.getwidth() / 2,
                                                       720 / 2 - img_ui_fight.getheight() / 2,
                                                       img_ui_fight.getwidth(),
                                                       img_ui_fight.getheight()};
                    putimageEx(camera_ui, &img_ui_fight, &rect_ui_fight);
                } break;
                default:
                    break;
            }

            // 绘制文本框
            if (stage == Stage::Racing) {
                static const Rect rect_textbox = {0, 720 - img_ui_textbox.getheight(),
                                                  img_ui_textbox.getwidth(),
                                                  img_ui_textbox.getheight()};
                // convert 字符串编码转换
                static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
                std::wstring wstr_line = convert.from_bytes(str_line_list[idx_line]);
                std::wstring wstr_completed =
                    convert.from_bytes(str_line_list[idx_line].substr(0, idx_char));

                putimageEx(camera_ui, &img_ui_textbox, &rect_textbox);
                settextcolor(RGB(125, 125, 125));
                outtextxy(185 + 2, rect_textbox.y + 65 + 2, wstr_completed.c_str());
                settextcolor(RGB(25, 25, 25));
                outtextxy(185, rect_textbox.y + 65, wstr_line.c_str());
                settextcolor(RGB(0, 149, 217));
                outtextxy(185, rect_textbox.y + 65, wstr_completed.c_str());
            }
        }

        FlushBatchDraw();

        last_tick = frame_start;
        nanoseconds sleep_duration = frame_duration - (steady_clock::now() - frame_start);
        if (sleep_duration > nanoseconds(0)) std::this_thread::sleep_for(sleep_duration);
    }

    EndBatchDraw();
    closegraph();

    return 0;
}
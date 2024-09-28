#include "resourcesManager.h"
#include "util.h"
#include <string>

ResourcesManager* ResourcesManager::instance()
{
    static ResourcesManager manager;
    return &manager;
}

ResourcesManager::ResourcesManager() = default;
ResourcesManager::~ResourcesManager() = default;

struct ImageResInfo
{
    std::string id;
    LPCTSTR path;
};

struct AtlasResInfo
{
    std::string id;
    LPCTSTR path;
    int numFrame = 0;  // 图片数量
};

// 添加图片资源信息列表定义
static const std::vector<ImageResInfo> imageInfoList = 
{
    { "background",                     _T(R"(resources\background.png)") },
    { "ui_heart",                       _T(R"(resources\ui_heart.png)") },

    { "player_attack_right",            _T(R"(resources\player\attack.png)") },
    { "player_dead_right",              _T(R"(resources\player\dead.png)") },  
    { "player_fall_right",              _T(R"(resources\player\fall.png)") },  
    { "player_idle_right",              _T(R"(resources\player\idle.png)") },  
    { "player_jump_right",              _T(R"(resources\player\jump.png)") },  
    { "player_run_right",               _T(R"(resources\player\run.png)") },
    { "player_roll_right",              _T(R"(resources\player\roll.png)") },
    
    { "player_vfx_attack_down",         _T(R"(resources\player\vfx_attack_down.png)") },
    { "player_vfx_attack_left",         _T(R"(resources\player\vfx_attack_left.png)") },
    { "player_vfx_attack_right",        _T(R"(resources\player\vfx_attack_right.png)") },
    { "player_vfx_attack_up",           _T(R"(resources\player\vfx_attack_up.png)") },
    { "player_vfx_jump",                _T(R"(resources\player\vfx_jump.png)") },
    { "player_vfx_land",                _T(R"(resources\player\vfx_land.png)") },
};

// 添加图集资源信息列表定义
static const std::vector<AtlasResInfo> atlasInfoList = 
{
    { "barb_break",                     _T(R"(resources\enemy\barb_break\%d.png)"),          3 },
    { "barb_loose",                     _T(R"(resources\enemy\barb_loose\%d.png)"),          5 },
    { "silk",                           _T(R"(resources\enemy\silk\%d.png)"),                9 },
    { "sword_left",                     _T(R"(resources\enemy\sword\%d.png)"),               3 },

    { "enemy_aim_left",                 _T(R"(resources\enemy\aim\%d.png)"),                 9 },
    { "enemy_dash_in_air_left",         _T(R"(resources\enemy\dash_in_air\%d.png)"),         2 },
    { "enemy_dash_on_floor_left",       _T(R"(resources\enemy\dash_on_floor\%d.png)"),       2 },
    { "enemy_fall_left",                _T(R"(resources\enemy\fall\%d.png)"),                4 },
    { "enemy_idle_left",                _T(R"(resources\enemy\idle\%d.png)"),                6 },
    { "enemy_jump_left",                _T(R"(resources\enemy\jump\%d.png)"),                8 },
    { "enemy_run_left",                 _T(R"(resources\enemy\run\%d.png)"),                 8 },
    { "enemy_squat_left",               _T(R"(resources\enemy\squat\%d.png)"),              10 },
    { "enemy_throw_barb_left",          _T(R"(resources\enemy\throw_barb\%d.png)"),          8 },
    { "enemy_throw_silk_left",          _T(R"(resources\enemy\throw_silk\%d.png)"),         17 },
    { "enemy_throw_sword_left",         _T(R"(resources\enemy\throw_sword\%d.png)"),        16 },

    { "enemy_vfx_dash_in_air_left",     _T(R"(resources\enemy\vfx_dash_in_air\%d.png)"),    5 },
    { "enemy_vfx_dash_on_floor_left",   _T(R"(resources\enemy\vfx_dash_on_floor\%d.png)"),  6 },
};

// 检查图片对象是否加载成功
static inline bool checkImageValid(IMAGE* image)
{
    // TODO: 存在问题，即使没有加载成功也不会返回nullptr
    // return GetImageBuffer(image);  // 获取图片像素缓冲区（如果加载失败，这里获取的便就是空指针）
    // 通过图片尺寸存在0进行判断
    return image->getwidth() && image->getwidth();
}


// 封装ResourcesManager其余方法

// 加载磁盘中的全部对象
void ResourcesManager::load()
{
    // 读取当前资源列表：图片和图集
    for(const auto& info : imageInfoList)
    {
        IMAGE* image = new IMAGE();
        loadimage(image, info.path);
        if (!checkImageValid(image))
        {
            delete image;
            throw info.path;
        }
        imagePool[info.id] = image;
    }

    for(const auto& info : atlasInfoList)
    {
        Atlas* atlas = new Atlas();
        atlas->load(info.path, info.numFrame);
        for (int i = 0; i < atlas->getSize(); i++)
        {
            IMAGE* image = atlas->getImage(i);
            if (!checkImageValid(image))
            {
                delete atlas;
                throw info.path;
            }
        }
        atlasPool[info.id] = atlas;
    }

    // 处理下列id对应的水平翻转加入内存
    flipImage("player_attack_right", "player_attack_left", 5);
    flipImage("player_dead_right", "player_dead_left", 6);
    flipImage("player_fall_right", "player_fall_left", 5);
    flipImage("player_idle_right", "player_idle_left", 5);
    flipImage("player_jump_right", "player_jump_left", 5);
    flipImage("player_run_right", "player_run_left", 10);
    flipImage("player_roll_right", "player_roll_left", 7);

    flipAtlas("sword_left", "sword_right");
    flipAtlas("enemy_aim_left", "enemy_aim_right");
    flipAtlas("enemy_dash_in_air_left", "enemy_dash_in_air_right");
    flipAtlas("enemy_dash_on_floor_left", "enemy_dash_on_floor_right");
    flipAtlas("enemy_fall_left", "enemy_fall_right");
    flipAtlas("enemy_idle_left", "enemy_idle_right");
    flipAtlas("enemy_jump_left", "enemy_jump_right");
    flipAtlas("enemy_run_left", "enemy_run_right");
    flipAtlas("enemy_squat_left", "enemy_squat_right");
    flipAtlas("enemy_throw_barb_left", "enemy_throw_barb_right");
    flipAtlas("enemy_throw_silk_left", "enemy_throw_silk_right");
    flipAtlas("enemy_throw_sword_left", "enemy_throw_sword_right");

    flipAtlas("enemy_vfx_dash_in_air_left", "enemy_vfx_dash_in_air_right");
    flipAtlas("enemy_vfx_dash_on_floor_left", "enemy_vfx_dash_on_floor_right");

    // 加载音频
    loadAudio(_T(R"(resources\audio\bgm.mp3)"), _T("bgm"));
    loadAudio(_T(R"(resources\audio\barb_break.mp3)"), _T("barb_break"));
    loadAudio(_T(R"(resources\audio\bullet_time.mp3)"), _T("bullet_time"));

    loadAudio(_T(R"(resources\audio\enemy_dash.mp3)"), _T("enemy_dash"));
    loadAudio(_T(R"(resources\audio\enemy_run.mp3)"), _T("enemy_run"));
    loadAudio(_T(R"(resources\audio\enemy_hurt_1.mp3)"), _T("enemy_hurt_1"));
    loadAudio(_T(R"(resources\audio\enemy_hurt_2.mp3)"), _T("enemy_hurt_2"));
    loadAudio(_T(R"(resources\audio\enemy_hurt_3.mp3)"), _T("enemy_hurt_3"));
    loadAudio(_T(R"(resources\audio\enemy_throw_barbs.mp3)"), _T("enemy_throw_barbs"));
    loadAudio(_T(R"(resources\audio\enemy_throw_silk.mp3)"), _T("enemy_throw_silk"));
    loadAudio(_T(R"(resources\audio\enemy_throw_sword.mp3)"), _T("enemy_throw_sword"));

    loadAudio(_T(R"(resources\audio\player_attack_1.mp3)"), _T("player_attack_1"));
    loadAudio(_T(R"(resources\audio\player_attack_2.mp3)"), _T("player_attack_2"));
    loadAudio(_T(R"(resources\audio\player_attack_3.mp3)"), _T("player_attack_3"));
    loadAudio(_T(R"(resources\audio\player_dead.mp3)"), _T("player_dead"));
    loadAudio(_T(R"(resources\audio\player_hurt.mp3)"), _T("player_hurt"));
    loadAudio(_T(R"(resources\audio\player_jump.mp3)"), _T("player_jump"));
    loadAudio(_T(R"(resources\audio\player_land.mp3)"), _T("player_land"));
    loadAudio(_T(R"(resources\audio\player_roll.mp3)"), _T("player_roll"));
    loadAudio(_T(R"(resources\audio\player_run.mp3)"), _T("player_run"));
}

Atlas* ResourcesManager::findAtlas(const std::string& id) const
{
    const auto& itor = atlasPool.find(id);
    if (itor == atlasPool.end())
        return nullptr;
    
    return itor->second;
}

IMAGE* ResourcesManager::findImage(const std::string& id) const
{
    const auto& itor = imagePool.find(id);
    if (itor == imagePool.end())
        return nullptr;
    
    return itor->second;
}

// 图像翻转

void ResourcesManager::flipImage(IMAGE* srcImage, IMAGE* dstImage, int numH)
{
    int w = srcImage->getwidth();
    int h = srcImage->getheight();
    int wFrame = w / numH;
    Resize(dstImage, w, h);
    DWORD* srcBuffer = GetImageBuffer(srcImage);
    DWORD* dstBuffer = GetImageBuffer(dstImage);
    for (int i = 0; i < numH; i++)
    {
        int xLeft = i * wFrame;
        int xRight = (i + 1) * wFrame;
        for (int y = 0; y < h; y++)
        {
            for (int x = xLeft; x < xRight; x++)
            {
                int idxSrc = y * w + x;
                int idxDst = y * w + xRight - (x - xLeft);
                dstBuffer[idxDst] = srcBuffer[idxSrc];
            }
        }
    }
}

void ResourcesManager::flipImage(const std::string& srcId, const std::string dstId, int numH)
{
    IMAGE* srcImg = imagePool[srcId];
    IMAGE* dstImg = new IMAGE();

    flipImage(srcImg, dstImg, numH);

    imagePool[dstId] = dstImg;
}

void ResourcesManager::flipAtlas(const std::string& srcId, const std::string dstId)
{
    Atlas* srcAtlas = atlasPool[srcId];
    Atlas* dstAtlas = new Atlas();

    for (int i = 0; i < srcAtlas->getSize(); i++)
    {
        IMAGE imgFlipped;
        flipImage(srcAtlas->getImage(i), &imgFlipped);
        dstAtlas->addImage(imgFlipped);
    }

    atlasPool[dstId] = dstAtlas;
}
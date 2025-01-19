#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include <graphics.h>

#include <string>
#include <unordered_map>

#include "atlas.hpp"
#include "util.hpp"

// 资源管理器
class ResourcesManager
{
public:
    static ResourcesManager* instance();

    /// @brief 加载全部资源
    void load();

    /// @brief 加载前置 - 字体、音频
    /// @note 当前的音频、字体多线程加载存在问题,所以分离加载
    void loadPre();

    /// @brief 异步加载资源
    void loadAsync();

    Atlas* findAtlas(const std::string& id) const;
    IMAGE* findImage(const std::string& id) const;

    const StrData& findSetData(const std::string& id);

private:
    std::unordered_map<std::string, Atlas*> atlasPool;
    std::unordered_map<std::string, IMAGE*> imagePool;

    DataTransformation userSettings;  // 用户设置信息
private:
    ResourcesManager();
    ~ResourcesManager();
    ResourcesManager(const ResourcesManager&) = delete;

    /// @brief 这是一个水平翻转图片的操作
    /// @param srcImage 源Image对象
    /// @param dstImage 翻转处理后的Image对象
    /// @param numH 这一张动画图片素材包含多少个子序列帧
    void flipImage(IMAGE* srcImage, IMAGE* dstImage, int numH = 1);

    /// @brief 这是一个水平翻转图片的操作
    /// @param srcId 源Image对象池中的ID
    /// @param dstId 翻转处理后的Image对象在对象池中的ID
    /// @param numH 这一张动画图片素材包含多少个子序列帧
    void flipImage(const std::string& srcId, const std::string dstId, int numH = 1);

    /// @brief 这是一个水平翻转图集的操作
    /// @param srcId 源Atlas对象池中的ID
    /// @param dstId 翻转处理后的Atlas对象在对象池中的ID
    void flipAtlas(const std::string& srcId, const std::string dstId);
};

#endif
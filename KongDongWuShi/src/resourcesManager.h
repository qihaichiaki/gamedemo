#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include "atlas.h"

#include <string>
#include <graphics.h>
#include <unordered_map>

// 资源管理器
class ResourcesManager
{
public:
    static ResourcesManager* instance();

    void load();

    Atlas* findAtlas(const std::string& id) const;
    IMAGE* findImage(const std::string& id) const;

private:
    std::unordered_map<std::string, Atlas*> atlasPool;
    std::unordered_map<std::string, IMAGE*> imagePool;

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
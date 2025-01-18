#ifndef _ATLAS_H_
#define _ATLAS_H_

// 图集，管理一套存在动画关系的图片素材

#include <vector>
#include <graphics.h>
#include <string>

class Atlas
{
public:
    Atlas() = default;
    ~Atlas() = default;

    /// @brief 根据图集文件夹目录按照其中序号依次加载图集
    /// @param pathTemplate ...图集文件夹路径/%d.png
    /// @param num 一共存在多少个(注意序号应该从1开始，并且连续)
    void load(LPCTSTR pathTemplate, int num)
    {
        imgList.clear();
        imgList.resize(num);

        TCHAR pathFile[256];
        for (int i = 0; i < num; ++i)
        {
            _stprintf_s(pathFile, pathTemplate, i + 1);
            loadimage(&imgList[i], pathFile);
        }
    }

    /// @brief 清楚图集中存储的全部图片数据
    void clear()
    {
        imgList.clear();
    }

    /// @brief 查询当前图集的图片数量
    /// @return 图片数量
    int getSize() const
    {
        return static_cast<int>(imgList.size());
    }

    /// @brief 根据图片在图集中的索引获取单个图片数据
    /// @param idx 图片在图集中的索引
    /// @return 图片对象
    IMAGE* getImage(int idx)
    {
        if (idx < 0 || idx >= imgList.size())
            return nullptr;
        return &imgList[idx];
    }

    /// @brief 尾插图片对象到图集中去
    /// @param img 图片对象
    void addImage(const IMAGE& img)
    {
        imgList.push_back(img);
    }
private:
    std::vector<IMAGE> imgList;     // 图集图片资源列表
};



#endif
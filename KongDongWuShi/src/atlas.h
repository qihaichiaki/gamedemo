#ifndef _ATLAS_H_
#define _ATLAS_H_

// 图集，管理一套存在动画关系的图片素材

#include <vector>
#include <graphics.h>

class Atlas
{
public:
    Atlas() = default;
    ~Atlas() = default;

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

    void clear()
    {
        imgList.clear();
    }

    int getSize() const
    {
        return static_cast<int>(imgList.size());
    }

    IMAGE* getImage(int idx)
    {
        if (idx < 0 || idx >= imgList.size())
            return nullptr;
        return &imgList[idx];
    }

    void addImage(const IMAGE& img)
    {
        imgList.push_back(img);
    }
private:
    std::vector<IMAGE> imgList;
};



#endif
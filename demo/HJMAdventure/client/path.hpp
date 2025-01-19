#ifndef _PATH_H_
#define _PATH_H_

#include <vector2.hpp>
#include <vector>

class Path
{
public:
    Path(const std::vector<Vector2>& point_list) : total_length(0), point_list(point_list)
    {
        for (int i = 1; i < point_list.size(); ++i) {
            float segment_len = (point_list[i] - point_list[i - 1]).length();
            segment_len_list.push_back(segment_len);
            total_length += segment_len;
        }
    }

    ~Path() = default;

    Vector2 getPositionAtProgress(float progress) const
    {
        if (progress <= 0) return point_list.front();
        if (progress >= 1) return point_list.back();

        float target_len = total_length * progress;
        float current_len = 0.0f;
        for (int i = 1; i < segment_len_list.size(); ++i) {
            current_len += segment_len_list[i - 1];
            if (current_len >= target_len) {
                // 计算目标距离在当前段的进度 |____|____|____|__*__|
                float segment_progress = (target_len - (current_len - segment_len_list[i - 1])) /
                                         segment_len_list[i - 1];

                return point_list[i - 1] + (point_list[i] - point_list[i - 1]) * segment_progress;
            }
        }

        return point_list.back();
    }

    float getProgress(const Vector2& point)
    {
        float current_len = 0;
        for (int i = 0; i < segment_len_list.size(); ++i) {
            current_len += segment_len_list[i];
        }

        return 1;
    }

private:
    int total_length;
    std::vector<Vector2> point_list;
    std::vector<float> segment_len_list;
};

#endif
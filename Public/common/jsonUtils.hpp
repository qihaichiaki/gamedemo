#ifndef _JSON_UTILS_H_
#define _JSON_UTILS_H_

#include <json/json.h>

namespace JsonUtils {

class JsonWriter
{
public:
    JsonWriter() = default;
    ~JsonWriter() = default;

    Json::Value& operator[](const char* key) { return _root[key]; }

    /// @brief 将当前的json对象转换为字符串
    std::string str() const
    {
        auto sw = _writer_builder.newStreamWriter();
        std::stringstream ss;
        sw->write(_root, &ss);
        return ss.str();
    }

    /// @brief 根据传入的json_path路径写入json文件
    bool wirteFile(const std::string& json_path) const
    {
        try {
            std::ofstream ofs(json_path);
            bool is_write = write(ofs);
            ofs.close();
            if (!is_write) return false;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return false;
        }

        return true;
    }

    /// @brief 根据传入的输出流写入json文件
    bool write(std::ostream& os) const
    {
        try {
            auto sw = _writer_builder.newStreamWriter();
            sw->write(_root, &os);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return false;
        }

        return true;
    }

    /// @brief 清空json对象
    void clear() { _root = Json::Value(); }

private:
    Json::Value _root;
    Json::StreamWriterBuilder _writer_builder;
};

inline std::ostream& operator<<(std::ostream& os, const JsonWriter& writer)
{
    writer.write(os);
    return os;
}

class JsonReader
{
public:
    JsonReader() = default;
    ~JsonReader() = default;

    Json::Value& operator[](const char* key) { return _root[key]; }

    /// @brief 从文件中读取json数据
    bool readFile(const std::string& json_path)
    {
        try {
            std::ifstream ifs(json_path);
            bool is_read = read(ifs);
            ifs.close();
            if (!is_read) return false;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return false;
        }

        return true;
    }

    /// @brief 从输入流中读取json数据
    bool read(std::istream& is)
    {
        try {
            std::string errs;
            if (!Json::parseFromStream(_reader_builder, is, &_root, &errs)) {
                std::cerr << errs << '\n';
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return false;
        }

        return true;
    }

    bool read(const std::string& json_str)
    {
        try {
            auto cs = _reader_builder.newCharReader();
            std::string errs;
            if (!cs->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &_root, &errs)) {
                std::cerr << errs << '\n';
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return false;
        }

        return true;
    }

    /// @brief 清空json对象
    void clear() { _root = Json::Value(); }

private:
    Json::Value _root;
    Json::CharReaderBuilder _reader_builder;
};

}  // namespace JsonUtils

#endif  // !_JSON_UTILS_H_
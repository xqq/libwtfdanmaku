#include "BilibiliParser.hpp"
#include <Windows.h>
#include <fstream>
#include "../3rdparty/rapidxml/rapidxml.hpp"
#include "../3rdparty/rapidxml/rapidxml_utils.hpp"
#include "DanmakuFactory.hpp"
#include "StringUtils.hpp"

using namespace rapidxml;

namespace WTFDanmaku {

    BilibiliParser::BilibiliParser() {
        mDanmakus = std::move(std::unique_ptr<std::vector<DanmakuRef>>(new std::vector<DanmakuRef>));
    }

    bool BilibiliParser::ParseStringSource(const char* str) {
        return ParseXml(str);
    }

    bool BilibiliParser::ParseFileSource(const char* filePath) {
        file<> xmlFile(filePath);
        return ParseXml(xmlFile.data(), true);
    }

    bool BilibiliParser::ParseFileSource(const wchar_t* filePath) {
        std::ifstream file;
        file.open(filePath, std::ios::in || std::ios::binary || std::ios::ate);
        if (file.fail())
            return false;

        file.seekg(0, std::ios::end);
        size_t length = static_cast<size_t>(file.tellg());

        std::string buffer;
        buffer.reserve(length + 1);
        buffer.resize(length);

        file.seekg(0, std::ios::beg);
        file.read(&buffer[0], length);
        file.close();

        return ParseXml(buffer.c_str(), true);
    }

    bool BilibiliParser::ParseXml(const char* data, bool inplace) {
        if (mDanmakus.get() == nullptr)
            return false;

        char* buffer = nullptr;
        std::unique_ptr<char[]> raii_buffer;

        if (inplace) {
            buffer = const_cast<char*>(data);
        } else {
            size_t buffer_size = strlen(data) + 1;
            buffer = new char[buffer_size];
            raii_buffer = std::move(std::unique_ptr<char[]>(buffer));
            memset(buffer, 0, buffer_size);
            strcpy_s(buffer, buffer_size, data);
        }

        xml_document<> doc;
        doc.parse<0>(buffer);

        xml_node<char>* root = doc.first_node("i");
        if (root == nullptr)
            return false;

        for (auto node = root->first_node("d"); node != nullptr; node = node->next_sibling()) {
            const char* attr = node->first_attribute("p")->value();
            std::vector<std::string> attributes;
            attributes.reserve(8);
            SplitString(attr, ',', attributes);

            if (attributes.size() >= 8) {
                time_t time = static_cast<time_t>(std::stod(attributes[0]) * 1000);
                DanmakuType type = static_cast<DanmakuType>(std::stoi(attributes[1]));
                int textSize = static_cast<int>(std::stof(attributes[2]));
                int textColor = std::stoi(attributes[3]) | 0xFF000000;
                time_t timestamp = std::stoull(attributes[4]);
                int danmakuId = std::stoi(attributes[7]);
                std::wstring comment = UTF8ToWideString(node->value());

                DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(type, time, comment, textSize, textColor, timestamp, danmakuId);
                if (danmaku != nullptr) {
                    mDanmakus->push_back(danmaku);
                }
            }
        }
        return true;
    }

    std::unique_ptr<std::vector<DanmakuRef>> BilibiliParser::GetDanmakus() {
        return std::move(mDanmakus);
    }

}
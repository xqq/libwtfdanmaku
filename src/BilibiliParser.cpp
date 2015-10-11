#include "BilibiliParser.hpp"
#include <Windows.h>
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
        return ParseXml(xmlFile.data());
    }

    bool BilibiliParser::ParseXml(const char* data) {
        if (mDanmakus.get() == nullptr)
            return false;

        size_t bufferSize = strlen(data) + 1;
        char* buf = new char[bufferSize];
        std::unique_ptr<char[]> buffer(buf);
        memset(buf, 0, bufferSize);
        strcpy_s(buf, bufferSize, data);

        xml_document<> doc;
        doc.parse<0>(buf);

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
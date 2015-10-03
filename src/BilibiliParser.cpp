#include <Windows.h>
#include "../3rdparty/rapidxml/rapidxml.hpp"
#include "../3rdparty/rapidxml/rapidxml_utils.hpp"
#include "../3rdparty/rapidjson/include/rapidjson/document.h"
#include "DanmakuFactory.hpp"
#include "PositionDanmaku.hpp"
#include "BilibiliParser.hpp"

using namespace rapidxml;
using namespace rapidjson;

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
                std::wstring comment;
                if (type != DanmakuType::kPosition) {
                    comment = UTF8ToWideString(node->value());
                }

                DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(type, time, comment, textSize, textColor, timestamp, danmakuId);
                if (danmaku != nullptr) {
                    if (type == DanmakuType::kPosition) {
                        bool succ = ParsePositionDanmaku(danmaku, node->value());
                        if (!succ) continue;
                    }
                    mDanmakus->push_back(danmaku);
                }
            }
        }
        return true;
    }

    bool BilibiliParser::ParsePositionDanmaku(DanmakuRef dstDanmaku, const char* params) {
        if (dstDanmaku->GetType() != DanmakuType::kPosition || *params != '[')
            return false;

        PositionDanmaku* danmaku = static_cast<PositionDanmaku*>(dstDanmaku.Get());

        Document d;
        d.Parse(params);
        if (!d.IsArray())
            return false;
        if (d.Size() < 5)
            return false;

        danmaku->mSrcPoint = Point<float>(strtof(d[0].GetString(), nullptr), strtof(d[1].GetString(), nullptr));
        std::vector<std::string> alphas;
        alphas.reserve(2);
        SplitString(d[2].GetString(), '-', alphas);
        danmaku->mSrcAlpha = std::stof(alphas[0]);
        danmaku->mDestAlpha = std::stof(alphas[1]);
        danmaku->mDuration = static_cast<time_t>(strtof(d[3].GetString(), nullptr) * 1000);
        danmaku->mComment = UTF8ToWideString(d[4].GetString());
        DanmakuFactory::ReplaceStringInplace(danmaku->mComment, L"/n", L"\r\n");

        if (d.Size() > 5) {  // maybe 7 params
            danmaku->mRotateZ = std::atoi(d[5].GetString());
            danmaku->mRotateY = std::atoi(d[6].GetString());
        }

        if (d.Size() > 7) {  // maybe 9 params
            danmaku->mHasMovement = true;
            danmaku->mDestPoint = Point<float>(strtof(d[7].GetString(), nullptr), strtof(d[8].GetString(), nullptr));
        }

        if (d.Size() > 9) {  // maybe 11 params
            danmaku->mDelayAfterStop = strtoll(d[9].GetString(), nullptr, 10);
            danmaku->mOffsetTime = strtoll(d[10].GetString(), nullptr, 10);
        }

        if (d.Size() > 11) {  // maybe 13 params
            danmaku->mHasCustomFont = strcmp(d[11].GetString(), "true") == 0;
            if (danmaku->mHasCustomFont) {
                danmaku->mCustomFontName = UTF8ToWideString(d[12].GetString());
            }
        }

        danmaku->Calculate();
        return true;
    }

    std::unique_ptr<std::vector<DanmakuRef>> BilibiliParser::GetDanmakus() {
        return std::move(mDanmakus);
    }

    void BilibiliParser::SplitString(const char* input, char delimiter, std::vector<std::string>& output) {
        while (auto next = strchr(input, delimiter)) {
            output.push_back(std::string(input, next));
            input = next + 1;
        }
        output.push_back(std::string(input));
    }

    std::wstring BilibiliParser::UTF8ToWideString(const char* input) {
        int length = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
        std::wstring result;
        if (length > 0) {
            result.resize(length);
            MultiByteToWideChar(CP_UTF8, 0, input, -1, const_cast<LPWSTR>(result.c_str()), length);
        }
        return result;
    }

}
#include "BilibiliParser.hpp"
#include <Windows.h>
#include <fstream>
#include "../3rdparty/rapidxml/rapidxml.hpp"
#include "../3rdparty/rapidxml/rapidxml_utils.hpp"
#include "../3rdparty/rapidjson/include/rapidjson/document.h"
#include "DanmakuFactory.hpp"
#include "PositionDanmaku.hpp"
#include "BilibiliParser.hpp"
#include "StringUtils.hpp"

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
                std::wstring comment;
                if (type != DanmakuType::kPosition) {
                    comment = UTF8ToWideString(node->value());
                }

                DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(type, time, comment, textSize, textColor, timestamp, danmakuId);
                if (danmaku != nullptr) {
                    if (type == DanmakuType::kPosition) {
                        std::string params;
                        params.resize(node->value_size() + 1);
                        memcpy(&params[0], node->value(), node->value_size());
                        bool succ = ParsePositionDanmaku(danmaku, params.c_str());
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

        if (d[0].GetType() == Type::kNumberType) {
            danmaku->mSrcPoint = Point<float>((float)(d[0].GetDouble()), (float)(d[1].GetDouble()));
        } else if (d[0].GetType() == Type::kStringType) {
            danmaku->mSrcPoint = Point<float>(strtof(d[0].GetString(), nullptr), strtof(d[1].GetString(), nullptr));
        }

        std::vector<std::string> alphas;
        alphas.reserve(2);
        SplitString(d[2].GetString(), '-', alphas);
        danmaku->mSrcAlpha = std::stof(alphas[0]);
        danmaku->mDestAlpha = std::stof(alphas[1]);

        if (d[3].GetType() == Type::kNumberType) {
            danmaku->mDuration = static_cast<time_t>(d[3].GetDouble() * 1000);
        } else if (d[3].GetType() == Type::kStringType) {
            danmaku->mDuration = static_cast<time_t>(strtof(d[3].GetString(), nullptr) * 1000);
        }

        danmaku->mComment = UTF8ToWideString(d[4].GetString());
        ReplaceStringInplace(danmaku->mComment, L"/n", L"\r\n");

        if (d.Size() > 5) {  // maybe 7 params
            if (d[5].GetType() == Type::kNumberType) {
                danmaku->mRotateZ = d[5].GetInt();
                danmaku->mRotateY = d[6].GetInt();
            } else if (d[5].GetType() == Type::kStringType) {
                danmaku->mRotateZ = std::atoi(d[5].GetString());
                danmaku->mRotateY = std::atoi(d[6].GetString());
            }
        }

        if (d.Size() > 7) {  // maybe 9 params
            danmaku->mHasMovement = true;
            if (d[7].GetType() == Type::kNumberType) {
                danmaku->mDestPoint = Point<float>((float)(d[7].GetDouble()), (float)(d[8].GetDouble()));
            } else if (d[7].GetType() == Type::kStringType) {
                danmaku->mDestPoint = Point<float>(strtof(d[7].GetString(), nullptr), strtof(d[8].GetString(), nullptr));
            }
        }

        if (d.Size() > 9) {  // maybe 11 params
            if (d[9].GetType() == Type::kNumberType) {
                danmaku->mDelayAfterStop = d[9].GetInt64();
                danmaku->mOffsetTime = d[10].GetInt64();
            } else if (d[9].GetType() == Type::kStringType) {
                danmaku->mDelayAfterStop = strtoll(d[9].GetString(), nullptr, 10);
                danmaku->mOffsetTime = strtoll(d[10].GetString(), nullptr, 10);
            }
        }

        if (d.Size() > 11) {  // maybe 13 params
            if (d[11].GetType() == Type::kStringType) {
                danmaku->mHasCustomFont = strcmp(d[11].GetString(), "true") == 0;
            } else if (d[11].GetType() == Type::kTrueType) {
                danmaku->mHasCustomFont = true;
            } else if (d[11].GetType() == Type::kFalseType) {
                danmaku->mHasCustomFont = false;
            }

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

}
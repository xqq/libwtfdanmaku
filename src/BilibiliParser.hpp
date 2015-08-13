#ifndef _WTF_BILIBILI_PARSER_HPP
#define _WTF_BILIBILI_PARSER_HPP

#include "IParser.hpp"

namespace WTFDanmaku {

    class BilibiliParser : public IParser {
    public:
        static inline ParserRef Create() {
            return std::make_shared<BilibiliParser>();
        }
    public:
        explicit BilibiliParser();
        virtual ~BilibiliParser() = default;
        virtual bool ParseStringSource(const char* str) override;
        virtual bool ParseFileSource(const char* filePath) override;
        virtual std::unique_ptr<std::vector<DanmakuRef>> GetDanmakus() override;
    private:
        bool ParseXml(const char* data);
        static std::vector<std::string> BilibiliParser::SplitString(const char* input, char delimiter);
        static std::wstring UTF8ToWideString(const char* input);
    private:
        std::unique_ptr<std::vector<DanmakuRef>> mDanmakus;
    };

}

#endif // _WTF_BILIBILI_PARSER_HPP
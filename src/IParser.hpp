#ifndef _WTF_IPARSER_HPP
#define _WTF_IPARSER_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"

namespace WTFDanmaku {

    class IParser : public Noncopyable {
    public:
        explicit IParser() = default;
        virtual ~IParser() = default;
        virtual bool ParseStringSource(const char* str) = 0;
        virtual bool ParseFileSource(const char* filePath) = 0;
        virtual std::unique_ptr<std::vector<DanmakuRef>> GetDanmakus() = 0;
    };

    typedef std::shared_ptr<IParser> ParserRef;

}

#endif // _WTF_IPARSER_HPP
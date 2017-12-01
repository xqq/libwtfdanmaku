#include "StringUtils.hpp"
#include <Windows.h>

namespace WTFDanmaku {

    std::wstring UTF8ToWideString(const char* input) {
        int length = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
        std::wstring result;
        if (length > 0) {
            result.resize(length);
            MultiByteToWideChar(CP_UTF8, 0, input, -1, const_cast<LPWSTR>(result.c_str()), length);
        }
        return result;
    }

    void SplitString(const char* input, char delimiter, std::vector<std::string>& output) {
        while (auto next = strchr(input, delimiter)) {
            output.push_back(std::string(input, next));
            input = next + 1;
        }
        output.push_back(std::string(input));
    }

    void ReplaceStringInplace(std::wstring& str, const wchar_t* search, const wchar_t* replace) {
        size_t search_length = wcslen(search);
        size_t replace_length = wcslen(replace);

        size_t pos = 0;
        while ((pos = str.find(search, pos)) != std::wstring::npos) {
            str.replace(pos, search_length, replace);
            pos += replace_length;
        }
    }

}
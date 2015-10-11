#ifndef _WTF_STRING_UTILS_HPP
#define _WTF_STRING_UTILS_HPP

#include <string>
#include <vector>

namespace WTFDanmaku {

    std::wstring UTF8ToWideString(const char* input);
    void SplitString(const char* input, char delimiter, std::vector<std::string>& output);
    void ReplaceStringInplace(std::wstring& str, const wchar_t* search, const wchar_t* replace);

}

#endif // _WTF_STRING_UTILS_HPP
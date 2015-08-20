#include <cstdio>
#include <cstdlib>
#include "WTFDanmaku.h"
#include "WTFEngine.hpp"

using namespace WTFDanmaku;

int main(int argc, char** argv) {
    WTFInstance* instance = reinterpret_cast<WTFInstance*>(WTF_CreateInstance());
    WTF_ReleaseInstance(instance);
    WTFEngine engine;
    printf("Hello WTF");
    return 0;
}
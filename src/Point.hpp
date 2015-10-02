#ifndef _WTF_POINT_HPP
#define _WTF_POINT_HPP

namespace WTFDanmaku {

    template <typename T = int>
    struct Point {
    public:
        Point() = default;

        Point(T _x, T _y) : x(_x), y(_y) {}
    public:
        T x = 0;
        T y = 0;
    };

}

#endif // _WTF_POINT_HPP
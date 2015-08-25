#ifndef _WTF_RECT_HPP
#define _WTF_RECT_HPP

namespace WTFDanmaku {

    template <typename T = int>
    struct Rect {
    public:
        Rect() = default;

        Rect(T left, T top, T right, T bottom) {
            this->left = left;
            this->top = top;
            this->right = right;
            this->bottom = bottom;
        }
    public:
        T left = 0;
        T top = 0;
        T right = 0;
        T bottom = 0;
    };

}

#endif // _WTF_RECT_HPP

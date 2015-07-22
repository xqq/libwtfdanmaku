#ifndef _WTF_NONCOPYABLE_HPP
#define _WTF_NONCOPYABLE_HPP

namespace WTFDanmaku {

    class Noncopyable {
    public:
        explicit Noncopyable() = default;
        ~Noncopyable() = default;
    private:
        explicit Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
    };

}

#endif // _WTF_NONCOPYABLE_HPP
#ifndef _DCD_NONCOPYABLE_HPP
#define _DCD_NONCOPYABLE_HPP

namespace DCDanmaku {

    class Noncopyable {
    public:
        explicit Noncopyable() = default;
        ~Noncopyable() = default;
    private:
        explicit Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
    };

}

#endif // _DCD_NONCOPYABLE_HPP
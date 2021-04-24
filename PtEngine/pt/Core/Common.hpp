#pragma once

#ifdef NDEBUG
#   define PT_BUILDMODE_RELEASE
#else
#   define PT_BUILDMODE_DEBUG
#endif

#define PT_NONCOPYABLE(className) \
    className(const className&) = delete; \
    className &operator=(const className&) = delete;

#define PT_NONMOVEABLE(className) \
    className(const className&&) = delete; \
    className &operator=(className&&) = delete;

#define PT_STATIC_CLASS(className) \
    className() = delete; \
    ~className() = delete; \
    PT_NONCOPYABLE(className) \
    PT_NONMOVEABLE(className)

#define PT_SINGLETON(className) \
    public: \
        PT_NONCOPYABLE(className) \
        PT_NONMOVEABLE(className) \
        static className &get() { \
            static className instance; \
            return instance; \
        } \
    private:

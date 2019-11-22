#include <iostream>
#include <tuple>

namespace aop
{
class NonCopyable
{
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
};

class BaseAspect
{
public:
    BaseAspect() = default;
    ~BaseAspect() = default;
    template <typename... Args>
    void Before(Args &&... args) {}
    template <typename... Args>
    void After(Args &&... args) {}
};
template <typename FUNC, typename... AP>
class Aspects : public NonCopyable
{
public:
    Aspects(FUNC &&func, AP &&... aps) : m_func_(std::forward<FUNC>(func)), m_aps_(std::make_tuple(std::forward<AP>(aps)...))
    {
    }
    template <std::size_t I = 0, typename... Args>
    typename std::enable_if<I == sizeof...(AP)>::type Invoke(Args &&... args)
    {
        m_func_(std::forward<Args>(args)...);
    }

    template <std::size_t I = 0, typename... Args>
        typename std::enable_if < I<sizeof...(AP)>::type Invoke(Args &&... args)
    {
        static_assert(std::is_base_of<BaseAspect, typename std::decay<decltype(std::get<I>(m_aps_))>::type>::value, "Aspect class must derives from BaseAspect.\n");
        std::get<I>(m_aps_).Before(std::forward<Args>(args)...);
        Invoke<I + 1>(std::forward<Args>(args)...);
        std::get<I>(m_aps_).After(std::forward<Args>(args)...);
    }

private:
    FUNC m_func_;
    std::tuple<AP...> m_aps_;
};

template <typename... AP, typename FUNC, typename... Args>
typename std::enable_if<!std::is_member_function_pointer<FUNC>::value>::type Invoke(AP &&... aps, FUNC &&func, Args &&... args)
{
    Aspects<FUNC, AP...>(std::forward<FUNC>(func), std::forward<AP>(aps)...).Invoke(std::forward<Args>(args)...);
}

template <typename... AP, typename FUNC, typename TP, typename... Args>
typename std::enable_if<std::is_member_function_pointer<FUNC>::value>::type Invoke(AP &&... aps, FUNC &&func, TP *target, Args &&... args)
{
    auto bind = [&](Args &&... args) -> void {
        (target->*std::forward<FUNC>(func))(std::forward<Args>(args)...);
    };
    Invoke<AP...>(std::forward<AP>(aps)..., bind, std::forward<Args>(args)...);
}

} // namespace aop

struct Class;
struct Field;

template<typename Sig>
class Method;

struct Class
{
    static Class* Find();
    Field* GetField();
    template<typename Sig>
    Method<Sig> GetMethod();
};

template<typename Ret, typename... Args>
struct Method<Ret(Args...)>
{
    Ret Invoke(MonoObject* obj, Args&&... args) const {
        void* arguments[] = { Pack(std::forward<Args>(args))... };
        MonoObject* exc = nullptr;
        MonoObject* result = mono_runtime_invoke(m_Method, obj, arguments, &exc);
        if (exc)
            throw std::runtime_error("mono_runtime_invoke exception");

        return Unpack<Ret>(result);
    }
};
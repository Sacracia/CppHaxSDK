
HaxType = System.Type

struct HaxType {
	void* m_NativePtr;
};

struct HaxMethod {
	
}

struct HaxClass {
	static HaxClass* Find(const char* assembly, const char* nameSpace, const char* klass);
	int32_t FindField(const char* field);
	void* FindStaticField(const char* field);
	ManagedMethod FindMethod(const char* method);
	HaxType* GetType();
	
	void* m_NativePtr; // Il2cppClass* or MonoClass*
};

struct HaxObject {
	HaxClass* GetClass();
	void* Unbox();
	
	template<typename T>
    T as() const {
        static_assert(std::is_base_of_v<HaxObject, T>, "T must inherit from HaxObject");
        return T(m_NativePtr);
    }
	
	void* m_NativePtr; // Il2cppObject* or MonoObject*
};

struct ManagedMethod {
	
	void* Invoke(void* __this, void** args);
	void* Address;
	void* Thunk;
	
private:
	void* m_NativePtr;
};

template<typename Ret, typename... Args>
struct HaxMethod<Ret(Args...)>
	
	HaxMethod(ManagedMethod managed) 
	{
		Address = HookOrig = (Ret(*)(Args...))(managed.Address);
		Thunk = (Ret(__stdcall *)(Args...))(managed.Thunk);
	}
	
	Ret Thunk(const Args&... args) {
		void* exc = nullptr;
		if constexpr (std::is_void_v<Ret>) 
		{
			m_Thunk(args..., exc);
			if (exc)
				throw std::runtime_error();
			return;
		}
		else {
			Ret ret = m_Thunk(args..., exc);
			if (exc)
				throw std::runtime_error();
			return ret;
		}
	};
	
	bool ThunkAvailable() { return m_Thunk != nullptr; }
	
	Ret Invoke(const Args&... args) const
    {
        void* packedArgs[] = { Pack(args)... };
        int nArgs = sizeof(packedArgs) / sizeof(void*);
        HaxObject obj = m_ManagedMethod.Invoke(packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1, nullptr);
		
        if constexpr (std::is_void_v<Ret>) return;
        else if constexpr (std::is_base_of_v<HaxObject, Ret>) return obj.as<Ret>();
        else return *(Ret*)(obj.Unbox());
    }

    Ret InvokeStatic(const Args&... args) const
    {
        if constexpr (sizeof...(args) == 0)
        {
            HaxObject res = m_ManagedMethod.Invoke(nullptr, nullptr);
			
			if constexpr (std::is_void_v<Ret>) return;
			else if constexpr (std::is_base_of_v<HaxObject, Ret>) return res.as<Ret>();
			else return *(Ret*)(res.Unbox());
        }
        else
        {
            void* packedArgs[] = { Pack(args)... };
            HaxObject res = m_ManagedMethod.Invoke(nullptr, packedArgs);
			
			if constexpr (std::is_void_v<Ret>) return;
			else if constexpr (std::is_base_of_v<HaxObject, Ret>) return res.as<Ret>();
			else return *(Ret*)(res.Unbox());
        }
    }
	
private:
	template <typename T>
    void* Pack(const T& val)
    {
        if constexpr (std::is_pointer_v<T> || std::is_base_of_v<HaxObject, T>)
        {
            return (void*)(val);
        }
        else
        {
            return (void*)(&val);
        }
    }

public:
	Ret(*Address)(Args...);
    Ret(*HookOrig)(Args...);
	
private:
    Ret(__stdcall *m_Thunk)(Args...,void**);
	ManagedMethod m_ManagedMethod;
};


struct System::Object : HaxObject {
	
	int32_t GetHashCode() {
		static HaxMethod<int32_t(System::Object)> method = this->GetClass()->FindMethod("GetHashCode");
		return ThunkAvailable() ? method.Thunk(*this) : method.Invoke(*this);
	};
	
	// void* m_NativePtr; наследуется от HaxObject
};
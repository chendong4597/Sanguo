/*!
 * \file LuaCallback.h
 *
 * \author zjhlogo
 * \date 2019/10/30
 *
 * 
 */
#pragma once
#include "../BaseType.h"
#include "LuaPlus/LuaPlus.h"
#include "ScriptVM.h"
#include "LuaBind.h"
#include <sstream>
#include <list>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <memory>

template <int> struct LuaObjectBaseT { LuaPlus::LuaObject m_object; };
template <int N> class LuaObjectT : protected LuaObjectBaseT<N>
{
public:
    enum { LuaObjectN = N };

    LuaObjectT(void) : LuaObjectBaseT<N>{} {}
    LuaObjectT(const LuaObjectT& other) : LuaObjectBaseT<N>{ other.data() } {}
    LuaObjectT(lua_State* L, int idx) : LuaObjectBaseT<N>{ { L, idx } } {}
    LuaObjectT(const LuaPlus::LuaObject& obj) : LuaObjectBaseT<N>{ obj } {}
    LuaObjectT(const NS::tstring& name) : LuaObjectBaseT<N>{ { ScriptVM::getState()->GetGlobal(name.c_str()) } } {}

    inline const LuaPlus::LuaObject& data(void) const { return m_object; }
    inline       LuaPlus::LuaObject& data(void) { return m_object; }

private:
    using LuaObjectBaseT<N>::m_object;
};

namespace LPCD
{
    // Enum
    template <typename EnumT> struct Type<EnumT, typename std::enable_if<std::is_enum<EnumT>::value>::type>
    {
        static void Push(lua_State* L, EnumT value)
        {
            lua_pushinteger(L, static_cast<int>(value));
        }

        static bool Match(lua_State* L, int idx)
        {
            return lua_type(L, idx) == LUA_TNUMBER;
        }

        static EnumT Get(lua_State* L, int idx)
        {
            return static_cast<EnumT>(lua_tointeger(L, idx));
        }
    };


    // T*
    template <typename T> struct Type<T*>
    {
        static void Push(lua_State* L, T* p)
        {
            if (p)
            {
                if (!p->__lua_p_object)
                {
                    p->__lua_p_object = LuaBind::createLuaObject(p->getRtti(), (void*)(p), L);
                }
                p->__lua_p_object->Push(L);
            }
            else
            {
                lua_pushnil(L);
            }
        }

        static bool Match(lua_State* L, int idx)
        {
            int b = lua_istable(L, idx);
            if (!b) return false;

            lua_getfield(L, idx, "__object");
            b = lua_islightuserdata(L, -1);
            lua_pop(L, 1);
            if (!b) return false;

            return true;
        }

        static T* Get(lua_State* L, int idx)
        {
            lua_getfield(L, idx, "__object");
            auto ret = reinterpret_cast<T*>(lua_touserdata(L, -1));
            lua_pop(L, 1);

            return ret;
        }
    };


    // NumberType
    template <typename T> struct NumberType
    {
        static void Push(lua_State* L, T value)
        {
            std::ostringstream os;
            os << value;
            lua_pushstring(L, os.str().c_str());
        }

        static bool Match(lua_State* L, int idx)
        {
            return lua_type(L, idx) == LUA_TSTRING;
        }

        static T Get(lua_State* L, int idx)
        {
            std::istringstream is(lua_tostring(L, idx));
            T ret{};
            is >> ret;
            return ret;
        }
    };
    template<> struct Type< int64_t> : public NumberType< int64_t> {};
#if !defined(_WIN64)
    template<> struct Type<uint64_t> : public NumberType<uint64_t> {};
#endif // _WIN64

    // LuaObjectT
    template <int N> void lua_object_t(LuaObjectT<N>*);
    template <typename T> struct Type<T, decltype(lua_object_t((T*)0))>
    {
        static void Push(lua_State* L, const T& value)
        {
            value.LuaObjectT<T::LuaObjectN>::data().Push(L);
        }

        static bool Match(lua_State* L, int idx)
        {
            return lua_type(L, idx) == T::LuaObjectN;
        }

        static T Get(lua_State* L, int idx)
        {
            return LuaPlus::LuaObject(L, idx);
        }
    };

    // ContainerT
    template <typename ContainerT> struct ContainerSetter
    {
        static void Push(lua_State* L, const ContainerT& container)
        {
            lua_createtable(L, static_cast<int>(container.size()), 0);
            auto table = lua_gettop(L);

            lua_Integer i = 1;
            for (const auto& v : container)
            {
                LPCD::Type<typename ContainerT::value_type>::Push(L, v);
                lua_rawseti(L, table, i++);
            }
        }
    };
    template <typename ContainerT> struct ContainerSetter_Stack
    {
        static void Push(lua_State* L, const ContainerT& container)
        {
            lua_createtable(L, container.size(), 0);
            auto table = lua_gettop(L);

            lua_Integer i = 1;
            ContainerT t(container);
            while (!t.empty())
            {
                LPCD::Type<typename ContainerT::value_type>::Push(L, t.top());
                lua_rawseti(L, table, i++);
                t.pop();
            }
        }
    };
    template <typename ContainerT, typename InserterT> struct ContainerGetter
    {
        static bool Match(lua_State* L, int idx)
        {
            return lua_istable(L, idx);
        }

        static ContainerT Get(lua_State* L, int idx)
        {
            ContainerT ret;
            auto length = lua_rawlen(L, idx);
            for (size_t i = 1; i <= length; ++i)
            {
                lua_rawgeti(L, idx, i);
                InserterT::insert(ret, Type<typename ContainerT::value_type>::Get(L, lua_gettop(L)));
                lua_pop(L, 1);
            }
            return std::move(ret);
        }
    };
    struct Inserter_Sequence    final { template <typename ContainerT> static inline void insert(ContainerT& container, typename ContainerT::const_reference value) { container.push_back(value); } };
    struct Inserter_Forward     final { template <typename ContainerT> static inline void insert(ContainerT& container, typename ContainerT::const_reference value) { container.push_front(value); } };
    struct Inserter_Associative final { template <typename ContainerT> static inline void insert(ContainerT& container, typename ContainerT::const_reference value) { container.insert(value); } };
    struct Inserter_Adaptor     final { template <typename ContainerT> static inline void insert(ContainerT& container, typename ContainerT::const_reference value) { container.push(value); } };
    template <typename ContainerT, typename InserterT, template <typename> class SetterT> struct ContainerType : public ContainerGetter<ContainerT, InserterT>, public SetterT<ContainerT> {};
    template <typename T> struct Type<std::vector            <T>> : public ContainerType<std::vector            <T>, Inserter_Sequence, ContainerSetter      > {};
    template <typename T> struct Type<std::deque             <T>> : public ContainerType<std::deque             <T>, Inserter_Sequence, ContainerSetter      > {};
    template <typename T> struct Type<std::list              <T>> : public ContainerType<std::list              <T>, Inserter_Sequence, ContainerSetter      > {};
    template <typename T> struct Type<std::set               <T>> : public ContainerType<std::set               <T>, Inserter_Associative, ContainerSetter      > {};
    template <typename T> struct Type<std::multiset          <T>> : public ContainerType<std::multiset          <T>, Inserter_Associative, ContainerSetter      > {};
    template <typename T> struct Type<std::unordered_set     <T>> : public ContainerType<std::unordered_set     <T>, Inserter_Associative, ContainerSetter      > {};
    template <typename T> struct Type<std::unordered_multiset<T>> : public ContainerType<std::unordered_multiset<T>, Inserter_Associative, ContainerSetter      > {};
    template <typename T> struct Type<std::stack             <T>> : public ContainerType<std::stack             <T>, Inserter_Adaptor, ContainerSetter_Stack> {};
    template <typename T> struct Type<std::priority_queue    <T>> : public ContainerType<std::priority_queue    <T>, Inserter_Adaptor, ContainerSetter_Stack> {};


    // std::forward_list
    template <typename T> struct Type<std::forward_list<T>> : public ContainerGetter<std::forward_list<T>, Inserter_Forward>
    {
        static void Push(lua_State* L, const std::forward_list<T>& container)
        {
            lua_newtable(L);
            auto table = lua_gettop(L);

            lua_Integer i = 1;
            for (const auto& v : container)
            {
                LPCD::Type<T>::Push(L, v);
                lua_rawseti(L, table, i++);
            }
        }
    };


    // std::queue
    template <typename T> struct Type<std::queue<T>> : public ContainerGetter<std::queue<T>, Inserter_Adaptor>
    {
        static void Push(lua_State* L, const std::queue<T>& container)
        {
            lua_createtable(L, container.size(), 0);
            auto table = lua_gettop(L);

            lua_Integer i = 1;
            std::queue<T> t(container);
            while (!t.empty())
            {
                LPCD::Type<T>::Push(L, t.front());
                lua_rawseti(L, table, i++);
                t.pop();
            }
        }
    };


    // std::array
    template <typename T, std::size_t N> struct Type<std::array<T, N>> : public ContainerSetter<std::array<T, N>>
    {
        static bool Match(lua_State* L, int idx)
        {
            return lua_istable(L, idx);
        }

        static std::array<T, N> Get(lua_State* L, int idx)
        {
            std::array<T, N> ret;

            for (int i = 0; i < N; ++i)
            {
                lua_geti(L, idx, i + 1);
                if (!lua_isnil(L, -1))
                    ret[i] = Type<T>::Get(L, lua_gettop(L));
                else
                    ret[i] = {};
            }
            lua_pop(L, static_cast<int>(N));

            return std::move(ret);
        }
    };


    // tstring
    template<> struct Type<NS::tstring>
    {
        static inline void Push(lua_State* L, const NS::tstring& value)
        {
            lua_pushstring(L, value.c_str());
        }

        static inline bool Match(lua_State* L, int idx)
        {
            return lua_type(L, idx) == LUA_TSTRING;
        }

        static inline NS::tstring Get(lua_State* L, int idx)
        {
            return NS::tstring(lua_tostring(L, idx));
        }
    };


    // glm::vec2
    template<> struct Type<glm::vec2>
    {
        static void Push(lua_State* L, glm::vec2 value);
        static bool Match(lua_State* L, int idx);
        static glm::vec2 Get(lua_State* L, int idx);
    };
    template<> struct Type<glm::ivec2> : public Type<glm::vec2> {};


    // glm::vec3
    template<> struct Type<glm::vec3>
    {
        static void Push(lua_State* L, glm::vec3 value);
        static bool Match(lua_State* L, int idx);
        static glm::vec3 Get(lua_State* L, int idx);
    };
    template<> struct Type<glm::ivec3> : public Type<glm::vec3> {};


    // glm::vec4
    template<> struct Type<glm::vec4>
    {
        static void Push(lua_State* L, glm::vec4 value);
        static bool Match(lua_State* L, int idx);
        static glm::vec4 Get(lua_State* L, int idx);
    };
    template<> struct Type<glm::ivec4> : public Type<glm::vec4> {};


    // glm::quat
    template<> struct Type<glm::quat>
    {
        static void Push(lua_State* L, glm::quat value);
        static bool Match(lua_State* L, int idx);
        static glm::quat Get(lua_State* L, int idx);
    };


    // glm::mat4
    template<> struct Type<glm::mat4>
    {
        static void Push(lua_State* L, glm::mat4 value);
        static bool Match(lua_State* L, int idx);
        static glm::mat4 Get(lua_State* L, int idx);
    };
}


class LuaTableRaw final
{
public:
	LuaTableRaw() {}
	~LuaTableRaw() {}
    LuaTableRaw(const LuaTableRaw&) = delete;
    LuaTableRaw(const LuaTableRaw&&) = delete;
    LuaTableRaw& operator= (const LuaTableRaw&) = delete;
    LuaTableRaw& operator= (const LuaTableRaw&&) = delete;

#define _TABLE_ LuaTableRaw
#define _GETTER_ lua_rawget
#define _SETTER_ lua_rawset
#define _SETTER_I_ lua_rawseti
#include "LuaTableAccessor.inl"

private:
    inline const LuaPlus::LuaObject& data(void) const noexcept { return reinterpret_cast<const LuaPlus::LuaObject&>(*this); }
    inline       LuaPlus::LuaObject& data(void)       noexcept { return reinterpret_cast<LuaPlus::LuaObject&>(*this); }
};


template <> struct LuaObjectBaseT<LUA_TTABLE>
{
    LuaObjectBaseT() : m_object{} {}
    LuaObjectBaseT(const LuaPlus::LuaObject& other) : m_object{ other } {}
    ~LuaObjectBaseT(void) { m_object.~LuaObject(); }

    LuaObjectBaseT& operator= (const LuaObjectBaseT& other)
    {
        m_object = other.m_object;
        return *this;
    }

    //union
    //{
        LuaPlus::LuaObject m_object;
        LuaTableRaw raw;
    //};
};

class LuaTable final : public LuaObjectT<LUA_TTABLE>
{
public:
    using LuaObjectT::LuaObjectT;
    using LuaObjectT<LUA_TTABLE>::raw;
    LuaTable();

#define _TABLE_ LuaTable
#define _GETTER_ lua_gettable
#define _SETTER_ lua_settable
#define _SETTER_I_ lua_seti
#include "LuaTableAccessor.inl"

public:
    inline size_t length(void) const { return data().GetCount(); }

    template <typename T> inline bool get(const NS::tstring& key, T& value) const { value = raw[key]; return true; }
    template <typename T> inline bool geti(int                index, T& value) const { value = raw[index]; return true; }
    template <typename T> inline void set(const NS::tstring& key, const T& value) { raw[key] = value; }
    template <typename T> inline void seti(int                index, const T& value) { raw[index] = value; }
};

class LuaCallback final : public LuaObjectT<LUA_TFUNCTION>
{
public:
    using LuaObjectT::LuaObjectT;

    class LuaReturnValue
    {
    public:
        LuaReturnValue(const LuaTable& table) : m_table(table) {}

        template <typename T> inline operator T(void) const { return m_table.raw[1]; }

    public:
        class LuaValue final
        {
        public:
            LuaValue(void) = default;
            LuaValue(const LuaPlus::LuaObject& obj) : m_obj(obj) {}

            template <typename T> inline operator T(void) const
            {
                if (m_obj.IsNil()) return {};
                if (!LPCD::Type<T>::Match(m_obj.GetCState(), m_obj.GetRef())) return {};
                return LPCD::Type<T>::Get(m_obj.GetCState(), m_obj.GetRef());
            }

            inline bool isNil(void) const { return m_obj.IsNil(); }

        private:
            LuaPlus::LuaObject m_obj;
        };
        inline LuaValue operator[] (int index) const { return m_table.raw[index].operator LuaPlus::LuaObject(); }

    public:
        inline size_t length(void) const { return m_table.length(); }
        inline const LuaTable& data(void) const noexcept { return m_table; }

    private:
        LuaTable m_table;
    };

    template <typename... Args> LuaReturnValue operator() (const Args&... args) const
    {
        assert(!data().IsNil());
        if (data().IsNil())
        {
            return LuaReturnValue(LuaTable());
        }

        int nSizeArgs = sizeof...(Args) + 1 + 1 + LUA_MINSTACK;
        auto pState = ScriptVM::getState();
        int nCheckStackResult = pState->CheckStack(nSizeArgs);
        assert(nCheckStackResult > 0);
        if (nCheckStackResult <= 0)
        {
            return LuaReturnValue(LuaTable());
        }

        pState->GetGlobal("__try_ex").Push(pState);
        data().Push(pState);
        push(args...);

        auto pCState = pState->GetCState();
        lua_call(pCState, sizeof...(Args) + 1, 1);

        LuaReturnValue ret = LPCD::Type<LuaTable>::Get(pCState, lua_gettop(pCState));
        lua_pop(pCState, 1);
        return ret;
    }

    inline operator bool(void) const { return !data().IsNil(); }

private:
    template <typename T, typename... Args> void push(const T& v, const Args&... args) const
    {
        LPCD::Type<T>::Push(ScriptVM::getState()->GetCState(), v);
        push(args...);
    }

    inline void push(void) const {}
};

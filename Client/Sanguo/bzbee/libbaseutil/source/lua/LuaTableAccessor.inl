private:
	class GetterBase
	{
		GetterBase            (const GetterBase& ) = delete;
		GetterBase            (const GetterBase&&) = delete;
		GetterBase& operator= (const GetterBase& ) = delete;
		GetterBase& operator= (const GetterBase&&) = delete;

	protected:
		GetterBase(lua_State* pState, int refTable)
			: m_pState(pState)
			, m_refTable(refTable)
		{}

		template <typename T> T get_impl(void) const
		{
			_GETTER_(m_pState, m_refTable);

			if (!LPCD::Type<T>::Match(m_pState, -1))
			{
				lua_pop(m_pState, 1);
				return {};
			}

			auto ret = LPCD::Type<T>::Get(m_pState, -1);
			lua_pop(m_pState, 1);
			return std::move(ret);
		}

	protected:
		lua_State* m_pState;
		int m_refTable;
	};

public:
	class GetterByIndex : protected GetterBase
	{
		friend class _TABLE_;

	protected:
		GetterByIndex(lua_State* pState, int refTable, int index)
			: GetterBase(pState, refTable)
			, m_index(index)
		{}

	public:
		template <typename T, typename = decltype(LPCD::Type<T>::Get), typename = std::enable_if_t<!std::is_same<T, NS::tstring>::value && !std::is_same<T, char>::value>>
		inline          operator T          (void) const { push(); return get_impl<T          >(); }
		inline          operator NS::tstring(void) const { push(); return get_impl<NS::tstring>(); }
		inline explicit operator char       (void) const { push(); return get_impl<char       >(); }

	private:
		inline void push(void) const { lua_pushinteger(m_pState, m_index); }

	protected:
		int m_index;
	};

	class GetterByName : protected GetterBase
	{
		friend class _TABLE_;

	protected:
		GetterByName(lua_State* pState, int refTable, const NS::tstring& name)
			: GetterBase(pState, refTable)
			, m_name(name)
		{}

	public:
		template <typename T, typename = decltype(LPCD::Type<T>::Get), typename = std::enable_if_t<!std::is_same<T, NS::tstring>::value && !std::is_same<T, char>::value>>
		inline          operator T          (void) const { push(); return get_impl<T          >(); }
		inline          operator NS::tstring(void) const { push(); return get_impl<NS::tstring>(); }
		inline explicit operator char       (void) const { push(); return get_impl<char       >(); }

	private:
		inline void push(void) const { lua_pushstring(m_pState, m_name.c_str()); }

	protected:
		NS::tstring m_name;
	};

	class AccessorByIndex final : public GetterByIndex
	{
		friend class _TABLE_;
		using GetterByIndex::GetterByIndex;

	public:
#define __OP_ASSIGN__(T) { LPCD::Type<T>::Push(m_pState, value); _SETTER_I_(m_pState, m_refTable, m_index); return value; }
		template <typename T                                                        > inline       T* operator= (      T* value) __OP_ASSIGN__(T*);
		template <typename T, typename = std::enable_if_t< std::is_scalar<T>::value>> inline       T  operator= (      T  value) __OP_ASSIGN__(T );
		template <typename T, typename = std::enable_if_t<!std::is_scalar<T>::value>> inline       T& operator= (      T& value) __OP_ASSIGN__(T );
		template <typename T, typename = std::enable_if_t<!std::is_scalar<T>::value>> inline const T& operator= (const T& value) __OP_ASSIGN__(T );
#undef  __OP_ASSIGN__
	};

	class AccessorByName final : public GetterByName
	{
		friend class _TABLE_;
		using GetterByName::GetterByName;

	public:
#define __OP_ASSIGN__(T) { lua_pushstring(m_pState, m_name.c_str()); LPCD::Type<T>::Push(m_pState, value); _SETTER_(m_pState, m_refTable); return value; }
		template <typename T                                                        > inline       T* operator= (      T* value) __OP_ASSIGN__(T*);
		template <typename T, typename = std::enable_if_t< std::is_scalar<T>::value>> inline       T  operator= (      T  value) __OP_ASSIGN__(T );
		template <typename T, typename = std::enable_if_t<!std::is_scalar<T>::value>> inline       T& operator= (      T& value) __OP_ASSIGN__(T );
		template <typename T, typename = std::enable_if_t<!std::is_scalar<T>::value>> inline const T& operator= (const T& value) __OP_ASSIGN__(T );
#undef  __OP_ASSIGN__
	};

	inline GetterByIndex   operator[] (int index              ) const { return { data().GetCState(), data().GetRef(), index }; }
	inline GetterByName    operator[] (const NS::tstring& name) const { return { data().GetCState(), data().GetRef(), name  }; }
	inline AccessorByIndex operator[] (int index              )       { return { data().GetCState(), data().GetRef(), index }; }
	inline AccessorByName  operator[] (const NS::tstring& name)       { return { data().GetCState(), data().GetRef(), name  }; }

#undef _TABLE_
#undef _GETTER_
#undef _SETTER_
#undef _SETTER_I_

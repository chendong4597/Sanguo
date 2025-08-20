/*! bzb engine
 * file inName.h
 * author xkj
 * date 2019/09/11
 */
#pragma once

#include "BaseType.h"

#define NAME_NO_NUMBER_INTERNAL	0

/** Maximum size of name. */
enum {NAME_SIZE	= 128};

/*----------------------------------------------------------------------------
	NameEntry.
----------------------------------------------------------------------------*/
/** Enumeration for finding name. */
enum EFindName
{
	/** Find a name; return 0 if it doesn't exist. */
	NAME_Find,

	/** Find a name or add it if it doesn't exist. */
	NAME_Add,

	/** Finds a name and replaces it. Adds it if missing */
	NAME_Replace,
};

/**
 * A global name, as stored in the global name table.
 */
struct NameEntry
{
public:
	/** Index of name in hash. */
	int		Index;

	/** Pointer to the next entry in this hash bin's linked list. */
	NameEntry*		HashNext;

	/** inName, variable-sized - note that AllocateNameEntry only allocates memory as needed. */
	std::string			inName;

	// Friend for access to Flags.
	friend  NameEntry* allocateNameEntry( const char* inName, int Index, NameEntry* HashNext );
};

 NameEntry*  allocateNameEntry( const char* inName, int Index, NameEntry* HashNext );

class Name 
{
public:

	/** @name Accessors */
	//@{
	inline int getIndex() const
	{
		assert(m_index >= 0 && m_index < (int)ms_names.size());
		return m_index;
	}

	int getNumber() const;

	inline const char* c_str() const {
		return getName();
	}

	inline const char* getName() const
	{
		return (m_index>=0 && m_index<(int)ms_names.size() && ms_names[m_index]) ? ms_names[m_index]->inName.c_str() : "";
	}
	//@}

	inline bool operator<( const Name& inOther) const
	{
		return this->m_index < inOther.m_index ? true 
			: this->m_index > inOther.m_index ? false
			: this->m_number < inOther.m_number;
	}

	inline bool operator>( const Name& inOther) const
	{
		return this->m_index > inOther.m_index ? true
			: this->m_index < inOther.m_index ? false
			: this->m_number > inOther.m_number;
	}

	inline bool operator==( const Name& inOther ) const
	{
		return m_index == inOther.m_index && m_number == inOther.m_number;
	}
	inline bool operator!=( const Name& inOther ) const
	{
		return m_index != inOther.m_index || m_number != inOther.m_number;
	}
	inline bool IsValid() const
	{
		return m_index>=0 && m_index<(int)ms_names.size() && ms_names[m_index]!=NULL;
	}

	/**
	 * Create an Name with a hardcoded string index.
	 *
	 * @param N The harcdcoded value the string portion of the name will have. The number portion will be NAME_NO_NUMBER
	 */
	inline Name( int N )
	: m_index( N )
	, m_number( NAME_NO_NUMBER_INTERNAL )
	{}

	/**
	 * Create an Name with a hardcoded string index and (instance).
	 *
	 * @param N The harcdcoded value the string portion of the name will have
	 * @param inNumber The hardcoded value for the number portion of the name
	 */
	inline Name( int N, int inNumber )
	: m_index( N )
	, m_number( inNumber )
	{}

	/**
	 * Create an uninitialized Name
	 */
	inline Name()
		:m_index(0)
		,m_number(0)
	{
	}

	/**
	 * Create an Name. If FindType is Name_Find, and the string part of the name 
	 * doesn't already exist, then the name will be NAME_None
	 *
	 * @param inName			Value for the string portion of the name
	 * @param FindType		Action to take (see EFindName)
	 * @param unused
	 */
	Name( const char* inName, EFindName findType = NAME_Replace );

	/**
	 * Create an Name. If FindType is Name_Find, and the string part of the name 
	 * doesn't already exist, then the name will be NAME_None
	 *
	 * @param inName Value for the string portion of the name
	 * @param Number Value for the number portion of the name
	 * @param FindType Action to take (see EFindName)
	 */
	inline Name( const char* inName, int inNumber, EFindName findType = NAME_Replace );

	/** @name inName subsystem */
	//@{
	static void staticInit();
	static void staticExit();
	static void hardcode( NameEntry* autoName );
	//@}

	/** @name inName subsystem accessors */
	//@{
	inline static const char *safeString( int index, int instanceNumber=NAME_NO_NUMBER_INTERNAL )
	{
		return isInitialized()
			? (0<=index && index<(int)ms_names.size() && ms_names[index])
			? Name(index, instanceNumber).getName()
			: "Invalid"
			: "Uninitialized";
	}
	inline static int getMaxNames()
	{
		return (int)ms_names.size();
	}
	/**
	 * @return Size of all name entries.
	 */
	inline static int getNameEntryMemorySize()
	{
		return ms_nameEntryMemorySize;
	}
	inline static NameEntry* GetEntry( int i )
	{
		return ms_names[i];
	}
	inline static bool getInitialized()
	{
		return isInitialized();
	}
	//@}

	/**
	 * Helper function to split an old-style name (Class_Number, ie Rocket_17) into
	 * the component parts usable by new-style Names. Only use results if this function
	 * returns TRUE.
	 *
	 * @param OldName		Old-style name
	 * @param NewName		Ouput string portion of the name/number pair
	 * @param NewNameLen	Size of NewName buffer (in char units)
	 * @param NewNumber		Number portion of the name/number pair
	 *
	 * @return TRUE if the name was split, only then will NewName/NewNumber have valid values
	 */
	//static bool SplitNameWithCheck(const char* OldName, char* NewName, int NewNameLen, int& NewNumber);

private:
	/** Index into the Names array (used to find String portion of the string/number pair) */
	int m_index;

	/** Number portion of the string/number pair (stored internally as 1 more than actual, so zero'd memory will be the default, no-instance case) */
	int			m_number;

	// Static subsystem variables.
	/** Table of all names. This is a NoInit because we may need to use it before the constructor is called due to random static variable initialization order */
	static std::vector<NameEntry*> 	ms_names;					// Table of all names.
	static NameEntry*					ms_nameHash[4096];			// Hashed names.
	static int							ms_nameEntryMemorySize;	// Size of all name entries.

	/**
	 * Return the static initialized flag. Must be in a function like this so we don't have problems with 
	 * different initialization order of static variables across the codebase. Use this function to get or set the variable.
	 */
	inline static bool& isInitialized()
	{
		static bool bIsInitialized = 0;
		return bIsInitialized;
	}

	//friend  const char* DebugName(int);
	//friend  const char* DebugName(int, int);
	//friend  const char* DebugName(Name&);
	//friend std::string* AllocateNameEntry( const char* inName, int Index, std::string* HashNext );

	/**
	 * Shared initialization code (between two constructors)
	 * 
	 * @param inName String name of the name/number pair
	 * @param inNumber Number part of the name/number pair
	 * @param FindType Operation to perform on names
	 * @param bSplitName If TRUE, this function will attempt to split a number off of the string portion (turning Rocket_17 to Rocket and number 17)
	 */
	void init(const char* inName, int inNumber, EFindName findType);

	const char *toString() const;
	void toString(std::string &out) const;

public:
	/**
	 * This function is only for use by the debugger Name add-in; it should never be called from code
	 */
	inline static std::vector<NameEntry*>&	getPureNamesTable(void)
	{
		return Name::ms_names;
	}
};




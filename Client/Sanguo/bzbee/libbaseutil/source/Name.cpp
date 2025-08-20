#include "Name.h"
#include <ctype.h>

/*-----------------------------------------------------------------------------
	CRC functions. (MOVED FROM UNMISC.CPP FOR MASSIVE WORKAROUND @todo put back!
-----------------------------------------------------------------------------*/
// CRC 32 polynomial.
#define CRC32_POLY 0x04c11db7

/** 
* Helper class for initializing the global GCRCTable
*/
static int sg_GCRCTable[256];
class FCRCTableInit
{
public:
	/**
	* Constructor
	*/
	FCRCTableInit()
	{
		// Init CRC table.
		for( int iCRC=0; iCRC<256; iCRC++ )
		{
			for( int c=iCRC<<24, j=8; j!=0; j-- )
			{
				sg_GCRCTable[iCRC] = c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
			}
		}
	}	
};

/** Case insensitive string hash function. */
inline int appStrihash( const char* Data )
{
	int Hash=0;
	while( *Data )
	{
		char Ch = toupper(*Data++);
		unsigned char  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ sg_GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

/*-----------------------------------------------------------------------------
	Name helpers.
-----------------------------------------------------------------------------*/

/**
* Helper function that can be used inside the debuggers watch window. E.g. "DebugName(Class->inName.Index)". 
*
* @param	Index	inName index to look up string for
* @return			Associated name
*/
const char* debugGetName(int Index)
{
	return Name::safeString(Index);
}

/**
* Helper function that can be used inside the debuggers watch window. E.g. "DebugName(Class->inName.Index, Class->inName.Number)". 
*
* @param	Index	inName index to look up string for
* @param	Number	Internal instance number of the Name to print (which is 1 more than the printed number)
* @return			Associated name
*/
const char* debugGetName(int Index, int Number)
{
	return Name::safeString(Index, Number);
}

/**
 * Helper function that can be used inside the debuggers watch window. E.g. "DebugName(Class->inName)". 
 *
 * @param	inName	inName to look up string for
 * @return			Associated name
 */
const char* debugGetName(Name& inName)
{
	return inName.getName();
}


/*-----------------------------------------------------------------------------
	Name statics.
-----------------------------------------------------------------------------*/

// Static variables.
NameEntry*				Name::ms_nameHash[4096];
std::vector<NameEntry*>	Name::ms_names;
int							Name::ms_nameEntryMemorySize;
#define ARRAY_COUNT(array)  (sizeof(array)/sizeof(array[0]))

/*-----------------------------------------------------------------------------
	Name implementation.
-----------------------------------------------------------------------------*/

//
// Hardcode a name.
//
void Name::hardcode(NameEntry* AutoName)
{
	// Add name to name hash.
	int iHash          = appStrihash(AutoName->inName.c_str()) & (ARRAY_COUNT(ms_nameHash) - 1);
	AutoName->HashNext = ms_nameHash[iHash];
	ms_nameHash[iHash]    = AutoName;

	// Expand the table if needed.
	for( int i= (int)ms_names.size(); i<=AutoName->Index; i++ )
	{
		ms_names.push_back( NULL );
	}

	// Add name to table.
	//if( Names(AutoName->Index) )
	//{
	//	appErrorf( TEXT("Hardcoded name '%s' at index %i was duplicated. Existing entry is '%s'."), AutoName->inName, AutoName->Index, Names(AutoName->Index)->inName );
	//}

	ms_names[AutoName->Index] = AutoName;
}

/**
 * Create an Name. If FindType is NAME_Find, and the string part of the name 
 * doesn't already exist, then the name will be NAME_None
 *
 * @param inName Value for the string portion of the name
 * @param FindType Action to take (see EFindName)
 */
Name::Name( const char* inName, EFindName FindType)
{
	init(inName, NAME_NO_NUMBER_INTERNAL, FindType);
}

/**
 * Create an Name. If FindType is NAME_Find, and the string part of the name 
 * doesn't already exist, then the name will be NAME_None
 *
 * @param inName Value for the string portion of the name
 * @param Number Value for the number portion of the name
 * @param FindType Action to take (see EFindName)
 */
Name::Name( const char* inName, int Number, EFindName FindType )
{
	init(inName, Number, FindType);
}

/**
 * Shared initialization code (between two constructors)
 * 
 * @param inName		String name of the name/number pair
 * @param inNumber		Number part of the name/number pair
 * @param FindType		Operation to perform on names
 * @param bSplitName	If TRUE, the constructor will attempt to split a number off of the string portion (turning Rocket_17 to Rocket and number 17)
 */
void Name::init(const char* inName, int inNumber, EFindName FindType)
{
	// initialiuze the name subsystem if necessary
	if (!isInitialized())
	{
		//StaticInit();
		assert(0 && "Name has not been initialized");
		return;
	}

	//// If empty or invalid name was specified, return NAME_None.
	//if ( inName[0] == '\0' )
	//{
	//	Index = 0;
	//	Number = NAME_NO_NUMBER_INTERNAL;
	//	return;
	//}

	// set the number
	m_number = inNumber;

	// Try to find the name in the hash.
	int iHash = appStrihash(inName) & (ARRAY_COUNT(ms_nameHash)-1);
	for( NameEntry* Hash=ms_nameHash[iHash]; Hash; Hash=Hash->HashNext )
	{

#ifdef _WIN32
		if( _stricmp( inName, Hash->inName.c_str() ) == 0 )
#else
		if( strcasecmp( inName, Hash->inName.c_str() ) == 0 )
#endif
//end
		{
			// Found it in the hash.
			m_index = Hash->Index;

			// Check to see if the caller wants to replace the contents of the
			// Name with the specified value. This is useful for compiling
			// script classes where the file name is lower case but the class
			// was intended to be uppercase.
			if (FindType == NAME_Replace)
			{
				// Can't rely on the template override for static arrays since the safe crt version of strcpy will fill in
				// the remainder of the array of NAME_SIZE with 0xfd.  So, we have to pass in the length of the dynamically allocated array instead.
				Hash->inName = inName;
			}

			return;
		}
	}

	// Didn't find name.
	if( FindType==NAME_Find )
	{
		// Not found.
		m_index = 0;
		m_number = NAME_NO_NUMBER_INTERNAL;
		return;
	}

	// Add new entry.
	m_index = (int)ms_names.size();
	ms_names.push_back(NULL);

	// Allocate and set the name.
	ms_names[m_index] = ms_nameHash[iHash] = allocateNameEntry( inName, m_index, ms_nameHash[iHash] );
}


/**
 * Converts an Name to a readable format
 *
 * @return String representation of the name
 */
const char *Name::toString() const
{
	assert(m_index < (int)ms_names.size());
	assert(ms_names[m_index]);
	return ms_names[m_index]->inName.c_str();
}

/**
 * Converts an Name to a readable format, in place
 * 
 * @param Out String to fill ot with the string representation of the name
 */
void Name::toString(std::string &Out) const
{
	// a version of ToString that saves at least one string copy
	assert(m_index < (int)ms_names.size());
	assert(ms_names[m_index]);
	Out = ms_names[m_index]->inName;
}



inline int Name::getNumber() const
{
	return m_number;
}


/*-----------------------------------------------------------------------------
	Name subsystem.
-----------------------------------------------------------------------------*/

//
// Initialize the name subsystem.
//
void Name::staticInit()
{
	/** Global instance used to initialize the GCRCTable. It used to be initialized in appInit() */
	//@todo: Massive workaround for static init order without needing to use a function call for every use of GCRCTable
	// This ASSUMES that fname::StaticINit is going to be called BEFORE ANY use of GCRCTable
	static FCRCTableInit GCRCTableInit;

	assert(isInitialized() == false);
	assert((ARRAY_COUNT(ms_nameHash)&(ARRAY_COUNT(ms_nameHash)-1)) == 0);
	isInitialized() = 1;

	// initialize the std::vector() on first use, NOT when the constructor is called (as it could happen
	// AFTER this function)
	//Name::Names.clear();

	

	// Init the name hash.
	for (int HashIndex = 0; HashIndex < ARRAY_COUNT(Name::ms_nameHash); HashIndex++)
	{
		ms_nameHash[HashIndex] = NULL;
	}

	Name nullName = Name("null");

}

//
// Shut down the name subsystem.
//
void Name::staticExit()
{
	assert(isInitialized());

	// Kill all names.
	for( int i=0; i<(int)ms_names.size(); i++ )
	{
		if( ms_names[i] )
		{
			delete ms_names[i];
		}
	}

	// Empty tables.
	ms_names.clear();
	isInitialized() = 0;
}




/*-----------------------------------------------------------------------------
	std::string implementation.
-----------------------------------------------------------------------------*/


NameEntry* allocateNameEntry(const char* inName, int inIndex, NameEntry* inHashNext)
{
	NameEntry* nameEntry = new NameEntry();
	nameEntry->inName = inName;
	nameEntry->Index = inIndex;
	nameEntry->HashNext = inHashNext;
	return nameEntry;
}



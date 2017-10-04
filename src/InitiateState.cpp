#include "InitState.h"
#include <Windows.h>

#include "signatures/signatures.h"
#include "util/util.h"
#include "console/console.h"
#include "threading/queue.h"
#include "http/http.h"

#include <thread>
#include <list>
#include <fstream>

// Code taken from LuaJIT 2.1.0-beta2
namespace pd2hook
{
	struct lua_State;

	typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);
	typedef int(*lua_CFunction) (lua_State *L);
	typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
	typedef struct luaL_Reg {
		const char* name;
		lua_CFunction func;
	} luaL_Reg;

	// From src/luaconf.h
#define LUA_NUMBER		double

	// From src/lua.h
	// type of numbers in Lua
	typedef LUA_NUMBER lua_Number;
	typedef struct lua_Debug lua_Debug;	// activation record
	// Functions to be called by the debuger in specific events
	typedef void(*lua_Hook) (lua_State* L, lua_Debug* ar);


	// From src/luaconf.h
#define LUA_NUMBER		double

	// From src/lua.h
	// type of numbers in Lua
	typedef LUA_NUMBER lua_Number;
	typedef struct lua_Debug lua_Debug;	// activation record
	// Functions to be called by the debuger in specific events
	typedef void(*lua_Hook) (lua_State* L, lua_Debug* ar);

	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_call, void, "\x48\x63\xC2\x48\x8B\x51\x28\x4C\x8B\xD1\x48\xC1\xE0\x03\x4C\x8B", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pcall, int, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x59\x10\x41\x8B\xF0\x4C\x63", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_gettop, int, "\x48\x8B\x41\x28\x48\x2B\x41\x20\x48\xC1\xF8\x03\xC3", "xxxxxxxxxxxxx", 0, lua_State*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_settop, void, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x85\xD2\x78\x7A\x4C\x8B\x41", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_toboolean, int, "\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x8B\x08\x33\xC0\x48\xC1", "xxxxx????xxxxxxx", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tointeger, ptrdiff_t, "\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x8B\x10\x48\x8B\xCA\x48\xC1\xF9\x2F\x83\xF9\xF2\x73\x0E", "xxxxx????xxxxxxxxxxxxxxx", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tonumber, lua_Number, "\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x8B\x10\x48\x8B\xCA\x48\xC1\xF9\x2F\x83\xF9\xF2\x77\x09", "xxxxx????xxxxxxxxxxxxxxx", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tolstring, const char*, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x49\x8B\xF8\x8B\xDA\x48\x8B\xF1\xE8", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, size_t*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_objlen, size_t, "\x40\x53\x48\x83\xEC\x20\x4C\x8B\xD1\xE8\x00\x00\x00\x00\x48\x8B", "xxxxxxxxxx????xx", 0, lua_State*, int)
		// This is actually luaL_loadfilex() (as per Lua 5.2) now. The new parameter corresponds to mode, and specifying NULL causes Lua
		// to default to "bt", i.e. 'binary and text'
		// https://www.lua.org/manual/5.2/manual.html#luaL_loadfilex
		// https://www.lua.org/manual/5.2/manual.html#pdf-load
		CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_loadfilex, int, "\x48\x89\x5C\x24\x20\x55\x56\x57\x48\x81\xEC\x50\x02\x00\x00\x48\x8B", "xxxxxxxxxxxxxxxxx", 0, lua_State*, const char*, const char*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_loadstring, int, "\x48\x83\xEC\x48\x48\x89\x54\x24\x30\x48\x83\xC8\xFF\x0F\x1F\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*)
		//CREATE_CALLABLE_SIGNATURE(lua_load, int, "\x8B\x4C\x24\x10\x33\xD2\x83\xEC\x18\x3B\xCA", "xxxxxxxxxxx", 0, lua_State*, lua_Reader, void*, const char*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_getfield, void, "\x48\x89\x5C\x24\x10\x57\x48\x83\xEC\x20\x4D\x8B\xD0\x48\x8B\xD9", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_setfield, void, "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x4D\x8B\xD0\x48\x8B\xD9", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_createtable, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4C\x8B\x49\x10\x41\x8B\xF8\x8B\xF2", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_insert, void, "\x4C\x8B\xC9\x85\xD2\x7E\x24\x8D\x42\xFF\x48\x63\xD0\x48\x8B\x41", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
		//CREATE_NORMAL_CALLABLE_SIGNATURE(lua_replace, void, "\x56\x57\x8B\x7C\x24\x10\x81\xFF\xEE\xD8\xFF\xFF\x75\x16\x8B\x4C\x24\x0C\x5F\x8B\x41\x14\x8D\x71\x14\x8B\x40\xF8\x83\x06\xF8\x89", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_remove, void, "\x4C\x8B\xC1\x85\xD2\x7E\x23\x8D\x42\xFF\x48\x63\xD0\x48\x8B\x41\x20\x48\x8B\x49\x28\x48\x8D\x04", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_newstate, lua_State*, "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x48\x8B\xF2\x48\x8B\xE9\x41\xB9\x70\x0B\x00\x00", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_Alloc, void*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_close, void, "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x48\x8B\x79\x10\x48\x8B\xB7\xC8\x00\x00\x00\x48", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*)

		//CREATE_CALLABLE_SIGNATURE(lua_rawset, void, "\x51\x53\x55\x56\x57\x8B\xF1\xE8\x00\x00\x00\x00", "xxxxxxxx????", 0, lua_State*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_settable, void, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x4C\x8B\x43\x28\x49\x83\xE8\x10\x48\x8B", "xxxxxxxxxx????xxxxxxxxxx", 0, lua_State*, int)

		//CREATE_CALLABLE_SIGNATURE(lua_pushnumber, void, "\x8B\x44\x24\x04\x8B\x48\x08\xF3\x0F\x10\x44\x24\x08", "xxxxxxxxxxxxx", 0, lua_State*, double)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushinteger, void, "\x48\x8B\x41\x28\x0F\x57\xC0\xF2\x48\x0F\x2A\xC2\xF2\x0F\x11\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, ptrdiff_t)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushboolean, void, "\x33\xC0\x85\xD2\x0F\x95\xC0\xFF\xC0\x48\x63\xD0\x48\x8B\x41\x28", "xxxxxxxxxxxxxxxx", 0, lua_State*, bool)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushcclosure, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4C\x8B\x49\x10\x49\x63\xF8\x48\x8B", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, lua_CFunction, int);
	// lua_pushstring()'s signature was found before lua_pushlstring()'s, so I'm leaving it here now since it's valid anyway
	// It was used as a quick and dirty - and broken - workaround since most lua_pushlstring() calls are inlined, but it ended up
	// breaking HTTP downloads of zip archives due to its sensitivity to premature null characters
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushlstring, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4C\x8B\x49\x10\x49\x8B\xF8\x48\x8B", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, const char*, size_t)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushstring, void, "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x48\x8B\xFA\x48\x8B\xD9\x48\x85\xD2\x75\x0D\x48\x8B\x41", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, const char*)
		CREATE_NORMAL_CALLABLE_SIGNATURE(lua_checkstack, int, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x81\xFA\x40\x1F\x00\x00\x7F", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)

		// luaI_openlib() is really luaL_openlib(), see lauxlib.h in Lua 5.1's source code
		CREATE_NORMAL_CALLABLE_SIGNATURE(luaI_openlib, void, "\x48\x89\x5C\x24\x18\x55\x56\x41\x56\x48\x83\xEC\x20\x48\x8B\x41", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*, const luaL_Reg*, int)
		CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_ref, int, "\x48\x89\x5C\x24\x20\x57\x48\x83\xEC\x20\x8D\x82\x0F\x27\x00\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, int);
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawgeti, void, "\x40\x53\x48\x83\xEC\x20\x4D\x63\xD0\x48\x8B\xD9\xE8\x00\x00\x00\x00\x48\x8B\x08\x48\xBA\xFF\xFF", "xxxxxxxxxxxxx????xxxxxxx", 0, lua_State*, int, int);
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawseti, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4D\x63\xD0\x48\x8B\xD9\xE8", "xxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int);
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_type, int, "\x48\x83\xEC\x28\x4C\x8B\xD1\xE8\x00\x00\x00\x00\x48\x8B\x08\x4C", "xxxxxxxx????xxxx", 0, lua_State*, int);
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_typename, const char*, "\x48\x8D\x00\x00\x00\x00\x00\x48\x63\xC2\x48\x8B\x04\xC1\xC3\xCC", "xx?????xxxxxxxxx", 0, lua_State*, int);
	CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_unref, void, "\x45\x85\xC0\x0F\x88\x00\x00\x00\x00\x48\x89\x5C\x24\x08\x48\x89", "xxxxx????xxxxxxx", 0, lua_State*, int, int);
	CREATE_CALLABLE_CLASS_SIGNATURE(do_game_update, void*, "\x56\xFF\x74\x24\x0C\x8B\xF1\x68\x00\x00\x00\x00\xFF\x36\xE8", "xxxxxxxx????xxx", 0, int*, int*)
		// Possibly dsl::LuaInterface::newstate() rather than luaL_newstate()
		CREATE_CALLABLE_CLASS_SIGNATURE(luaL_newstate, int, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x41\x0F\xB6\xF8\x0F\xB6\xF2\x48\x8B", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, char, char, int)

		// lua c-functions

		// From src/lua.h
		// Pseudo-indices
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))

		// From src/lauxlib.h
#define LUA_NOREF       (-2)
#define LUA_REFNIL      (-1)

		// more bloody lua shit
		// Thread status; 0 is OK
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5
		// From src/lauxlib.h
		// Extra error code for 'luaL_load'
#define LUA_ERRFILE     (LUA_ERRERR+1)

		// From src/lua.h
		// Option for multiple returns in 'lua_pcall' and 'lua_call'
#define LUA_MULTRET	(-1)
#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define lua_pop(L,n)		lua_settop(L, -(n)-1)
#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)

#if 0
		// From src/lj_def.h
#define UNUSED(x) ((void)(x))

		// From src/lj_obj.h
#define tvchecklive(L, o) \
	UNUSED(L), lua_assert(!tvisgcv(o) || \
	((~itype(o) == gcval(o)->gch.gct) && !isdead(G(L), gcval(o))))

		// From src/lj_obj.h
#define mref(r, t)	((t *)(void *)(uintptr_t)(r).ptr32)
#define GCHeader	GCRef nextgc; uint8_t marked; uint8_t gct
#define LJ_TNIL			(~0u)
#define LJ_TFALSE		(~1u)
#define LJ_TSTR			(~4u)
#define LJ_TNUMX		(~13u)
#define LJ_TISTRUECOND		LJ_TFALSE
#define LJ_TISGCV		(LJ_TSTR+1)
#define niltv(L) \
	check_exp(tvisnil(&G(L)->nilnode.val), &G(L)->nilnode.val)
#define G(L)			(mref(L->glref, global_State))
#define itype(o)	((o)->it)
#define tvisnil(o)	(itype(o) == LJ_TNIL)
#define tvistruecond(o)	(itype(o) < LJ_TISTRUECOND)
#define tvisgcv(o)	((itype(o) - LJ_TISGCV) > (LJ_TNUMX - LJ_TISGCV))
#define gcref(r)	((GCobj *)(uintptr_t)(r).gcptr32)
#define gcval(o)	(gcref((o)->gcr))

		// From src/lj_gc.h
#define LJ_GC_WHITE0	0x01
#define LJ_GC_WHITE1	0x02
#define LJ_GC_WHITES	(LJ_GC_WHITE0 | LJ_GC_WHITE1)
#define otherwhite(g)	(g->gc.currentwhite ^ LJ_GC_WHITES)
#define isdead(g, v)	((v)->gch.marked & otherwhite(g) & LJ_GC_WHITES)

		// From src/luaconf.h
#define luai_apicheck(L, o)	{ (void)L; }

		// From src/lj_def.h
#if defined(_MSC_VER)
		typedef __int32 int32_t;
	typedef unsigned __int8 uint8_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;
	typedef unsigned __int32 uintptr_t;
#define LJ_NORET	__declspec(noreturn)
#define LJ_ALIGN(n)	__declspec(align(n))
#define LJ_INLINE	__inline
#define LJ_AINLINE	__forceinline
#define LJ_NOINLINE	__declspec(noinline)
#define LJ_LIKELY(x)	(x)
#define lua_assert(c)		((void)0)
#define check_exp(c, e)		(e)
#define api_check		luai_apicheck
#endif

	// From src/lj_arch.h
#define LJ_ENDIAN_LOHI(lo, hi)		lo hi

	// From src/lj_obj.h

	// Memory and GC object sizes
	typedef uint32_t MSize;
	typedef uint32_t GCSize;

	// Memory reference
	typedef struct MRef {
		uint32_t ptr32;		// Pseudo 32 bit pointer
	} MRef;

	// GCobj reference
	typedef struct GCRef {
		uint32_t gcptr32;	// Pseudo 32 bit pointer
	} GCRef;

	// Frame link
	typedef union {
		int32_t ftsz;		// Frame type and size of previous frame
		MRef pcr;			// Or PC for Lua frames
	} FrameLink;

	// Tagged value
	typedef LJ_ALIGN(8) union TValue {
		uint64_t u64;		// 64 bit pattern overlaps number
		lua_Number n;		// Number object overlaps split tag/value object
		struct {
			LJ_ENDIAN_LOHI(
			union {
				GCRef gcr;	// GCobj reference (if any)
				int32_t i;	// Integer value
			};
			, uint32_t it;	// Internal object tag. Must overlap MSW of number
			)
		};
		struct {
			LJ_ENDIAN_LOHI(
				GCRef func;	// Function for next frame (or dummy L)
			, FrameLink tp;	// Link to previous frame
			)
		} fr;
		struct {
			LJ_ENDIAN_LOHI(
				uint32_t lo;	// Lower 32 bits of number
			, uint32_t hi;	// Upper 32 bits of number
			)
		} u32;
	} TValue;

	typedef const TValue cTValue;

	typedef uint32_t BCIns;		// Bytecode instruction

	// Resizable string buffer. Need this here, details in lj_buf.h
	typedef struct SBuf {
		MRef p;		// String buffer pointer
		MRef e;		// String buffer end pointer
		MRef b;		// String buffer base
		MRef L;		// lua_State, used for buffer resizing
	} SBuf;

	// String object header. String payload follows
	typedef struct GCstr {
		GCHeader;
		uint8_t reserved;	// Used by lexer for fast lookup of reserved words
		uint8_t unused;
		MSize hash;		// Hash of string
		MSize len;		// Size of string
	} GCstr;

	typedef struct GCupval {
		GCHeader;
		uint8_t closed;	// Set if closed (i.e. uv->v == &uv->u.value)
		uint8_t immutable;	// Immutable value
		union {
			TValue tv;		// If closed: the value itself
			struct {		// If open: double linked list, anchored at thread
				GCRef prev;
				GCRef next;
			};
		};
		MRef v;		// Points to stack slot (open) or above (closed)
		uint32_t dhash;	// Disambiguation hash: dh1 != dh2 => cannot alias
	} GCupval;

	// Hash node
	typedef struct Node {
		TValue val;		// Value object. Must be first field
		TValue key;		// Key object
		MRef next;		// Hash chain
		MRef freetop;		// Top of free elements (stored in t->node[0])
	} Node;

#define MMDEF_FFI(_)
#define MMDEF_PAIRS(_)
#define MMDEF(_) \
	_(index) _(newindex) _(gc) _(mode) _(eq) _(len) \
	/* Only the above (fast) metamethods are negative cached (max. 8). */ \
	_(lt) _(le) _(concat) _(call) \
	/* The following must be in ORDER ARITH. */ \
	_(add) _(sub) _(mul) _(div) _(mod) _(pow) _(unm) \
	/* The following are used in the standard libraries. */ \
	_(metatable) _(tostring) MMDEF_FFI(_) MMDEF_PAIRS(_)

	typedef enum {
#define MMENUM(name)	MM_##name,
		MMDEF(MMENUM)
#undef MMENUM
		MM__MAX,
		MM____ = MM__MAX,
		MM_FAST = MM_len
	} MMS;

	// GC root IDs
	typedef enum {
		GCROOT_MMNAME,	// Metamethod names
		GCROOT_MMNAME_LAST = GCROOT_MMNAME + MM__MAX - 1,
		GCROOT_BASEMT,	// Metatables for base types
		GCROOT_BASEMT_NUM = GCROOT_BASEMT + ~LJ_TNUMX,
		GCROOT_IO_INPUT,	// Userdata for default I/O input file
		GCROOT_IO_OUTPUT,	// Userdata for default I/O output file
		GCROOT_MAX
	} GCRootID;

	typedef struct GCState {
		GCSize total;		// Memory currently allocated
		GCSize threshold;	// Memory threshold
		uint8_t currentwhite;	// Current white color
		uint8_t state;	// GC state
		uint8_t nocdatafin;	// No cdata finalizer called
		uint8_t unused2;
		MSize sweepstr;	// Sweep position in string table
		GCRef root;		// List of all collectable objects
		MRef sweep;		// Sweep position in root list
		GCRef gray;		// List of gray objects
		GCRef grayagain;	// List of objects for atomic traversal
		GCRef weak;		// List of weak tables (to be cleared)
		GCRef mmudata;	// List of userdata (to be finalized)
		GCSize debt;		// Debt (how much GC is behind schedule)
		GCSize estimate;	// Estimate of memory actually in use
		MSize stepmul;	// Incremental GC step granularity
		MSize pause;		// Pause between successive GC cycles
	} GCState;

	// Global state, shared by all threads of a Lua universe
	typedef struct global_State {
		GCRef* strhash;	// String hash table (hash chain anchors)
		MSize strmask;	// String hash mask (size of hash table - 1)
		MSize strnum;		// Number of strings in hash table
		lua_Alloc allocf;	// Memory allocator
		void* allocd;		// Memory allocator data
		GCState gc;		// Garbage collector
		volatile int32_t vmstate;  // VM state or current JIT code trace number
		SBuf tmpbuf;		// Temporary string buffer
		GCstr strempty;	// Empty string
		uint8_t stremptyz;	// Zero terminator of empty string
		uint8_t hookmask;	// Hook mask
		uint8_t dispatchmode;	// Dispatch mode
		uint8_t vmevmask;	// VM event mask
		GCRef mainthref;	// Link to main thread
		TValue registrytv;	// Anchor for registry
		TValue tmptv, tmptv2;	// Temporary TValues
		Node nilnode;		// Fallback 1-element hash part (nil key and value)
		GCupval uvhead;	// Head of double-linked list of all open upvalues
		int32_t hookcount;	// Instruction hook countdown
		int32_t hookcstart;	// Start count for instruction hook counter
		lua_Hook hookf;	// Hook function
		lua_CFunction wrapf;	// Wrapper for C function calls
		lua_CFunction panic;	// Called as a last resort for errors
		BCIns bc_cfunc_int;	// Bytecode for internal C function calls
		BCIns bc_cfunc_ext;	// Bytecode for external C function calls
		GCRef cur_L;		// Currently executing lua_State
		MRef jit_base;	// Current JIT code L->base or NULL
		MRef ctype_state;	// Pointer to C type state
		GCRef gcroot[GCROOT_MAX];	// GC roots
	} global_State;

	// Per-thread state object
	struct lua_State {
		GCHeader;
		uint8_t dummy_ffid;	// Fake FF_C for curr_funcisL() on dummy frames
		uint8_t status;		// Thread status
		MRef glref;			// Link to global state
		GCRef gclist;		// GC chain
		TValue* base;		// Base of currently executing function
		TValue* top;		// First free slot in the stack
		MRef maxstack;		// Last free slot in the stack
		MRef stack;			// Stack base
		GCRef openupval;	// List of open upvalues in the stack
		GCRef env;			// Thread environment (table of globals)
		void* cframe;		// End of C stack frame chain
		MSize stacksize;	// True stack size (incl. LJ_STACK_EXTRA)
	};

	// Copy tagged values
	static LJ_AINLINE void copyTV(lua_State* L, TValue* o1, const TValue* o2)
	{
		*o1 = *o2;
		tvchecklive(L, o1);
	}

	// From src/lj_api.c
#define api_checkvalidindex(L, i)	api_check(L, (i) != niltv(L))

	CREATE_NORMAL_CALLABLE_SIGNATURE(index2adr, TValue*, "\x8B\x4C\x24\x08\x85\xC9\x7E\x1F\x8B\x54\x24\x04\x8B\x42\x10\x8D", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
		// Old internal Lua VM function signatures from Crimefest 2015; incompatible with LuaJIT
		//CREATE_CALLABLE_SIGNATURE(luaV_settable, void, "\x83\xEC\x08\x53\x55\x56\x8B\xEA\x57\x89\x6C\x24\x10\x8B\xD9\xC7\x44\x24\x00\x00\x00\x00\x00\xEB\x07\x8D\xA4\x24\x00\x00\x00\x00", "xxxxxxxxxxxxxxxxxx?????xxxxx????", 0, lua_State*, TValue*, TValue*, StkId)
		//CREATE_CALLABLE_SIGNATURE(luaH_getnum, TValue*, "\x51\x57\x8B\xF9\x8D\x42\xFF\x3B\x47\x1C\x73\x0C", "xxxxxxxxxxxx", 0, LTable*, int)

		// The following functions are unusable with LuaJIT since they rely upon internal Lua VM functions
		/*
		void lua_rawgeti(lua_State* L, int idx, int n) {
		StkId o;
		o = index2adr(L, idx);
		LTable* ott = (LTable*)(o)->value.gc;
		TValue* o2 = luaH_getnum(ott, n);
		L->top->value = o2->value;
		L->top->tt = o2->tt;
		L->top++;
		}

		void lua_settable(lua_State* L, int idx) {
		StkId t;
		t = index2adr(L, idx);
		luaV_settable(L, t, L->top - 2, L->top - 1);
		_asm add esp, 8
		L->top -= 2;
		}

		int lua_gettop(lua_State* L) {
		return L->top - L->base;
		}

		void lua_pushboolean(lua_State* L, int b)
		{
		TValue* i_o = L->top;
		i_o->value.b = (b != 0);
		i_o->tt = 1; // LUA_TBOOLEAN
		L->top++;
		}

		void lua_pushinteger(lua_State* L, int n)
		{
		TValue* i_o = L->top;
		i_o->value.n = n;
		i_o->tt = 3; // LUA_TNUMBER
		L->top++;
		}
		*/

		// HACK: Resorting to these since I can't find any non-inlined instances of lua_toboolean(), lua_remove(), etc.
		/*
		// From src/lj_api.c
		int lua_toboolean(lua_State* L, int idx)
		{
		cTValue* o = index2adr(L, idx);
		return tvistruecond(o);
		}

		static TValue* stkindex2adr(lua_State* L, int idx)
		{
		if (idx > 0) {
		TValue *o = L->base + (idx - 1);
		return o < L->top ? o : niltv(L);
		} else {
		api_check(L, idx != 0 && -idx <= L->top - L->base);
		return L->top + idx;
		}
		}

		void lua_remove(lua_State* L, int idx)
		{
		TValue *p = stkindex2adr(L, idx);
		api_checkvalidindex(L, p);
		while (++p < L->top) copyTV(L, p-1, p);
		L->top--;
		}
		*/
#endif	// 0
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//                                                         BLT Code                                                          //
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::list<lua_State*> activeStates;
	void add_active_state(lua_State* L)
	{
		activeStates.push_back(L);
	}

	void remove_active_state(lua_State* L)
	{
		activeStates.remove(L);
	}

	bool check_active_state(lua_State* L)
	{
		std::list<lua_State*>::iterator it;
		for (it = activeStates.begin(); it != activeStates.end(); it++)
		{
			if (*it == L)
			{
				return true;
			}
		}
		return false;
	}

	FuncDetour* luaCallDetour = nullptr;

	// Not tracking the error count here so it can automatically be reset to 0 whenever the Lua state is deleted and re-created (e.g.
	// when transitioning to / from the menu to a level)
	void NotifyErrorOverlay(lua_State* L, const char* message)
	{
		lua_getglobal(L, "NotifyErrorOverlay");
		if (lua_isfunction(L, -1))
		{
			int args = 0;
			if (message)
			{
				lua_pushstring(L, message);
				args = 1;
			}
			int error = lua_pcall(L, args, 0, 0);
			if (error == LUA_ERRRUN)
			{
				// Don't bother logging the error since the error overlay is designed to be an optional component, just pop the error
				// message off the stack to keep it balanced
				lua_pop(L, 1);
				return;
			}
		}
		else
		{
			lua_pop(L, 1);
			static bool printed = false;
			if (!printed)
			{
				printf("Warning: Failed to find the NotifyErrorOverlay function in the Lua environment; no in-game notifications will be displayed for caught errors\n");
				printed = true;
			}
		}
	}

	void lua_newcall(lua_State* L, int args, int returns){
		// TODO: Optimize this to avoid incurring global table lookup costs whenever a function is called
		// https://stackoverflow.com/questions/30021904/lua-set-default-error-handler/30022216#30022216
		lua_getglobal(L, "debug");
		lua_getfield(L, -1, "traceback");
		lua_remove(L, -2);
		// Do not index from the top (i.e. use a negative index) as this has the potential to mess up if the callee function returns
		// values /and/ lua_pcall() is set up with a > 0 nresults argument
		int errorhandler = lua_gettop(L) - args - 1;
		lua_insert(L, errorhandler);

		int result = lua_pcall(L, args, returns, errorhandler);
		if (result != 0) {
			size_t len;
			const char* message = lua_tolstring(L, -1, &len);
			PD2HOOK_LOG_ERROR(message);
			NotifyErrorOverlay(L, message);
			// This call pops the error message off the stack
			lua_pop(L, 1);
			// To emphasize that the string is no longer on the stack and should not be accessed
			message = nullptr;
		}
		// This call removes the error handler from the stack. Do not use lua_pop() as the callee function's return values may be
		// present, which would pop one of those instead and leave the error handler on the stack
		lua_remove(L, errorhandler);
	}

	int luaF_ispcallforced(lua_State* L)
	{
		lua_pushboolean(L, luaCallDetour ? true : false);
		return 1;
	}

	int luaF_forcepcalls(lua_State* L){
		int n = lua_gettop(L);	// Number of arguments
		if (n < 1) {
			PD2HOOK_LOG_WARN("blt.forcepcalls(): Called with no arguments, ignoring request");
			return 0;
		}

		if (lua_toboolean(L, 1)) {
			if (!luaCallDetour) {
				luaCallDetour = new FuncDetour(lua_call, lua_newcall);
				lua_call = reinterpret_cast<lua_callptr>(luaCallDetour->GetTrampoline());
				PD2HOOK_LOG_LOG("blt.forcepcalls(): Protected calls will now be forced");
			}
			//		else Logging::Log("blt.forcepcalls(): Protected calls are already being forced, ignoring request", Logging::LOGGING_WARN);
		}
		else {
			if (luaCallDetour) {
				delete luaCallDetour;
				luaCallDetour = nullptr;
				PD2HOOK_LOG_LOG("blt.forcepcalls(): Protected calls are no longer being forced");
			}
			//		else Logging::Log("blt.forcepcalls(): Protected calls are not currently being forced, ignoring request", Logging::LOGGING_WARN);
		}
		return 0;
	}
	int luaH_getcontents(lua_State* L, bool files)
	{
		size_t len;
		const char* dirc = lua_tolstring(L, 1, &len);
		std::string dir(dirc, len);
		std::vector<std::string> directories;

		try
		{
			directories = Util::GetDirectoryContents(dir, files);
		}
		catch (int unused)
		{
			// Okay, okay - now shut up, compiler
			(void)unused;
			lua_pushboolean(L, false);
			return 1;
		}

		lua_createtable(L, 0, 0);

		std::vector<std::string>::iterator it;
		int index = 1;
		for (it = directories.begin(); it < directories.end(); it++)
		{
			if (*it == "." || *it == "..") continue;
			lua_pushinteger(L, index);
			lua_pushlstring(L, it->c_str(), it->length());
			lua_settable(L, -3);
			index++;
		}

		return 1;
	}

	int luaF_getdir(lua_State* L)
	{
		return luaH_getcontents(L, true);
	}

	int luaF_getfiles(lua_State* L)
	{
		return luaH_getcontents(L, false);
	}

	int luaF_directoryExists(lua_State* L)
	{
		size_t len;
		const char* dirc = lua_tolstring(L, 1, &len);
		bool doesExist = Util::DirectoryExists(dirc);
		lua_pushboolean(L, doesExist);
		return 1;
	}

	int luaF_unzipfile(lua_State* L)
	{
		size_t len;
		const char* archivePath = lua_tolstring(L, 1, &len);
		const char* extractPath = lua_tolstring(L, 2, &len);

		pd2hook::ExtractZIPArchive(archivePath, extractPath);
		return 0;
	}

	int luaF_removeDirectory(lua_State* L)
	{
		size_t len;
		const char* directory = lua_tolstring(L, 1, &len);
		bool success = Util::RemoveEmptyDirectory(directory);
		lua_pushboolean(L, success);
		return 1;
	}

	int luaF_pcall(lua_State* L)
	{
		int args = lua_gettop(L) - 1;

		lua_getglobal(L, "debug");
		lua_getfield(L, -1, "traceback");
		lua_remove(L, -2);
		// Do not index from the top (i.e. don't use a negative index)
		int errorhandler = lua_gettop(L) - args - 1;
		lua_insert(L, errorhandler);

		int result = lua_pcall(L, args, LUA_MULTRET, errorhandler);
		// lua_pcall() automatically pops the callee function and its arguments off the stack. Then, if no errors were encountered
		// during execution, it pushes the return values onto the stack, if any. Otherwise, if an error was encountered, it pushes
		// the error message onto the stack, which should manually be popped off when done using to keep the stack balanced
		if (result == LUA_ERRRUN)
		{
			size_t len;
			PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
			// This call pops the error message off the stack
			lua_pop(L, 1);
			return 0;
		}
		// Do not use lua_pop() as the callee function's return values may be present, which would pop one of those instead and leave
		// the error handler on the stack
		lua_remove(L, errorhandler);
		lua_pushboolean(L, result == 0);
		lua_insert(L, 1);

		//if (result != 0) return 1;

		return lua_gettop(L);
	}

	int luaF_dofile(lua_State* L)
	{

		int n = lua_gettop(L);

		size_t length = 0;
		const char* filename = lua_tolstring(L, 1, &length);
		int error = luaL_loadfilex(L, filename, nullptr);
		if (error != 0)
		{
			size_t len;
			//		Logging::Log(filename, Logging::LOGGING_ERROR);
			PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
		}
		else
		{
			lua_getglobal(L, "debug");
			lua_getfield(L, -1, "traceback");
			lua_remove(L, -2);
			// Example stack for visualization purposes:
			// 3 debug.traceback
			// 2 compiled code as a self-contained function
			// 1 filename
			// Do not index from the top (i.e. don't use a negative index)
			int errorhandler = 2;
			lua_insert(L, errorhandler);

			error = lua_pcall(L, 0, 0, errorhandler);
			if (error == LUA_ERRRUN)
			{
				size_t len;
				//			Logging::Log(filename, Logging::LOGGING_ERROR);
				PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
				// This call pops the error message off the stack
				lua_pop(L, 1);
			}
			// Do not use lua_pop() as the callee function's return values may be present, which would pop one of those instead and
			// leave the error handler on the stack
			lua_remove(L, errorhandler);
		}
		return 0;
	}

	struct lua_http_data
	{
		int funcRef;
		int progressRef;
		int requestIdentifier;
		lua_State* L;
	};

	void return_lua_http(void* data, std::string& urlcontents)
	{
		lua_http_data* ourData = (lua_http_data*)data;
		if (!check_active_state(ourData->L))
		{
			delete ourData;
			return;
		}

		lua_rawgeti(ourData->L, LUA_REGISTRYINDEX, ourData->funcRef);
		lua_pushlstring(ourData->L, urlcontents.c_str(), urlcontents.length());
		lua_pushinteger(ourData->L, ourData->requestIdentifier);
		lua_pcall(ourData->L, 2, 0, 0);
		luaL_unref(ourData->L, LUA_REGISTRYINDEX, ourData->funcRef);
		luaL_unref(ourData->L, LUA_REGISTRYINDEX, ourData->progressRef);
		delete ourData;
	}

	void progress_lua_http(void* data, long progress, long total)
	{
		lua_http_data* ourData = (lua_http_data*)data;

		if (!check_active_state(ourData->L))
		{
			return;
		}

		if (ourData->progressRef == 0) return;
		lua_rawgeti(ourData->L, LUA_REGISTRYINDEX, ourData->progressRef);
		lua_pushinteger(ourData->L, ourData->requestIdentifier);
		lua_pushinteger(ourData->L, progress);
		lua_pushinteger(ourData->L, total);
		lua_pcall(ourData->L, 3, 0, 0);
	}

	int luaF_directoryhash(lua_State* L)
	{ 
		PD2HOOK_TRACE_FUNC; 
		int n = lua_gettop(L); 

		size_t length = 0; 
		const char* filename = lua_tolstring(L, 1, &length); 
		std::string hash = Util::GetDirectoryHash(filename); 
		lua_pushlstring(L, hash.c_str(), hash.length()); 

		return 1; 
	} 

	int luaF_filehash(lua_State* L)
	{
		int n = lua_gettop(L);
		size_t l = 0;
		const char * fileName = lua_tolstring(L, 1, &l);
		std::string hash = Util::GetFileHash(fileName);
		lua_pushlstring(L, hash.c_str(), hash.length());
		return 1;
	}

	static int HTTPReqIdent = 0;

	int luaF_dohttpreq(lua_State* L)
	{
		PD2HOOK_LOG_LOG("Incoming HTTP Request/Request");

		int args = lua_gettop(L);
		int progressReference = 0;
		if (args >= 3)
		{
			progressReference = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		int functionReference = luaL_ref(L, LUA_REGISTRYINDEX);
		size_t len;
		const char* url_c = lua_tolstring(L, 1, &len);
		std::string url = std::string(url_c, len);

		PD2HOOK_LOG_LOG(std::string(url_c, len) << " - " << functionReference);

		lua_http_data* ourData = new lua_http_data();
		ourData->funcRef = functionReference;
		ourData->progressRef = progressReference;
		ourData->L = L;

		HTTPReqIdent++;
		ourData->requestIdentifier = HTTPReqIdent;

		std::unique_ptr<HTTPItem> reqItem(new HTTPItem());
		reqItem->call = return_lua_http;
		reqItem->data = ourData;
		reqItem->url = url;

		if (progressReference != 0)
		{
			reqItem->progress = progress_lua_http;
		}

		HTTPManager::GetSingleton()->LaunchHTTPRequest(std::move(reqItem));
		lua_pushinteger(L, HTTPReqIdent);
		return 1;
	}

	CConsole* gbl_mConsole = NULL;

	int luaF_createconsole(lua_State* L)
	{
		if (gbl_mConsole) return 0;
		gbl_mConsole = new CConsole();
		return 0;
	}

	int luaF_destroyconsole(lua_State* L)
	{
		if (!gbl_mConsole) return 0;
		delete gbl_mConsole;
		gbl_mConsole = NULL;
		return 0;
	}

	int luaF_print(lua_State* L)
	{
		int top = lua_gettop(L);
		std::stringstream stream;
		for (int i = 0; i < top; ++i)
		{
			size_t len;
			const char* str = lua_tolstring(L, i + 1, &len);
			stream << (i > 0 ? "    " : "") << str;
		}
		PD2HOOK_LOG_LUA(stream.str());

		return 0;
	}

	int luaF_moveDirectory(lua_State * L)
	{
		int top = lua_gettop(L);
		size_t lf = 0;
		const char * fromStr = lua_tolstring(L, 1, &lf);
		size_t ld = 0;
		const char * destStr = lua_tolstring(L, 2, &ld);

		bool success = Util::MoveDirectory(fromStr, destStr);
		lua_pushboolean(L, success);
		return 1;
	}

	int updates = 0;
	std::thread::id main_thread_id;

	void* __fastcall do_game_update_new(void* thislol, int edx, int* a, int* b)
	{

		// If someone has a better way of doing this, I'd like to know about it.
		// I could save the this pointer?
		// I'll check if it's even different at all later.
		if (std::this_thread::get_id() != main_thread_id)
		{
			return do_game_update(thislol, a, b);
		}

		lua_State* L = (lua_State*)*((void**)thislol);
		if (updates == 0)
		{
			HTTPManager::GetSingleton()->init_locks();
		}

		if (updates > 1)
		{
			EventQueueMaster::GetSingleton().ProcessEvents();
		}

		updates++;
		return do_game_update(thislol, a, b);
	}

	// Random dude who wrote what's his face?
	// I 'unno, I stole this method from the guy who wrote the 'underground-light-lua-hook'
	// Mine worked fine, but this seems more elegant.
	void* luaL_newstate_new(void* thislol, bool no, bool freakin, int clue)
	{
		void* ret = reinterpret_cast<void*>(luaL_newstate(thislol, no, freakin, clue));

		lua_State* L = (lua_State*)*((void**)thislol);
		printf("Lua State: %p\n", (void*)L);
		if (!L) return ret;

		add_active_state(L);

		lua_pushcclosure(L, luaF_print, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "log");

		lua_pushcclosure(L, luaF_pcall, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "pcall");

		lua_pushcclosure(L, luaF_dofile, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "dofile");

		lua_pushcclosure(L, luaF_unzipfile, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "unzip");

		lua_pushcclosure(L, luaF_dohttpreq, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "dohttpreq");

		luaL_Reg consoleLib[] = {
			{ "CreateConsole", luaF_createconsole },
			{ "DestroyConsole", luaF_destroyconsole },
			{ NULL, NULL }
		};
		luaI_openlib(L, "console", consoleLib, 0);

		luaL_Reg fileLib[] = {
			{ "GetDirectories", luaF_getdir },
			{ "GetFiles", luaF_getfiles },
			{ "RemoveDirectory", luaF_removeDirectory },
			{ "DirectoryExists", luaF_directoryExists },
			{ "DirectoryHash", luaF_directoryhash },
			{ "FileHash", luaF_filehash },
			{ "MoveDirectory", luaF_moveDirectory },
			{ NULL, NULL }
		};
		luaI_openlib(L, "file", fileLib, 0);

		// Keeping everything in lowercase since IspcallForced / IsPCallForced and Forcepcalls / ForcePCalls look rather weird anyway
		luaL_Reg bltLib[] = {
			{ "ispcallforced", luaF_ispcallforced },
			{ "forcepcalls", luaF_forcepcalls },
			{ NULL, NULL }
		};
		luaI_openlib(L, "blt", bltLib, 0);

		int result;
		PD2HOOK_LOG_LOG("Initiating Hook");

		result = luaL_loadfilex(L, "mods/base/base.lua", nullptr);
		if (result == LUA_ERRSYNTAX)
		{
			size_t len;
			PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
			return ret;
		}
		result = lua_pcall(L, 0, 1, 0);
		if (result == LUA_ERRRUN)
		{
			size_t len;
			PD2HOOK_LOG_LOG(lua_tolstring(L, -1, &len));
			return ret;
		}

		return ret;
	}

	void luaF_close(lua_State* L)
	{
		remove_active_state(L);
		lua_close(L);
	}

	FuncDetour* gameUpdateDetour = nullptr;
	FuncDetour* newStateDetour = nullptr;
	FuncDetour* luaCloseDetour = nullptr;

	void InitiateStates(){

		main_thread_id = std::this_thread::get_id();

		if (!InitializeMinHook()){
			printf("Critical Error: MinHook library failed to initialize\n");
		}

		SignatureSearch::Search();

		// Snh20: do_game_update() is extensively inlined so I haven't been able to locate a single independent instance, disabling for now
		//	gameUpdateDetour = new FuncDetour(do_game_update, do_game_update_new);
		// Snh20: This looks crazy, I know, but hear me out - MinHook differs from Microsoft Research Detours in that the detoured
		// function must not be called again at the same address (otherwise Very Bad Things(TM) will happen) since the detour would
		// be recursively calling back to itself. But that's exactly what the trampoline is for - it is used as a way to bypass said
		// detour and call directly into the original function, so clobber the address of the original function (which is now stored
		// in the FuncDetour instance anyway) with the address of the trampoline so the rest of the code does not need to be changed
		// just to account for this difference. Since the FuncDetour instances will only ever be deleted at shutdown, there should
		// not be any issue with dangling pointers
		//	do_game_update = reinterpret_cast<do_game_updateptr>(gameUpdateDetour->GetTrampoline());

		newStateDetour = new FuncDetour(luaL_newstate, luaL_newstate_new);
		luaL_newstate = reinterpret_cast<luaL_newstateptr>(newStateDetour->GetTrampoline());

		//	luaCallDetour = new FuncDetour(lua_call, lua_newcall);
		//	lua_call = reinterpret_cast<lua_callptr>(luaCallDetour->GetTrampoline());

		luaCloseDetour = new FuncDetour(lua_close, luaF_close);
		lua_close = reinterpret_cast<lua_closeptr>(luaCloseDetour->GetTrampoline());

		std::ifstream infile("mods/developer.txt");
		if (infile.good())
		{
			gbl_mConsole = new CConsole();
		}
	}


	void DestroyStates(){
		// Okay... let's not do that.
		// I don't want to keep this in memory, but it CRASHES THE SHIT OUT if you delete this after all is said and done.
		// if (gbl_mConsole) delete gbl_mConsole;

		if (gameUpdateDetour) delete gameUpdateDetour;
		if (newStateDetour) delete newStateDetour;
		if (luaCallDetour) delete luaCallDetour;
		if (luaCloseDetour) delete luaCloseDetour;
		if (gbl_mConsole) delete gbl_mConsole;

		UninitializeMinHook();
	}
}
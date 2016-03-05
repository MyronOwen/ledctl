/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This header file defines the interface that the SQLite library
** presents to client programs.  If a C-function, structure, datatype,
** or constant definition does not appear in this file, then it is
** not a published API of SQLite, is subject to change without
** notice, and should not be referenced by programs that use SQLite.
**
** Some of the definitions that are in this file are marked as
** "experimental".  Experimental interfaces are normally new
** features recently added to SQLite.  We do not anticipate changes
** to experimental interfaces but reserve the right to make minor changes
** if experience from use "in the wild" suggest such changes are prudent.
**
** The official C-language API documentation for SQLite is derived
** from comments in this file.  This file is the authoritative source
** on how SQLite interfaces are suppose to operate.
**
** The name of this file under configuration management is "sqlite.h.in".
** The makefile makes some minor changes to this file (such as inserting
** the version number) and changes its name to "sqlite3.h" as
** part of the build process.
*/
#ifndef _SQLITE3_H_
#define _SQLITE3_H_
#include <stdarg.h>     /* Needed for the definition of va_list */

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
** Add the ability to override 'extern'
*/
#ifndef SQLITE_EXTERN
# define SQLITE_EXTERN extern
#endif

#ifndef SQLITE_API
# define SQLITE_API
#endif


/*
** These no-op macros are used in front of interfaces to mark those
** interfaces as either deprecated or experimental.  New applications
** should not use deprecated interfaces - they are supported for backwards
** compatibility only.  Application writers should be aware that
** experimental interfaces are subject to change in point releases.
**
** These macros used to resolve to various kinds of compiler magic that
** would generate warning messages when they were used.  But that
** compiler magic ended up generating such a flurry of bug reports
** that we have taken it all out and gone back to using simple
** noop macros.
*/
#define SQLITE_DEPRECATED
#define SQLITE_EXPERIMENTAL

/*
** Ensure these symbols were not defined by some previous header file.
*/
#ifdef SQLITE_VERSION
# undef SQLITE_VERSION
#endif
#ifdef SQLITE_VERSION_NUMBER
# undef SQLITE_VERSION_NUMBER
#endif

/*
** CAPI3REF: Compile-Time Library Version Numbers
**
** ^(The [SQLITE_VERSION] C preprocessor macro in the sqlite3.h header
** evaluates to a string literal that is the SQLite version in the
** format "X.Y.Z" where X is the major version number (always 3 for
** SQLite3) and Y is the minor version number and Z is the release number.)^
** ^(The [SQLITE_VERSION_NUMBER] C preprocessor macro resolves to an integer
** with the value (X*1000000 + Y*1000 + Z) where X, Y, and Z are the same
** numbers used in [SQLITE_VERSION].)^
** The SQLITE_VERSION_NUMBER for any given release of SQLite will also
** be larger than the release from which it is derived.  Either Y will
** be held constant and Z will be incremented or else Y will be incremented
** and Z will be reset to zero.
**
** Since version 3.6.18, SQLite source code has been stored in the
** <a href="http://www.fossil-scm.org/">Fossil configuration management
** system</a>.  ^The SQLITE_SOURCE_ID macro evaluates to
** a string which identifies a particular check-in of SQLite
** within its configuration management system.  ^The SQLITE_SOURCE_ID
** string contains the date and time of the check-in (UTC) and an SHA1
** hash of the entire source tree.
**
** See also: [sqlite3_libversion()],
** [sqlite3_libversion_number()], [sqlite3_sourceid()],
** [sqlite_version()] and [sqlite_source_id()].
*/
#define SQLITE_VERSION        "3.8.8"
#define SQLITE_VERSION_NUMBER 3008008
#define SQLITE_SOURCE_ID      "2015-01-16 12:08:06 7d68a42face3ab14ed88407d4331872f5b243fdf"

/*
** CAPI3REF: Run-Time Library Version Numbers
** KEYWORDS: sqlite3_version, sqlite3_sourceid
**
** These interfaces provide the same information as the [SQLITE_VERSION],
** [SQLITE_VERSION_NUMBER], and [SQLITE_SOURCE_ID] C preprocessor macros
** but are associated with the library instead of the header file.  ^(Cautious
** programmers might include assert() statements in their application to
** verify that values returned by these interfaces match the macros in
** the header, and thus insure that the application is
** compiled with matching library and header files.
**
** <blockquote><pre>
** assert( sqlite3_libversion_number()==SQLITE_VERSION_NUMBER );
** assert( strcmp(sqlite3_sourceid(),SQLITE_SOURCE_ID)==0 );
** assert( strcmp(sqlite3_libversion(),SQLITE_VERSION)==0 );
** </pre></blockquote>)^
**
** ^The sqlite3_version[] string constant contains the text of [SQLITE_VERSION]
** macro.  ^The sqlite3_libversion() function returns a pointer to the
** to the sqlite3_version[] string constant.  The sqlite3_libversion()
** function is provided for use in DLLs since DLL users usually do not have
** direct access to string constants within the DLL.  ^The
** sqlite3_libversion_number() function returns an integer equal to
** [SQLITE_VERSION_NUMBER].  ^The sqlite3_sourceid() function returns 
** a pointer to a string constant whose value is the same as the 
** [SQLITE_SOURCE_ID] C preprocessor macro.
**
** See also: [sqlite_version()] and [sqlite_source_id()].
*/
SQLITE_API SQLITE_EXTERN const char sqlite3_version[];
SQLITE_API const char *sqlite3_libversion(void);
SQLITE_API const char *sqlite3_sourceid(void);
SQLITE_API int sqlite3_libversion_number(void);

/*
** CAPI3REF: Run-Time Library Compilation Options Diagnostics
**
** ^The sqlite3_compileoption_used() function returns 0 or 1 
** indicating whether the specified option was defined at 
** compile time.  ^The SQLITE_ prefix may be omitted from the 
** option name passed to sqlite3_compileoption_used().  
**
** ^The sqlite3_compileoption_get() function allows iterating
** over the list of options that were defined at compile time by
** returning the N-th compile time option string.  ^If N is out of range,
** sqlite3_compileoption_get() returns a NULL pointer.  ^The SQLITE_ 
** prefix is omitted from any strings returned by 
** sqlite3_compileoption_get().
**
** ^Support for the diagnostic functions sqlite3_compileoption_used()
** and sqlite3_compileoption_get() may be omitted by specifying the 
** [SQLITE_OMIT_COMPILEOPTION_DIAGS] option at compile time.
**
** See also: SQL functions [sqlite_compileoption_used()] and
** [sqlite_compileoption_get()] and the [compile_options pragma].
*/
#ifndef SQLITE_OMIT_COMPILEOPTION_DIAGS
SQLITE_API int sqlite3_compileoption_used(const char *zOptName);
SQLITE_API const char *sqlite3_compileoption_get(int N);
#endif

/*
** CAPI3REF: Test To See If The Library Is Threadsafe
**
** ^The sqlite3_threadsafe() function returns zero if and only if
** SQLite was compiled with mutexing code omitted due to the
** [SQLITE_THREADSAFE] compile-time option being set to 0.
**
** SQLite can be compiled with or without mutexes.  When
** the [SQLITE_THREADSAFE] C preprocessor macro is 1 or 2, mutexes
** are enabled and SQLite is threadsafe.  When the
** [SQLITE_THREADSAFE] macro is 0, 
** the mutexes are omitted.  Without the mutexes, it is not safe
** to use SQLite concurrently from more than one thread.
**
** Enabling mutexes incurs a measurable performance penalty.
** So if speed is of utmost importance, it makes sense to disable
** the mutexes.  But for maximum safety, mutexes should be enabled.
** ^The default behavior is for mutexes to be enabled.
**
** This interface can be used by an application to make sure that the
** version of SQLite that it is linking against was compiled with
** the desired setting of the [SQLITE_THREADSAFE] macro.
**
** This interface only reports on the compile-time mutex setting
** of the [SQLITE_THREADSAFE] flag.  If SQLite is compiled with
** SQLITE_THREADSAFE=1 or =2 then mutexes are enabled by default but
** can be fully or partially disabled using a call to [sqlite3_config()]
** with the verbs [SQLITE_CONFIG_SINGLETHREAD], [SQLITE_CONFIG_MULTITHREAD],
** or [SQLITE_CONFIG_SERIALIZED].  ^(The return value of the
** sqlite3_threadsafe() function shows only the compile-time setting of
** thread safety, not any run-time changes to that setting made by
** sqlite3_config(). In other words, the return value from sqlite3_threadsafe()
** is unchanged by calls to sqlite3_config().)^
**
** See the [threading mode] documentation for additional information.
*/
SQLITE_API int sqlite3_threadsafe(void);

/*
** CAPI3REF: Database Connection Handle
** KEYWORDS: {database connection} {database connections}
**
** Each open SQLite database is represented by a pointer to an instance of
** the opaque structure named "sqlite3".  It is useful to think of an sqlite3
** pointer as an object.  The [sqlite3_open()], [sqlite3_open16()], and
** [sqlite3_open_v2()] interfaces are its constructors, and [sqlite3_close()]
** and [sqlite3_close_v2()] are its destructors.  There are many other
** interfaces (such as
** [sqlite3_prepare_v2()], [sqlite3_create_function()], and
** [sqlite3_busy_timeout()] to name but three) that are methods on an
** sqlite3 object.
*/
typedef struct sqlite3 sqlite3;

/*
** CAPI3REF: 64-Bit Integer Types
** KEYWORDS: sqlite_int64 sqlite_uint64
**
** Because there is no cross-platform way to specify 64-bit integer types
** SQLite includes typedefs for 64-bit signed and unsigned integers.
**
** The sqlite3_int64 and sqlite3_uint64 are the preferred type definitions.
** The sqlite_int64 and sqlite_uint64 types are supported for backwards
** compatibility only.
**
** ^The sqlite3_int64 and sqlite_int64 types can store integer values
** between -9223372036854775808 and +9223372036854775807 inclusive.  ^The
** sqlite3_uint64 and sqlite_uint64 types can store integer values 
** between 0 and +18446744073709551615 inclusive.
*/
#ifdef SQLITE_INT64_TYPE
  typedef SQLITE_INT64_TYPE sqlite_int64;
  typedef unsigned SQLITE_INT64_TYPE sqlite_uint64;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 sqlite_int64;
  typedef unsigned __int64 sqlite_uint64;
#else
  typedef long long int sqlite_int64;
  typedef unsigned long long int sqlite_uint64;
#endif
typedef sqlite_int64 sqlite3_int64;
typedef sqlite_uint64 sqlite3_uint64;

/*
** If compiling for a processor that lacks floating point support,
** substitute integer for floating-point.
*/
#ifdef SQLITE_OMIT_FLOATING_POINT
# define double sqlite3_int64
#endif

/*
** CAPI3REF: Closing A Database Connection
**
** ^The sqlite3_close() and sqlite3_close_v2() routines are destructors
** for the [sqlite3] object.
** ^Calls to sqlite3_close() and sqlite3_close_v2() return [SQLITE_OK] if
** the [sqlite3] object is successfully destroyed and all associated
** resources are deallocated.
**
** ^If the database connection is associated with unfinalized prepared
** statements or unfinished sqlite3_backup objects then sqlite3_close()
** will leave the database connection open and return [SQLITE_BUSY].
** ^If sqlite3_close_v2() is called with unfinalized prepared statements
** and/or unfinished sqlite3_backups, then the database connection becomes
** an unusable "zombie" which will automatically be deallocated when the
** last prepared statement is finalized or the last sqlite3_backup is
** finished.  The sqlite3_close_v2() interface is intended for use with
** host languages that are garbage collected, and where the order in which
** destructors are called is arbitrary.
**
** Applications should [sqlite3_finalize | finalize] all [prepared statements],
** [sqlite3_blob_close | close] all [BLOB handles], and 
** [sqlite3_backup_finish | finish] all [sqlite3_backup] objects associated
** with the [sqlite3] object prior to attempting to close the object.  ^If
** sqlite3_close_v2() is called on a [database connection] that still has
** outstanding [prepared statements], [BLOB handles], and/or
** [sqlite3_backup] objects then it returns [SQLITE_OK] and the deallocation
** of resources is deferred until all [prepared statements], [BLOB handles],
** and [sqlite3_backup] objects are also destroyed.
**
** ^If an [sqlite3] object is destroyed while a transaction is open,
** the transaction is automatically rolled back.
**
** The C parameter to [sqlite3_close(C)] and [sqlite3_close_v2(C)]
** must be either a NULL
** pointer or an [sqlite3] object pointer obtained
** from [sqlite3_open()], [sqlite3_open16()], or
** [sqlite3_open_v2()], and not previously closed.
** ^Calling sqlite3_close() or sqlite3_close_v2() with a NULL pointer
** argument is a harmless no-op.
*/
SQLITE_API int sqlite3_close(sqlite3*);
SQLITE_API int sqlite3_close_v2(sqlite3*);

/*
** The type for a callback function.
** This is legacy and deprecated.  It is included for historical
** compatibility and is not documented.
*/
typedef int (*sqlite3_callback)(void*,int,char**, char**);

/*
** CAPI3REF: One-Step Query Execution Interface
**
** The sqlite3_exec() interface is a convenience wrapper around
** [sqlite3_prepare_v2()], [sqlite3_step()], and [sqlite3_finalize()],
** that allows an application to run multiple statements of SQL
** without having to use a lot of C code. 
**
** ^The sqlite3_exec() interface runs zero or more UTF-8 encoded,
** semicolon-separate SQL statements passed into its 2nd argument,
** in the context of the [database connection] passed in as its 1st
** argument.  ^If the callback function of the 3rd argument to
** sqlite3_exec() is not NULL, then it is invoked for each result row
** coming out of the evaluated SQL statements.  ^The 4th argument to
** sqlite3_exec() is relayed through to the 1st argument of each
** callback invocation.  ^If the callback pointer to sqlite3_exec()
** is NULL, then no callback is ever invoked and result rows are
** ignored.
**
** ^If an error occurs while evaluating the SQL statements passed into
** sqlite3_exec(), then execution of the current statement stops and
** subsequent statements are skipped.  ^If the 5th parameter to sqlite3_exec()
** is not NULL then any error message is written into memory obtained
** from [sqlite3_malloc()] and passed back through the 5th parameter.
** To avoid memory leaks, the application should invoke [sqlite3_free()]
** on error message strings returned through the 5th parameter of
** of sqlite3_exec() after the error message string is no longer needed.
** ^If the 5th parameter to sqlite3_exec() is not NULL and no errors
** occur, then sqlite3_exec() sets the pointer in its 5th parameter to
** NULL before returning.
**
** ^If an sqlite3_exec() callback returns non-zero, the sqlite3_exec()
** routine returns SQLITE_ABORT without invoking the callback again and
** without running any subsequent SQL statements.
**
** ^The 2nd argument to the sqlite3_exec() callback function is the
** number of columns in the result.  ^The 3rd argument to the sqlite3_exec()
** callback is an array of pointers to strings obtained as if from
** [sqlite3_column_text()], one for each column.  ^If an element of a
** result row is NULL then the corresponding string pointer for the
** sqlite3_exec() callback is a NULL pointer.  ^The 4th argument to the
** sqlite3_exec() callback is an array of pointers to strings where each
** entry represents the name of corresponding result column as obtained
** from [sqlite3_column_name()].
**
** ^If the 2nd parameter to sqlite3_exec() is a NULL pointer, a pointer
** to an empty string, or a pointer that contains only whitespace and/or 
** SQL comments, then no SQL statements are evaluated and the database
** is not changed.
**
** Restrictions:
**
** <ul>
** <li> The application must insure that the 1st parameter to sqlite3_exec()
**      is a valid and open [database connection].
** <li> The application must not close the [database connection] specified by
**      the 1st parameter to sqlite3_exec() while sqlite3_exec() is running.
** <li> The application must not modify the SQL statement text passed into
**      the 2nd parameter of sqlite3_exec() while sqlite3_exec() is running.
** </ul>
*/
SQLITE_API int sqlite3_exec(
  sqlite3*,                                  /* An open database */
  const char *sql,                           /* SQL to be evaluated */
  int (*callback)(void*,int,char**,char**),  /* Callback function */
  void *,                                    /* 1st argument to callback */
  char **errmsg                              /* Error msg written here */
);

/*
** CAPI3REF: Result Codes
** KEYWORDS: {result code definitions}
**
** Many SQLite functions return an integer result code from the set shown
** here in order to indicate success or failure.
**
** New error codes may be added in future versions of SQLite.
**
** See also: [extended result code definitions]
*/
#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Auxiliary database format error */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
#define SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */
/* end-of-error-codes */

/*
** CAPI3REF: Extended Result Codes
** KEYWORDS: {extended result code definitions}
**
** In its default configuration, SQLite API routines return one of 30 integer
** [result codes].  However, experience has shown that many of
** these result codes are too coarse-grained.  They do not provide as
** much information about problems as programmers might like.  In an effort to
** address this, newer versions of SQLite (version 3.3.8 and later) include
** support for additional result codes that provide more detailed information
** about errors. These [extended result codes] are enabled or disabled
** on a per database connection basis using the
** [sqlite3_extended_result_codes()] API.  Or, the extended code for
** the most recent error can be obtained using
** [sqlite3_extended_errcode()].
*/
#define SQLITE_IOERR_READ              (SQLITE_IOERR | (1<<8))
#define SQLITE_IOERR_SHORT_READ        (SQLITE_IOERR | (2<<8))
#define SQLITE_IOERR_WRITE             (SQLITE_IOERR | (3<<8))
#define SQLITE_IOERR_FSYNC             (SQLITE_IOERR | (4<<8))
#define SQLITE_IOERR_DIR_FSYNC         (SQLITE_IOERR | (5<<8))
#define SQLITE_IOERR_TRUNCATE          (SQLITE_IOERR | (6<<8))
#define SQLITE_IOERR_FSTAT             (SQLITE_IOERR | (7<<8))
#define SQLITE_IOERR_UNLOCK            (SQLITE_IOERR | (8<<8))
#define SQLITE_IOERR_RDLOCK            (SQLITE_IOERR | (9<<8))
#define SQLITE_IOERR_DELETE            (SQLITE_IOERR | (10<<8))
#define SQLITE_IOERR_BLOCKED           (SQLITE_IOERR | (11<<8))
#define SQLITE_IOERR_NOMEM             (SQLITE_IOERR | (12<<8))
#define SQLITE_IOERR_ACCESS            (SQLITE_IOERR | (13<<8))
#define SQLITE_IOERR_CHECKRESERVEDLOCK (SQLITE_IOERR | (14<<8))
#define SQLITE_IOERR_LOCK              (SQLITE_IOERR | (15<<8))
#define SQLITE_IOERR_CLOSE             (SQLITE_IOERR | (16<<8))
#define SQLITE_IOERR_DIR_CLOSE         (SQLITE_IOERR | (17<<8))
#define SQLITE_IOERR_SHMOPEN           (SQLITE_IOERR | (18<<8))
#define SQLITE_IOERR_SHMSIZE           (SQLITE_IOERR | (19<<8))
#define SQLITE_IOERR_SHMLOCK           (SQLITE_IOERR | (20<<8))
#define SQLITE_IOERR_SHMMAP            (SQLITE_IOERR | (21<<8))
#define SQLITE_IOERR_SEEK              (SQLITE_IOERR | (22<<8))
#define SQLITE_IOERR_DELETE_NOENT      (SQLITE_IOERR | (23<<8))
#define SQLITE_IOERR_MMAP              (SQLITE_IOERR | (24<<8))
#define SQLITE_IOERR_GETTEMPPATH       (SQLITE_IOERR | (25<<8))
#define SQLITE_IOERR_CONVPATH          (SQLITE_IOERR | (26<<8))
#define SQLITE_LOCKED_SHAREDCACHE      (SQLITE_LOCKED |  (1<<8))
#define SQLITE_BUSY_RECOVERY           (SQLITE_BUSY   |  (1<<8))
#define SQLITE_BUSY_SNAPSHOT           (SQLITE_BUSY   |  (2<<8))
#define SQLITE_CANTOPEN_NOTEMPDIR      (SQLITE_CANTOPEN | (1<<8))
#define SQLITE_CANTOPEN_ISDIR          (SQLITE_CANTOPEN | (2<<8))
#define SQLITE_CANTOPEN_FULLPATH       (SQLITE_CANTOPEN | (3<<8))
#define SQLITE_CANTOPEN_CONVPATH       (SQLITE_CANTOPEN | (4<<8))
#define SQLITE_CORRUPT_VTAB            (SQLITE_CORRUPT | (1<<8))
#define SQLITE_READONLY_RECOVERY       (SQLITE_READONLY | (1<<8))
#define SQLITE_READONLY_CANTLOCK       (SQLITE_READONLY | (2<<8))
#define SQLITE_READONLY_ROLLBACK       (SQLITE_READONLY | (3<<8))
#define SQLITE_READONLY_DBMOVED        (SQLITE_READONLY | (4<<8))
#define SQLITE_ABORT_ROLLBACK          (SQLITE_ABORT | (2<<8))
#define SQLITE_CONSTRAINT_CHECK        (SQLITE_CONSTRAINT | (1<<8))
#define SQLITE_CONSTRAINT_COMMITHOOK   (SQLITE_CONSTRAINT | (2<<8))
#define SQLITE_CONSTRAINT_FOREIGNKEY   (SQLITE_CONSTRAINT | (3<<8))
#define SQLITE_CONSTRAINT_FUNCTION     (SQLITE_CONSTRAINT | (4<<8))
#define SQLITE_CONSTRAINT_NOTNULL      (SQLITE_CONSTRAINT | (5<<8))
#define SQLITE_CONSTRAINT_PRIMARYKEY   (SQLITE_CONSTRAINT | (6<<8))
#define SQLITE_CONSTRAINT_TRIGGER      (SQLITE_CONSTRAINT | (7<<8))
#define SQLITE_CONSTRAINT_UNIQUE       (SQLITE_CONSTRAINT | (8<<8))
#define SQLITE_CONSTRAINT_VTAB         (SQLITE_CONSTRAINT | (9<<8))
#define SQLITE_CONSTRAINT_ROWID        (SQLITE_CONSTRAINT |(10<<8))
#define SQLITE_NOTICE_RECOVER_WAL      (SQLITE_NOTICE | (1<<8))
#define SQLITE_NOTICE_RECOVER_ROLLBACK (SQLITE_NOTICE | (2<<8))
#define SQLITE_WARNING_AUTOINDEX       (SQLITE_WARNING | (1<<8))
#define SQLITE_AUTH_USER               (SQLITE_AUTH | (1<<8))

/*
** CAPI3REF: Flags For File Open Operations
**
** These bit values are intended for use in the
** 3rd parameter to the [sqlite3_open_v2()] interface and
** in the 4th parameter to the [sqlite3_vfs.xOpen] method.
*/
#define SQLITE_OPEN_READONLY         0x00000001  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_READWRITE        0x00000002  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_CREATE           0x00000004  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_DELETEONCLOSE    0x00000008  /* VFS only */
#define SQLITE_OPEN_EXCLUSIVE        0x00000010  /* VFS only */
#define SQLITE_OPEN_AUTOPROXY        0x00000020  /* VFS only */
#define SQLITE_OPEN_URI              0x00000040  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_MEMORY           0x00000080  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_MAIN_DB          0x00000100  /* VFS only */
#define SQLITE_OPEN_TEMP_DB          0x00000200  /* VFS only */
#define SQLITE_OPEN_TRANSIENT_DB     0x00000400  /* VFS only */
#define SQLITE_OPEN_MAIN_JOURNAL     0x00000800  /* VFS only */
#define SQLITE_OPEN_TEMP_JOURNAL     0x00001000  /* VFS only */
#define SQLITE_OPEN_SUBJOURNAL       0x00002000  /* VFS only */
#define SQLITE_OPEN_MASTER_JOURNAL   0x00004000  /* VFS only */
#define SQLITE_OPEN_NOMUTEX          0x00008000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_FULLMUTEX        0x00010000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_SHAREDCACHE      0x00020000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_PRIVATECACHE     0x00040000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_WAL              0x00080000  /* VFS only */

/* Reserved:                         0x00F00000 */

/*
** CAPI3REF: Device Characteristics
**
** The xDeviceCharacteristics method of the [sqlite3_io_methods]
** object returns an integer which is a vector of these
** bit values expressing I/O characteristics of the mass storage
** device that holds the file that the [sqlite3_io_methods]
** refers to.
**
** The SQLITE_IOCAP_ATOMIC property means that all writes of
** any size are atomic.  The SQLITE_IOCAP_ATOMICnnn values
** mean that writes of blocks that are nnn bytes in size and
** are aligned to an address which is an integer multiple of
** nnn are atomic.  The SQLITE_IOCAP_SAFE_APPEND value means
** that when data is appended to a file, the data is appended
** first then the size of the file is extended, never the other
** way around.  The SQLITE_IOCAP_SEQUENTIAL property means that
** information is written to disk in the same order as calls
** to xWrite().  The SQLITE_IOCAP_POWERSAFE_OVERWRITE property means that
** after reboot following a crash or power loss, the only bytes in a
** file that were written at the application level might have changed
** and that adjacent bytes, even bytes within the same sector are
** guaranteed to be unchanged.  The SQLITE_IOCAP_UNDELETABLE_WHEN_OPEN
** flag indicate that a file cannot be deleted when open.  The
** SQLITE_IOCAP_IMMUTABLE flag indicates that the file is on
** read-only media and cannot be changed even by processes with
** elevated privileges.
*/
#define SQLITE_IOCAP_ATOMIC                 0x00000001
#define SQLITE_IOCAP_ATOMIC512              0x00000002
#define SQLITE_IOCAP_ATOMIC1K               0x00000004
#define SQLITE_IOCAP_ATOMIC2K               0x00000008
#define SQLITE_IOCAP_ATOMIC4K               0x00000010
#define SQLITE_IOCAP_ATOMIC8K               0x00000020
#define SQLITE_IOCAP_ATOMIC16K              0x00000040
#define SQLITE_IOCAP_ATOMIC32K              0x00000080
#define SQLITE_IOCAP_ATOMIC64K              0x00000100
#define SQLITE_IOCAP_SAFE_APPEND            0x00000200
#define SQLITE_IOCAP_SEQUENTIAL             0x00000400
#define SQLITE_IOCAP_UNDELETABLE_WHEN_OPEN  0x00000800
#define SQLITE_IOCAP_POWERSAFE_OVERWRITE    0x00001000
#define SQLITE_IOCAP_IMMUTABLE              0x00002000

/*
** CAPI3REF: File Locking Levels
**
** SQLite uses one of these integer values as the second
** argument to calls it makes to the xLock() and xUnlock() methods
** of an [sqlite3_io_methods] object.
*/
#define SQLITE_LOCK_NONE          0
#define SQLITE_LOCK_SHARED        1
#define SQLITE_LOCK_RESERVED      2
#define SQLITE_LOCK_PENDING       3
#define SQLITE_LOCK_EXCLUSIVE     4

/*
** CAPI3REF: Synchronization Type Flags
**
** When SQLite invokes the xSync() method of an
** [sqlite3_io_methods] object it uses a combination of
** these integer values as the second argument.
**
** When the SQLITE_SYNC_DATAONLY flag is used, it means that the
** sync operation only needs to flush data to mass storage.  Inode
** information need not be flushed. If the lower four bits of the flag
** equal SQLITE_SYNC_NORMAL, that means to use normal fsync() semantics.
** If the lower four bits equal SQLITE_SYNC_FULL, that means
** to use Mac OS X style fullsync instead of fsync().
**
** Do not confuse the SQLITE_SYNC_NORMAL and SQLITE_SYNC_FULL flags
** with the [PRAGMA synchronous]=NORMAL and [PRAGMA synchronous]=FULL
** settings.  The [synchronous pragma] determines when calls to the
** xSync VFS method occur and applies uniformly across all platforms.
** The SQLITE_SYNC_NORMAL and SQLITE_SYNC_FULL flags determine how
** energetic or rigorous or forceful the sync operations are and
** only make a difference on Mac OSX for the default SQLite code.
** (Third-party VFS implementations might also make the distinction
** between SQLITE_SYNC_NORMAL and SQLITE_SYNC_FULL, but among the
** operating systems natively supported by SQLite, only Mac OSX
** cares about the difference.)
*/
#define SQLITE_SYNC_NORMAL        0x00002
#define SQLITE_SYNC_FULL          0x00003
#define SQLITE_SYNC_DATAONLY      0x00010

/*
** CAPI3REF: OS Interface Open File Handle
**
** An [sqlite3_file] object represents an open file in the 
** [sqlite3_vfs | OS interface layer].  Individual OS interface
** implementations will
** want to subclass this object by appending additional fields
** for their own use.  The pMethods entry is a pointer to an
** [sqlite3_io_methods] object that defines methods for performing
** I/O operations on the open file.
*/
typedef struct sqlite3_file sqlite3_file;
struct sqlite3_file {
  const struct sqlite3_io_methods *pMethods;  /* Methods for an open file */
};

/*
** CAPI3REF: OS Interface File Virtual Methods Object
**
** Every file opened by the [sqlite3_vfs.xOpen] method populates an
** [sqlite3_file] object (or, more commonly, a subclass of the
** [sqlite3_file] object) with a pointer to an instance of this object.
** This object defines the methods used to perform various operations
** against the open file represented by the [sqlite3_file] object.
**
** If the [sqlite3_vfs.xOpen] method sets the sqlite3_file.pMethods element 
** to a non-NULL pointer, then the sqlite3_io_methods.xClose method
** may be invoked even if the [sqlite3_vfs.xOpen] reported that it failed.  The
** only way to prevent a call to xClose following a failed [sqlite3_vfs.xOpen]
** is for the [sqlite3_vfs.xOpen] to set the sqlite3_file.pMethods element
** to NULL.
**
** The flags argument to xSync may be one of [SQLITE_SYNC_NORMAL] or
** [SQLITE_SYNC_FULL].  The first choice is the normal fsync().
** The second choice is a Mac OS X style fullsync.  The [SQLITE_SYNC_DATAONLY]
** flag may be ORed in to indicate that only the data of the file
** and not its inode needs to be synced.
**
** The integer values to xLock() and xUnlock() are one of
** <ul>
** <li> [SQLITE_LOCK_NONE],
** <li> [SQLITE_LOCK_SHARED],
** <li> [SQLITE_LOCK_RESERVED],
** <li> [SQLITE_LOCK_PENDING], or
** <li> [SQLITE_LOCK_EXCLUSIVE].
** </ul>
** xLock() increases the lock. xUnlock() decreases the lock.
** The xCheckReservedLock() method checks whether any database connection,
** either in this process or in some other process, is holding a RESERVED,
** PENDING, or EXCLUSIVE lock on the file.  It returns true
** if such a lock exists and false otherwise.
**
** The xFileControl() method is a generic interface that allows custom
** VFS implementations to directly control an open file using the
** [sqlite3_file_control()] interface.  The second "op" argument is an
** integer opcode.  The third argument is a generic pointer intended to
** point to a structure that may contain arguments or space in which to
** write return values.  Potential uses for xFileControl() might be
** functions to enable blocking locks with timeouts, to change the
** locking strategy (for example to use dot-file locks), to inquire
** about the status of a lock, or to break stale locks.  The SQLite
** core reserves all opcodes less than 100 for its own use.
** A [file control opcodes | list of opcodes] less than 100 is available.
** Applications that define a custom xFileControl method should use opcodes
** greater than 100 to avoid conflicts.  VFS implementations should
** return [SQLITE_NOTFOUND] for file control opcodes that they do not
** recognize.
**
** The xSectorSize() method returns the sector size of the
** device that underlies the file.  The sector size is the
** minimum write that can be performed without disturbing
** other bytes in the file.  The xDeviceCharacteristics()
** method returns a bit vector describing behaviors of the
** underlying device:
**
** <ul>
** <li> [SQLITE_IOCAP_ATOMIC]
** <li> [SQLITE_IOCAP_ATOMIC512]
** <li> [SQLITE_IOCAP_ATOMIC1K]
** <li> [SQLITE_IOCAP_ATOMIC2K]
** <li> [SQLITE_IOCAP_ATOMIC4K]
** <li> [SQLITE_IOCAP_ATOMIC8K]
** <li> [SQLITE_IOCAP_ATOMIC16K]
** <li> [SQLITE_IOCAP_ATOMIC32K]
** <li> [SQLITE_IOCAP_ATOMIC64K]
** <li> [SQLITE_IOCAP_SAFE_APPEND]
** <li> [SQLITE_IOCAP_SEQUENTIAL]
** </ul>
**
** The SQLITE_IOCAP_ATOMIC property means that all writes of
** any size are atomic.  The SQLITE_IOCAP_ATOMICnnn values
** mean that writes of blocks that are nnn bytes in size and
** are aligned to an address which is an integer multiple of
** nnn are atomic.  The SQLITE_IOCAP_SAFE_APPEND value means
** that when data is appended to a file, the data is appended
** first then the size of the file is extended, never the other
** way around.  The SQLITE_IOCAP_SEQUENTIAL property means that
** information is written to disk in the same order as calls
** to xWrite().
**
** If xRead() returns SQLITE_IOERR_SHORT_READ it must also fill
** in the unread portions of the buffer with zeros.  A VFS that
** fails to zero-fill short reads might seem to work.  However,
** failure to zero-fill short reads will eventually lead to
** database corruption.
*/
typedef struct sqlite3_io_methods sqlite3_io_methods;
struct sqlite3_io_methods {
  int iVersion;
  int (*xClose)(sqlite3_file*);
  int (*xRead)(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
  int (*xWrite)(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
  int (*xTruncate)(sqlite3_file*, sqlite3_int64 size);
  int (*xSync)(sqlite3_file*, int flags);
  int (*xFileSize)(sqlite3_file*, sqlite3_int64 *pSize);
  int (*xLock)(sqlite3_file*, int);
  int (*xUnlock)(sqlite3_file*, int);
  int (*xCheckReservedLock)(sqlite3_file*, int *pResOut);
  int (*xFileControl)(sqlite3_file*, int op, void *pArg);
  int (*xSectorSize)(sqlite3_file*);
  int (*xDeviceCharacteristics)(sqlite3_file*);
  /* Methods above are valid for version 1 */
  int (*xShmMap)(sqlite3_file*, int iPg, int pgsz, int, void volatile**);
  int (*xShmLock)(sqlite3_file*, int offset, int n, int flags);
  void (*xShmBarrier)(sqlite3_file*);
  int (*xShmUnmap)(sqlite3_file*, int deleteFlag);
  /* Methods above are valid for version 2 */
  int (*xFetch)(sqlite3_file*, sqlite3_int64 iOfst, int iAmt, void **pp);
  int (*xUnfetch)(sqlite3_file*, sqlite3_int64 iOfst, void *p);
  /* Methods above are valid for version 3 */
  /* Additional methods may be added in future releases */
};

/*
** CAPI3REF: Standard File Control Opcodes
** KEYWORDS: {file control opcodes} {file control opcode}
**
** These integer constants are opcodes for the xFileControl method
** of the [sqlite3_io_methods] object and for the [sqlite3_file_control()]
** interface.
**
** The [SQLITE_FCNTL_LOCKSTATE] opcode is used for debugging.  This
** opcode causes the xFileControl method to write the current state of
** the lock (one of [SQLITE_LOCK_NONE], [SQLITE_LOCK_SHARED],
** [SQLITE_LOCK_RESERVED], [SQLITE_LOCK_PENDING], or [SQLITE_LOCK_EXCLUSIVE])
** into an integer that the pArg argument points to. This capability
** is used during testing and only needs to be supported when SQLITE_TEST
** is defined.
** <ul>
** <li>[[SQLITE_FCNTL_SIZE_HINT]]
** The [SQLITE_FCNTL_SIZE_HINT] opcode is used by SQLite to give the VFS
** layer a hint of how large the database file will grow to be during the
** current transaction.  This hint is not guaranteed to be accurate but it
** is often close.  The underlying VFS might choose to preallocate database
** file space based on this hint in order to help writes to the database
** file run faster.
**
** <li>[[SQLITE_FCNTL_CHUNK_SIZE]]
** The [SQLITE_FCNTL_CHUNK_SIZE] opcode is used to request that the VFS
** extends and truncates the database file in chunks of a size specified
** by the user. The fourth argument to [sqlite3_file_control()] should 
** point to an integer (type int) containing the new chunk-size to use
** for the nominated database. Allocating database file space in large
** chunks (say 1MB at a time), may reduce file-system fragmentation and
** improve performance on some systems.
**
** <li>[[SQLITE_FCNTL_FILE_POINTER]]
** The [SQLITE_FCNTL_FILE_POINTER] opcode is used to obtain a pointer
** to the [sqlite3_file] object associated with a particular database
** connection.  See the [sqlite3_file_control()] documentation for
** additional information.
**
** <li>[[SQLITE_FCNTL_SYNC_OMITTED]]
** No longer in use.
**
** <li>[[SQLITE_FCNTL_SYNC]]
** The [SQLITE_FCNTL_SYNC] opcode is generated internally by SQLite and
** sent to the VFS immediately before the xSync method is invoked on a
** database file descriptor. Or, if the xSync method is not invoked 
** because the user has configured SQLite with 
** [PRAGMA synchronous | PRAGMA synchronous=OFF] it is invoked in place 
** of the xSync method. In most cases, the pointer argument passed with
** this file-control is NULL. However, if the database file is being synced
** as part of a multi-database commit, the argument points to a nul-terminated
** string containing the transactions master-journal file name. VFSes that 
** do not need this signal should silently ignore this opcode. Applications 
** should not call [sqlite3_file_control()] with this opcode as doing so may 
** disrupt the operation of the specialized VFSes that do require it.  
**
** <li>[[SQLITE_FCNTL_COMMIT_PHASETWO]]
** The [SQLITE_FCNTL_COMMIT_PHASETWO] opcode is generated internally by SQLite
** and sent to the VFS after a transaction has been committed immediately
** but before the database is unlocked. VFSes that do not need this signal
** should silently ignore this opcode. Applications should not call
** [sqlite3_file_control()] with this opcode as doing so may disrupt the 
** operation of the specialized VFSes that do require it.  
**
** <li>[[SQLITE_FCNTL_WIN32_AV_RETRY]]
** ^The [SQLITE_FCNTL_WIN32_AV_RETRY] opcode is used to configure automatic
** retry counts and intervals for certain disk I/O operations for the
** windows [VFS] in order to provide robustness in the presence of
** anti-virus programs.  By default, the windows VFS will retry file read,
** file write, and file delete operations up to 10 times, with a delay
** of 25 milliseconds before the first retry and with the delay increasing
** by an additional 25 milliseconds with each subsequent retry.  This
** opcode allows these two values (10 retries and 25 milliseconds of delay)
** to be adjusted.  The values are changed for all database connections
** within the same process.  The argument is a pointer to an array of two
** integers where the first integer i the new retry count and the second
** integer is the delay.  If either integer is negative, then the setting
** is not changed but instead the prior value of that setting is written
** into the array entry, allowing the current retry settings to be
** interrogated.  The zDbName parameter is ignored.
**
** <li>[[SQLITE_FCNTL_PERSIST_WAL]]
** ^The [SQLITE_FCNTL_PERSIST_WAL] opcode is used to set or query the
** persistent [WAL | Write Ahead Log] setting.  By default, the auxiliary
** write ahead log and shared memory files used for transaction control
** are automatically deleted when the latest connection to the database
** closes.  Setting persistent WAL mode causes those files to persist after
** close.  Persisting the files is useful when other processes that do not
** have write permission on the directory containing the database file want
** to read the database file, as the WAL and shared memory files must exist
** in order for the database to be readable.  The fourth parameter to
** [sqlite3_file_control()] for this opcode should be a pointer to an integer.
** That integer is 0 to disable persistent WAL mode or 1 to enable persistent
** WAL mode.  If the integer is -1, then it is overwritten with the current
** WAL persistence setting.
**
** <li>[[SQLITE_FCNTL_POWERSAFE_OVERWRITE]]
** ^The [SQLITE_FCNTL_POWERSAFE_OVERWRITE] opcode is used to set or query the
** persistent "powersafe-overwrite" or "PSOW" setting.  The PSOW setting
** determines the [SQLITE_IOCAP_POWERSAFE_OVERWRITE] bit of the
** xDeviceCharacteristics methods. The fourth parameter to
** [sqlite3_file_control()] for this opcode should be a pointer to an integer.
** That integer is 0 to disable zero-damage mode or 1 to enable zero-damage
** mode.  If the integer is -1, then it is overwritten with the current
** zero-damage mode setting.
**
** <li>[[SQLITE_FCNTL_OVERWRITE]]
** ^The [SQLITE_FCNTL_OVERWRITE] opcode is invoked by SQLite after opening
** a write transaction to indicate that, unless it is rolled back for some
** reason, the entire database file will be overwritten by the current 
** transaction. This is used by VACUUM operations.
**
** <li>[[SQLITE_FCNTL_VFSNAME]]
** ^The [SQLITE_FCNTL_VFSNAME] opcode can be used to obtain the names of
** all [VFSes] in the VFS stack.  The names are of all VFS shims and the
** final bottom-level VFS are written into memory obtained from 
** [sqlite3_malloc()] and the result is stored in the char* variable
** that the fourth parameter of [sqlite3_file_control()] points to.
** The caller is responsible for freeing the memory when done.  As with
** all file-control actions, there is no guarantee that this will actually
** do anything.  Callers should initialize the char* variable to a NULL
** pointer in case this file-control is not implemented.  This file-control
** is intended for diagnostic use only.
**
** <li>[[SQLITE_FCNTL_PRAGMA]]
** ^Whenever a [PRAGMA] statement is parsed, an [SQLITE_FCNTL_PRAGMA] 
** file control is sent to the open [sqlite3_file] object corresponding
** to the database file to which the pragma statement refers. ^The argument
** to the [SQLITE_FCNTL_PRAGMA] file control is an array of
** pointers to strings (char**) in which the second element of the array
** is the name of the pragma and the third element is the argument to the
** pragma or NULL if the pragma has no argument.  ^The handler for an
** [SQLITE_FCNTL_PRAGMA] file control can optionally make the first element
** of the char** argument point to a string obtained from [sqlite3_mprintf()]
** or the equivalent and that string will become the result of the pragma or
** the error message if the pragma fails. ^If the
** [SQLITE_FCNTL_PRAGMA] file control returns [SQLITE_NOTFOUND], then normal 
** [PRAGMA] processing continues.  ^If the [SQLITE_FCNTL_PRAGMA]
** file control returns [SQLITE_OK], then the parser assumes that the
** VFS has handled the PRAGMA itself and the parser generates a no-op
** prepared statement.  ^If the [SQLITE_FCNTL_PRAGMA] file control returns
** any result code other than [SQLITE_OK] or [SQLITE_NOTFOUND], that means
** that the VFS encountered an error while handling the [PRAGMA] and the
** compilation of the PRAGMA fails with an error.  ^The [SQLITE_FCNTL_PRAGMA]
** file control occurs at the beginning of pragma statement analysis and so
** it is able to override built-in [PRAGMA] statements.
**
** <li>[[SQLITE_FCNTL_BUSYHANDLER]]
** ^The [SQLITE_FCNTL_BUSYHANDLER]
** file-control may be invoked by SQLite on the database file handle
** shortly after it is opened in order to provide a custom VFS with access
** to the connections busy-handler callback. The argument is of type (void **)
** - an array of two (void *) values. The first (void *) actually points
** to a function of type (int (*)(void *)). In order to invoke the connections
** busy-handler, this function should be invoked with the second (void *) in
** the array as the only argument. If it returns non-zero, then the operation
** should be retried. If it returns zero, the custom VFS should abandon the
** current operation.
**
** <li>[[SQLITE_FCNTL_TEMPFILENAME]]
** ^Application can invoke the [SQLITE_FCNTL_TEMPFILENAME] file-control
** to have SQLite generate a
** temporary filename using the same algorithm that is followed to generate
** temporary filenames for TEMP tables and other internal uses.  The
** argument should be a char** which will be filled with the filename
** written into memory obtained from [sqlite3_malloc()].  The caller should
** invoke [sqlite3_free()] on the result to avoid a memory leak.
**
** <li>[[SQLITE_FCNTL_MMAP_SIZE]]
** The [SQLITE_FCNTL_MMAP_SIZE] file control is used to query or set the
** maximum number of bytes that will be used for memory-mapped I/O.
** The argument is a pointer to a value of type sqlite3_int64 that
** is an advisory maximum number of bytes in the file to memory map.  The
** pointer is overwritten with the old value.  The limit is not changed if
** the value originally pointed to is negative, and so the current limit 
** can be queried by passing in a pointer to a negative number.  This
** file-control is used internally to implement [PRAGMA mmap_size].
**
** <li>[[SQLITE_FCNTL_TRACE]]
** The [SQLITE_FCNTL_TRACE] file control provides advisory information
** to the VFS about what the higher layers of the SQLite stack are doing.
** This file control is used by some VFS activity tracing [shims].
** The argument is a zero-terminated string.  Higher layers in the
** SQLite stack may generate instances of this file control if
** the [SQLITE_USE_FCNTL_TRACE] compile-time option is enabled.
**
** <li>[[SQLITE_FCNTL_HAS_MOVED]]
** The [SQLITE_FCNTL_HAS_MOVED] file control interprets its argument as a
** pointer to an integer and it writes a boolean into that integer depending
** on whether or not the file has been renamed, moved, or deleted since it
** was first opened.
**
** <li>[[SQLITE_FCNTL_WIN32_SET_HANDLE]]
** The [SQLITE_FCNTL_WIN32_SET_HANDLE] opcode is used for debugging.  This
** opcode causes the xFileControl method to swap the file handle with the one
** pointed to by the pArg argument.  This capability is used during testing
** and only needs to be supported when SQLITE_TEST is defined.
**
** </ul>
*/
#define SQLITE_FCNTL_LOCKSTATE               1
#define SQLITE_GET_LOCKPROXYFILE             2
#define SQLITE_SET_LOCKPROXYFILE             3
#define SQLITE_LAST_ERRNO                    4
#define SQLITE_FCNTL_SIZE_HINT               5
#define SQLITE_FCNTL_CHUNK_SIZE              6
#define SQLITE_FCNTL_FILE_POINTER            7
#define SQLITE_FCNTL_SYNC_OMITTED            8
#define SQLITE_FCNTL_WIN32_AV_RETRY          9
#define SQLITE_FCNTL_PERSIST_WAL            10
#define SQLITE_FCNTL_OVERWRITE              11
#define SQLITE_FCNTL_VFSNAME                12
#define SQLITE_FCNTL_POWERSAFE_OVERWRITE    13
#define SQLITE_FCNTL_PRAGMA                 14
#define SQLITE_FCNTL_BUSYHANDLER            15
#define SQLITE_FCNTL_TEMPFILENAME           16
#define SQLITE_FCNTL_MMAP_SIZE              18
#define SQLITE_FCNTL_TRACE                  19
#define SQLITE_FCNTL_HAS_MOVED              20
#define SQLITE_FCNTL_SYNC                   21
#define SQLITE_FCNTL_COMMIT_PHASETWO        22
#define SQLITE_FCNTL_WIN32_SET_HANDLE       23

/*
** CAPI3REF: Mutex Handle
**
** The mutex module within SQLite defines [sqlite3_mutex] to be an
** abstract type for a mutex object.  The SQLite core never looks
** at the internal representation of an [sqlite3_mutex].  It only
** deals with pointers to the [sqlite3_mutex] object.
**
** Mutexes are created using [sqlite3_mutex_alloc()].
*/
typedef struct sqlite3_mutex sqlite3_mutex;

/*
** CAPI3REF: OS Interface Object
**
** An instance of the sqlite3_vfs object defines the interface between
** the SQLite core and the underlying operating system.  The "vfs"
** in the name of the object stands for "virtual file system".  See
** the [VFS | VFS documentation] for further information.
**
** The value of the iVersion field is initially 1 but may be larger in
** future versions of SQLite.  Additional fields may be appended to this
** object when the iVersion value is increased.  Note that the structure
** of the sqlite3_vfs object changes in the transaction between
** SQLite version 3.5.9 and 3.6.0 and yet the iVersion field was not
** modified.
**
** The szOsFile field is the size of the subclassed [sqlite3_file]
** structure used by this VFS.  mxPathname is the maximum length of
** a pathname in this VFS.
**
** Registered sqlite3_vfs objects are kept on a linked list formed by
** the pNext pointer.  The [sqlite3_vfs_register()]
** and [sqlite3_vfs_unregister()] interfaces manage this list
** in a thread-safe way.  The [sqlite3_vfs_find()] interface
** searches the list.  Neither the application code nor the VFS
** implementation should use the pNext pointer.
**
** The pNext field is the only field in the sqlite3_vfs
** structure that SQLite will ever modify.  SQLite will only access
** or modify this field while holding a particular static mutex.
** The application should never modify anything within the sqlite3_vfs
** object once the object has been registered.
**
** The zName field holds the name of the VFS module.  The name must
** be unique across all VFS modules.
**
** [[sqlite3_vfs.xOpen]]
** ^SQLite guarantees that the zFilename parameter to xOpen
** is either a NULL pointer or string obtained
** from xFullPathname() with an optional suffix added.
** ^If a suffix is added to the zFilename parameter, it will
** consist of a single "-" character followed by no more than
** 11 alphanumeric and/or "-" characters.
** ^SQLite further guarantees that
** the string will be valid and unchanged until xClose() is
** called. Because of the previous sentence,
** the [sqlite3_file] can safely store a pointer to the
** filename if it needs to remember the filename for some reason.
** If the zFilename parameter to xOpen is a NULL pointer then xOpen
** must invent its own temporary name for the file.  ^Whenever the 
** xFilename parameter is NULL it will also be the case that the
** flags parameter will include [SQLITE_OPEN_DELETEONCLOSE].
**
** The flags argument to xOpen() includes all bits set in
** the flags argument to [sqlite3_open_v2()].  Or if [sqlite3_open()]
** or [sqlite3_open16()] is used, then flags includes at least
** [SQLITE_OPEN_READWRITE] | [SQLITE_OPEN_CREATE]. 
** If xOpen() opens a file read-only then it sets *pOutFlags to
** include [SQLITE_OPEN_READONLY].  Other bits in *pOutFlags may be set.
**
** ^(SQLite will also add one of the following flags to the xOpen()
** call, depending on the object being opened:
**
** <ul>
** <li>  [SQLITE_OPEN_MAIN_DB]
** <li>  [SQLITE_OPEN_MAIN_JOURNAL]
** <li>  [SQLITE_OPEN_TEMP_DB]
** <li>  [SQLITE_OPEN_TEMP_JOURNAL]
** <li>  [SQLITE_OPEN_TRANSIENT_DB]
** <li>  [SQLITE_OPEN_SUBJOURNAL]
** <li>  [SQLITE_OPEN_MASTER_JOURNAL]
** <li>  [SQLITE_OPEN_WAL]
** </ul>)^
**
** The file I/O implementation can use the object type flags to
** change the way it deals with files.  For example, an application
** that does not care about crash recovery or rollback might make
** the open of a journal file a no-op.  Writes to this journal would
** also be no-ops, and any attempt to read the journal would return
** SQLITE_IOERR.  Or the implementation might recognize that a database
** file will be doing page-aligned sector reads and writes in a random
** order and set up its I/O subsystem accordingly.
**
** SQLite might also add one of the following flags to the xOpen method:
**
** <ul>
** <li> [SQLITE_OPEN_DELETEONCLOSE]
** <li> [SQLITE_OPEN_EXCLUSIVE]
** </ul>
**
** The [SQLITE_OPEN_DELETEONCLOSE] flag means the file should be
** deleted when it is closed.  ^The [SQLITE_OPEN_DELETEONCLOSE]
** will be set for TEMP databases and their journals, transient
** databases, and subjournals.
**
** ^The [SQLITE_OPEN_EXCLUSIVE] flag is always used in conjunction
** with the [SQLITE_OPEN_CREATE] flag, which are both directly
** analogous to the O_EXCL and O_CREAT flags of the POSIX open()
** API.  The SQLITE_OPEN_EXCLUSIVE flag, when paired with the 
** SQLITE_OPEN_CREATE, is used to indicate that file should always
** be created, and that it is an error if it already exists.
** It is <i>not</i> used to indicate the file should be opened 
** for exclusive access.
**
** ^At least szOsFile bytes of memory are allocated by SQLite
** to hold the  [sqlite3_file] structure passed as the third
** argument to xOpen.  The xOpen method does not have to
** allocate the structure; it should just fill it in.  Note that
** the xOpen method must set the sqlite3_file.pMethods to either
** a valid [sqlite3_io_methods] object or to NULL.  xOpen must do
** this even if the open fails.  SQLite expects that the sqlite3_file.pMethods
** element will be valid after xOpen returns regardless of the success
** or failure of the xOpen call.
**
** [[sqlite3_vfs.xAccess]]
** ^The flags argument to xAccess() may be [SQLITE_ACCESS_EXISTS]
** to test for the existence of a file, or [SQLITE_ACCESS_READWRITE] to
** test whether a file is readable and writable, or [SQLITE_ACCESS_READ]
** to test whether a file is at least readable.   The file can be a
** directory.
**
** ^SQLite will always allocate at least mxPathname+1 bytes for the
** output buffer xFullPathname.  The exact size of the output buffer
** is also passed as a parameter to both  methods. If the output buffer
** is not large enough, [SQLITE_CANTOPEN] should be returned. Since this is
** handled as a fatal error by SQLite, vfs implementations should endeavor
** to prevent this by setting mxPathname to a sufficiently large value.
**
** The xRandomness(), xSleep(), xCurrentTime(), and xCurrentTimeInt64()
** interfaces are not strictly a part of the filesystem, but they are
** included in the VFS structure for completeness.
** The xRandomness() function attempts to return nBytes bytes
** of good-quality randomness into zOut.  The return value is
** the actual number of bytes of randomness obtained.
** The xSleep() method causes the calling thread to sleep for at
** least the number of microseconds given.  ^The xCurrentTime()
** method returns a Julian Day Number for the current date and time as
** a floating point value.
** ^The xCurrentTimeInt64() method returns, as an integer, the Julian
** Day Number multiplied by 86400000 (the number of milliseconds in 
** a 24-hour day).  
** ^SQLite will use the xCurrentTimeInt64() method to get the current
** date and time if that method is available (if iVersion is 2 or 
** greater and the function pointer is not NULL) and will fall back
** to xCurrentTime() if xCurrentTimeInt64() is unavailable.
**
** ^The xSetSystemCall(), xGetSystemCall(), and xNestSystemCall() interfaces
** are not used by the SQLite core.  These optional interfaces are provided
** by some VFSes to facilitate testing of the VFS code. By overriding 
** system calls with functions under its control, a test program can
** simulate faults and error conditions that would otherwise be difficult
** or impossible to induce.  The set of system calls that can be overridden
** varies from one VFS to another, and from one version of the same VFS to the
** next.  Applications that use these interfaces must be prepared for any
** or all of these interfaces to be NULL or for their behavior to change
** from one release to the next.  Applications must not attempt to access
** any of these methods if the iVersion of the VFS is less than 3.
*/
typedef struct sqlite3_vfs sqlite3_vfs;
typedef void (*sqlite3_syscall_ptr)(void);
struct sqlite3_vfs {
  int iVersion;            /* Structure version number (currently 3) */
  int szOsFile;            /* Size of subclassed sqlite3_file */
  int mxPathname;          /* Maximum file pathname length */
  sqlite3_vfs *pNext;      /* Next registered VFS */
  const char *zName;       /* Name of this virtual file system */
  void *pAppData;          /* Pointer to application-specific data */
  int (*xOpen)(sqlite3_vfs*, const char *zName, sqlite3_file*,
               int flags, int *pOutFlags);
  int (*xDelete)(sqlite3_vfs*, const char *zName, int syncDir);
  int (*xAccess)(sqlite3_vfs*, const char *zName, int flags, int *pResOut);
  int (*xFullPathname)(sqlite3_vfs*, const char *zName, int nOut, char *zOut);
  void *(*xDlOpen)(sqlite3_vfs*, const char *zFilename);
  void (*xDlError)(sqlite3_vfs*, int nByte, char *zErrMsg);
  void (*(*xDlSym)(sqlite3_vfs*,void*, const char *zSymbol))(void);
  void (*xDlClose)(sqlite3_vfs*, void*);
  int (*xRandomness)(sqlite3_vfs*, int nByte, char *zOut);
  int (*xSleep)(sqlite3_vfs*, int microseconds);
  int (*xCurrentTime)(sqlite3_vfs*, double*);
  int (*xGetLastError)(sqlite3_vfs*, int, char *);
  /*
  ** The methods above are in version 1 of the sqlite_vfs object
  ** definition.  Those that follow are added in version 2 or later
  */
  int (*xCurrentTimeInt64)(sqlite3_vfs*, sqlite3_int64*);
  /*
  ** The methods above are in versions 1 and 2 of the sqlite_vfs object.
  ** Those below are for version 3 and greater.
  */
  int (*xSetSystemCall)(sqlite3_vfs*, const char *zName, sqlite3_syscall_ptr);
  sqlite3_syscall_ptr (*xGetSystemCall)(sqlite3_vfs*, const char *zName);
  const char *(*xNextSystemCall)(sqlite3_vfs*, const char *zName);
  /*
  ** The methods above are in versions 1 through 3 of the sqlite_vfs object.
  ** New fields may be appended in figure versions.  The iVersion
  ** value will increment whenever this happens. 
  */
};

/*
** CAPI3REF: Flags for the xAccess VFS method
**
** These integer constants can be used as the third parameter to
** the xAccess method of an [sqlite3_vfs] object.  They determine
** what kind of permissions the xAccess method is looking for.
** With SQLITE_ACCESS_EXISTS, the xAccess method
** simply checks whether the file exists.
** With SQLITE_ACCESS_READWRITE, the xAccess method
** checks whether the named directory is both readable and writable
** (in other words, if files can be added, removed, and renamed within
** the directory).
** The SQLITE_ACCESS_READWRITE constant is currently used only by the
** [temp_store_directory pragma], though this could change in a future
** release of SQLite.
** With SQLITE_ACCESS_READ, the xAccess method
** checks whether the file is readable.  The SQLITE_ACCESS_READ constant is
** currently unused, though it might be used in a future release of
** SQLite.
*/
#define SQLITE_ACCESS_EXISTS    0
#define SQLITE_ACCESS_READWRITE 1   /* Used by PRAGMA temp_store_directory */
#define SQLITE_ACCESS_READ      2   /* Unused */

/*
** CAPI3REF: Flags for the xShmLock VFS method
**
** These integer constants define the various locking operations
** allowed by the xShmLock method of [sqlite3_io_methods].  The
** following are the only legal combinations of flags to the
** xShmLock method:
**
** <ul>
** <li>  SQLITE_SHM_LOCK | SQLITE_SHM_SHARED
** <li>  SQLITE_SHM_LOCK | SQLITE_SHM_EXCLUSIVE
** <li>  SQLITE_SHM_UNLOCK | SQLITE_SHM_SHARED
** <li>  SQLITE_SHM_UNLOCK | SQLITE_SHM_EXCLUSIVE
** </ul>
**
** When unlocking, the same SHARED or EXCLUSIVE flag must be supplied as
** was given on the corresponding lock.  
**
** The xShmLock method can transition between unlocked and SHARED or
** between unlocked and EXCLUSIVE.  It cannot transition between SHARED
** and EXCLUSIVE.
*/
#define SQLITE_SHM_UNLOCK       1
#define SQLITE_SHM_LOCK         2
#define SQLITE_SHM_SHARED       4
#define SQLITE_SHM_EXCLUSIVE    8

/*
** CAPI3REF: Maximum xShmLock index
**
** The xShmLock method on [sqlite3_io_methods] may use values
** between 0 and this upper bound as its "offset" argument.
** The SQLite core will never attempt to acquire or release a
** lock outside of this range
*/
#define SQLITE_SHM_NLOCK        8


/*
** CAPI3REF: Initialize The SQLite Library
**
** ^The sqlite3_initialize() routine initializes the
** SQLite library.  ^The sqlite3_shutdown() routine
** deallocates any resources that were allocated by sqlite3_initialize().
** These routines are designed to aid in process initialization and
** shutdown on embedded systems.  Workstation applications using
** SQLite normally do not need to invoke either of these routines.
**
** A call to sqlite3_initialize() is an "effective" call if it is
** the first time sqlite3_initialize() is invoked during the lifetime of
** the process, or if it is the first time sqlite3_initialize() is invoked
** following a call to sqlite3_shutdown().  ^(Only an effective call
** of sqlite3_initialize() does any initialization.  All other calls
** are harmless no-ops.)^
**
** A call to sqlite3_shutdown() is an "effective" call if it is the first
** call to sqlite3_shutdown() since the last sqlite3_initialize().  ^(Only
** an effective call to sqlite3_shutdown() does any deinitialization.
** All other valid calls to sqlite3_shutdown() are harmless no-ops.)^
**
** The sqlite3_initialize() interface is threadsafe, but sqlite3_shutdown()
** is not.  The sqlite3_shutdown() interface must only be called from a
** single thread.  All open [database connections] must be closed and all
** other SQLite resources must be deallocated prior to invoking
** sqlite3_shutdown().
**
** Among other things, ^sqlite3_initialize() will invoke
** sqlite3_os_init().  Similarly, ^sqlite3_shutdown()
** will invoke sqlite3_os_end().
**
** ^The sqlite3_initialize() routine returns [SQLITE_OK] on success.
** ^If for some reason, sqlite3_initialize() is unable to initialize
** the library (perhaps it is unable to allocate a needed resource such
** as a mutex) it returns an [error code] other than [SQLITE_OK].
**
** ^The sqlite3_initialize() routine is called internally by many other
** SQLite interfaces so that an application usually does not need to
** invoke sqlite3_initialize() directly.  For example, [sqlite3_open()]
** calls sqlite3_initialize() so the SQLite library will be automatically
** initialized when [sqlite3_open()] is called if it has not be initialized
** already.  ^However, if SQLite is compiled with the [SQLITE_OMIT_AUTOINIT]
** compile-time option, then the automatic calls to sqlite3_initialize()
** are omitted and the application must call sqlite3_initialize() directly
** prior to using any other SQLite interface.  For maximum portability,
** it is recommended that applications always invoke sqlite3_initialize()
** directly prior to using any other SQLite interface.  Future releases
** of SQLite may require this.  In other words, the behavior exhibited
** when SQLite is compiled with [SQLITE_OMIT_AUTOINIT] might become the
** default behavior in some future release of SQLite.
**
** The sqlite3_os_init() routine does operating-system specific
** initialization of the SQLite library.  The sqlite3_os_end()
** routine undoes the effect of sqlite3_os_init().  Typical tasks
** performed by these routines include allocation or deallocation
** of static resources, initialization of global variables,
** setting up a default [sqlite3_vfs] module, or setting up
** a default configuration using [sqlite3_config()].
**
** The application should never invoke either sqlite3_os_init()
** or sqlite3_os_end() directly.  The application should only invoke
** sqlite3_initialize() and sqlite3_shutdown().  The sqlite3_os_init()
** interface is called automatically by sqlite3_initialize() and
** sqlite3_os_end() is called by sqlite3_shutdown().  Appropriate
** implementations for sqlite3_os_init() and sqlite3_os_end()
** are built into SQLite when it is compiled for Unix, Windows, or OS/2.
** When [custom builds | built for other platforms]
** (using the [SQLITE_OS_OTHER=1] compile-time
** option) the application must supply a suitable implementation for
** sqlite3_os_init() and sqlite3_os_end().  An application-supplied
** implementation of sqlite3_os_init() or sqlite3_os_end()
** must return [SQLITE_OK] on success and some other [error code] upon
** failure.
*/
SQLITE_API int sqlite3_initialize(void);
SQLITE_API int sqlite3_shutdown(void);
SQLITE_API int sqlite3_os_init(void);
SQLITE_API int sqlite3_os_end(void);

/*
** CAPI3REF: Configuring The SQLite Library
**
** The sqlite3_config() interface is used to make global configuration
** changes to SQLite in order to tune SQLite to the specific needs of
** the application.  The default configuration is recommended for most
** applications and so this routine is usually not necessary.  It is
** provided to support rare applications with unusual needs.
**
** The sqlite3_config() interface is not threadsafe.  The application
** must insure that no other SQLite interfaces are invoked by other
** threads while sqlite3_config() is running.  Furthermore, sqlite3_config()
** may only be invoked prior to library initialization using
** [sqlite3_initialize()] or after shutdown by [sqlite3_shutdown()].
** ^If sqlite3_config() is called after [sqlite3_initialize()] and before
** [sqlite3_shutdown()] then it will return SQLITE_MISUSE.
** Note, however, that ^sqlite3_config() can be called as part of the
** implementation of an application-defined [sqlite3_os_init()].
**
** The first argument to sqlite3_config() is an integer
** [configuration option] that determines
** what property of SQLite is to be configured.  Subsequent arguments
** vary depending on the [configuration option]
** in the first argument.
**
** ^When a configuration option is set, sqlite3_config() returns [SQLITE_OK].
** ^If the option is unknown or SQLite is unable to set the option
** then this routine returns a non-zero [error code].
*/
SQLITE_API int sqlite3_config(int, ...);

/*
** CAPI3REF: Configure database connections
**
** The sqlite3_db_config() interface is used to make configuration
** changes to a [database connection].  The interface is similar to
** [sqlite3_config()] except that the changes apply to a single
** [database connection] (specified in the first argument).
**
** The second argument to sqlite3_db_config(D,V,...)  is the
** [SQLITE_DBCONFIG_LOOKASIDE | configuration verb] - an integer code 
** that indicates what aspect of the [database connection] is being configured.
** Subsequent arguments vary depending on the configuration verb.
**
** ^Calls to sqlite3_db_config() return SQLITE_OK if and only if
** the call is considered successful.
*/
SQLITE_API int sqlite3_db_config(sqlite3*, int op, ...);

/*
** CAPI3REF: Memory Allocation Routines
**
** An instance of this object defines the interface between SQLite
** and low-level memory allocation routines.
**
** This object is used in only one place in the SQLite interface.
** A pointer to an instance of this object is the argument to
** [sqlite3_config()] when the configuration option is
** [SQLITE_CONFIG_MALLOC] or [SQLITE_CONFIG_GETMALLOC].  
** By creating an instance of this object
** and passing it to [sqlite3_config]([SQLITE_CONFIG_MALLOC])
** during configuration, an application can specify an alternative
** memory allocation subsystem for SQLite to use for all of its
** dynamic memory needs.
**
** Note that SQLite comes with several [built-in memory allocators]
** that are perfectly adequate for the overwhelming majority of applications
** and that this object is only useful to a tiny minority of applications
** with specialized memory allocation requirements.  This object is
** also used during testing of SQLite in order to specify an alternative
** memory allocator that simulates memory out-of-memory conditions in
** order to verify that SQLite recovers gracefully from such
** conditions.
**
** The xMalloc, xRealloc, and xFree methods must work like the
** malloc(), realloc() and free() functions from the standard C library.
** ^SQLite guarantees that the second argument to
** xRealloc is always a value returned by a prior call to xRoundup.
**
** xSize should return the allocated size of a memory allocation
** previously obtained from xMalloc or xRealloc.  The allocated size
** is always at least as big as the requested size but may be larger.
**
** The xRoundup method returns what would be the allocated size of
** a memory allocation given a particular requested size.  Most memory
** allocators round up memory allocations at least to the next multiple
** of 8.  Some allocators round up to a larger multiple or to a power of 2.
** Every memory allocation request coming in through [sqlite3_malloc()]
** or [sqlite3_realloc()] first calls xRoundup.  If xRoundup returns 0, 
** that causes the corresponding memory allocation to fail.
**
** The xInit method initializes the memory allocator.  For example,
** it might allocate any require mutexes or initialize internal data
** structures.  The xShutdown method is invoked (indirectly) by
** [sqlite3_shutdown()] and should deallocate any resources acquired
** by xInit.  The pAppData pointer is used as the only parameter to
** xInit and xShutdown.
**
** SQLite holds the [SQLITE_MUTEX_STATIC_MASTER] mutex when it invokes
** the xInit method, so the xInit method need not be threadsafe.  The
** xShutdown method is only called from [sqlite3_shutdown()] so it does
** not need to be threadsafe either.  For all other methods, SQLite
** holds the [SQLITE_MUTEX_STATIC_MEM] mutex as long as the
** [SQLITE_CONFIG_MEMSTATUS] configuration option is turned on (which
** it is by default) and so the methods are automatically serialized.
** However, if [SQLITE_CONFIG_MEMSTATUS] is disabled, then the other
** methods must be threadsafe or else make their own arrangements for
** serialization.
**
** SQLite will never invoke xInit() more than once without an intervening
** call to xShutdown().
*/
typedef struct sqlite3_mem_methods sqlite3_mem_methods;
struct sqlite3_mem_methods {
  void *(*xMalloc)(int);         /* Memory allocation function */
  void (*xFree)(void*);          /* Free a prior allocation */
  void *(*xRealloc)(void*,int);  /* Resize an allocation */
  int (*xSize)(void*);           /* Return the size of an allocation */
  int (*xRoundup)(int);          /* Round up request size to allocation size */
  int (*xInit)(void*);           /* Initialize the memory allocator */
  void (*xShutdown)(void*);      /* Deinitialize the memory allocator */
  void *pAppData;                /* Argument to xInit() and xShutdown() */
};

/*
** CAPI3REF: Configuration Options
** KEYWORDS: {configuration option}
**
** These constants are the available integer configuration options that
** can be passed as the first argument to the [sqlite3_config()] interface.
**
** New configuration options may be added in future releases of SQLite.
** Existing configuration options might be discontinued.  Applications
** should check the return code from [sqlite3_config()] to make sure that
** the call worked.  The [sqlite3_config()] interface will return a
** non-zero [error code] if a discontinued or unsupported configuration option
** is invoked.
**
** <dl>
** [[SQLITE_CONFIG_SINGLETHREAD]] <dt>SQLITE_CONFIG_SINGLETHREAD</dt>
** <dd>There are no arguments to this option.  ^This option sets the
** [threading mode] to Single-thread.  In other words, it disables
** all mutexing and puts SQLite into a mode where it can only be used
** by a single thread.   ^If SQLite is compiled with
** the [SQLITE_THREADSAFE | SQLITE_THREADSAFE=0] compile-time option then
** it is not possible to change the [threading mode] from its default
** value of Single-thread and so [sqlite3_config()] will return 
** [SQLITE_ERROR] if called with the SQLITE_CONFIG_SINGLETHREAD
** configuration option.</dd>
**
** [[SQLITE_CONFIG_MULTITHREAD]] <dt>SQLITE_CONFIG_MULTITHREAD</dt>
** <dd>There are no arguments to this option.  ^This option sets the
** [threading mode] to Multi-thread.  In other words, it disables
** mutexing on [database connection] and [prepared statement] objects.
** The application is responsible for serializing access to
** [database connections] and [prepared statements].  But other mutexes
** are enabled so that SQLite will be safe to use in a multi-threaded
** environment as long as no two threads attempt to use the same
** [database connection] at the same time.  ^If SQLite is compiled with
** the [SQLITE_THREADSAFE | SQLITE_THREADSAFE=0] compile-time option then
** it is not possible to set the Multi-thread [threading mode] and
** [sqlite3_config()] will return [SQLITE_ERROR] if called with the
** SQLITE_CONFIG_MULTITHREAD configuration option.</dd>
**
** [[SQLITE_CONFIG_SERIALIZED]] <dt>SQLITE_CONFIG_SERIALIZED</dt>
** <dd>There are no arguments to this option.  ^This option sets the
** [threading mode] to Serialized. In other words, this option enables
** all mutexes including the recursive
** mutexes on [database connection] and [prepared statement] objects.
** In this mode (which is the default when SQLite is compiled with
** [SQLITE_THREADSAFE=1]) the SQLite library will itself serialize access
** to [database connections] and [prepared statements] so that the
** application is free to use the same [database connection] or the
** same [prepared statement] in different threads at the same time.
** ^If SQLite is compiled with
** the [SQLITE_THREADSAFE | SQLITE_THREADSAFE=0] compile-time option then
** it is not possible to set the Serialized [threading mode] and
** [sqlite3_config()] will return [SQLITE_ERROR] if called with the
** SQLITE_CONFIG_SERIALIZED configuration option.</dd>
**
** [[SQLITE_CONFIG_MALLOC]] <dt>SQLITE_CONFIG_MALLOC</dt>
** <dd> ^(The SQLITE_CONFIG_MALLOC option takes a single argument which is 
** a pointer to an instance of the [sqlite3_mem_methods] structure.
** The argument specifies
** alternative low-level memory allocation routines to be used in place of
** the memory allocation routines built into SQLite.)^ ^SQLite makes
** its own private copy of the content of the [sqlite3_mem_methods] structure
** before the [sqlite3_config()] call returns.</dd>
**
** [[SQLITE_CONFIG_GETMALLOC]] <dt>SQLITE_CONFIG_GETMALLOC</dt>
** <dd> ^(The SQLITE_CONFIG_GETMALLOC option takes a single argument which
** is a pointer to an instance of the [sqlite3_mem_methods] structure.
** The [sqlite3_mem_methods]
** structure is filled with the currently defined memory allocation routines.)^
** This option can be used to overload the default memory allocation
** routines with a wrapper that simulations memory allocation failure or
** tracks memory usage, for example. </dd>
**
** [[SQLITE_CONFIG_MEMSTATUS]] <dt>SQLITE_CONFIG_MEMSTATUS</dt>
** <dd> ^The SQLITE_CONFIG_MEMSTATUS option takes single argument of type int,
** interpreted as a boolean, which enables or disables the collection of
** memory allocation statistics. ^(When memory allocation statistics are
** disabled, the following SQLite interfaces become non-operational:
**   <ul>
**   <li> [sqlite3_memory_used()]
**   <li> [sqlite3_memory_highwater()]
**   <li> [sqlite3_soft_heap_limit64()]
**   <li> [sqlite3_status()]
**   </ul>)^
** ^Memory allocation statistics are enabled by default unless SQLite is
** compiled with [SQLITE_DEFAULT_MEMSTATUS]=0 in which case memory
** allocation statistics are disabled by default.
** </dd>
**
** [[SQLITE_CONFIG_SCRATCH]] <dt>SQLITE_CONFIG_SCRATCH</dt>
** <dd> ^The SQLITE_CONFIG_SCRATCH option specifies a static memory buffer
** that SQLite can use for scratch memory.  ^(There are three arguments
** to SQLITE_CONFIG_SCRATCH:  A pointer an 8-byte
** aligned memory buffer from which the scratch allocations will be
** drawn, the size of each scratch allocation (sz),
** and the maximum number of scratch allocations (N).)^
** The first argument must be a pointer to an 8-byte aligned buffer
** of at least sz*N bytes of memory.
** ^SQLite will not use more than one scratch buffers per thread.
** ^SQLite will never request a scratch buffer that is more than 6
** times the database page size.
** ^If SQLite needs needs additional
** scratch memory beyond what is provided by this configuration option, then 
** [sqlite3_malloc()] will be used to obtain the memory needed.<p>
** ^When the application provides any amount of scratch memory using
** SQLITE_CONFIG_SCRATCH, SQLite avoids unnecessary large
** [sqlite3_malloc|heap allocations].
** This can help [Robson proof|prevent memory allocation failures] due to heap
** fragmentation in low-memory embedded systems.
** </dd>
**
** [[SQLITE_CONFIG_PAGECACHE]] <dt>SQLITE_CONFIG_PAGECACHE</dt>
** <dd> ^The SQLITE_CONFIG_PAGECACHE option specifies a static memory buffer
** that SQLite can use for the database page cache with the default page
** cache implementation.  
** This configuration should not be used if an application-define page
** cache implementation is loaded using the [SQLITE_CONFIG_PCACHE2]
** configuration option.
** ^There are three arguments to SQLITE_CONFIG_PAGECACHE: A pointer to
** 8-byte aligned
** memory, the size of each page buffer (sz), and the number of pages (N).
** The sz argument should be the size of the largest database page
** (a power of two between 512 and 65536) plus some extra bytes for each
** page header.  ^The number of extra bytes needed by the page header
** can be determined using the [SQLITE_CONFIG_PCACHE_HDRSZ] option 
** to [sqlite3_config()].
** ^It is harmless, apart from the wasted memory,
** for the sz parameter to be larger than necessary.  The first
** argument should pointer to an 8-byte aligned block of memory that
** is at least sz*N bytes of memory, otherwise subsequent behavior is
** undefined.
** ^SQLite will use the memory provided by the first argument to satisfy its
** memory needs for the first N pages that it adds to cache.  ^If additional
** page cache memory is needed beyond what is provided by this option, then
** SQLite goes to [sqlite3_malloc()] for the additional storage space.</dd>
**
** [[SQLITE_CONFIG_HEAP]] <dt>SQLITE_CONFIG_HEAP</dt>
** <dd> ^The SQLITE_CONFIG_HEAP option specifies a static memory buffer 
** that SQLite will use for all of its dynamic memory allocation needs
** beyond those provided for by [SQLITE_CONFIG_SCRATCH] and
** [SQLITE_CONFIG_PAGECACHE].
** ^The SQLITE_CONFIG_HEAP option is only available if SQLite is compiled
** with either [SQLITE_ENABLE_MEMSYS3] or [SQLITE_ENABLE_MEMSYS5] and returns
** [SQLITE_ERROR] if invoked otherwise.
** ^There are three arguments to SQLITE_CONFIG_HEAP:
** An 8-byte aligned pointer to the memory,
** the number of bytes in the memory buffer, and the minimum allocation size.
** ^If the first pointer (the memory pointer) is NULL, then SQLite reverts
** to using its default memory allocator (the system malloc() implementation),
** undoing any prior invocation of [SQLITE_CONFIG_MALLOC].  ^If the
** memory pointer is not NULL then the alternative memory
** allocator is engaged to handle all of SQLites memory allocation needs.
** The first pointer (the memory pointer) must be aligned to an 8-byte
** boundary or subsequent behavior of SQLite will be undefined.
** The minimum allocation size is capped at 2**12. Reasonable values
** for the minimum allocation size are 2**5 through 2**8.</dd>
**
** [[SQLITE_CONFIG_MUTEX]] <dt>SQLITE_CONFIG_MUTEX</dt>
** <dd> ^(The SQLITE_CONFIG_MUTEX option takes a single argument which is a
** pointer to an instance of the [sqlite3_mutex_methods] structure.
** The argument specifies alternative low-level mutex routines to be used
** in place the mutex routines built into SQLite.)^  ^SQLite makes a copy of
** the content of the [sqlite3_mutex_methods] structure before the call to
** [sqlite3_config()] returns. ^If SQLite is compiled with
** the [SQLITE_THREADSAFE | SQLITE_THREADSAFE=0] compile-time option then
** the entire mutexing subsystem is omitted from the build and hence calls to
** [sqlite3_config()] with the SQLITE_CONFIG_MUTEX configuration option will
** return [SQLITE_ERROR].</dd>
**
** [[SQLITE_CONFIG_GETMUTEX]] <dt>SQLITE_CONFIG_GETMUTEX</dt>
** <dd> ^(The SQLITE_CONFIG_GETMUTEX option takes a single argument which
** is a pointer to an instance of the [sqlite3_mutex_methods] structure.  The
** [sqlite3_mutex_methods]
** structure is filled with the currently defined mutex routines.)^
** This option can be used to overload the default mutex allocation
** routines with a wrapper used to track mutex usage for performance
** profiling or testing, for example.   ^If SQLite is compiled with
** the [SQLITE_THREADSAFE | SQLITE_THREADSAFE=0] compile-time option then
** the entire mutexing subsystem is omitted from the build and hence calls to
** [sqlite3_config()] with the SQLITE_CONFIG_GETMUTEX configuration option will
** return [SQLITE_ERROR].</dd>
**
** [[SQLITE_CONFIG_LOOKASIDE]] <dt>SQLITE_CONFIG_LOOKASIDE</dt>
** <dd> ^(The SQLITE_CONFIG_LOOKASIDE option takes two arguments that determine
** the default size of lookaside memory on each [database connection].
** The first argument is the
** size of each lookaside buffer slot and the second is the number of
** slots allocated to each database connection.)^  ^(SQLITE_CONFIG_LOOKASIDE
** sets the <i>default</i> lookaside size. The [SQLITE_DBCONFIG_LOOKASIDE]
** option to [sqlite3_db_config()] can be used to change the lookaside
** configuration on individual connections.)^ </dd>
**
** [[SQLITE_CONFIG_PCACHE2]] <dt>SQLITE_CONFIG_PCACHE2</dt>
** <dd> ^(The SQLITE_CONFIG_PCACHE2 option takes a single argument which is 
** a pointer to an [sqlite3_pcache_methods2] object.  This object specifies
** the interface to a custom page cache implementation.)^
** ^SQLite makes a copy of the [sqlite3_pcache_methods2] object.</dd>
**
** [[SQLITE_CONFIG_GETPCACHE2]] <dt>SQLITE_CONFIG_GETPCACHE2</dt>
** <dd> ^(The SQLITE_CONFIG_GETPCACHE2 option takes a single argument which
** is a pointer to an [sqlite3_pcache_methods2] object.  SQLite copies of
** the current page cache implementation into that object.)^ </dd>
**
** [[SQLITE_CONFIG_LOG]] <dt>SQLITE_CONFIG_LOG</dt>
** <dd> The SQLITE_CONFIG_LOG option is used to configure the SQLite
** global [error log].
** (^The SQLITE_CONFIG_LOG option takes two arguments: a pointer to a
** function with a call signature of void(*)(void*,int,const char*), 
** and a pointer to void. ^If the function pointer is not NULL, it is
** invoked by [sqlite3_log()] to process each logging event.  ^If the
** function pointer is NULL, the [sqlite3_log()] interface becomes a no-op.
** ^The void pointer that is the second argument to SQLITE_CONFIG_LOG is
** passed through as the first parameter to the application-defined logger
** function whenever that function is invoked.  ^The second parameter to
** the logger function is a copy of the first parameter to the corresponding
** [sqlite3_log()] call and is intended to be a [result code] or an
** [extended result code].  ^The third parameter passed to the logger is
** log message after formatting via [sqlite3_snprintf()].
** The SQLite logging interface is not reentrant; the logger function
** supplied by the application must not invoke any SQLite interface.
** In a multi-threaded application, the application-defined logger
** function must be threadsafe. </dd>
**
** [[SQLITE_CONFIG_URI]] <dt>SQLITE_CONFIG_URI
** <dd>^(The SQLITE_CONFIG_URI option takes a single argument of type int.
** If non-zero, then URI handling is globally enabled. If the parameter is zero,
** then URI handling is globally disabled.)^ ^If URI handling is globally
** enabled, all filenames passed to [sqlite3_open()], [sqlite3_open_v2()],
** [sqlite3_open16()] or
** specified as part of [ATTACH] commands are interpreted as URIs, regardless
** of whether or not the [SQLITE_OPEN_URI] flag is set when the database
** connection is opened. ^If it is globally disabled, filenames are
** only interpreted as URIs if the SQLITE_OPEN_URI flag is set when the
** database connection is opened. ^(By default, URI handling is globally
** disabled. The default value may be changed by compiling with the
** [SQLITE_USE_URI] symbol defined.)^
**
** [[SQLITE_CONFIG_COVERING_INDEX_SCAN]] <dt>SQLITE_CONFIG_COVERING_INDEX_SCAN
** <dd>^The SQLITE_CONFIG_COVERING_INDEX_SCAN option takes a single integer
** argument which is interpreted as a boolean in order to enable or disable
** the use of covering indices for full table scans in the query optimizer.
** ^The default setting is determined
** by the [SQLITE_ALLOW_COVERING_INDEX_SCAN] compile-time option, or is "on"
** if that compile-time option is omitted.
** The ability to disable the use of covering indices for full table scans
** is because some incorrectly coded legacy applications might malfunction
** when the optimization is enabled.  Providing the ability to
** disable the optimization allows the older, buggy application code to work
** without change even with newer versions of SQLite.
**
** [[SQLITE_CONFIG_PCACHE]] [[SQLITE_CONFIG_GETPCACHE]]
** <dt>SQLITE_CONFIG_PCACHE and SQLITE_CONFIG_GETPCACHE
** <dd> These options are obsolete and should not be used by new code.
** They are retained for backwards compatibility but are now no-ops.
** </dd>
**
** [[SQLITE_CONFIG_SQLLOG]]
** <dt>SQLITE_CONFIG_SQLLOG
** <dd>This option is only available if sqlite is compiled with the
** [SQLITE_ENABLE_SQLLOG] pre-processor macro defined. The first argument should
** be a pointer to a function of type void(*)(void*,sqlite3*,const char*, int).
** The second should be of type (void*). The callback is invoked by the library
** in three separate circumstances, identified by the value passed as the
** fourth parameter. If the fourth parameter is 0, then the database connection
** passed as the second argument has just been opened. The third argument
** points to a buffer containing the name of the main database file. If the
** fourth parameter is 1, then the SQL statement that the third parameter
** points to has just been executed. Or, if the fourth parameter is 2, then
** the connection being passed as the second parameter is being closed. The
** third parameter is passed NULL In this case.  An example of using this
** configuration option can be seen in the "test_sqllog.c" source file in
** the canonical SQLite source tree.</dd>
**
** [[SQLITE_CONFIG_MMAP_SIZE]]
** <dt>SQLITE_CONFIG_MMAP_SIZE
** <dd>^SQLITE_CONFIG_MMAP_SIZE takes two 64-bit integer (sqlite3_int64) values
** that are the default mmap size limit (the default setting for
** [PRAGMA mmap_size]) and the maximum allowed mmap size limit.
** ^The default setting can be overridden by each database connection using
** either the [PRAGMA mmap_size] command, or by using the
** [SQLITE_FCNTL_MMAP_SIZE] file control.  ^(The maximum allowed mmap size
** will be silently truncated if necessary so that it does not exceed the
** compile-time maximum mmap size set by the
** [SQLITE_MAX_MMAP_SIZE] compile-time option.)^
** ^If either argument to this option is negative, then that argument is
** changed to its compile-time default.
**
** [[SQLITE_CONFIG_WIN32_HEAPSIZE]]
** <dt>SQLITE_CONFIG_WIN32_HEAPSIZE
** <dd>^The SQLITE_CONFIG_WIN32_HEAPSIZE option is only available if SQLite is
** compiled for Windows with the [SQLITE_WIN32_MALLOC] pre-processor macro
** defined. ^SQLITE_CONFIG_WIN32_HEAPSIZE takes a 32-bit unsigned integer value
** that specifies the maximum size of the created heap.
** </dl>
**
** [[SQLITE_CONFIG_PCACHE_HDRSZ]]
** <dt>SQLITE_CONFIG_PCACHE_HDRSZ
** <dd>^The SQLITE_CONFIG_PCACHE_HDRSZ option takes a single parameter which
** is a pointer to an integer and writes into that integer the number of extra
** bytes per page required for each page in [SQLITE_CONFIG_PAGECACHE].
** The amount of extra space required can change depending on the compiler,
** target platform, and SQLite version.
**
** [[SQLITE_CONFIG_PMASZ]]
** <dt>SQLITE_CONFIG_PMASZ
** <dd>^The SQLITE_CONFIG_PMASZ option takes a single parameter which
** is an unsigned integer and sets the "Minimum PMA Size" for the multithreaded
** sorter to that integer.  The default minimum PMA Size is set by the
** [SQLITE_SORTER_PMASZ] compile-time option.  New threads are launched
** to help with sort operations when multithreaded sorting
** is enabled (using the [PRAGMA threads] command) and the amount of content
** to be sorted exceeds the page size times the minimum of the
** [PRAGMA cache_size] setting and this value.
** </dl>
*/
#define SQLITE_CONFIG_SINGLETHREAD  1  /* nil */
#define SQLITE_CONFIG_MULTITHREAD   2  /* nil */
#define SQLITE_CONFIG_SERIALIZED    3  /* nil */
#define SQLITE_CONFIG_MALLOC        4  /* sqlite3_mem_methods* */
#define SQLITE_CONFIG_GETMALLOC     5  /* sqlite3_mem_methods* */
#define SQLITE_CONFIG_SCRATCH       6  /* void*, int sz, int N */
#define SQLITE_CONFIG_PAGECACHE     7  /* void*, int sz, int N */
#define SQLITE_CONFIG_HEAP          8  /* void*, int nByte, int min */
#define SQLITE_CONFIG_MEMSTATUS     9  /* boolean */
#define SQLITE_CONFIG_MUTEX        10  /* sqlite3_mutex_methods* */
#define SQLITE_CONFIG_GETMUTEX     11  /* sqlite3_mutex_methods* */
/* previously SQLITE_CONFIG_CHUNKALLOC 12 which is now unused. */ 
#define SQLITE_CONFIG_LOOKASIDE    13  /* int int */
#define SQLITE_CONFIG_PCACHE       14  /* no-op */
#define SQLITE_CONFIG_GETPCACHE    15  /* no-op */
#define SQLITE_CONFIG_LOG          16  /* xFunc, void* */
#define SQLITE_CONFIG_URI          17  /* int */
#define SQLITE_CONFIG_PCACHE2      18  /* sqlite3_pcache_methods2* */
#define SQLITE_CONFIG_GETPCACHE2   19  /* sqlite3_pcache_methods2* */
#define SQLITE_CONFIG_COVERING_INDEX_SCAN 20  /* int */
#define SQLITE_CONFIG_SQLLOG       21  /* xSqllog, void* */
#define SQLITE_CONFIG_MMAP_SIZE    22  /* sqlite3_int64, sqlite3_int64 */
#define SQLITE_CONFIG_WIN32_HEAPSIZE      23  /* int nByte */
#define SQLITE_CONFIG_PCACHE_HDRSZ        24  /* int *psz */
#define SQLITE_CONFIG_PMASZ               25  /* unsigned int szPma */

/*
** CAPI3REF: Database Connection Configuration Options
**
** These constants are the available integer configuration options that
** can be passed as the second argument to the [sqlite3_db_config()] interface.
**
** New configuration options may be added in future releases of SQLite.
** Existing configuration options might be discontinued.  Applications
** should check the return code from [sqlite3_db_config()] to make sure that
** the call worked.  ^The [sqlite3_db_config()] interface will return a
** non-zero [error code] if a discontinued or unsupported configuration option
** is invoked.
**
** <dl>
** <dt>SQLITE_DBCONFIG_LOOKASIDE</dt>
** <dd> ^This option takes three additional arguments that determine the 
** [lookaside memory allocator] configuration for the [database connection].
** ^The first argument (the third parameter to [sqlite3_db_config()] is a
** pointer to a memory buffer to use for lookaside memory.
** ^The first argument after the SQLITE_DBCONFIG_LOOKASIDE verb
** may be NULL in which case SQLite will allocate the
** lookaside buffer itself using [sqlite3_malloc()]. ^The second argument is the
** size of each lookaside buffer slot.  ^The third argument is the number of
** slots.  The size of the buffer in the first argument must be greater than
** or equal to the product of the second and third arguments.  The buffer
** must be aligned to an 8-byte boundary.  ^If the second argument to
** SQLITE_DBCONFIG_LOOKASIDE is not a multiple of 8, it is internally
** rounded down to the next smaller multiple of 8.  ^(The lookaside memory
** configuration for a database connection can only be changed when that
** connection is not currently using lookaside memory, or in other words
** when the "current value" returned by
** [sqlite3_db_status](D,[SQLITE_CONFIG_LOOKASIDE],...) is zero.
** Any attempt to change the lookaside memory configuration when lookaside
** memory is in use leaves the configuration unchanged and returns 
** [SQLITE_BUSY].)^</dd>
**
** <dt>SQLITE_DBCONFIG_ENABLE_FKEY</dt>
** <dd> ^This option is used to enable or disable the enforcement of
** [foreign key constraints].  There should be two additional arguments.
** The first argument is an integer which is 0 to disable FK enforcement,
** positive to enable FK enforcement or negative to leave FK enforcement
** unchanged.  The second parameter is a pointer to an integer into which

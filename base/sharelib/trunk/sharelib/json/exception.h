/**
 * Guildelines for exception handling
 * 1. How to throw
 *    SHARELIB_JSON_THROW(the_exception_to_show, description_string)
 *    e.g. SHARELIB_JSON_THROW(ParameterInvalidException, "Cannot be empty string");
 *    If you throw not using SHARELIB_JSON_THROW, the location of throw will not be recorded.
 *
 * 2. Define an new apsara exception:
 *    a. SHOULD inherit from apsara::ExceptionBase or its sub-classes.
 *    b. SHOULD use macro SHARELIB_JSON_DEFINE_EXCEPTION inside public: region of the class definition.
 *
 * 3. GetClassName(): exception class name.
 *    GetMessage(): (virtual) One can customize it to return some specific details. By default, it returns the
 *        description string when you construct the exception.
 *    GetStackTrace(): call stack when the exception is thrown.
 *    ToString(): return a string describing:
 *        a) where the exception is thown (if available)
 *        b) the exception class name
 *        c) the content of GetMessage
 *        d) stack trace
 *        e) cause, if available
 *    what(): Same as ToString(). But the return type is const char *.
 *    GetExceptionChain(): Same as ToString(). Kept for backward compatibility.
 */

#ifndef SHARELIB_JSON_EXCEPTION_H
#define SHARELIB_JSON_EXCEPTION_H
#include <sharelib/common.h>
#include <exception>
#include <tr1/memory>
#include <string>
SHARELIB_BS;

#define SHARELIB_JSON_THROW(ExClass, args...)                                  \
    do                                                                  \
    {                                                                   \
        ExClass tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0(args);         \
        tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.Init(__FILE__, __PRETTY_FUNCTION__, __LINE__); \
        throw tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0;                 \
    } while (false)

#define SHARELIB_JSON_THROW_CHAIN(ExClass, cause, args...)                     \
    do                                                                  \
    {                                                                   \
        ExClass tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0(args);         \
        tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.Init(__FILE__, __PRETTY_FUNCTION__, __LINE__); \
        tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.SetCause(cause);       \
        throw tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0;                 \
    } while (false)

#define SHARELIB_JSON_THROW_LOG(logger, additionalInfo, ExClass, args...)      \
    do                                                                  \
    {                                                                   \
        ExClass tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0(args);         \
        tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.Init(__FILE__, __PRETTY_FUNCTION__, __LINE__); \
        LOG_INFO(logger,                                                \
                 ("ExceptionClass", #ExClass)                           \
                 ("Message", tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.GetMessage()) \
                 ("CallStack", tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.GetStackTrace()) \
                 ("What", tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.ToString()) \
                 additionalInfo);                                       \
        throw tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0;                 \
    } while(false)

#define SHARELIB_JSON_THROW_CHAIN_LOG(logger, additionalInfo, ExClass, cause, args...) \
    do                                                                  \
    {                                                                   \
        ExClass tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0(args);         \
        tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.Init(__FILE__, __PRETTY_FUNCTION__, __LINE__); \
        tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.SetCause(cause);       \
        LOG_INFO(logger,                                                \
                 ("ExceptionClass", #ExClass)                           \
                 ("Message", tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.GetMessage()) \
                 ("CallStack", tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.GetStackTrace()) \
                 ("What", tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0.ToString()) \
                 additionalInfo);                                       \
        throw tmp_3d3079a0_61ec_48e6_abd6_8a33b0eb91f0;                 \
    } while(false)

#define SHARELIB_JSON_DEFINE_EXCEPTION(ExClass, Base)          \
    ExClass(const std::string& strMsg="") throw()       \
        : Base(strMsg)                                  \
    {}                                                  \
                                                        \
    ~ExClass() throw(){}                                      \
                                                              \
    /* override */ std::string GetClassName() const           \
    {                                                         \
        return #ExClass;                                      \
    }                                                         \
                                                              \
    /* override */ std::tr1::shared_ptr<ExceptionBase> Clone() const    \
    {                                                                   \
        return std::tr1::shared_ptr<ExceptionBase>(new ExClass(*this)); \
    }

class Any;

class ExceptionBase : public std::exception
{
public:
    ExceptionBase(const std::string& message = "") throw();

    virtual ~ExceptionBase() throw();

    virtual std::tr1::shared_ptr<ExceptionBase> Clone() const;

    void Init(const char* file, const char* function, int line);

    virtual void SetCause(const ExceptionBase& cause);

    virtual void SetCause(std::tr1::shared_ptr<ExceptionBase> cause);

    virtual std::tr1::shared_ptr<ExceptionBase> GetCause() const;
   
    // Return the root cause, if the exception has the root cause; else return itself 
    virtual std::tr1::shared_ptr<ExceptionBase> GetRootCause() const;

    virtual std::string GetClassName() const;

    virtual std::string GetMessage() const;

    /**
     * With a) detailed throw location (file + lineno) (if availabe), b) Exception class name, and
     * c) content of GetMessage() (if not empty)
     */
    /* override */ const char* what() const throw();

    /**
     * Synonym of what(), except for the return type.
     */
    const std::string& ToString() const;

    const std::string& GetExceptionChain() const;

    std::string GetStackTrace() const;

protected:
    std::tr1::shared_ptr<ExceptionBase> mNestedException;
    std::string mMessage;
    const char* mFile;
    const char* mFunction;
    int mLine;

private:
    enum { MAX_STACK_TRACE_SIZE = 50 };
    void* mStackTrace[MAX_STACK_TRACE_SIZE];
    size_t mStackTraceSize;

    mutable std::string mWhat;

    friend Any ToJson(const ExceptionBase& e);
    friend void FromJson(ExceptionBase& e, const Any& a);
};

class InvalidOperation : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(InvalidOperation, ExceptionBase);
};

class RuntimeError : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(RuntimeError, ExceptionBase);
};

class TimeoutError : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(TimeoutError, ExceptionBase);
};

class LogicError : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(LogicError, ExceptionBase);
};

class OverflowError : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(OverflowError, ExceptionBase);
};

class AlreadyExistException: public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(AlreadyExistException, ExceptionBase);
};

class NotExistException: public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(NotExistException, ExceptionBase);
};

class NotImplementedException: public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(NotImplementedException, ExceptionBase);
};


// parameter invalid
class ParameterInvalidException : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(ParameterInvalidException, ExceptionBase);
};

// operation is denied
class AuthenticationFailureException : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(AuthenticationFailureException, ExceptionBase);
};

// base class for all exceptions in storage system
class StorageExceptionBase : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(StorageExceptionBase, ExceptionBase);
};

// when create an exist file
class FileExistException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(FileExistException, StorageExceptionBase);
};

// when open/delete/rename/... an non-exist file
class FileNotExistException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(FileNotExistException, StorageExceptionBase);
};

class DirectoryExistException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(DirectoryExistException, StorageExceptionBase);
};

class DirectoryNotExistException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(DirectoryNotExistException, StorageExceptionBase);
};

class SameNameEntryExistException : public StorageExceptionBase
{
    public:
        SHARELIB_JSON_DEFINE_EXCEPTION(SameNameEntryExistException, StorageExceptionBase);
};

// when append/delete a file that being appended
class FileAppendingException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(FileAppendingException, StorageExceptionBase);
};

// when opening a file that cannot be overwritten
class FileOverwriteException: public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(FileOverwriteException, StorageExceptionBase);
};

// running into unimplemented code
class UnimplementedException : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(UnimplementedException, ExceptionBase);
};

// when end of stream comes unexpected
class UnexpectedEndOfStreamException: public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(UnexpectedEndOfStreamException, StorageExceptionBase);
};
class DataUnavailableException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(DataUnavailableException, StorageExceptionBase);
};

// when append/commit, data stream is corrupted due to internal error
class StreamCorruptedException : public StorageExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(StreamCorruptedException, StorageExceptionBase);
};

class FileIOException : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(FileIOException, ExceptionBase);
};

SHARELIB_ES;
#endif

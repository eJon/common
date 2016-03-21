#ifndef SHARELIB_JSON_JSONIZABLE_H
#define SHARELIB_JSON_JSONIZABLE_H

#include "sharelib/common.h"
#include "sharelib/json/json.h"
#include "sharelib/json/exception.h"
#include "sharelib/json/base64.h"
#include <vector>
#include <deque>
#include <list>
#include <tr1/memory>
#include <set>
#include <map>
#include <sstream>

SHARELIB_BS;

class NotJsonizableException : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(NotJsonizableException, ExceptionBase);
};

// Usage:
// struct S : public Jsonizable
// {
//     int n;
//     string s;

//     /* override */ void Jsonize(JsonWrapper& json)
//     {
//         json.Jsonize("n", n);
//         json.Jsonize("s", s, "default_value");
//     }
// };


#define LOCAL_DEFINE(LOCAL_TYPE) \
inline Any ToJson(const LOCAL_TYPE& i) \
{ \
    return i; \
} \
\
inline void FromJson(LOCAL_TYPE& i, const Any& a) \
{ \
    i = sharelib::JsonNumberCast<LOCAL_TYPE>(a); \
} \

LOCAL_DEFINE(int8_t)
LOCAL_DEFINE(uint8_t)
LOCAL_DEFINE(int16_t)
LOCAL_DEFINE(uint16_t)
LOCAL_DEFINE(int32_t)
LOCAL_DEFINE(uint32_t)
LOCAL_DEFINE(int64_t)
LOCAL_DEFINE(uint64_t)

#undef LOCAL_DEFINE

inline Any ToJson(const Any& a);
inline void FromJson(Any& t, const Any& a);
inline Any ToJson(bool b);
inline void FromJson(bool& b, const Any& a);
inline Any ToJson(const std::string& s);
inline void FromJson(std::string& s, const Any& a);
inline Any ToJson(const char* s);
inline Any ToJson(double d);
inline void FromJson(double& d, const Any& a);
inline Any ToJson(const ExceptionBase& e);
inline void FromJson(ExceptionBase& e, const Any& a);

template<typename T>
Any ToJson(T* p);

template<typename T>
void FromJson(T*& p, const Any& a);

template<typename T>
Any ToJson(const std::tr1::shared_ptr<T>& p);

template<typename T>
void FromJson(std::tr1::shared_ptr<T>& p, const Any& a);

template<typename T>
Any ToJson(const std::vector<T>& m);

template<typename T>
void FromJson(std::vector<T>& tVec, const Any& a);

template<typename T>
Any ToJson(const std::deque<T>& m);

template<typename T>
void FromJson(std::deque<T>& tVec, const Any& a);

template<typename T, typename C>
Any ToJson(const std::set<T, C>& s);

template<typename T, typename C>
void FromJson(std::set<T, C>& tSet, const Any& a);

template<typename T>
Any ToJson(const std::list<T>& s);

template<typename T>
void FromJson(std::list<T>& tSet, const Any& a);


template<typename T, typename U>
Any ToJson(const std::pair<T, U>& s);

// NO FromJson(std::pair<T, U>& tPair, const Any& a); because it's not define how to convert from Any

template<typename T, typename U, typename C>
Any ToJson(const std::map<T, U, C>& m);

template<typename T, typename U, typename C>
void FromJson(std::map<T, U, C>& tMap, const Any& a); //because it's not define how to convert from Any

template<typename T, typename U, typename C>
Any ToJson(const std::multimap<T, U, C>& m);

template<typename T, typename U, typename C>
void FromJson(std::multimap<T, U, C>& tMap, const Any& a); //because it's not define how to convert from Any

template<typename T, typename C>
Any ToJson(const std::map<std::string, T, C>& m);

template<typename T, typename C>
void FromJson(std::map<std::string, T, C>& tMap, const Any& a);

template<typename T, typename U, typename C>
void FromJson(std::multimap<T, U, C>& tMap, const Any& a);

class Jsonizable
{
public:
    enum Mode { TO_JSON, FROM_JSON };

    class JsonWrapper
    {
    public:
        JsonWrapper()
            : mMode(TO_JSON)
        {}

        JsonWrapper(const Any& json)
            : mMode(FROM_JSON)
        {
            mJsonMap = AnyCast<std::map<std::string, Any> >(json);
        }

        Mode GetMode() const
        { return mMode; }

        std::map<std::string, Any> GetMap()
        { return mJsonMap; }

        template<typename T>
        void Jsonize(const std::string& key, T& value)
        {
            if (mMode == TO_JSON)
                mJsonMap[key] = ToJson(value);
            else
            {
                std::map<std::string, Any>::const_iterator it = mJsonMap.find(key);
                if (it == mJsonMap.end())
                {
                    std::stringstream ss;
                    ss << "context:{";
                    for (std::map<std::string, Any>::const_iterator it = mJsonMap.begin();
                         it != mJsonMap.end();
                         it++)
                    {
                        ss << "," << it->first;
                    }
                    ss << "}";
                    throw NotJsonizableException(key +
                            " not found when try to parse from Json." + ss.str());
                }
                FromJson(value, it->second);
            }
        }

        template<typename T>
        void Jsonize(const std::string& key, T& value, const T& defaultValue)
        {
            if (mMode == TO_JSON)
                mJsonMap[key] = ToJson(value);
            else
            {
                std::map<std::string, Any>::const_iterator it = mJsonMap.find(key);
                if (it == mJsonMap.end())
                    value = defaultValue;
                else
                    FromJson(value, it->second);
            }
        }

        void JsonizeBinary(const std::string& key, std::string& value)
        {
            if (mMode == TO_JSON)
            {
                std::istringstream in(value);
                std::ostringstream out;
                Base64Encoding(in, out);
                mJsonMap[key] = out.str();
            }
            else
            {
                std::map<std::string, Any>::const_iterator it = mJsonMap.find(key);
                if (it == mJsonMap.end())
                {
                    std::stringstream ss;
                    ss << "context:{";
                    for (std::map<std::string, Any>::const_iterator it = mJsonMap.begin();
                         it != mJsonMap.end();
                         it++)
                    {
                        ss << "," << it->first;
                    }
                    ss << "}";
                    SHARELIB_JSON_THROW(NotJsonizableException,
                                 key + " not found when try to parse from Json." + ss.str());
                }
                std::string v;
                FromJson(v, it->second);
                std::istringstream in(v);
                std::ostringstream out;
                Base64Decoding(in, out);
                value = out.str();
            }
        }

        void JsonizeBinary(const std::string& key, std::string& value, const std::string& defaultValue)
        {
            if (mMode == TO_JSON)
            {
                std::istringstream in(value);
                std::ostringstream out;
                Base64Encoding(in, out);
                mJsonMap[key] = out.str();
            }
            else
            {
                std::map<std::string, Any>::const_iterator it = mJsonMap.find(key);
                if (it == mJsonMap.end())
                {
                    value = defaultValue;
                }
                std::string v;
                FromJson(v, it->second);
                std::istringstream in(v);
                std::ostringstream out;
                Base64Decoding(in, out);
                value = out.str();
            }
        }

        void Jsonize(const std::string& key, std::string& value, const std::string& defaultValue)
        { return Jsonize<std::string>(key, value, defaultValue); }

        void Jsonize(const std::string& key, int64_t& value, const int64_t& defaultValue)
        { return Jsonize<int64_t>(key, value, defaultValue); }

    private:
        Mode mMode;
        std::map<std::string, Any> mJsonMap;
    };

    virtual ~Jsonizable() {}
    virtual void Jsonize(JsonWrapper& json) =0;
};

inline Any ToJson(const Any& a)
{
    return a;
}

inline void FromJson(Any& t, const Any& a)
{
    t = a;
}

inline Any ToJson(const Jsonizable& t)
{
    Jsonizable::JsonWrapper w;
    const_cast<Jsonizable&>(t).Jsonize(w);
    return w.GetMap();
}

inline void FromJson(Jsonizable& t, const Any& a)
{
    Jsonizable::JsonWrapper w(a);
    t.Jsonize(w);
}

inline Any ToJson(bool b)
{
    return b;
}

inline void FromJson(bool& b, const Any& a)
{
    b = AnyCast<bool>(a);
}

inline Any ToJson(const std::string& s)
{
    return s;
}

inline void FromJson(std::string& s, const Any& a)
{
    s = AnyCast<std::string>(a);
}

inline Any ToJson(const char* s)
{
    return std::string(s);
}

inline Any ToJson(float f)
{
    return f;
}

inline void FromJson(float& f, const Any& a)
{
    f = JsonNumberCast<float>(a);
}

inline Any ToJson(double d)
{
    return d;
}

inline void FromJson(double& d, const Any& a)
{
    d = JsonNumberCast<double>(a);
}


template<typename T>
Any ToJson(const std::vector<T>& m)
{
    std::vector<Any> anyVec;
    for (typename std::vector<T>::const_iterator it = m.begin();
         it != m.end();
         it++)
    {
        anyVec.push_back(ToJson(*it));
    }
    return anyVec;
}

template<typename T>
void FromJson(std::vector<T>& tVec, const Any& a)
{
    tVec.clear();
    std::vector<Any> anyVec = AnyCast<std::vector<Any> >(a);
    for (typename std::vector<Any>::const_iterator it = anyVec.begin();
         it != anyVec.end();
         it++)
    {
        T t;
        FromJson(t, *it);
        tVec.push_back(t);
    }
}

template<typename T>
Any ToJson(const std::deque<T>& m)
{
    std::vector<Any> anyVec;
    for (typename std::deque<T>::const_iterator it = m.begin();
         it != m.end();
         it++)
    {
        anyVec.push_back(ToJson(*it));
    }
    return anyVec;
}

template<typename T>
void FromJson(std::deque<T>& tDeque, const Any& a)
{
    tDeque.clear();
    std::vector<Any> anyVec = AnyCast<std::vector<Any> >(a);
    for (typename std::vector<Any>::const_iterator it = anyVec.begin();
         it != anyVec.end();
         it++)
    {
        T t;
        FromJson(t, *it);
        tDeque.push_back(t);
    }
}

template<typename T>
Any ToJson(const std::list<T>& m)
{
    std::vector<Any> anyVec;
    for (typename std::list<T>::const_iterator it = m.begin();
         it != m.end();
         it++)
    {
        anyVec.push_back(ToJson(*it));
    }
    return anyVec;
}

template<typename T>
void FromJson(std::list<T>& tList, const Any& a)
{
    tList.clear();
    std::vector<Any> anyVec = AnyCast<std::vector<Any> >(a);
    for (typename std::vector<Any>::const_iterator it = anyVec.begin();
         it != anyVec.end();
         it++)
    {
        T t;
        FromJson(t, *it);
        tList.push_back(t);
    }
}

template<typename T, typename U>
Any ToJson(const std::pair<T, U>& s)
{
    std::vector<Any> anyVec;
    anyVec.push_back(ToJson(s.first));
    anyVec.push_back(ToJson(s.second));
    return anyVec;
}

template<typename T, typename U>
void FromJson(std::pair<T, U>& s, const Any& a)
{
    std::vector<Any> anyVec;
    FromJson(anyVec, a);
    if (anyVec.size() != 2)
        SHARELIB_JSON_THROW(NotJsonizableException,
                     "FromJson(std::pair<T, U>& s, const Any& a): vector.size()!=2");
    FromJson(s.first, anyVec[0]);
    FromJson(s.second, anyVec[1]);
}

template<typename T, typename U, typename C>
Any ToJson(const std::map<T, U, C>& m)
{
    std::vector<Any> anyVec;
    std::vector<std::pair<T, U> > vec(m.begin(), m.end());

    for (typename std::vector<std::pair<T, U> >::const_iterator it = vec.begin();
         it != vec.end();
         it++)
    {
        anyVec.push_back(ToJson(*it));
    }

    return anyVec;
}

template<typename T, typename U, typename C>
Any ToJson(const std::multimap<T, U, C>& m)
{
    std::vector<Any> anyVec;
    std::vector<std::pair<T, U> > vec(m.begin(), m.end());

    for (typename std::vector<std::pair<T, U> >::const_iterator it = vec.begin();
         it != vec.end();
         it++)
    {
        anyVec.push_back(ToJson(*it));
    }

    return anyVec;
}

template<typename T, typename U, typename C>
void FromJson(std::map<T, U, C>& m, const Any& a)
{
    const std::vector<Any>& anyVec = AnyCast<std::vector<Any> >(a);
    std::pair<T, U> p;
    m.clear();
    typeof(m.begin()) insPos = m.begin();
    for (typename std::vector<Any>::const_iterator it = anyVec.begin();
         it != anyVec.end();
         it++)
    {
        FromJson(p, *it);
        insPos = m.insert(insPos, p);
    }
}

template<typename T, typename U, typename C>
void FromJson(std::multimap<T, U, C>& m, const Any& a)
{
    const std::vector<Any>& anyVec = AnyCast<std::vector<Any> >(a);
    std::pair<T, U> p;
    m.clear();
    typeof(m.begin()) insPos = m.begin();
    for (typename std::vector<Any>::const_iterator it = anyVec.begin();
         it != anyVec.end();
         it++)
    {
        FromJson(p, *it);
        insPos = m.insert(insPos, p);
        ++ insPos;
    }
}

template<typename T, typename C>
Any ToJson(const std::map<std::string, T, C>& m)
{
    std::map<std::string, Any> anyMap;
    std::map<std::string, Any>::iterator insPos = anyMap.begin();
    for (typename std::map<std::string, T>::const_iterator it = m.begin();
        it != m.end();
        it++)
    {
        insPos = anyMap.insert(insPos, std::make_pair(it->first, ToJson(it->second)));
        //anyMap[it->first] = ToJson(it->second);
    }
    return anyMap;
}

template<typename T, typename C>
void FromJson(std::map<std::string, T, C>& tMap, const Any& a)
{
    tMap.clear();
    typeof(tMap.begin()) insPos = tMap.begin();
    std::pair<std::string, T> p;
    const std::map<std::string, Any>& anyMap = AnyCast<std::map<std::string, Any> >(a);
    for (typename std::map<std::string, Any>::const_iterator it = anyMap.begin();
         it != anyMap.end();
         it++)
    {
        //FromJson(tMap[it->first], it->second);
        p.first = it->first;
        FromJson(p.second, it->second);
        insPos = tMap.insert(insPos, p);
    }
}

template<typename T, typename C>
Any ToJson(const std::set<T, C>& m)
{
    std::vector<T> anyVec(m.begin(), m.end());
    return ToJson(anyVec);
}

template<typename T, typename C>
void FromJson(std::set<T, C>& tSet, const Any& a)
{
    tSet.clear();
    typeof(tSet.begin()) insPos = tSet.begin();
    T t;
    const std::vector<Any>& anyVec = AnyCast<std::vector<Any> >(a);
    for (typename std::vector<Any>::const_iterator it = anyVec.begin();
         it != anyVec.end();
         it++)
    {
        FromJson(t, *it);
        insPos = tSet.insert(insPos, t);
    }
}

template<typename T>
Any ToJson(T* p)
{
    if (p)
        return ToJson(*p);
    else
        return Any();
}

template<typename T>
void FromJson(T*& p, const Any& a)
{
    if (a.GetType() == typeid(void))
        p = NULL;
    else
    {
        std::auto_ptr<T> t(new T);
        FromJson(*t, a);
        p = t.release();
    }
}

template<typename T>
Any ToJson(const std::tr1::shared_ptr<T>& p)
{
    if (p)
        return ToJson(*p);
    else
        return Any();
}

template<typename T>
void FromJson(std::tr1::shared_ptr<T>& p, const Any& a)
{
    if (a.GetType() == typeid(void))
        p.reset();
    else
    {
        std::tr1::shared_ptr<T> t(new T);
        FromJson(*t, a);
        std::swap(p, t);
    }
}

template<typename T>
std::string ToJsonStringCompact(const T& t)
{
    return ToString(ToJson(t), true);
}
template<typename T>
std::string ToJsonString(const T& t)
{
    return ToString(ToJson(t));
}

template<typename T>
void FromJsonString(T& t, const std::string& str)
{
    Any a = ParseJson(str);
    FromJson(t, a);
}

inline Any ToJson(const ExceptionBase& e)
{
    JsonMap m;
    m["Message"] = e.mMessage;
    m["File"] = std::string(e.mFile);
    m["Function"] = std::string(e.mFunction);
    m["Line"] = e.mLine;
    m["ClassName"] = e.GetClassName();
    if (e.mNestedException)
        m["Cause"] = ToJson(*e.mNestedException);
    return m;
}

inline void FromJson(ExceptionBase& e, const Any& a)
{
    JsonMap am = AnyCast<JsonMap>(a);
    e.mMessage = AnyCast<std::string>(am["Message"]);
    if (am.find("Cause") != am.end())
    {
        ExceptionBase cause;
        FromJson(cause, am["Cause"]);
        e.SetCause(cause);
    }
}

SHARELIB_ES;

#endif//SHARELIB_JSON_JSONIZABLE_H

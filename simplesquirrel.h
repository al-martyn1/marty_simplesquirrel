#pragma once

#include <simplesquirrel/simplesquirrel.hpp>

#include "marty_cpp/marty_cpp.h"

//
#include <exception>
#include <stdexcept>

// marty_simplesquirrel::

namespace marty_simplesquirrel {


//------------------------------
inline
std::string to_ascii(const char* str)
{
    return str ? std::string(str) : std::string();
}

//------------------------------
inline
std::string to_ascii(const std::string &str)
{
    return str;
}

//------------------------------
inline
std::string to_ascii(const std::wstring &str)
{
    std::string strRes; strRes.reserve(str.size());
    for(auto ch : str)
        strRes.append(1, (char)(unsigned char)ch);
    return strRes;
}

//------------------------------
inline
std::string to_ascii(const wchar_t* str)
{
    return str ? to_ascii(std::wstring(str)) : std::string();
}

//------------------------------
struct CToAscii
{
    std::string operator()(const char* str) const
    {
        return to_ascii(str);
    }

    std::string operator()(const std::string &str) const
    {
        return to_ascii(str);
    }

    std::string operator()(const std::wstring &str) const
    {
        return to_ascii(str);
    }

    std::string operator()(const wchar_t* str) const
    {
        return to_ascii(str);
    }

    std::string operator()() const
    {
    }

}; // struct CToAscii

//------------------------------



//------------------------------
inline
std::wstring to_wide(const wchar_t* str)
{
    return str ? std::wstring(str) : std::wstring();
}

//------------------------------
inline
std::wstring to_wide(const std::wstring &str)
{
    return str;
}

//------------------------------
inline
std::wstring to_wide(const std::string &str)
{
    std::wstring strRes; strRes.reserve(str.size());
    for(auto ch : str)
        strRes.append(1, (wchar_t)ch);
    return strRes;
}

//------------------------------
inline
std::wstring to_wide(const char* str)
{
    return str ? to_wide(std::string(str)) : std::wstring();
}

//------------------------------
struct CToWide
{
    std::wstring operator()(const wchar_t* str) const
    {
        return to_wide(str);
    }

    std::wstring operator()(const std::wstring &str) const
    {
        return to_wide(str);
    }

    std::wstring operator()(const std::string &str) const
    {
        return to_wide(str);
    }

    std::wstring operator()(const char* str) const
    {
        return to_wide(str);
    }

}; // struct CToWide

//------------------------------



//------------------------------
#if !defined(SQUNICODE)

    template<typename CharType>
    inline std::string to_sqstring(const CharType* pStr)
    {
        return to_ascii(pStr);
    }

    template<typename StringType>
    inline std::string to_sqstring(const StringType &str)
    {
        return to_ascii(str);
    }

#else

    template<typename CharType>
    inline std::wstring to_sqstring(const CharType* pStr)
    {
        return to_wide(pStr);
    }

    template<typename StringType>
    inline std::wstring to_sqstring(const StringType &str)
    {
        return to_wide(str);
    }

#endif

//------------------------------



//----------------------------------------------------------------------------
template<typename TargetType> inline
TargetType fromObjectConvertHelper(ssq::Object &o, const SQChar *paramName)
{
    MARTY_DC_BIND_SQUIRREL_ASSERT_FAIL(); // not implemented for generic type
}

//----------------------------------------------------------------------------
template<> inline
bool fromObjectConvertHelper<bool>(ssq::Object &o, const SQChar *paramName)
{
    (void)paramName;

    if (o.isNull() || o.isEmpty())
    {
        return false;
    }

    ssq::Type t = o.getType();
    switch(t)
    {
        case ssq::Type::INTEGER:
            return o.toInt()!=0;

        case ssq::Type::FLOAT:
            return ((int)(o.toFloat()+0.5))!=0;

        case ssq::Type::STRING:
            {
                auto str = o.toString();
                try
                {
                    return str==_SC("TRUE") || str==_SC("True") || str==_SC("true") 
                        || str==_SC("YES") || str==_SC("Yes") || str==_SC("yes") || str==_SC("Y") || str==_SC("y")
                        || str==_SC("1")
                         ;
                }
                catch(const std::invalid_argument &)
                {
                    throw ssq::TypeException("invalid argument", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
                catch(const std::out_of_range &)
                {
                    throw ssq::TypeException("out of range", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
                catch(...)
                {
                    throw ssq::TypeException("unknown error", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
            }

        case ssq::Type::BOOL:
            {
                return o.toBool();
            }

        case ssq::Type::NULLPTR:
        case ssq::Type::TABLE:
        case ssq::Type::ARRAY:
        case ssq::Type::USERDATA:
        case ssq::Type::CLOSURE:
        case ssq::Type::NATIVECLOSURE:
        case ssq::Type::GENERATOR:
        case ssq::Type::USERPOINTER:
        case ssq::Type::THREAD:
        case ssq::Type::FUNCPROTO:
        case ssq::Type::CLASS:
        case ssq::Type::INSTANCE:
        case ssq::Type::WEAKREF:
        case ssq::Type::OUTER:
            [[fallthrough]];		
        default: {}
    }

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
float fromObjectConvertHelper<float>(ssq::Object &o, const SQChar *paramName)
{
    (void)paramName;

    if (o.isNull() || o.isEmpty())
    {
        return 0.0;
    }

    ssq::Type t = o.getType();
    switch(t)
    {
        case ssq::Type::INTEGER:
            return (float)o.toInt();

        case ssq::Type::FLOAT:
            return o.toFloat();

        case ssq::Type::STRING:
            {
                auto str = o.toString();
                try
                {
                    return std::stof(str);
                }
                catch(const std::invalid_argument &)
                {
                    throw ssq::TypeException("invalid argument", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
                catch(const std::out_of_range &)
                {
                    throw ssq::TypeException("out of range", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
                catch(...)
                {
                    throw ssq::TypeException("unknown error", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
            }

        case ssq::Type::BOOL:
            {
                return o.toBool() ? 1.0f : 0.0f;
            }

        case ssq::Type::NULLPTR:
        case ssq::Type::TABLE:
        case ssq::Type::ARRAY:
        case ssq::Type::USERDATA:
        case ssq::Type::CLOSURE:
        case ssq::Type::NATIVECLOSURE:
        case ssq::Type::GENERATOR:
        case ssq::Type::USERPOINTER:
        case ssq::Type::THREAD:
        case ssq::Type::FUNCPROTO:
        case ssq::Type::CLASS:
        case ssq::Type::INSTANCE:
        case ssq::Type::WEAKREF:
        case ssq::Type::OUTER:
            [[fallthrough]];		
        default: {}
    }

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
int fromObjectConvertHelper<int>(ssq::Object &o, const SQChar *paramName)
{
    (void)paramName;

    if (o.isNull() || o.isEmpty())
    {
        return 0;
    }

    ssq::Type t = o.getType();
    switch(t)
    {
        case ssq::Type::INTEGER:
            return (int)o.toInt();

        case ssq::Type::FLOAT:
            {
                float f = o.toFloat();
                return (int)(f+0.5);
            }

        case ssq::Type::STRING:
            {
                auto str = o.toString();
                try
                {
                    return std::stoi(str);
                }
                catch(const std::invalid_argument &)
                {
                    throw ssq::TypeException("invalid argument", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
                catch(const std::out_of_range &)
                {
                    throw ssq::TypeException("out of range", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
                catch(...)
                {
                    throw ssq::TypeException("unknown error", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));
                }
            }

        case ssq::Type::BOOL:
            {
                return o.toBool() ? 1 : 0;
            }

        case ssq::Type::NULLPTR:
        case ssq::Type::TABLE:
        case ssq::Type::ARRAY:
        case ssq::Type::USERDATA:
        case ssq::Type::CLOSURE:
        case ssq::Type::NATIVECLOSURE:
        case ssq::Type::GENERATOR:
        case ssq::Type::USERPOINTER:
        case ssq::Type::THREAD:
        case ssq::Type::FUNCPROTO:
        case ssq::Type::CLASS:
        case ssq::Type::INSTANCE:
        case ssq::Type::WEAKREF:
        case ssq::Type::OUTER:
            [[fallthrough]];		
        default: {}
    }

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::FLOAT), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
ssq::sqstring fromObjectConvertHelper<ssq::sqstring>(ssq::Object &o, const SQChar *paramName)
{
    (void)paramName;

    if (o.isNull() || o.isEmpty())
    {
        return 0;
    }

    ssq::Type t = o.getType();
    switch(t)
    {
        case ssq::Type::INTEGER:
            return to_sqstring(std::to_string(o.toInt()));

        case ssq::Type::FLOAT:
            return to_sqstring(std::to_string(o.toFloat()));

        case ssq::Type::STRING:
            return o.toString();

        case ssq::Type::BOOL:
            {
                return o.toBool() ? _SC("true") : _SC("false");
            }

        case ssq::Type::NULLPTR:
        case ssq::Type::TABLE:
        case ssq::Type::ARRAY:
        case ssq::Type::USERDATA:
        case ssq::Type::CLOSURE:
        case ssq::Type::NATIVECLOSURE:
        case ssq::Type::GENERATOR:
        case ssq::Type::USERPOINTER:
        case ssq::Type::THREAD:
        case ssq::Type::FUNCPROTO:
        case ssq::Type::CLASS:
        case ssq::Type::INSTANCE:
        case ssq::Type::WEAKREF:
        case ssq::Type::OUTER:
            [[fallthrough]];		
        default: {}
    }

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::STRING), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// https://learn.microsoft.com/ru-ru/cpp/cpp/ellipses-and-variadic-templates?view=msvc-170

inline
void makeEnumValuesVectorHelper(std::vector< std::pair<std::string, int> > &vec)
{
    (void)vec;
    // (void)flagEnum;
}

template<typename EnumVal> inline
void makeEnumValuesVectorHelper(std::vector< std::pair<std::string, int> > &vec, EnumVal val)
{
    auto strVal = enum_serialize(val);

    if (strVal.empty()) // || strVal=="0"
    {
        // strVal = "None";
        // Если флаговый enum и передали что-то неизвестное, то будет пустая строка
        // Если enum и получили пустую строку - то вообще-то это что-то странное
        throw std::runtime_error("Invalid enum/flag value");
    }

    // Значение "0" вообще не трогаем, оно может быть вполне валидным

    vec.emplace_back(strVal, (int)val);
}

template<typename First, typename... EnumVal> inline
void makeEnumValuesVectorHelper(std::vector< std::pair<std::string, int> > &vec, First first, EnumVal... vals)
{
    makeEnumValuesVectorHelper(vec, first);
    makeEnumValuesVectorHelper(vec, vals...);
}

template<typename... EnumVal> inline
std::vector< std::pair<std::string, int> > makeEnumValuesVector( EnumVal... vals )
{
    std::vector< std::pair<std::string, int> > vec;
    makeEnumValuesVectorHelper(vec, vals...);
    return vec;
}

template<typename First, typename... EnumVal> inline
std::vector< std::pair<std::string, int> > makeEnumValuesVector( First first, EnumVal... vals )
{
    std::vector< std::pair<std::string, int> > vec;
    makeEnumValuesVectorHelper(vec, first);
    makeEnumValuesVectorHelper(vec, vals...);
    return vec;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename... EnumVal> inline
ssq::sqstring makeEnumScriptString( const std::string &enumPrefix, const std::string &enumNameOnly, char itemSep, char enumSep, std::set<ssq::sqstring> &known, EnumVal... vals)
{
    known.insert(marty_simplesquirrel::to_sqstring(enumNameOnly));

    std::string enumName = enumPrefix+enumNameOnly;

    std::vector< std::pair<std::string, int> > valNameVec = makeEnumValuesVector(vals...);

    std::string res = "enum " + enumName + "{";

    for(auto p: valNameVec)
    {
        res.append(p.first);
        res.append("=");
        res.append(std::to_string(p.second));
        res.append(1, itemSep );
    }

    res.append("}");
    res.append(1, enumSep );

    return marty_simplesquirrel::to_sqstring(res);
}

//----------------------------------------------------------------------------
template<typename... EnumVal> inline
ssq::sqstring makeFlagScriptString( const std::string &enumPrefix, const std::string &enumNameOnly, char itemSep, char enumSep, std::set<ssq::sqstring> &known, EnumVal... vals)
{
    known.insert(utils::to_sqstring(enumNameOnly));

    std::string enumName = enumPrefix+enumNameOnly;

    std::vector< std::pair<std::string, int> > valNameVec = makeFlagValuesVector(vals...);

    std::string res = "enum " + enumName + "{";

    for(auto p: valNameVec)
    {
        res.append(p.first);
        res.append("=");

        if (!p.first.empty())
        {
            char firstCh = !p.first[0];
            if (firstCh>='0' && firstCh<='9')
            {
                res.append(std::to_string("_"));
            }
        }

        res.append(std::to_string(p.second));
        res.append(1, itemSep );
    }

    res.append("}");
    res.append(1, enumSep );

    return utils::to_sqstring(res);
}

//----------------------------------------------------------------------------
template<typename... EnumVal> inline
ssq::sqstring makeEnumClassScriptString( const std::string &enumPrefix, const std::string &enumNameOnly, const std::string &itemTypeString, char itemSep, char enumSep, EnumVal... vals)
{
    //known.insert(utils::to_sqstring(enumNameOnly));
    //(void)known;

    std::string enumName = enumPrefix+enumNameOnly;

    std::vector< std::pair<std::string, int> > valNameVec = makeEnumValuesVector(vals...);

    std::string res = "class " + enumName + "{";

    for(auto p: valNameVec)
    {
        res.append("static ");
        if (!p.first.empty())
        {
            char firstCh = p.first[0];
            if (firstCh>='0' && firstCh<='9')
            {
                res.append("_");
            }
        }

        res.append(p.first);
        //res.append("<-");
        res.append("=");
        if (!itemTypeString.empty())
        {
            res.append(itemTypeString);
            res.append("(");
        }

        res.append(std::to_string(p.second));

        if (!itemTypeString.empty())
        {
            res.append(")");
        }
        res.append(1, itemSep );
    }

    res.append("}");
    res.append(1, enumSep );

    return marty_simplesquirrel::to_sqstring(res);
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Table findOrCreateTable(TVM &vm, const std::string &name)
{
    try
    {
        ssq::Object obj = vm.find(to_sqstring(name).c_str());
        if (obj.getType()==ssq::Type::TABLE)
        {
            return ssq::Table(obj);
        }

        throw std::runtime_error("marty_simplesquirrel::findOrCreateTable: failed to find or create table");

    }
    catch (ssq::NotFoundException&)
    {
    }

    return vm.addTable(to_sqstring(name).c_str());
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Function findFunc(TVM &vm, const std::string &name)
{
    std::vector<std::string> names = marty_cpp::simple_string_split(name, '.' /* , -1 */  /* nSplits */ );

    if (names.empty())
    {
        throw std::runtime_error("marty_simplesquirrel::findFunc: invalid function name");
    }

    std::vector<std::string>::const_iterator e = names.end();
    --e; // Оставляем имя функции на сладкое

    ssq::Object obj = vm;

    std::vector<std::string>::const_iterator it = names.begin();
    for(; it!=e; ++it)
    {
        ssq::Object objNext = obj.find(to_sqstring(*it).c_str());
        if (objNext.isNull())
        {
            throw std::runtime_error("marty_simplesquirrel::findFunc: table not found");
        }

        if (objNext.getType()!=ssq::Type::TABLE)
        {
            throw std::runtime_error("marty_simplesquirrel::findFunc: found object is not a table");
        }

        obj = objNext;
    }

    ssq::Table t = ssq::Table(obj);

    return t.findFunc(to_sqstring(*e).c_str());

}

// //Object find(const SQChar* name) const;
//  
// inline std::string to_sqstring(const CharType* pStr)
// vm.findFunc(_SC("onPaint"));
// Function findFunc(const SQChar* name) const;



} // namespace marty_simplesquirrel


#pragma once


#include "umba/umba.h"
//

#include "warnings_disable.h"
//

#include <squirrel.h>
#include <simplesquirrel/simplesquirrel.hpp>
//
#include "warnings_restore.h"
//
#include "marty_cpp/marty_cpp.h"
#include "marty_cpp/marty_enum.h"
//
#include "umba/filename.h"
#include "umba/utf8.h"

//
#include <exception>
#include <stdexcept>
#include <optional>
#include <type_traits>
#include <map>
#include <unordered_map>

//
#include "types.h"
#include "enums.h"

#include "marty_tr/marty_tr.h"


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


    inline
    std::string toSqStringFromUtf8(const std::string &str)
    {
        return str;
    }

    inline
    std::string toSqStringFromUtf8(const char *pStr)
    {
        return pStr ? std::string() : std::string(pStr);
    }

    inline
    std::string toSqStringFromUtf8(const std::wstring &str)
    {
        return umba::toUtf8(str);
    }

    inline
    std::string toSqStringFromUtf8(const wchar_t *pStr)
    {
        return pStr ? std::string() : umba::toUtf8(pStr);
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


    inline
    std::wstring toSqStringFromUtf8(const std::string &str)
    {
        return umba::fromUtf8(str);
    }

    inline
    std::wstring toSqStringFromUtf8(const char *pStr)
    {
        return pStr ? std::wstring() : umba::fromUtf8(pStr);
    }

    inline
    std::wstring toSqStringFromUtf8(const std::wstring &str)
    {
        return str;
    }

    inline
    std::wstring toSqStringFromUtf8(const wchar_t *pStr)
    {
        return pStr ? std::wstring() : std::wstring(pStr);
    }


#endif


//------------------------------



//----------------------------------------------------------------------------
template<typename TargetType> inline
TargetType fromObjectConvertHelper(const ssq::Object &o, const SQChar *paramName)
{
    MARTY_DC_BIND_SQUIRREL_ASSERT_FAIL(); // not implemented for generic type
}

//----------------------------------------------------------------------------
template<> inline
bool fromObjectConvertHelper<bool>(const ssq::Object &o, const SQChar *paramName)
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

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::BOOL), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
float fromObjectConvertHelper<float>(const ssq::Object &o, const SQChar *paramName)
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
int fromObjectConvertHelper<int>(const ssq::Object &o, const SQChar *paramName)
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

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::INTEGER), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
unsigned fromObjectConvertHelper<unsigned>(const ssq::Object &o, const SQChar *paramName)
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
            return (unsigned)(int)o.toInt();

        case ssq::Type::FLOAT:
            {
                float f = o.toFloat();
                return (unsigned)(int)(f+0.5);
            }

        case ssq::Type::STRING:
            {
                auto str = o.toString();
                try
                {
                    return (unsigned)std::stoi(str);
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
                return o.toBool() ? 1u : 0u;
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

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::INTEGER), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
ssq::sqstring fromObjectConvertHelper<ssq::sqstring>(const ssq::Object &o, const SQChar *paramName)
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
template<typename TargetType, typename BasicType> inline
std::vector<TargetType> fromArrayObjectToVectorConvertEx(const ssq::Object &o, const SQChar *paramName, bool allowSingleVal=false)
{
    (void)paramName;

    if (o.isNull() || o.isEmpty())
    {
        return std::vector<TargetType>();
    }

    std::vector<TargetType> resVec;

    ssq::Type t = o.getType();
    switch(t)
    {
        case ssq::Type::INTEGER:
        case ssq::Type::FLOAT:
        case ssq::Type::STRING:
        case ssq::Type::BOOL:
            if (allowSingleVal)
            {
                resVec.emplace_back(TargetType(fromObjectConvertHelper<BasicType>(o, paramName)));
                return resVec;
            }
            break;

        case ssq::Type::ARRAY:
        {
            //TODO: !!! Не работает, разобраться. convertRaw() почему-то после первого элемента на втором кидает исключение
            // TypeException("Failed to pop value from back of the array");
            // //ssq::Array a = o.toArray();
            // std::vector<ssq::Object> vObjects = o.toArray().convertRaw();
            // for(auto obj : vObjects)
            // {
            //     resVec.emplace_back((TargetType)fromObjectConvertHelper<BasicType>(obj, paramName));
            // }

            ssq::Array a     = o.toArray();
            std::size_t size = a.size();
    
            for(std::size_t i=0; i!=size; ++i)
            {
                // DrawingCoords drawingCoords = a.get<DrawingCoords>(i);

                auto obj = a.get<ssq::Object>(i);
                resVec.emplace_back(TargetType(fromObjectConvertHelper<BasicType>(obj, paramName)));
            }

            return resVec;
        }

        case ssq::Type::NULLPTR:
        case ssq::Type::TABLE:
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
template<typename TargetType> inline
std::vector<TargetType> fromArrayObjectToVectorConvert(const ssq::Object &o, const SQChar *paramName, bool allowSingleVal=false)
{
    return fromArrayObjectToVectorConvertEx<TargetType, TargetType>(o, paramName, allowSingleVal);
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename TargetClassType, typename BoundClassType>
std::vector<TargetClassType> fromArrayObjectToClassVectorConvertEx(const ssq::Object &o, const SQChar *paramName)
{
    (void)paramName;

    std::vector<TargetClassType>  resVec;

    if (o.isNull() || o.isEmpty())
    {
        return resVec;
    }

    ssq::Type t = o.getType();
    switch(t)
    {
        case ssq::Type::ARRAY:
        {
            ssq::Array a     = o.toArray();
            std::size_t size = a.size();
    
            for(std::size_t i=0; i!=size; ++i)
            {
                auto obj = a.get<ssq::Object>(i);
                ssq::Type ot = obj.getType();
                if (ot!=ssq::Type::INSTANCE)
                {
                    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::ARRAY), ssq::typeToStr(t));
                }

                BoundClassType bcVal = obj.to<BoundClassType>();
                resVec.emplace_back(TargetClassType(bcVal));
            }

            return resVec;
        }

        case ssq::Type::INTEGER:
        case ssq::Type::FLOAT:
        case ssq::Type::STRING:
        case ssq::Type::BOOL:
        case ssq::Type::NULLPTR:
        case ssq::Type::TABLE:
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

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::ARRAY), ssq::typeToStr(t));
}

//----------------------------------------------------------------------------
template<typename BoundClassType>
std::vector<BoundClassType> fromArrayObjectToClassVectorConvert(const ssq::Object &o, const SQChar *paramName)
{
    return fromArrayObjectToClassVectorConvertEx<BoundClassType,BoundClassType>(o, paramName) const
}


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
#if 0
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
#endif
//----------------------------------------------------------------------------
#if 0
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
#endif

//----------------------------------------------------------------------------
template<typename... EnumVal> inline
ssq::sqstring makeEnumClassScriptString( const std::string &enumPrefix
                                       , const std::string &enumNameOnly
                                       , const std::string &itemTypeString
                                       , const EnumScriptGenerationOptions &generationOptions
                                       // , char itemSep
                                       // , char enumSep
                                       , EnumVal... vals
                                       )
{
    //known.insert(utils::to_sqstring(enumNameOnly));
    //(void)known;

    std::string enumFqName;
    if (enumPrefix.empty() || enumPrefix.back()=='.')
    {
        enumFqName = enumPrefix + enumNameOnly;
    }
    else
    {
        enumFqName = enumPrefix + "." + enumNameOnly;
    }

    std::vector< std::pair<std::string, int> > valNameVec = makeEnumValuesVector(vals...);

    std::string res;
    bool bMultiline = true;

    auto appendLinefeed = [&]()
    {
        if (bMultiline)
        {
            res.append(1, '\n');
        }
    };

    auto appendItemSep = [&]()
    {
        if (!bMultiline)
        {
            res.append(1, ' ');
        }
    };

    auto appendIndend = [&]()
    {
        res.append(bMultiline ? 4u : 1u, ' ');
    };

    auto appendName = [&](const std::string &name)
    {
        if (!name.empty())
        {
            char firstCh = name[0];
            if (firstCh>='0' && firstCh<='9')
            {
                res.append(1,'_');
            }
        }

        res.append(name);
    
    };

    auto appendValue = [&](auto val)
    {
        if (!itemTypeString.empty())
        {
            res.append(itemTypeString);
            res.append("(");
        }

        res.append(std::to_string(val));

        if (!itemTypeString.empty())
        {
            res.append(")");
        }
    };


    if (generationOptions.generationType==EnumScriptGenerationType::singleLineScript || generationOptions.generationType==EnumScriptGenerationType::multiLineScript)
    {
        bMultiline = generationOptions.generationType==EnumScriptGenerationType::multiLineScript;
        res = "class " + enumFqName + "{";
        appendLinefeed();
    
        for(auto p: valNameVec)
        {
            appendIndend();
            res.append("static ");
            appendName(p.first);
            //res.append("<-");

            res.append("=");
            appendValue(p.second);

            //res.append(1,';'); // itemSep
            //res.append(1,' '); // itemSep
            appendItemSep();
            appendLinefeed();
        }
    
        res.append("}");
        //res.append(1, enumSep );
        res.append(1,';');
        appendLinefeed();
        appendLinefeed();

        return marty_simplesquirrel::to_sqstring(res);

    }
    else if (generationOptions.generationType==EnumScriptGenerationType::trTemplate)
    {
        marty_tr::tr_add(std::string("__DESCRIPTION"), std::string(), enumFqName, generationOptions.mdLang);

        for(auto p: valNameVec)
        {
            marty_tr::tr_add(p.first, std::string() /* p.first */ , enumFqName, generationOptions.mdLang);
        }

        marty_tr::tr_add(std::string("__REMARKS"), std::string(), enumFqName, generationOptions.mdLang);

        return toSqStringFromUtf8(marty_tr::tr_serialize_translations(marty_tr::tr_get_all_translations(), 2  /* indent */));
    }
    else if (generationOptions.generationType==EnumScriptGenerationType::mdDoc)
    {
        bMultiline = true;
        
        res.append(generationOptions.mdSectionSep);
        appendLinefeed();
        res.append(generationOptions.mdSectionLevel, '#'); res.append(1, ' '); res.append(enumFqName);

        if (marty_tr::tr_has_msg(std::string("__DESCRIPTION"), enumFqName, generationOptions.mdLang))
        {
            res.append(marty_tr::tr(std::string("__DESCRIPTION"), enumFqName, generationOptions.mdLang));
            appendLinefeed(); appendLinefeed();
        }
    
        for(auto p: valNameVec)
        {
            appendLinefeed(); appendLinefeed();
            res.append("**");
            appendName(p.first);
            res.append("**");
            if (generationOptions.mdAddVals)
            {
                res.append(" (");
                appendValue(p.second);
                res.append(")");
            }
    
            res.append(" - ");
            res.append(marty_tr::tr(p.first, enumFqName, generationOptions.mdLang));
        }
    
        appendLinefeed(); appendLinefeed();

        if (marty_tr::tr_has_msg(std::string("__REMARKS"), enumFqName, generationOptions.mdLang))
        {
            res.append(generationOptions.mdSectionLevel+1, '#'); res.append(1, ' '); 
            res.append(marty_tr::tr(std::string("remarks-subsection-title"), std::string("md-common"), generationOptions.mdLang));
            appendLinefeed(); appendLinefeed();
            res.append(marty_tr::tr(std::string("__REMARKS"), enumFqName, generationOptions.mdLang));
            appendLinefeed(); appendLinefeed();
        }

        return toSqStringFromUtf8(res);
        

        //tr_add(const std::string &msgId, const std::string &msgText, const std::string &catId)

    // bool                          mdGenerateTr   = false;
    // std::size_t                   mdSectionLevel = 3;
    // std::string                   mdLang         = "en";
    // std::string                   mdSectionSep   = "-------";

    //    std::string res;

        // marty_tr::
        // std::string tr(const std::string &msgId, std::string catId, std::string langId)
        // std::string tr(const std::string &msgId, const std::string &catId)
    }
    else // EnumScriptGenerationType::mdDoc
    {
        throw std::runtime_error("Not implemented");
    }
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
    catch (const ssq::NotFoundException&)
    {
    }

    // return std::move(vm.addTable(to_sqstring(name).c_str()));
    return vm.addTable(to_sqstring(name).c_str());
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Table findOrCreateTable(TVM &vm, const std::wstring &name)
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
    catch (const ssq::NotFoundException&)
    {
    }

    // return std::move(vm.addTable(to_sqstring(name).c_str()));
    return vm.addTable(to_sqstring(name).c_str());
}

//----------------------------------------------------------------------------
inline
std::string prepareFunctionFullQualifiedName(const std::string &name)
{
    // В squirrel полностью квалифицированное имя функции разделяется как и в плюсиках, через двойное двоеточие, но это неудобно
    // А иногда через точку, там хз как
    // В именах не допустим символ '/', поэтому, раз пошла такая пьянка, то добавим возможность задавать FQNs как пути в юниксах
    std::string res;
    bool prevColon = false;

    for(const auto ch: name)
    {
        if (ch==':' || ch=='/')
        {
            if (prevColon)
            {
                // ничего делать не надо
            }
            else
            {
                res.append(1,'.');
                prevColon = true;
            }
        }
        else
        {
            res.append(1, ch);
            prevColon = false;
        }
    }

    return res;
}

//----------------------------------------------------------------------------
inline
std::wstring prepareFunctionFullQualifiedName(const std::wstring &name)
{
    // В squirrel полностью квалифицированное имя функции разделяется как и в плюсиках, через двойное двоеточие, но это неудобно
    // А иногда через точку, там хз как
    // В именах не допустим символ '/', поэтому, раз пошла такая пьянка, то добавим возможность задавать FQNs как пути в юниксах
    std::wstring res;
    bool prevColon = false;

    for(const auto ch: name)
    {
        if (ch==L':' || ch==L'/')
        {
            if (prevColon)
            {
                // ничего делать не надо
            }
            else
            {
                res.append(1,L'.');
                prevColon = true;
            }
        }
        else
        {
            res.append(1, ch);
            prevColon = false;
        }
    }

    return res;
}


//----------------------------------------------------------------------------
template<typename TObject, typename IterType > inline
bool findObjectByFullQualifiedName(TObject &rootObj, IterType b, IterType e, ssq::Object &objOut)
{
    ssq::Object obj = rootObj;

    for(; b!=e; ++b)
    {
        if (b->empty())
        {
            // Просто пропускаем пустые имена
            continue;
        }

        try
        {
            ssq::Object objNext = obj.find(to_sqstring(*b).c_str());
            if (objNext.isNull())
            {
                // throw std::runtime_error("marty_simplesquirrel::findObjectByFullQualifiedName: object not found");
                return false;
            }
    
            obj = objNext;
        }
        catch(...)
        {
            return false;
        }
    }

    objOut = obj;

    return true;
}


//----------------------------------------------------------------------------
template<typename TObject> inline
bool findObjectByFullQualifiedName(TObject &rootObj, const ssq::sqstring &name, ssq::Object &objFound)
{
    typedef ssq::sqstring::value_type CharType;

    auto preparedFqn = prepareFunctionFullQualifiedName(name);

    std::vector<ssq::sqstring> names = marty_cpp::simple_string_split(preparedFqn, (CharType)'.' /* , -1 */  /* nSplits */ );
    if (names.empty() || (names.size()==1 && names[0].empty())) //TODO: !!! Bug workaround, bug is need to be fixed
    {
        objFound = rootObj;
        return true;
    }

    return findObjectByFullQualifiedName(rootObj, names.cbegin(), names.cend(), objFound);
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Table findOrCreateTableByFullQualifiedName(TVM &vm, const std::string &name)
{
    auto preparedFqn = prepareFunctionFullQualifiedName(name);

    std::vector<std::string> names = marty_cpp::simple_string_split(preparedFqn, '.' /* , -1 */  /* nSplits */ );
    if (names.empty() || (names.size()==1 && names[0].empty())) //TODO: !!! Bug workaround, bug is need to be fixed
    {
        throw std::runtime_error("Invalid table FQN taken");
    }

    ssq::Table resTable = ssq::Table(vm);
    std::vector<std::string>::const_iterator nit = names.begin();
    for(; nit!=names.end(); ++nit)
    {
        resTable = findOrCreateTable(resTable, *nit);
    }

    // return std::move(resTable);
    return resTable;
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Table findOrCreateTableByFullQualifiedName(TVM &vm, const std::wstring &name)
{
    auto preparedFqn = prepareFunctionFullQualifiedName(name);

    std::vector<std::wstring> names = marty_cpp::simple_string_split(preparedFqn, L'.' /* , -1 */  /* nSplits */ );
    if (names.empty() || (names.size()==1 && names[0].empty())) //TODO: !!! Bug workaround, bug is need to be fixed
    {
        throw std::runtime_error("Invalid table FQN taken");
    }

    ssq::Table resTable = ssq::Table(vm);
    std::vector<std::wstring>::const_iterator nit = names.begin();
    for(; nit!=names.end(); ++nit)
    {
        resTable = findOrCreateTable(resTable, *nit);
    }

    // return std::move(resTable);
    return resTable;
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Table findOrCreateTableByFqn(TVM &vm, const std::string &name)
{
    // return std::move(findOrCreateTableByFullQualifiedName(vm, name));
    return findOrCreateTableByFullQualifiedName(vm, name);
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Table findOrCreateTableByFqn(TVM &vm, const std::wstring &name)
{
    // return std::move(findOrCreateTableByFullQualifiedName(vm, name));
    return findOrCreateTableByFullQualifiedName(vm, name);
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Function findFuncEx(TVM &vm, const ssq::sqstring &name, ssq::Object &objFound)
{
    typedef ssq::sqstring::value_type CharType;

    auto preparedFqn = prepareFunctionFullQualifiedName(name);
    std::vector<ssq::sqstring> names = marty_cpp::simple_string_split(preparedFqn, (CharType)'.' /* , -1 */  /* nSplits */ );

    // if (names.empty() || (names.size()==1 && names[1].empty()))
    if (names.empty() || names.back().empty()) //TODO: !!! Bug workaround, bug is need to be fixed
    {
        throw std::runtime_error("marty_simplesquirrel::findFunc: invalid function name");
    }

    std::vector<ssq::sqstring>::const_iterator e = names.end();
    --e; // Оставляем имя функции на сладкое


    if (!findObjectByFullQualifiedName(vm, names.cbegin(), e, objFound))
    {
        throw std::runtime_error("marty_simplesquirrel::findFunc: table not found");
    }

    // if (objFound.getType()!=ssq::Type::TABLE && objFound.getType()!=ssq::Type::INSTANCE)
    // {
    //     throw std::runtime_error("marty_simplesquirrel::findFunc: found object is not a table or instance");
    // }

    // ssq::Object obj = vm;
    //  
    // std::vector<std::string>::const_iterator it = names.begin();
    // for(; it!=e; ++it)
    // {
    //     ssq::Object objNext = obj.find(to_sqstring(*it).c_str());
    //     if (objNext.isNull())
    //     {
    //         throw std::runtime_error("marty_simplesquirrel::findFunc: table not found");
    //     }
    //  
    //     if (objNext.getType()!=ssq::Type::TABLE)
    //     {
    //         throw std::runtime_error("marty_simplesquirrel::findFunc: found object is not a table");
    //     }
    //  
    //     obj = objNext;
    // }

    // Валидно для
    // include\simplesquirrel\class.hpp
    // include\simplesquirrel\table.hpp
    // include\simplesquirrel\vm.hpp

    auto funcName = to_sqstring(*e);

    if (objFound.getType()==ssq::Type::TABLE)
    {
        ssq::Table t = ssq::Table(objFound);
        return t.findFunc(funcName.c_str());
    }
    else if (objFound.getType()==ssq::Type::INSTANCE)
    {
        //ssq::Table t = ssq::Table(objFound);
        //return t.findFunc(to_sqstring(*e).c_str());

    // Function Table::findFunc(const SQChar* name) const {
    //     Object object = Object::find(name);
    //     return Function(object);
    // }

    // Function Class::findFunc(const SQChar* name) const {
    //     Object object = Object::find(name);
    //     return Function(object);
    // }

        ssq::Object funcObject = objFound.find(funcName.c_str());
        return ssq::Function(funcObject);

    }

    throw std::runtime_error("marty_simplesquirrel::findFunc: found object is not a table or instance");

}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Function findFunc(TVM &vm, const ssq::sqstring &name)
{
    ssq::Object objFound;
    return findFuncEx(vm, name, objFound);
}

//----------------------------------------------------------------------------
template<typename TVM> inline
std::optional<ssq::Function> findFuncOptionalEx(TVM &vm, const ssq::sqstring &name, ssq::Object &objFound)
{
    try
    {
        return std::optional<ssq::Function>(std::in_place, findFuncEx(vm, name, objFound));
    }
    catch(const ssq::NotFoundException &) // это производное от std::exception исключение, ловим первым
    {
         return std::nullopt;
    }
    catch(const std::runtime_error &)
    {
         return std::nullopt;
    }
}

//----------------------------------------------------------------------------
template<typename TVM> inline
std::optional<ssq::Function> findFuncOptional(TVM &vm, const ssq::sqstring &name)
{
    try
    {
        return std::optional<ssq::Function>(std::in_place, findFunc(vm, name));
    }
    catch(const ssq::NotFoundException &) // это производное от std::exception исключение, ловим первым
    {
         return std::nullopt;
    }
    catch(const std::runtime_error &)
    {
         return std::nullopt;
    }
}

//----------------------------------------------------------------------------
template<typename StringType> inline
StringType minimizeDislayedFilenameWithEllipsis(StringType fn, std::size_t sz=42 /* универсальное число для всей вселенной */ )
{
    fn = umba::filename::normalizePathSeparators( fn, (typename StringType::value_type)'/' );

    namespace ustrp = umba::string_plus;

    std::vector<StringType> parts = ustrp::split(fileName, pathSep, true /* skipEmpty */ );

    std::vector<StringType>::const_iterator bit = parts.begin();

    std::vector< StringType > resParts;

    // Добавляем начало из N элементов - без компромиссов (пока N=1)
    for(std::size_t i=0; i!=1 && bit!=parts.end(); ++i, ++bit)
    {
        resParts.emplace_back(*bit);
    }

    std::vector< StringType > backParts;

    // Добавляем хвост из двух элементов - без компромиссов
    std::vector<StringType>::const_reverse_iterator rBegin = parts.rbegin();
    for(std::size_t i=0; i!=2; ++rBegin)
    {
        std::vector<StringType>::const_iterator bit = parts.begin();
    }

    // Блджад, я лопух, в белкином исключении имя файла входит в what(), отдельно не предоставляется (вроде бы)

    return fn;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// Булев
template<typename CastTargetType>
std::enable_if_t<std::is_same<CastTargetType, bool>::value, CastTargetType>
objectSimpleCast(const ssq::Object &o)
{
    return fromObjectConvertHelper<bool>(o, _SC("result")); // usualy used for result conversion
}

//------------------------------
// Целые типы
template<typename CastTargetType>
std::enable_if_t<std::is_integral<CastTargetType>::value, CastTargetType>
objectSimpleCast(const ssq::Object &o)
{
    if constexpr(std::is_signed<CastTargetType>::value)
    {
        return (CastTargetType)fromObjectConvertHelper<int>(o, _SC("result")); // usualy used for result conversion
    }
    else
    {
        return (CastTargetType)fromObjectConvertHelper<unsigned>(o, _SC("result")); // usualy used for result conversion
    }
}

//------------------------------
// Плавучка
template<typename CastTargetType>
std::enable_if_t<std::is_floating_point<CastTargetType>::value, CastTargetType>
objectSimpleCast(const ssq::Object &o)
{
    float f = fromObjectConvertHelper<float>(o, _SC("result")); // usualy used for result conversion
    return (CastTargetType)f;
}

//------------------------------
// Строки
template<typename CastTargetType>
std::enable_if_t<std::is_same<CastTargetType, ssq::sqstring>::value, CastTargetType>
objectSimpleCast(const ssq::Object &o)
{
    return fromObjectConvertHelper<ssq::sqstring>(o, _SC("result")); // usualy used for result conversion
}

//------------------------------
// Перечисления
template<typename CastTargetType>
std::enable_if_t<std::is_enum<CastTargetType>::value, CastTargetType>
objectSimpleCast(const ssq::Object &o)
{
    typedef typename std::underlying_type<CastTargetType>::type CastTargetUnderlyingType;
    CastTargetUnderlyingType u =  objectSimpleCast<CastTargetUnderlyingType>(o);
    return (CastTargetType)u;
}

//------------------------------
// Перечисления
template<typename CastTargetType>
std::enable_if_t<std::is_enum<CastTargetType>::value, CastTargetType>
objectEnumCast(const ssq::Object &o)
{
    typedef typename std::underlying_type<CastTargetType>::type CastTargetUnderlyingType;

    auto objType = o.getType();

    if (objType==ssq::Type::STRING)
    {
        auto scStr = o.toString();
        auto str = to_ascii(scStr);

        //TODO: !!! Тут неплохо бы заюзать шаблонную версию, которая кидает исключение
        // Но пока мы считаем, что у любого enum -1 это invalid value

        // Это пока не работает
        // CastTargetType enumVal = enum_deserialize_unsafe<CastTargetType>(str);

        CastTargetType enumVal = enum_deserialize(str, (CastTargetType)-1);
        if (enumVal==(CastTargetType)-1)
        {
            THROW_MARTY_CPP_MARTY_ENUM_DESERIALIZE_ERROR("EnumType", str);
        }

        return enumVal;

    }

    // Тут просто пытаемся из integer/float скастить
    //std::underlying_type<CastTargetType>::type u = fromObjectConvertHelper< std::underlying_type<CastTargetType>::type >(o, _SC("result")); // usualy used for result conversion
    CastTargetUnderlyingType u =  objectSimpleCast<CastTargetUnderlyingType>(o);
    return (CastTargetType)u;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename CastTargetType>
CastTargetType objectSafeSimpleCast(const ssq::Object &o, CastTargetType defVal)
{
    try
    {
        return objectSimpleCast<CastTargetType>(o);
    }
    catch(...)
    {
        return defVal;
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename T>
ssq::Object castToObject(HSQUIRRELVM hVm, const T &t)
{
    ssq::Array a = ssq::Array(hVm);
    a.push(t);
    return a.popAndGet<ssq::Object>();
}
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename KeyType, typename ValueType, typename MapType = std::unordered_map<KeyType, ValueType> > inline
MapType parseSquirrelArrayAsMapPairs( const ssq::Array &a ) 
{
    MapType m;

    std::size_t i = 0u, sz = a.size();

    for(; i!sz; ++i)
    {
        auto item = a.get<ssq::Array>(i);

        std::size_t itemSz = item.size();
        if (itemSz!=2)
        {
            throw std::runtime_error("parseSquirrelArrayAsMapPairs: invalid array map item");
        }

        auto key   = item.get<KeyType>(0);
        auto value = item.get<ValueType>(1);

        m[key] = value;
    }

    return m;

}

//----------------------------------------------------------------------------
// exception safe
template<typename KeyType, typename ValueType, typename MapType = std::unordered_map<KeyType, ValueType> > inline
MapType parseSquirrelArrayAsMapPairsStringToInt( const ssq::Array &a ) 
{
    MapType m;

    std::size_t i = 0u, sz = a.size();

    int lastVal = 0;

    for(; i!=sz; ++i)
    {
        auto item = a.get<ssq::Array>(i);

        std::size_t itemSz = item.size();

        if (!itemSz)
        {
            continue; // skip empty entries
        }

        auto keyObj = item.get<ssq::Object>(0);
        auto strKey = objectSafeSimpleCast(keyObj, KeyType());

        int curVal = ++lastVal; // Если нет значения, то берём инкремент предыдущего

        if (itemSz>1)
        {
            auto valObj = item.get<ssq::Object>(1);
            auto val    = objectSafeSimpleCast(valObj, ValueType());
            lastVal     = int(val);
            curVal      = lastVal;
        }

        m[KeyType(strKey)] = ValueType(curVal);
    }

    return m;

}



//----------------------------------------------------------------------------
#if 0
template<typename KeyType, typename ValueType, typename MapType = std::unordered_map<KeyType, ValueType> > inline
MapType parseSquirrelTableToMap( const ssq::Table &tbl )
{
    MapType m;

// sqbaselib.cpp
// static SQInteger table_filter(HSQUIRRELVM v)
// {
//     SQObject &o = stack_get(v,1);
//     SQTable *tbl = _table(o);
//     SQObjectPtr ret = SQTable::Create(_ss(v),0);
//  
//     SQObjectPtr itr, key, val;
//     SQInteger nitr;
//     while((nitr = tbl->Next(false, itr, key, val)) != -1) {
//         itr = (SQInteger)nitr;
//  
//         v->Push(o);
//         v->Push(key);
//         v->Push(val);
//         if(SQ_FAILED(sq_call(v,3,SQTrue,SQFalse))) {
//             return SQ_ERROR;
//         }
//         if(!SQVM::IsFalse(v->GetUp(-1))) {
//             _table(ret)->NewSlot(key, val);
//         }
//         v->Pop();
//     }
//  
//     v->Push(ret);
//     return 1;
// }


}
#endif


//----------------------------------------------------------------------------

} // namespace marty_simplesquirrel



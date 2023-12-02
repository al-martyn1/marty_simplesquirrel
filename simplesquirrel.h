#pragma once

#include <simplesquirrel/simplesquirrel.hpp>

#include "marty_cpp/marty_cpp.h"

//
#include <exception>
#include <stdexcept>
#include <optional>


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

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::BOOL), ssq::typeToStr(t));

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

    throw ssq::TypeException("bad cast", ssq::typeToStr(ssq::Type::INTEGER), ssq::typeToStr(t));

}

//----------------------------------------------------------------------------
template<> inline
unsigned fromObjectConvertHelper<unsigned>(ssq::Object &o, const SQChar *paramName)
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
template<typename TargetType, typename BasicType> inline
std::vector<TargetType> fromArrayObjectToVectorConvertEx(ssq::Object &o, const SQChar *paramName, bool allowSingleVal=false)
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
std::vector<TargetType> fromArrayObjectToVectorConvert(ssq::Object &o, const SQChar *paramName, bool allowSingleVal=false)
{
    return fromArrayObjectToVectorConvertEx<TargetType, TargetType>(o, paramName, allowSingleVal);
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename TargetClassType, typename BoundClassType>
std::vector<TargetClassType> fromArrayObjectToClassVectorConvertEx(ssq::Object &o, const SQChar *paramName)
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
std::vector<BoundClassType> fromArrayObjectToClassVectorConvert(ssq::Object &o, const SQChar *paramName)
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
inline
std::string prepareFunctionFullQualifiedName(const std::string &name)
{
    // В squirrel полностью квалифицированное имя функции разделяется как и в плюсиках, через двойное двоеточие, но это неудобно
    // В именах не допустим символ '/', поэтому, раз пошла такая пьянка, то добавим возможность задавать FQNs как пути в юниксах
    std::string res;
    bool prevColon = false;

    for(auto ch: name)
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
            res.append(1,ch);
            prevColon = false;
        }
    }

    return res;
}

//----------------------------------------------------------------------------
template<typename TVM> inline
ssq::Function findFunc(TVM &vm, const std::string &name)
{
    auto preparedFqn = prepareFunctionFullQualifiedName(name);
    std::vector<std::string> names = marty_cpp::simple_string_split(preparedFqn, '.' /* , -1 */  /* nSplits */ );

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

//----------------------------------------------------------------------------
template<typename TVM> inline
std::optional<ssq::Function> findFuncOptional(TVM &vm, const std::string &name)
{
    try
    {
        return std::optional<ssq::Function>(std::in_place, findFunc(vm, name));
    }
    catch(ssq::NotFoundException)
    {
         return std::nullopt;
    }
}

// //Object find(const SQChar* name) const;
//  
// inline std::string to_sqstring(const CharType* pStr)
// vm.findFunc(_SC("onPaint"));
// Function findFunc(const SQChar* name) const;



} // namespace marty_simplesquirrel


#pragma once

#include "umba/umba.h"
//
#include "umba/string_plus.h"
#include "umba/filename.h"
//
#include "umba/char_writers.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"
//
#include "warnings_disable.h"
//

#include <squirrel.h>
#include <simplesquirrel/simplesquirrel.hpp>
//
#include "warnings_restore.h"
//
#include <algorithm>

//
#include "marty_cpp/marty_cpp.h"
#include "marty_tr/marty_tr.h"

//
#include "simplesquirrel.h"

//
#include "md_utils.h"

//
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

// marty_simplesquirrel::
// marty_simplesquirrel::utils::
// marty_simplesquirrel::split_utils::

namespace marty_simplesquirrel {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace utils {



//----------------------------------------------------------------------------
inline
std::string convertTypeNameToPath(const std::string &name)
{
    std::string res; res.reserve(name.size());

    for(auto ch: name)
    {
        if (ch=='.' || ch==':' || ch=='\\')
        {
            ch = '/';
        }
    
        if (ch=='/' && !res.empty() && res.back()=='/')
        {
            continue;
        }

        res.append(1,ch);
    }

    return res;
}



} // namespace utils {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

// Встроенные типы: null(void)/any/bool/integer(int)/float/string/array[ of Type]/table

// array of MyType
// array of [integer,MyType]

//----------------------------------------------------------------------------
namespace split_utils {



//----------------------------------------------------------------------------
static const std::size_t RoundBraceIndex  = 0u;
static const std::size_t SquareBraceIndex = 1u;
static const std::size_t CurlyBraceIndex  = 2u;
static const std::size_t AngleBraceIndex  = 3u;



//----------------------------------------------------------------------------
inline
std::size_t getBraceIndex(char ch)
{
    if (ch=='(' || ch==')')
    {
        return RoundBraceIndex;
    }
    else if (ch=='[' || ch==']')
    {
        return SquareBraceIndex;
    }
    else if (ch=='{' || ch=='}')
    {
        return CurlyBraceIndex;
    }
    else if (ch=='<' || ch=='>')
    {
        return AngleBraceIndex;
    }
    else
    {
        return (std::size_t)-1;
    }
}

//----------------------------------------------------------------------------
inline
bool isOpenBrace(char ch)
{
    if (ch=='[' || ch=='{' || ch=='(' || ch=='<')
    {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
// -1 - close, +1 - open, 0 - not brace
inline
int getBraceKind(char ch)
{
    std::size_t idx = getBraceIndex(ch);
    if (idx==(std::size_t)-1)
    {
        return 0;
    }

    return isOpenBrace(ch) ? +1 : -1;
}

//----------------------------------------------------------------------------
bool isSameBraceEnclosed(const std::string &str)
{
    if (str.size()<2)
    {
        return false;
    }

    std::size_t braceIdxFront  = split_utils::getBraceIndex(str.front());
    std::size_t braceIdxBack   = split_utils::getBraceIndex(str.back() );
    int         braceKindFront = split_utils::getBraceKind (str.front());
    int         braceKindBack  = split_utils::getBraceKind (str.back() );
    braceKindBack = braceKindBack * -1;
    
    if ( braceIdxFront!=(std::size_t)-1 // первый символ - скобка
      && braceIdxBack !=(std::size_t)-1 // последний символ - скобка
      && braceKindFront!=0              // тип первой скобки - скобка
      && braceKindFront==braceKindBack  // конертнутый тип последней скобки равен типу первой
       )
    {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
inline
bool isBrace(char ch)
{
    return getBraceIndex(ch)!=(std::size_t)-1;
}

//----------------------------------------------------------------------------
inline
void splitTypeName(const std::string &str, std::string &type, std::string &name)
{
    type.clear();
    name.clear();

    if (str.empty())
        return;

    int braceCounters[4] = { 0 };

    auto isInBraces = [&]()
    {
        for(std::size_t i=0u; i!=4; ++i)
        {
            if (braceCounters[i]!=0)
                return true;
        }
        return false;
    };

    std::string::size_type pos = str.size();
    for(; pos; --pos)
    {
        char ch = str[pos-1];
        if (ch==' ')
        {
            if (!isInBraces())
            {
                --pos;
                break;
            }

            continue;
        }

        int braceKind = getBraceKind(ch);

        if (braceKind!=0)
        {
            braceCounters[getBraceIndex(ch)] += braceKind;
        }

    }

    if (!pos)
    {
        name = str;
        return;
    }


    name = std::string(str, pos+1);

    while(pos)
    {
        if (str[pos]!=' ')
            break;
        --pos;
    }

    type = std::string(str, 0, pos+1);

}

//----------------------------------------------------------------------------
inline
void splitFunctionPrototype(const std::string &str, std::string &type, std::string &name, std::vector<std::string> &params, std::string &suffix)
{
    type.clear();
    name.clear();
    params.clear();
    suffix.clear();

    if (str.empty())
        return;

    auto reverseParams = [&]()
    {
        std::reverse(params.begin(), params.end());
    };

    int braceCounters[4] = { 0 };

    auto isInBraces = [&]()
    {
        for(std::size_t i=0u; i!=4; ++i)
        {
            if (braceCounters[i]!=0)
                return true;
        }
        return false;
    };

    auto isExactInRoundBraces = [&]()
    {
        if (braceCounters[RoundBraceIndex]!=-1)
            return false;

        for(std::size_t i=0u; i!=4; ++i)
        {
            if (i==RoundBraceIndex)
                continue;

            if (braceCounters[i]!=0)
                return false;
        }
        return true;
    };


    //int braceCounter = 0;
    std::string::size_type endPos = str.size();

    std::string::size_type pos = str.size();
    for(; pos; --pos)
    {
        char ch = str[pos-1];

        int braceKind     = getBraceKind (ch);
        std::size_t brIdx = getBraceIndex(ch);

        if (braceKind==0) // not an any brace
        {
            if (ch==',')
            {
                if (isExactInRoundBraces())
                {
                    auto len = endPos-pos;
                    if (len)
                    {
                        --len;
                    }
                    auto paramStr = std::string(str, pos, len);
                    umba::string_plus::trim(paramStr);
                    if (!paramStr.empty())
                    {
                        params.emplace_back(paramStr);
                    }
    
                    endPos = pos;
                }
            }

            continue;
        }

        // any brace
        braceCounters[brIdx] += braceKind;

        if (brIdx!=RoundBraceIndex)
        {
            continue;
        }
        

        if (braceKind<0) // closing brace
        {
            //if (braceCounter==0)
            if (isExactInRoundBraces())
            {
                suffix.assign(str, pos);
                umba::string_plus::trim(suffix);
                endPos = pos;
            }

            //--braceCounter;
        }
        else // if (ch=='(') // open brace
        {
            //if (braceCounter==-1)
            if (braceCounters[RoundBraceIndex]==0)
            {
                auto len = endPos-pos;
                if (len)
                {
                    --len;
                }
                auto paramStr = std::string(str, pos, len);
                umba::string_plus::trim(paramStr);
                if (!paramStr.empty())
                {
                    params.emplace_back(paramStr);
                }

                endPos = pos;

                auto typeNameStr = std::string(str, 0, pos-1);
                umba::string_plus::trim(typeNameStr);
                splitTypeName(typeNameStr, type, name);
                reverseParams();
                return;
            }

            //++braceCounter;
        }

        // else //if (ch==',')
        // {
        //     // if (braceCounter==-1)
        // }
    }

    reverseParams();
    //return;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> splitTypeToParts(const std::string &str)
{
    std::vector<std::string> res;

    int braceCounters[4] = { 0 };

    auto isInBraces = [&]()
    {
        for(std::size_t i=0u; i!=4; ++i)
        {
            if (braceCounters[i]!=0)
                return true;
        }
        return false;
    };


    std::string curPart;

    auto appendCurPart = [&]()
    {
        if (curPart.empty())
        {
            return;
        }

        if (curPart[0]==' ')
        {
            // space
            umba::string_plus::trim(curPart);
            if (curPart.empty())
            {
                if (!res.empty() && !res.back().empty() && res.back().back()==' ')
                {
                    return; // уже пробельная строка добавлена
                }
                res.emplace_back(1, ' '); // add single space string
            }
            else
            {
                res.emplace_back(curPart);
            }
        }
        else
        {
            umba::string_plus::trim(curPart);
            res.emplace_back(curPart);
        }

        curPart.clear();
    };

    char prevCh = 0;

    std::string::size_type pos = 0;
    for(; pos!=str.size(); ++pos)
    {
        char ch = str[pos];

        int braceKind = getBraceKind(ch);

        if (ch=='-' || ch=='=')
        {
            if (!isInBraces())
            {
                appendCurPart(); // добавляем то, что уже наколлекционированно, если было
                curPart.assign(1,ch);
                prevCh = ch;
                continue;
            }
        }

        if (ch=='>' && (prevCh=='-' || prevCh=='='))
        {
            if (!isInBraces())
            {
                curPart.append(1, ch);
                appendCurPart(); // добавляем то, что уже наколлекционированно, если было
                prevCh = ch;
                continue;
            }
        }

        if (ch==' ' || ch==',')
        {
            if (!isInBraces())
            {
                appendCurPart(); // добавляем то, что уже наколлекционированно, если было
                curPart.assign(1,ch);
                appendCurPart(); // добавляем break символ как строку
            }
            else
            {
                curPart.append(1, ch);
            }

            prevCh = ch;
            continue;
        }


        if (braceKind==0) // не скобка
        {
            curPart.append(1, ch);
            prevCh = ch;
            continue;
        }

        // наткнулись на скобку
        if (!isInBraces())
        {
            // Были не в скобках
            appendCurPart(); // добавляем то, что уже наколлекционированно, если было
        }

        curPart.append(1, ch);

        braceCounters[getBraceIndex(ch)] += braceKind;

        prevCh = ch;

    }

    appendCurPart(); // добавляем то, что уже наколлекционированно, если было

    return res;

}

//----------------------------------------------------------------------------
inline
std::vector<std::string> splitTypeToAlters(const std::string &str)
{
    std::vector<std::string> res;

    int braceCounters[4] = { 0 };

    auto isInBraces = [&]()
    {
        for(std::size_t i=0u; i!=4; ++i)
        {
            if (braceCounters[i]!=0)
                return true;
        }
        return false;
    };


    std::string curPart;

    auto appendCurPart = [&]()
    {
        if (curPart.empty())
        {
            return;
        }

        res.emplace_back(curPart);
        curPart.clear();
    };

    std::string::size_type pos = 0;
    for(; pos!=str.size(); ++pos)
    {
        char ch = str[pos];

        int braceKind = getBraceKind(ch);

        if (ch=='|')
        {
            if (!isInBraces())
            {
                appendCurPart(); // добавляем то, что уже наколлекционированно, если было
                continue;
            }
        }

        if (braceKind==0) // не скобка
        {
            curPart.append(1, ch);
            continue;
        }

        // наткнулись на скобку
        curPart.append(1, ch);

        braceCounters[getBraceIndex(ch)] += braceKind;
    }

    appendCurPart(); // добавляем то, что уже наколлекционированно, если было

    return res;

}

inline
std::vector<std::string> splitFullQualifiedTypeName(const std::string &str)
{
    std::string asPath = utils::convertTypeNameToPath(str);

    std::vector<std::string> res;
    std::string part;

    auto appendPart = [&]()
    {
        if (!part.empty())
        {
            res.emplace_back(part);
        }

        part.clear();
    };

    for(char ch: asPath)
    {
        if (ch=='/')
        {
            appendPart();
        }
        else
        {
            part.append(1,ch);
        }
    }

    appendPart();

    return res;
}


} // namespace split_utils {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
namespace utils {



//! Добавляет тип
template<typename StringType> inline
StringType appendType( const StringType &n, StringType e )
{
    typedef typename StringType::value_type CharType;

    if (e.empty())
        return n;

    StringType res = n;
    if (!res.empty() && res.back()==(typename CharType)'.')
    {
        res.erase(res.size()-1u, 1u);
    }

    if (!e.empty() && e.front()==(typename CharType)'.')
    {
        e.erase(0u, 1u);
    }

    if (res.empty())
    {
        return e;
    }
    else
    {
        return res + StringType(1u, (typename CharType)'.') + e;
    }
    
}

//----------------------------------------------------------------------------
inline
bool hasBraces(const std::string &str)
{
    for(auto ch: str)
    {
        if (split_utils::getBraceIndex(ch)!=(std::size_t)-1)
        {
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------
inline
bool hasPunctuation(const std::string &str)
{
    for(auto ch: str)
    {
        if (ch==',' || ch==';' || ch=='!' || ch=='\'' || ch=='\"' || ch==':' || ch=='-' || ch=='=')
        {
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------
inline
bool isGenericTypeName(const std::string &str)
{
    if (str.empty())
    {
        return false;
    }

    auto firstChar = str[0];
    if (firstChar>='A' && firstChar<='Z')
    {
        return true; // Первый символ имени - латинская буква в верхнем регистре - вероятно, это имя типа
    }

    return false;
}

//----------------------------------------------------------------------------
inline
bool isSpaceString(const std::string &str)
{
    for(auto ch: str)
    {
        if (ch!=' ')
        {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
inline
int compareFullQualifiedTypeNames(const std::string &name1, const std::string &name2)
{
    std::vector<std::string> p1 = split_utils::splitFullQualifiedTypeName(name1);
    std::vector<std::string> p2 = split_utils::splitFullQualifiedTypeName(name2);

    std::size_t idx = 0;
    for(; idx!=p1.size() && idx!=p2.size(); ++idx)
    {
        int cmp = p1[idx].compare(p2[idx]);
        if (cmp!=0)
        {
            return cmp;
        }
    }

    if (p1.size()==p2.size())
    {
        return 0;
    }

    return p1.size()<p2.size() ? -1 : 1;

}

//----------------------------------------------------------------------------
struct FullQualifiedTypeNameLess
{
    bool operator()(const std::string &name1, const std::string &name2) const
    {
    int cmp = compareFullQualifiedTypeNames(name1, name2);
    return cmp<0;
    }

};

//----------------------------------------------------------------------------
inline
bool fullQualifiedTypeNameLess(const std::string &name1, const std::string &name2)
{
    int cmp = compareFullQualifiedTypeNames(name1, name2);
    return cmp<0;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeSpecialMethodsKeywordsMap()
{
    std::unordered_set<std::string> res;

    // http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html
    
    res.insert("_set");
    res.insert("_get");
    res.insert("_add");
    res.insert("_cmp");
    res.insert("_div");
    res.insert("_mul");
    res.insert("_sub");
    res.insert("_unm");
    res.insert("_modulo");
    res.insert("_tostring");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeBasicConvertMethodsKeywordsMap()
{
    std::unordered_set<std::string> res;

    // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html

    res.insert("tointeger");
    res.insert("tofloat");
    res.insert("tostring");

    res.insert("tointeger64");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeArrayCompatMethodsKeywordsMap()
{
    std::unordered_set<std::string> res;

    // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#string

    res.insert("len");
    res.insert("pop");
    res.insert("top");
    res.insert("slice");
    res.insert("push");
    res.insert("append");
    res.insert("extend");
    res.insert("insert");
    res.insert("remove");
    res.insert("resize");
    res.insert("reverse");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeAllKeywordsMap()
{
    std::unordered_set<std::string> res;

    res.insert("null");
    res.insert("nul");
    res.insert("nil");
    res.insert("void");

    res.insert("any");
    res.insert("any_basic");
    res.insert("any_integral");
    res.insert("any_number");

    res.insert("static");

    res.insert("bool");
    res.insert("int");
    res.insert("integer");
    res.insert("float");
    res.insert("string");

    res.insert("class");
    res.insert("array");
    res.insert("table");

    res.insert("object");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeVoidKeywordsMap()
{
    std::unordered_set<std::string> res;

    res.insert("null");
    res.insert("nul");
    res.insert("nil");
    res.insert("void");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeIntegralKeywordsMap()
{
    std::unordered_set<std::string> res;

    //res.insert("any");
    res.insert("any_basic");
    res.insert("any_integral");

    res.insert("bool");
    res.insert("int");
    res.insert("integer");
    res.insert("float");
    res.insert("string");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeContainerKeywordsMap()
{
    std::unordered_set<std::string> res;

    res.insert("any");

    res.insert("class");
    res.insert("array");
    res.insert("table");

    res.insert("object");

    return res;
}

//----------------------------------------------------------------------------
inline
std::unordered_set<std::string> makeSquirrelTypeIgnoreKeywordsMap()
{
    std::unordered_set<std::string> res;

    res.insert("of");
    res.insert("or");
    res.insert("and");

    return res;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(kwdKind) \
inline                                                                     \
const std::unordered_set<std::string>& get##kwdKind##Map()                 \
{                                                                          \
    static auto m = make##kwdKind##Map();                                  \
    return m;                                                              \
}

MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeAllKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeVoidKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeIntegralKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeContainerKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeIgnoreKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeSpecialMethodsKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeBasicConvertMethodsKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_GET_KEYWORD_MAP(SquirrelTypeArrayCompatMethodsKeywords)



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(kwdKind) \
inline                                                                \
bool is##kwdKind(const std::string &str)                              \
{                                                                     \
    const auto &m = get##kwdKind##Map();                              \
    return m.find(str)!=m.end();                                      \
}

// bool isSquirrelType*(std::string str)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeAllKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeVoidKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeIntegralKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeContainerKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeIgnoreKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeSpecialMethodsKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeBasicConvertMethodsKeywords)
MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeArrayCompatMethodsKeywords)
// bool isSquirrelType*(std::string str)




} // namespace utils {

//----------------------------------------------------------------------------


// null(void)/any/bool/integer(int)/float/string/array[ of Type]/table

//----------------------------------------------------------------------------
struct FunctionParameterInfo
{
    std::string   name;
    std::string   type;

    FunctionParameterInfo() = default;
    FunctionParameterInfo(const FunctionParameterInfo &) = default;
    FunctionParameterInfo& operator=(const FunctionParameterInfo &) = default;
    FunctionParameterInfo(FunctionParameterInfo &&) = default;
    FunctionParameterInfo& operator=(FunctionParameterInfo &&) = default;

    FunctionParameterInfo(const std::string &n, const std::string &t) : name(n), type(t) {}
    FunctionParameterInfo(const std::string &tnStr) : name(), type()
    {
        split_utils::splitTypeName(tnStr, type, name);
    }

    static
    FunctionParameterInfo fromString(const std::string &tnStr)
    {
        return FunctionParameterInfo(tnStr);
    }



}; // struct FunctionParameterInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct FunctionInfo
{
    bool                                memberField = false;
    std::string                         name      ; // function name, or member field name
    std::vector<FunctionParameterInfo>  parameters;
    std::string                         type      ; // function return type, or member field type
    bool                                isStatic  ;

    FunctionInfo() = default;
    FunctionInfo(const FunctionInfo &) = default;
    FunctionInfo& operator=(const FunctionInfo &) = default;
    FunctionInfo(FunctionInfo &&) = default;
    FunctionInfo& operator=(FunctionInfo &&) = default;

    FunctionInfo(const std::string &str, bool mf = false) : memberField(mf), name(), parameters(), type()
    {
        if (memberField)
        {
            split_utils::splitTypeName(str, type, name);
        }
        else
        {
            std::vector<std::string> strParamsVec;
            std::string suffix;
            split_utils::splitFunctionPrototype(str, type, name, strParamsVec, suffix);
            for(const auto &strParam: strParamsVec)
            {
                parameters.emplace_back(strParam);
            }
        }

        isStatic = false;
        if (umba::string_plus::starts_with(type, std::string("static ")))
        {
            isStatic = true;
            type.erase(0, 7);
            umba::string_plus::trim(type);
        }
    }

    static 
    FunctionInfo makeConstructorFunctionFromArgsString(const std::string &ctorArgs)
    {
        return FunctionInfo(std::string("constructor(")+ctorArgs+")", false /* not member */ );
    }

    static
    FunctionInfo fromString(const std::string &tnStr)
    {
        return FunctionInfo(tnStr);
    }


    // return hasParams
    bool findParametersMaxLen(std::size_t &maxNameLen, std::size_t &maxTypeLen) const
    {
        maxNameLen = 0;
        maxTypeLen = 0;

        for(const auto &param: parameters)
        {
            maxNameLen = std::max(maxNameLen, param.name.size());
            maxTypeLen = std::max(maxTypeLen, param.type.size());
        }

        return !parameters.empty();
    }

    bool hasReturnValue() const
    {
        if (type.empty())
            return false;

        if (utils::isSquirrelTypeVoidKeywords(type))
            return false;

        if (name=="constructor")
            return false;

        return true;
    }

    std::string formatVariableSquirrelLike( std::size_t indent
                                          , std::size_t nameLen
                                          , std::size_t typeLen
                                          , bool addTypeComment = true
                                          ) const
    {
        nameLen = std::max(nameLen, name.size());
        typeLen = std::max(typeLen, type.size());

        std::string res;

        res.append(indent, ' ');
        if (isStatic)
        {
            res.append("static ");
        }

        res.append(name);
        res.append(1u, ';');

        if (addTypeComment)
        {
            if (nameLen>name.size())
            {
                std::size_t n = nameLen - name.size();
                res.append(n, ' ');
            }
            res.append(" // ");
            res.append(type);
        }

        return res;
    }

    std::string formatFunctionPrototypeSquirrelLike( std::size_t indent
                                                   , std::size_t nameLen
                                                   , std::size_t typeLen
                                                   , bool addTypeComment = true
                                                   , bool addRetComment = true
                                                   //, bool makeTypeLinks = false
                                                   //, const std::string &refDocExt = ".md"
                                                   ) const
    {
        nameLen = std::max(nameLen, name.size());
        typeLen = std::max(typeLen, type.size());

        std::size_t maxParamNameLen = 0;
        std::size_t maxParamTypeLen = 0;

        std::string res;

        res.append(indent, ' ');

        std::size_t actualNameLen = 0;

        if (isStatic)
        {
            res.append("static\n");
            res.append(indent, ' ');
        }

        if (name!="constructor")
        {
            actualNameLen += 9;
            res.append("function ");
        }

        res.append(name);
        //res.append("(");
        actualNameLen += name.size();


        if (!findParametersMaxLen(maxParamNameLen, maxParamTypeLen))
        {
            // no params
            res.append("()");
        }
        else
        {
            std::size_t pIdx = 0;
            for(; pIdx!=parameters.size(); ++pIdx)
            {
               if (pIdx==0)
               {
                   res.append("( ");
               }
               else
               {
                   res.append(indent  , ' ');
                   // res.append(9u      , ' '); // [function ]
                   // res.append(nameLen , ' '); // name
                   res.append(actualNameLen, ' ');
                   res.append(", ");
               }

               res.append(parameters[pIdx].name);

               if (addTypeComment)
               {
                   std::size_t n = maxParamNameLen - parameters[pIdx].name.size();
                   res.append(n, ' ');
                   res.append("   // ");
                   res.append(parameters[pIdx].type);
               }

               res.append(1, '\n');
            }

            res.append(indent  , ' ');
            //res.append(9u      , ' '); // [function ]
            //res.append(nameLen , ' '); // name
            res.append(actualNameLen, ' ');
            res.append(")");

            //std::vector<FunctionParameterInfo>  parameters;
        }

        res.append(1, '\n');

        if (addRetComment && hasReturnValue())
        {
            res.append(indent  , ' ');
            res.append("// returns: ");
            res.append(type);
            res.append(1, '\n');
        }
        
        return res;
    }


    std::string formatSquirrelLike( std::size_t indent
                                  , std::size_t nameLen
                                  , std::size_t typeLen
                                  , bool addTypeComment = true
                                  , bool addRetComment = true
                                  //, bool makeTypeLinks = false
                                  //, const std::string &refDocExt = ".md"
                                  ) const
    {
        return memberField
             ? formatVariableSquirrelLike         (indent, nameLen, typeLen, addTypeComment)
             : formatFunctionPrototypeSquirrelLike(indent, nameLen, typeLen, addTypeComment, addRetComment)
             ;
    }


}; // struct FunctionInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct ClassInfo
{
    bool ns = false; // class with methods or ns with free functions

    std::string                         name     ;
    std::vector<FunctionInfo>           functions; // Тут также лежат поля класса/переменные пространства имен
    std::vector<ClassInfo>              classes  ;
    

    ClassInfo() = default;
    ClassInfo(const ClassInfo&) = default;
    ClassInfo& operator=(const ClassInfo&) = default;
    ClassInfo(ClassInfo&&) = default;
    ClassInfo& operator=(ClassInfo&&) = default;

    ClassInfo(const std::string &n) : name(n) {}
    ClassInfo(const std::string &n, bool isNs) : ns(isNs), name(n) {}
    ClassInfo(bool isNs) : ns(isNs), name() {}

    void addFunc(const FunctionInfo &fi)
    {
        functions.emplace_back(fi);
    }

    void addFunc(const std::string &fi)
    {
        functions.emplace_back(fi);
    }

    void addConstructor(const std::string &fi)
    {
        functions.emplace_back(FunctionInfo::makeConstructorFunctionFromArgsString(fi));
    }

    void addMember(const std::string &fi)
    {
        functions.emplace_back(fi, true);
    }

    void addClass(const ClassInfo &ci)
    {
        if (!ns)
        {
            throw std::runtime_error("ClassInfo: nested classes not allowed");
        }

        classes.emplace_back(ci);
    }

    void merge(const ClassInfo &ci)
    {
        if (ns!=ci.ns)
        {
            throw std::runtime_error("ClassInfo: Merged class infos have different types");
        }

        if (name!=ci.name)
        {
            throw std::runtime_error("ClassInfo: Merged class infos have different names");
        }

        functions.insert(functions.end(), ci.functions.begin(), ci.functions.end());
        classes.insert(classes.end(), ci.classes.begin(), ci.classes.end());
        
    }

    void sortMembers()
    {
        std::stable_sort( functions.begin(), functions.end()
                        , [](const FunctionInfo &fi1, const FunctionInfo &fi2) -> bool
                          {
                              if (fi1.memberField!=fi2.memberField)
                              {
                                  return fi1.memberField; // поля идут первыми, до методов
                              }

                              int cmp = utils::compareFullQualifiedTypeNames(fi1.name, fi2.name);
                              return cmp<0;
                          }
                        );
    }

    void sortClasses()
    {
        std::stable_sort( classes.begin(), classes.end()
                        , [](const ClassInfo &ci1, const ClassInfo &ci2)
                          {
                              if (ci1.ns!=ci2.ns)
                              {
                                  return ci1.ns; // namespace идут первыми, до функций
                              }

                              int cmp = utils::compareFullQualifiedTypeNames(ci1.name, ci2.name);
                              return cmp<0;
                          }
                        );
    }

    void splitFunctionsAndVariables(ClassInfo &ciFunctions, ClassInfo &ciVariables) const
    {
        ciFunctions.ns = ns;
        ciFunctions.ns = ns;

        ciVariables.name = name;
        ciVariables.name = name;

        for(const auto &fi: functions)
        {
            if (fi.memberField)
            {
                ciVariables.functions.emplace_back(fi);
            }
            else
            {
                ciFunctions.functions.emplace_back(fi);
            }
        }

    }

    void splitFunctionsAndVariablesAndSort(ClassInfo &ciFunctions, ClassInfo &ciVariables) const
    {
        splitFunctionsAndVariables(ciFunctions, ciVariables);
        ciFunctions.sortMembers();
        ciVariables.sortMembers();
    }

    void findFunctionNamesMaxLen(std::size_t &maxNameLen, std::size_t &maxTypeLen) const
    {
        maxNameLen = 0;
        maxTypeLen = 0;

        for(const auto &fn: functions)
        {
            std::size_t extra = 0u;

            if (fn.memberField && fn.isStatic)
            {
                extra = 7; // "static "
            }

            maxNameLen = std::max(maxNameLen, fn.name.size());
            maxTypeLen = std::max(maxTypeLen, fn.type.size());
        }
    }

    void findClassNamesMaxLen(std::size_t &maxNameLen) const
    {
        maxNameLen = 0;

        for(const auto &cls: classes)
        {
            maxNameLen = std::max(maxNameLen, cls.name.size());
        }

    }

    // std::string                         name      ; // function name, or member field name
    // std::vector<FunctionParameterInfo>  parameters;
    // std::string                         type      ; // function return type, or member field type


}; // struct ClassInfo

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://github.github.com/gfm/
// https://www.rubycoloredglasses.com/2013/04/languages-supported-by-github-flavored-markdown/
// https://github.com/github-linguist/linguist/blob/master/lib/linguist/languages.yml
// Squirrel:
//   type: programming
//   color: "#800000"
//   extensions:
//   - ".nut"
//   tm_scope: source.nut
//   ace_mode: c_cpp
//   codemirror_mode: clike

/*

---------
  Справка по классу

  ```
  class ИмяКласса (или ПолноеИмяКласса?)
  {
      // Тут поля класса
      переменная; // тип
      переменная; // тип


      // Тут методы класса

      function doSomething2( a    // A
                           , B    // Drawing.Context
                           , aE   // array of [string, string]
                           )
      // returns: table{array,int,string}

      function doSomething2( a    // A
                           , B    // Drawing.Context
                           , aE   // array of [string, string]
                           )
      // returns: table{array,int,string}

  }
  ```

  Поля класса

  имя поля - описание
  имя поля - описание
  имя поля - описание

  Методы класса

  имя метода - краткое brief описание
  имя метода - краткое brief описание

---------

  Справка по пространству имен

  ```
  переменная; // тип

  class ИмяКласса; // class

  table ИмяКласса; // namespace

  // Краткое описание функции (или не надо?)
  function doSomething2( a    // A
                       , B    // Drawing.Context
                       , aE   // array of [string, string]
                       )
  // returns: table{array,int,string}
  ```

*/

//----------------------------------------------------------------------------
template<typename TVal>
using FullQualifiedTypeNameMap = std::map< std::string, typename TVal, utils::FullQualifiedTypeNameLess >;

//----------------------------------------------------------------------------
inline 
void collectClasses( FullQualifiedTypeNameMap< FullQualifiedTypeNameMap<ClassInfo> > &mapCollectTo, const ClassInfo &ci, const std::string &rootNs=std::string() )
{
    // классы у нас всегда имеют неквалифицированное имя
    // пространства имён всегда имеют полностью квалифицированное имя (или нет?)

    if (ci.ns)
    {
        // Пространство имён

        std::string nsFullName = utils::appendType(rootNs, ci.name);

        if (!ci.functions.empty())
        {
            ClassInfo &nsInfo = mapCollectTo[nsFullName][""];
            nsInfo.ns = true;
            nsInfo.functions.insert(nsInfo.functions.end(), ci.functions.begin(), ci.functions.end()); // мержим функции  и переменные NS
        }

        for(const auto &classInfo: ci.classes)
        {
            collectClasses( mapCollectTo, classInfo, nsFullName );
        }

        //nsInfo.merge()

    // std::string                         name     ;
    // std::vector<FunctionInfo>           functions; // Тут также лежат поля класса/переменные пространства имен
    // std::vector<ClassInfo>              classes;

    }
    else
    {
        // Класс
        //TODO: !!! Надо бы проверить, есть ли уже такой класс
        mapCollectTo[rootNs][ci.name] = ci; // просто добавляем класс
    }
    // bool ns = false
    // for(const )

}

//----------------------------------------------------------------------------
inline
std::string formatExternalMemberRef( const EnumScriptGenerationOptions &genOptions
                                   , const std::string &memberName
                                   , const std::string &memberFullName
                                   , const std::string &curType   // Тип, которые описываем сейчас - относительно него надо сделать ссылку
                                 )
{
    std::string resBuf;
    auto resWritter = umba::StringRefCharWriter(resBuf);
    auto res = umba::SimpleFormatter(&resWritter);

    std::vector<std::string> curTypePathFileParts = split_utils::splitFullQualifiedTypeName(curType);
    std::string linkPrefix;

    std::size_t nPathLevelUp = 1;

    if (curTypePathFileParts.size()>nPathLevelUp)
    {
        std::size_t n = curTypePathFileParts.size()-nPathLevelUp;
        for(std::size_t i=0; i!=n; ++i)
        {
            if (!linkPrefix.empty())
            {
                linkPrefix.append(1u, '/');
            }

            linkPrefix.append(2u, '.');
        }

        if (!linkPrefix.empty())
        {
            linkPrefix.append(1u, '/');
        }
    }

    std::string typePathFile = umba::filename::appendExt(linkPrefix + utils::convertTypeNameToPath(memberFullName), genOptions.mdFileExt);
    res << md_utils::formatExternalHyperLink( memberName /* typeText */ , typePathFile);

    return resBuf;
}

//----------------------------------------------------------------------------
inline
std::string formatExternalTypeRef( const EnumScriptGenerationOptions &genOptions
                                 , const std::string &type      // Тип, для которого делаем ссылку
                                 , const std::string &curType   // Тип, которые описываем сейчас - относительно него надо сделать ссылку
                                 )
{
    std::string resBuf;
    auto resWritter = umba::StringRefCharWriter(resBuf);
    auto res = umba::SimpleFormatter(&resWritter);

    std::vector<std::string> curTypePathFileParts = split_utils::splitFullQualifiedTypeName(curType);
    std::string linkPrefix;

    std::size_t nPathLevelUp = 1;

    if (curTypePathFileParts.size()>nPathLevelUp)
    {
        std::size_t n = curTypePathFileParts.size()-nPathLevelUp;
        for(std::size_t i=0; i!=n; ++i)
        {
            if (!linkPrefix.empty())
            {
                linkPrefix.append(1u, '/');
            }

            linkPrefix.append(2u, '.');
        }

        if (!linkPrefix.empty())
        {
            linkPrefix.append(1u, '/');
        }
    }


    std::vector<std::string> alterParts = split_utils::splitTypeToAlters(type);
    std::size_t altPartIdx = 0u;
    for(; altPartIdx!=alterParts.size(); ++altPartIdx)
    {
        if (altPartIdx>0)
        {
            if (genOptions.mdAlterTypesSpacing)
                res << " ";
            res << "|";
            if (genOptions.mdAlterTypesSpacing)
                res << " ";
        }

        const std::string &altPart = alterParts[altPartIdx];

        std::vector<std::string> parts = split_utils::splitTypeToParts(altPart);
        for(const auto &p: parts)
        {
            if (p.empty())
            {
                continue;
            }
    
            // Пробельная строка или интегральный тип
            if ( utils::isSpaceString(p)
              || utils::isSquirrelTypeIntegralKeywords(p)
              || utils::isSquirrelTypeContainerKeywords(p)
              || utils::isSquirrelTypeIgnoreKeywords(p))
            {
                if ( utils::isSquirrelTypeIntegralKeywords(p)
                  || utils::isSquirrelTypeContainerKeywords(p)
                   )
                {
                    res << md_utils::formatIntegralOrContainerType(p);
                }
                else
                {
                    res << p;
                }
                continue;
            }
    
            // Что-то в скобках или имеет пунктуацию
            if ( utils::hasPunctuation(p)
              || utils::hasBraces(p)
               )
            {
                if (p.size()<2)
                {
                    res << p;
                    continue;
                }
    
                if (!split_utils::isSameBraceEnclosed(p))
                {
                    res << p;
                }
                else
                {
                    res << p.front() << formatExternalTypeRef(genOptions, std::string(p, 1u, p.size()-2), curType /* , memberLink */  /* , linkLevelUp */ ) << p.back();
                }
                continue;
            }
    
    
            //if (!memberLink)
            {
                // Для имен методов/полей/экземпляров используется camelCase
                // Для типов - PascalCase
                if (!utils::isGenericTypeName(p))
                {
                    res << p;
                    continue;
                }
            }
    
            // Тут пользовательский тип, а пользовательский тип может быть только наш, значит, дока есть, значит, можно делать ссылку
            // Если тип, на который ссылаемся - в текущем пространстве имён, значит, надо найти начало несовпадения, и остаток склеить в путь,
            // А если тип, на который ссылаемся, не в текущем пространстве имён?
    
            // std::vector<std::string> fqnParts = split_utils::splitFullQualifiedTypeName(p);
            // std::vector<std::string>::const_iterator fqnpIt = fqnParts.begin();
            // std::vector<std::string>::const_iterator ctpIt  = curTypePathFileParts.begin();
            // for(; fqnpIt!=fqnParts.end() && ctpIt!=curTypePathFileParts.end()) 
    
            //std::string linkPrefix;
    
            std::string typePathFile = umba::filename::appendExt(linkPrefix + utils::convertTypeNameToPath(p), genOptions.mdFileExt);
            res << md_utils::formatExternalHyperLink( p /* typeText */ , typePathFile);
    
        }

    }

    return resBuf;


    //md_utils::formatExternalHyperLink(const std::string &text, const std::string &link, bool makeLink = true) // 

// bool hasBraces(const std::string &str)
// bool isSpaceString(const std::string &str)
// convertTypeNameToPath

// // bool isSquirrelType*(std::string str)
// MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeAllKeywords)
// MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeVoidKeywords)
// MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeIntegralKeywords)
// MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeContainerKeywords)
// MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeIgnoreKeywords)


// std::vector<std::string> split_utils::splitFullQualifiedTypeName(const std::string &str)
// std::vector<std::string> split_utils::splitTypeToParts(const std::string &str)

// std::string convertTypeNameToPath(const std::string &name)
// std::vector<std::string> splitTypeToParts(const std::string &str)
// std::string md_utils::formatExternalHyperLink(const std::string &text, const std::string &link, bool makeLink = true) // 

}

//----------------------------------------------------------------------------
// struct FunctionInfo
// {
//     bool                                memberField = false;
//     std::string                         name      ; // function name, or member field name
//     std::vector<FunctionParameterInfo>  parameters;
//     std::string                         type      ; // function return type, or member field type

inline 
ssq::sqstring generateFunctionDocs( const FunctionInfo &fi
                                  , const EnumScriptGenerationOptions &genOptions
                                  , const std::string &classFullName // or ns for free functions
                                  , bool bMethod
                                  )
{
    if (fi.memberField) // Для полей не заводим отдельные странички
    {
        return ssq::sqstring();
    }

    std::string functionFullName = utils::appendType(classFullName, fi.name);

    std::string resBuf;
    auto resWritter = umba::StringRefCharWriter(resBuf);
    auto res = umba::SimpleFormatter(&resWritter);

    auto appendLinefeed = [&]()
    {
        res << "\n";
    };


    if (genOptions.generationType==EnumScriptGenerationType::trTemplate)
    {
        if (functionFullName=="DotNut/Tr/getDefCategory" || functionFullName=="DotNut.Tr.getDefCategory")
        {
            int a = 0;
            MARTY_ARG_USED(a);
        }
        genOptions.addTrIfEmpty("__BRIEF"         , functionFullName);
        genOptions.addTrIfEmpty("__DESCRIPTION"   , functionFullName);
        genOptions.addTrIfEmpty("__RETURNS"       , functionFullName);
        genOptions.addTrIfEmpty("__REMARKS"       , functionFullName);

    }

    else if (genOptions.generationType==EnumScriptGenerationType::mdDoc)
    {
        std::string fnTypeName = bMethod ? "method" : "function";
        res << genOptions.getSectionSeparatorString();
        res << genOptions.makeSectionInformerString( fnTypeName + " " + functionFullName);
        res << genOptions.makeSectionHeaderMdString(genOptions.getTrSubst(fnTypeName + "-section-title", "_md-common", {{"Name",functionFullName}}));

        // genOptions.getTr

        if (genOptions.hasTr("__BRIEF", functionFullName))
        {
            appendLinefeed(); //appendLinefeed();
            res << genOptions.getTr("__BRIEF", functionFullName) << "\n";
        }


        appendLinefeed(); appendLinefeed();
        res << genOptions.getCodeStartMdString();
        //res << genOptions.getNamespaceListingStartMdString(nsFullName);
        {
            res << fi.formatSquirrelLike( 0 /* indentStrLen() */ , 0 /* maxNameLen */ , 0 /* maxTypeLen */ , genOptions.mdAddTypeComment, genOptions.mdAddRetVal);
        }
        //res << genOptions.getNamespaceListingEndMdString(nsFullName);
        res << genOptions.getCodeEndMdString();
        appendLinefeed();


        if (genOptions.hasTr("__DESCRIPTION", functionFullName))
        {
            appendLinefeed();
            res << genOptions.getTr("__DESCRIPTION", functionFullName) << "\n";
            appendLinefeed();
        }

        appendLinefeed(); //appendLinefeed();
        res << genOptions.makeSubsectionHeaderMdString(genOptions.getTr(fnTypeName + "-parameters-section-title", "_md-common")) << "\n";
        if (fi.parameters.empty())
        {
            res << genOptions.getTr(fnTypeName + "-no-parameters-text", "_md-common") << "\n\n";
        }
        else
        {
            for(const auto &param: fi.parameters)
            {
                auto extTypeRefText = formatExternalTypeRef( genOptions, param.type, functionFullName);
                res << md_utils::formatEm(param.name) 
                    << " ("
                    << extTypeRefText
                    << ") - " 
                    << genOptions.getTr(param.name, functionFullName) 
                    << "\n\n"
                    ;
            }
            // struct FunctionParameterInfo
            // {
            //     std::string   name;
            //     std::string   type;
        }

        if (fi.hasReturnValue() && !utils::isSquirrelTypeVoidKeywords(fi.type))
        {
            res << genOptions.makeSubsectionHeaderMdString(genOptions.getTr(fnTypeName + "-returns-section-title", "_md-common")) << "\n";

            auto typeInfoText = formatExternalTypeRef( genOptions, fi.type, functionFullName);
            //auto formatStr    = genOptions.getTr(fnTypeName + "-return-type-text", "_md-common");

            res << genOptions.getTrSubst(fnTypeName + "-return-type-text", "_md-common", {{"TypeInfo",typeInfoText}}) << "\n\n";

            if (genOptions.hasTr("__RETURNS", functionFullName))
            {
                res << genOptions.getTr("__RETURNS", functionFullName) << "\n\n";
            }

        }

        if (genOptions.hasTr("__REMARKS", functionFullName))
        {
            res << genOptions.getTr("__REMARKS", functionFullName) << "\n\n";
        }

    }

    return toSqStringFromUtf8(resBuf);
}

//----------------------------------------------------------------------------
inline 
ssq::sqstring generateExactClassDocs( const ClassInfo &ci, const EnumScriptGenerationOptions &genOptions, const std::string &classNs )
{
    std::string resBuf;
    auto resWritter = umba::StringRefCharWriter(resBuf);
    auto res = umba::SimpleFormatter(&resWritter);

    std::string infoTextBuf;
    auto infoTextBufWritter = umba::StringRefCharWriter(infoTextBuf);
    auto info = umba::SimpleFormatter(&infoTextBufWritter);

    std::string methodsDocTextBuf;
    auto methodsDocTextBufWritter = umba::StringRefCharWriter(methodsDocTextBuf);
    auto methodsDoc = umba::SimpleFormatter(&methodsDocTextBufWritter);


    auto appendInfo = [&]()
    {
        res.flush();
        info.flush();
        resBuf.append(1u,'\n');
        resBuf.append(infoTextBuf);
        infoTextBuf.clear();
    };

    auto appendMethodsDoc = [&]()
    {
        res.flush();
        methodsDoc.flush();
        resBuf.append(1u,'\n');
        resBuf.append(methodsDocTextBuf);
        methodsDocTextBuf.clear();
    };

    auto baseIndentVal = genOptions.getClassListingInnerIndent();
    auto indentStr = [&](std::size_t extra=0u)
    {
        return std::string(baseIndentVal+extra, ' ');
    };

    auto indentStrLen = [&](std::size_t extra=0u)
    {
        return baseIndentVal+extra;
    };


    auto appendLinefeed = [&]()
    {
        res << "\n";
    };

    std::string classFullName = utils::appendType(classNs, ci.name);


    if (genOptions.generationType==EnumScriptGenerationType::trTemplate)
    {
        genOptions.addTrIfEmpty("__BRIEF", classFullName);
        genOptions.addTrIfEmpty("__DESCRIPTION", classFullName);
        //genOptions.addTr("__REMARKS", classFullName);

        for(const auto &fi : ci.functions)
        {
            if (fi.memberField) // для простых полей отдельных страничек не генерим
            {
                genOptions.addTrIfEmpty(fi.name, classFullName);
            }
            else // Для функций будет отдельная страничка, там и brief и полное описание
            {
                generateFunctionDocs( fi, genOptions, classFullName, true /* method */);
            }
        }


 
        // for ( FullQualifiedTypeNameMap<ClassInfo>::const_iterator cit = nsClassesMap.begin()
        //     ; cit!=nsClassesMap.end()
        //     ; ++cit
        //     )
        // {
        //     if (!cit->first.empty())
        //     {
        //         continue;
        //     }
        //  
        //     for(const auto &fi : cit->second.functions)
        //     {
        //         genOptions.addTrIfEmpty(fi.name, nsFullName);
        //     }
        // }
    }
 
    else if (genOptions.generationType==EnumScriptGenerationType::mdDoc)
    {
        ClassInfo ciVariables, ciFunctions;
        //ci.splitFunctionsAndVariablesAndSort(ciFunctions, ciVariables);
        ci.splitFunctionsAndVariables(ciFunctions, ciVariables);
    
        res << genOptions.getSectionSeparatorString();
        res << genOptions.makeSectionInformerString( std::string("class") + " " + classFullName);
        res << genOptions.makeSectionHeaderMdString(genOptions.getTrSubst(std::string("class") + "-section-title", "_md-common", {{"Name",classFullName}}));


        if (genOptions.hasTr("__BRIEF", classFullName))
        {
            appendLinefeed(); appendLinefeed();
            res << genOptions.getTr("__BRIEF", classFullName) << "\n";
        }


        appendLinefeed(); appendLinefeed();
        res << genOptions.getCodeStartMdString();
        res << genOptions.getClassListingStartMdString(classFullName);
        {
            //res << fi.formatSquirrelLike( 0 /* indentStrLen() */ , 0 /* maxNameLen */ , 0 /* maxTypeLen */ , genOptions.mdAddTypeComment, genOptions.mdAddRetVal);

            if (!ciVariables.functions.empty())
            {
                //res  << "\n" << 
                res  << indentStr() << "// Member Fields\n\n";
                info << "\n" << genOptions.makeSubsectionHeaderMdString(genOptions.getTr("class-members-section-title", "_md-common")) << "\n";
    
                std::size_t maxNameLen = 0, maxTypeLen = 0;
                ciVariables.findFunctionNamesMaxLen(maxNameLen, maxTypeLen);
    
                for (const auto &var: ciVariables.functions)
                {
                    auto listingStr = var.formatSquirrelLike(indentStrLen(), maxNameLen, maxTypeLen, genOptions.mdAddTypeComment, genOptions.mdAddRetVal);
                    res << listingStr << "\n";
                    auto extTypeRefText = formatExternalTypeRef( genOptions, var.type, classFullName);
                    info << md_utils::formatEm(var.name) 
                         << " (" 
                         << extTypeRefText
                         << ") - " 
                         << genOptions.getTr(var.name, classFullName) 
                         << "\n\n"
                         ;
                }
                res << "\n";
    
            } // if (!ciVariables.functions.empty())
    
            if (!ciFunctions.functions.empty())
            {
                if (!ciVariables.functions.empty())
                {
                    res << "\n";
                }
                res  << indentStr() << "// Functions\n\n";
                info << "\n" << genOptions.makeSubsectionHeaderMdString(genOptions.getTr("class-methods-section-title", "_md-common")) << "\n";
    
                std::size_t maxNameLen = 0, maxTypeLen = 0;
                ciFunctions.findFunctionNamesMaxLen(maxNameLen, maxTypeLen);
    
                for (const auto &fn: ciFunctions.functions)
                {
                    // generating class listing

                    res << fn.formatSquirrelLike(indentStrLen(), maxNameLen, maxTypeLen, genOptions.mdAddTypeComment, genOptions.mdAddRetVal);

                    if (genOptions.mdAddRetVal)
                        res << "\n";


                    // generating class info

                    auto refTypeFullName = utils::appendType(classFullName, fn.name);

                    bool isSpecialMethod      = utils::isSquirrelTypeSpecialMethodsKeywords(fn.name);
                    bool isBasicConvertMethod = utils::isSquirrelTypeBasicConvertMethodsKeywords(fn.name);
                    bool isArrayCompatMethod  = utils::isSquirrelTypeArrayCompatMethodsKeywords(fn.name);
                    

                    // SquirrelTypeBasicConvertMethodsKeywords

                    // MARTY_SIMPLESQUIRREL_CLASS_INFO_IMPLEMENT_IS_KEYWORD(SquirrelTypeSpecialMethodsKeywords)
                    // // bool isSquirrelType*(std::string str)

                    //auto fnTypeFullName  = utils::appendType(nsFullName, fn.name);
                    auto extRefText  = isSpecialMethod || isBasicConvertMethod || isArrayCompatMethod
                                     ? md_utils::formatSpecialMethod(fn.name)
                                     : formatExternalMemberRef(genOptions, fn.name, refTypeFullName, classFullName)
                                     ;

                    auto extRefBrief = isSpecialMethod || isBasicConvertMethod || isArrayCompatMethod
                                     ? genOptions.getTr(fn.name, "_sq-common")
                                     : genOptions.getTr("__BRIEF", refTypeFullName) ;

                    info << "\n" 
                         // << md_utils::formatEm(fn.name) 
                         << extRefText
                         << " - "
                         << extRefBrief
                         ;

                    info << "\n\n";

                    methodsDoc << fromSqStringToUtf8(generateFunctionDocs( fn, genOptions, classFullName, true /* method */));
                }
    
                res << "\n";
    
            } // if (!ciFunctions.functions.empty())

        }
        res << genOptions.getClassListingEndMdString(classFullName);
        res << genOptions.getCodeEndMdString();
        appendLinefeed();

        //res << "// Code goes here\n";

// class skill_info
// {
//   name = "";
//   note = [""];

// class shape_hider extends is_shape
// {
//   shape     = null;
//   visible   = true;
//   
//   constructor(p_shape, b_visible)
//   {
//     shape     = p_shape;
//     visible   = b_visible;
//   }
//   
//   function draw(dc, scr, at)
//   {
//     if( visible ) { shape.draw(dc, scr, at); }


        if (genOptions.hasTr("__DESCRIPTION", classFullName))
        {
            appendLinefeed(); appendLinefeed();
            res << genOptions.getTr("__DESCRIPTION", classFullName) << "\n";
        }

        // res << "// Parameters goes here\n";
        //  
        // if (genOptions.hasTr("__REMARKS", classFullName))
        // {
        //     appendLinefeed(); appendLinefeed();
        //     res << genOptions.getTr("__REMARKS", classFullName);
        // }

    }    

    appendInfo();
    appendMethodsDoc();

    return toSqStringFromUtf8(resBuf);

}

//----------------------------------------------------------------------------
inline 
ssq::sqstring generateClassDocs( const ClassInfo &ci, const EnumScriptGenerationOptions &genOptions /* , const std::string &rootNs=std::string() */  )
{
    FullQualifiedTypeNameMap< FullQualifiedTypeNameMap<ClassInfo> > collectedClasses;
    collectClasses( collectedClasses, ci, std::string() /* rootNs */ );


    std::string resBuf;
    auto resWritter = umba::StringRefCharWriter(resBuf);
    auto res = umba::SimpleFormatter(&resWritter);

    std::string infoTextBuf;
    auto infoTextBufWritter = umba::StringRefCharWriter(infoTextBuf);
    auto info = umba::SimpleFormatter(&infoTextBufWritter);

    std::string classDocTextBuf;
    auto classDocTextBufWritter = umba::StringRefCharWriter(classDocTextBuf);
    auto classDoc = umba::SimpleFormatter(&classDocTextBufWritter);


    auto appendInfo = [&]()
    {
        res.flush();
        info.flush();
        resBuf.append(1u,'\n');
        resBuf.append(infoTextBuf);
        infoTextBuf.clear();
    };

    auto appendClassDoc = [&]()
    {
        res.flush();
        classDoc.flush();
        resBuf.append(1u,'\n');
        resBuf.append(classDocTextBuf);
        classDocTextBuf.clear();
    };

    auto appendLinefeed = [&]()
    {
        res << "\n";
    };

    auto baseIndentVal = genOptions.getNamespaceListingInnerIndent();
    auto indentStr = [&](std::size_t extra=0u)
    {
        return std::string(baseIndentVal+extra, ' ');
    };

    auto indentStrLen = [&](std::size_t extra=0u)
    {
        return baseIndentVal+extra;
    };

    auto makeFillForSize = [](std::size_t nCur, std::size_t nMax)
    {
        if (nCur>=nMax)
            return std::string();
        return std::string(nMax-nCur, ' ');
    };

    auto makeFillForString = [&](const std::string &str, std::size_t nMax)
    {
        return makeFillForSize(str.size(), nMax);
    };


    
    for ( FullQualifiedTypeNameMap< FullQualifiedTypeNameMap<ClassInfo> >::const_iterator ccNsIt = collectedClasses.begin()
        ; ccNsIt!=collectedClasses.end()
        ; ++ccNsIt
        )
    {
        const std::string &nsFullName = ccNsIt->first;
        const FullQualifiedTypeNameMap<ClassInfo> &nsClassesMap = ccNsIt->second;

        // Тут делаем страничку по namespace'у

        if (genOptions.generationType==EnumScriptGenerationType::trTemplate)
        {
            genOptions.addTrIfEmpty(std::string("__DESCRIPTION"), nsFullName);

            for ( FullQualifiedTypeNameMap<ClassInfo>::const_iterator cit = nsClassesMap.begin()
                ; cit!=nsClassesMap.end()
                ; ++cit
                )
            {
                if (cit->first.empty())
                {
                    for(const auto &fi : cit->second.functions)
                    {
                        if (fi.memberField) // для простых полей отдельных страничек не генерим
                        {
                            genOptions.addTrIfEmpty(fi.name, nsFullName);
                        }
                        else // Для функций будет отдельная страничка, там и brief и полное описание
                        {
                            generateFunctionDocs( fi, genOptions, nsFullName, false /* free function */);
                        }
                    }
                }
                else
                {
                
                    ClassInfo clsInfo = cit->second;
                    if (genOptions.mdSortNamespaceClasses)
                    {
                        clsInfo.sortClasses();
                    }
                    generateExactClassDocs(clsInfo, genOptions, nsFullName);

                    // if (!clsInfo.name.empty())
                    // {
                    //     auto clsName = clsInfo.name;
                    //     auto clsFullName = utils::appendType(nsFullName, clsName);
                    //     genOptions.addTrIfEmpty("__BRIEF", clsFullName);
                    //     genOptions.addTrIfEmpty("__DESCRIPTION", clsFullName);
                    //     genOptions.addTrIfEmpty("__REMARKS", clsFullName);
                    // }

                }

            }
        }

        else if (genOptions.generationType==EnumScriptGenerationType::mdDoc)
        {
            if (nsClassesMap.empty())
            {
                continue;
            }

            res << genOptions.getSectionSeparatorString();
            res << genOptions.makeSectionInformerString("namespace " + nsFullName);
            res << genOptions.makeSectionHeaderMdString(genOptions.getTrSubst("namespace-section-title", "_md-common", {{"Name",nsFullName}}));

            if (genOptions.hasTr("__DESCRIPTION", nsFullName))
            {
                appendLinefeed(); appendLinefeed();
                res << genOptions.getTr("__DESCRIPTION", nsFullName) << "\n";
            }

            bool hasKlasse    = false;
            bool hasFunctions = false;
            bool hasReturnValue = false;

            res << genOptions.getCodeStartMdString();
            res << genOptions.getNamespaceListingStartMdString(nsFullName);
            {
                for ( FullQualifiedTypeNameMap<ClassInfo>::const_iterator cit = nsClassesMap.begin()
                    ; cit!=nsClassesMap.end()
                    ; ++cit
                    )
                {
                    hasFunctions = false;

                    if (!cit->first.empty())
                    {
                        hasKlasse = true;
                        continue; // пока пропускаем классы
                    }
                    else 
                    {
                    }

                    // У нас тут свободные функции в пространстве имён, и свободные переменные
                    ClassInfo ciVariables, ciFunctions;
                    //cit->second.splitFunctionsAndVariablesAndSort(ciFunctions, ciVariables);
                    cit->second.splitFunctionsAndVariables(ciFunctions, ciVariables);

                    if (!ciVariables.functions.empty())
                    {
                        hasFunctions = true;

                        res  << "\n" << indentStr() << "// Variables\n\n";
                        info << "\n" << genOptions.makeSubsectionHeaderMdString(genOptions.getTr("namespace-vars-section-title", "_md-common")) << "\n";

                        std::size_t maxNameLen = 0, maxTypeLen = 0;
                        ciVariables.findFunctionNamesMaxLen(maxNameLen, maxTypeLen);

                        for (const auto &var: ciVariables.functions)
                        {
                            res << var.formatSquirrelLike(indentStrLen(), maxNameLen, maxTypeLen, genOptions.mdAddTypeComment, genOptions.mdAddRetVal);
                            info << md_utils::formatEm(var.name) 
                                 << " (" 
                                 << formatExternalTypeRef( genOptions, var.type, nsFullName)
                                 << ") - " 
                                 << genOptions.getTr(var.name, nsFullName) 
                                 << "\n\n"
                                 ;
                        }
                        res << "\n";

                    } // if (!ciVariables.functions.empty())

                    if (!ciFunctions.functions.empty())
                    {
                        res << "\n";
                        res  << "\n" << indentStr() << "// Functions\n\n";
                        info << "\n" << genOptions.makeSubsectionHeaderMdString(genOptions.getTr("namespace-functions-section-title", "_md-common")) << "\n";

                        std::size_t maxNameLen = 0, maxTypeLen = 0;
                        ciFunctions.findFunctionNamesMaxLen(maxNameLen, maxTypeLen);

                        for (const auto &fn: ciFunctions.functions)
                        {
                            res << fn.formatSquirrelLike(indentStrLen(), maxNameLen, maxTypeLen, genOptions.mdAddTypeComment, genOptions.mdAddRetVal);
                            if (genOptions.mdAddRetVal)
                                res << "\n";
                            //auto fnTypeFullName  = utils::appendType(nsFullName, fn.name);
                            auto refTypeFullName = utils::appendType(nsFullName, fn.name);
                            auto extRefText  = formatExternalMemberRef(genOptions, fn.name, refTypeFullName, nsFullName);
                            info << "\n" 
                                 // << md_utils::formatEm(fn.name) 
                                 << extRefText
                                 << " - " 
                                 << genOptions.getTr(("__BRIEF"), refTypeFullName) 
                                 << "\n\n";
                            classDoc << fromSqStringToUtf8(generateFunctionDocs( fn, genOptions, nsFullName, false /* free function */));

                            hasReturnValue = fn.hasReturnValue();
                        }

                        res << "\n";

                    } // if (!ciFunctions.functions.empty())

                } // for not classes


                std::size_t maxClassNameLen = 0;

                if (!nsClassesMap.empty() && hasKlasse)
                {
                    if (hasFunctions && hasReturnValue && genOptions.mdAddRetVal)
                    {
                        res << "\n";
                    }
                    res  << indentStr() << "// Classes\n\n";
                    info << "\n" << genOptions.makeSubsectionHeaderMdString(genOptions.getTr("namespace-classes-section-title", "_md-common")) << "\n";
                }

                for ( FullQualifiedTypeNameMap<ClassInfo>::const_iterator cit = nsClassesMap.begin()
                    ; cit!=nsClassesMap.end()
                    ; ++cit
                    )
                {
                    if (cit->first.empty()) // ns member?
                        continue; // теперь пропускаем не классы

                    ClassInfo clsInfo = cit->second; clsInfo.sortClasses();

                    if (!clsInfo.name.empty())
                    {
                        auto clsName = clsInfo.name;
                        auto clsFullName = utils::appendType(nsFullName, clsName);

                        res  << indentStr() << "class " << clsName << makeFillForString(clsName, maxClassNameLen) << ";\n";
                        //auto clsFullName = utils::appendType(nsFullName, clsName);
                        //auto extRefText = formatExternalTypeRef(genOptions, clsName, clsFullName, nsFullName, false) ;
                        auto extRefText = formatExternalMemberRef(genOptions, clsName, clsFullName, nsFullName) ;
                        info << "\n" 
                             << extRefText
                             << " - " 
                             << genOptions.getTr("__BRIEF", clsFullName) << "\n";

                        auto exactClassDoc    = generateExactClassDocs(clsInfo, genOptions, nsFullName);
                        auto exactClassDocUtf = fromSqStringToUtf8(exactClassDoc);
                        classDoc << exactClassDocUtf;
                    }

                } // for classes

                if (!nsClassesMap.empty())
                {
                    res << "\n";
                }

            }
            res << genOptions.getNamespaceListingEndMdString(nsFullName);
            res << genOptions.getCodeEndMdString();

            appendInfo();

            
        } // else if

    } // for collectedClasses


    appendClassDoc();

    return toSqStringFromUtf8(resBuf);
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace marty_simplesquirrel




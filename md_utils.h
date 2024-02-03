#pragma once

#include "umba/umba.h"
//
#include <string>


namespace marty_simplesquirrel {

namespace md_utils {



// https://www.markdownguide.org/basic-syntax/
// https://www.markdownguide.org/basic-syntax/#links

inline
std::string formatExternalHyperLink(const std::string &text, const std::string &link, bool makeLink = true) // 
{
    if (!makeLink)
    {
        return text;
    }

    return std::string(1,'[') + text + "](" + link + ")";

}

inline
std::string formatCodeStart(const std::string &lang=std::string())
{
    return std::string("```") + lang + "\n";
}

inline
std::string formatCodeEnd(const std::string &lang=std::string())
{
    UMBA_USED(lang);
    return std::string("```")  + "\n";
}

inline
std::string formatBold(const std::string &text) // 
{
    return std::string(2,'*') + text + std::string(2,'*');
}

inline
std::string formatIntegralType(const std::string &text) // 
{
    return formatBold(text);
}

inline
std::string formatContainerType(const std::string &text) // 
{
    return formatBold(text);
}

inline
std::string formatIntegralOrContainerType(const std::string &text) // 
{
    return formatBold(text);
}

inline
std::string formatEm(const std::string &text) // 
{
    return formatBold(text);
}

inline
std::string formatSpecialMethod(const std::string &text) // 
{
    return formatBold(text);
}



} // namespace md_utils

} // namespace marty_simplesquirrel



#pragma once

#include "umba/umba.h"
//
#include "umba/string_plus.h"
//
#include "umba/macro_helpers.h"
#include "umba/macros.h"
//
#include "enums.h"
//
#include <cstddef>
//
#include "marty_tr/marty_tr.h"
//
#include "md_utils.h"



namespace marty_simplesquirrel{

//enum class EnumScriptGenerationType : std::uint32_t


struct EnumScriptGenerationOptions
{
    EnumScriptGenerationType      generationType                    = EnumScriptGenerationType::invalid;

    std::size_t                   mdSectionLevel                    = 2;
    bool                          mdAddVals                         = false; // Добавляет к именованным константам enum/flags их числовое значение
    bool                          mdAddRetVal                       = true ; // Добавляет к прототипам функций описание возвращаемого значения
    bool                          mdAddTypeComment                  = true ; // Добавляет к параметрам функций и к переменным коментарий с типом значения
    bool                          mdMakeListingsNamespaceScope      = true ; // Для namespace делает обрамление table /*namespace*/ NsName\n{\n}\n
    std::string                   mdLang                            = "en";
    std::string                   mdSectionSep                      = "-------";
    std::string                   mdCodeTag                         = "squirrel"; // c, c_cpp, clike?
    std::string                   mdFileExt                         = ".md";
    bool                          mdSortNamespaceClasses            = false ;
    bool                          mdAlterTypesSpacing               = false ;
    ListingNestedTagsMode         listingNestedTagsMode             = ListingNestedTagsMode::remove;
    std::string                   codeTagPrefix                     = "//!#"; // for listings


    void addTr(const std::string &str, const std::string &category) const
    {
        marty_tr::tr_add(str, std::string(), category, mdLang);
    }

    std::string makeSectionHeaderMdString(const std::string &title) const
    {
        return std::string(mdSectionLevel, '#') + std::string(1, ' ') + title + std::string(1, '\n');
    }

    std::string makeSubsectionHeaderMdString(const std::string &title) const
    {
        return std::string(mdSectionLevel+1, '#') + std::string(1, ' ') + title + std::string(1, '\n');
    }

    std::string getSectionSeparatorString() const
    {
        if (mdSectionSep.empty())
            return std::string(1,'\n'); // Пустую строку вставим в любом случае
            // return mdSectionSep;

        return mdSectionSep + "\n";
    }

    std::string makeSectionInformerString(const std::string &informerStr) const
    {
        if (mdSectionSep.empty())
            return std::string();

        return informerStr + "\n";
    }

    std::string getTr(const std::string &str, const std::string &category) const
    {
        return marty_tr::tr(str, category, mdLang);
    }

    bool hasTr(const std::string &str, const std::string &category) const
    {
        return marty_tr::tr_has_msg(str, category, mdLang);
    }

    std::string getTrSubst(const std::string &str, const std::string &category, const umba::macros::StringStringMap<std::string> &macros) const
    {
        return umba::macros::substMacros( getTr(str, category)
                                        , umba::macros::MacroTextFromMapRef<std::string>(macros)
                                        , umba::macros::keepUnknownVars
                                        );
    }

    std::string getCodeStartMdString() const
    {
        return md_utils::formatCodeStart(mdCodeTag);
    }

    std::string getCodeEndMdString() const
    {
        return md_utils::formatCodeEnd(mdCodeTag);
    }

    std::size_t getNamespaceListingInnerIndent() const
    {
        return mdMakeListingsNamespaceScope ? 4u : 0u;
    }

    std::size_t getClassListingInnerIndent() const
    {
        return 4u; // mdMakeListingsNamespaceScope ? 4u : 0u;
    }

    std::string getNamespaceListingStartMdString(const std::string &nsName) const
    {
        return mdMakeListingsNamespaceScope ? std::string("table /* namespace */ ") + nsName + std::string("\n{\n") : std::string();
    }

    std::string getNamespaceListingEndMdString(const std::string &nsName) const
    {
        return mdMakeListingsNamespaceScope ? std::string("} // table namespace ") + nsName + std::string("\n") : std::string();
    }

    std::string getClassListingStartMdString(const std::string &clsName) const
    {
        return std::string("class ") + clsName + std::string("\n{\n");
    }

    std::string getClassListingEndMdString(const std::string &clsName) const
    {
        return std::string("} // class ") + clsName + std::string("\n");
    }

    

}; // struct EnumScriptGenerationOptions



} // namespace marty_simplesquirrel


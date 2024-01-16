#pragma once

#include "enums.h"
//
#include <cstddef>


namespace marty_simplesquirrel{

//enum class EnumScriptGenerationType : std::uint32_t


struct EnumScriptGenerationOptions
{
    EnumScriptGenerationType      generationType = EnumScriptGenerationType::invalid;

    std::size_t                   mdSectionLevel = 3;
    bool                          mdAddVals      = false;
    std::string                   mdLang         = "en";
    std::string                   mdSectionSep   = "-------";

}; // struct EnumScriptGenerationOptions



} // namespace marty_simplesquirrel


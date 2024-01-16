#pragma once

#include "marty_cpp/marty_enum.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>



namespace marty_simplesquirrel{

enum class EnumScriptGenerationType : std::uint32_t
{
    invalid            = (std::uint32_t)(-1),
    unknown            = (std::uint32_t)(-1),
    singleLineScript   = 0x00,
    multiLineScript    = 0x01,
    mdDoc              = 0x02,
    trTemplate         = 0x03

}; // enum class EnumScriptGenerationType : std::uint32_t

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( EnumScriptGenerationType, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( EnumScriptGenerationType::invalid            , "Invalid"          );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( EnumScriptGenerationType::singleLineScript   , "SingleLineScript" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( EnumScriptGenerationType::multiLineScript    , "MultiLineScript"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( EnumScriptGenerationType::mdDoc              , "MdDoc"            );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( EnumScriptGenerationType::trTemplate         , "TrTemplate"       );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( EnumScriptGenerationType, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( EnumScriptGenerationType, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( EnumScriptGenerationType::invalid            , "invalid"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( EnumScriptGenerationType::invalid            , "unknown"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( EnumScriptGenerationType::singleLineScript   , "singlelinescript" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( EnumScriptGenerationType::multiLineScript    , "multilinescript"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( EnumScriptGenerationType::mdDoc              , "mddoc"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( EnumScriptGenerationType::trTemplate         , "trtemplate"       );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( EnumScriptGenerationType, std::map, 1 )

} // namespace marty_simplesquirrel


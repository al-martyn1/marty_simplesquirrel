#pragma once

#include "umba/umba.h"
//
#include "umba/string_plus.h"

//
#include "nlohmann/json.hpp"

//
#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"

//
#include "umba/gmesg.h"

//
#include "simplesquirrel.h"

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

// marty_simplesquirrel::json_helpers::

namespace marty_simplesquirrel {

namespace json_helpers {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
nlohmann::json::iterator findJsonAnyChild(nlohmann::json &j, const std::string &n1)
{
    return j.find(n1.c_str());
}

nlohmann::json::iterator findJsonAnyChild(nlohmann::json &j, const std::string &n1, const std::string &n2)
{
    auto iter = j.find(n1.c_str());
    if (iter==j.end())
        iter = j.find(n2.c_str());
    return iter;
}

//----------------------------------------------------------------------------
nlohmann::json readGenericJsonFromUtfString( const std::string &jsonStr
                                           , const std::string &fileName
                                           )
{
 
    std::string errMsg;
    std::string tmpJson;
    marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
    //nlohmann::json j = marty::json_utils::parseJsonOrYaml( "null" );
    nlohmann::json j = marty::json_utils::parseJsonOrYaml( jsonStr, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );
 
    if (detectedFormat==marty::json_utils::FileFormat::unknown)
    {
        std::ostringstream oss;
        oss << fileName << ": " << ": error: " << errMsg << "\n";
        if (!tmpJson.empty())
        {
            oss << "JSON:" << "\n";
            oss << tmpJson << "\n";
        }
 
        umba::gmesg(oss.str());

        throw std::runtime_error(oss.str());
    }
 
    return j;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace json_helpers


} // namespace marty_simplesquirrel {



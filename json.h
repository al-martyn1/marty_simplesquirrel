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
#include "umba/utf8.h"

//
#include "simplesquirrel.h"

#include "simplesquirrel/array.hpp"
#include "simplesquirrel/table.hpp"

//
#include <deque>
#include <queue>
#include <utility>

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

// marty_simplesquirrel::json_helpers::

namespace marty_simplesquirrel {

namespace json_helpers {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
inline
nlohmann::json::iterator findJsonAnyChild(nlohmann::json &j, const std::string &n1)
{
    return j.find(n1.c_str());
}

inline
nlohmann::json::iterator findJsonAnyChild(nlohmann::json &j, const std::string &n1, const std::string &n2)
{
    auto iter = j.find(n1.c_str());
    if (iter==j.end())
        iter = j.find(n2.c_str());
    return iter;
}

//----------------------------------------------------------------------------
inline
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
// void parseJsonObjects(HSQUIRRELVM hVm, std::queue< std::pair<nlohmann::json, ssq::Object> > &q, const std::string &fileName);
//  
// inline
// ssq::Object parseJsonNode(HSQUIRRELVM hVm, nlohmann::json j, std::queue< std::pair<nlohmann::json, ssq::Object> > &q, const std::string &fileName)
// {
//     (void)hVm;
//     (void)j;
//     (void)q;
//     (void)fileName;
// }

inline
void parseJsonObjectsToSqurrel(HSQUIRRELVM hVm, std::queue< std::pair<nlohmann::json, ssq::Object> > &q, const std::string &fileName)
{
    // (void)hVm;
    // (void)fileName;

// front - access the first element (public member function)
// back  - access the last element (public member function)
// empty - checks whether the container adaptor is empty (public member function)
// size  - returns the number of elements (public member function)
// push  - inserts element at the end (public member function)
// push_range - (C++23) inserts a range of elements at the end (public member function)
// emplace - (C++11) constructs element in-place at the end (public member function)
// pop   - removes the first element (public member function)

// // other stuff
// j.size();     // 4 entries
// j.empty();    // false
// j.type();     // json::value_t::array
// j.clear();    // the array is empty again
//  
// // convenience type checkers
// j.is_null();
// j.is_boolean();
// j.is_number();
// j.is_object();
// j.is_array();
// j.is_string();

    // https://github.com/nlohmann/json#stl-like-access

    while(!q.empty())
    {
        nlohmann::json j = q.front().first;

        if (j.is_null())
        {
        }
        else if (j.is_object())
        {
            ssq::Table curTable = ssq::Table(q.front().second);
            q.pop();

            for (nlohmann::json::iterator it=j.begin(); it!=j.end(); ++it)
            {
                auto name = toSqStringFromUtf8(it.key());

                auto &jVal = it.value();

                if (jVal.is_object())
                {
                    auto childTable = curTable.addTable(name.c_str());
                    q.emplace(std::make_pair(jVal, ssq::Object(childTable)));
                }
                else if (jVal.is_array())
                {
                    auto childArray = ssq::Array(hVm);
                    q.emplace(std::make_pair(jVal, ssq::Object(childArray)));
                    curTable.set(name.c_str(), childArray);
                }
                else if (jVal.is_boolean())
                {
                    bool bVal = jVal.get<bool>();
                    curTable.set(name.c_str(), bVal);
                }
                else if (jVal.is_string())
                {
                    std::string strVal = jVal.get<std::string>();
                    curTable.set(name.c_str(), toSqStringFromUtf8(strVal));
                }
                else if (jVal.is_number())
                {
                    double dVal = jVal.get<double>();
                    curTable.set(name.c_str(), float(dVal));
                }
                else
                {
                    std::ostringstream oss;
                    oss << fileName << ": " << ": error: unknown JSON node type\n";
                    throw std::runtime_error(oss.str());
                }
            }

        }
        else if (j.is_array())
        {
            ssq::Array curArray = ssq::Array(q.front().second);
            q.pop();

            for (nlohmann::json::iterator it=j.begin(); it!=j.end(); ++it)
            {
                auto &jVal = it.value();

                if (jVal.is_object())
                {
                    //auto childTable = curTable.addTable(name.c_str());
                    auto childTable = ssq::Table(hVm);
                    q.emplace(std::make_pair(jVal, ssq::Object(childTable)));
                    curArray.push(childTable);
                }
                else if (jVal.is_array())
                {
                    auto childArray = ssq::Array(hVm);
                    q.emplace(std::make_pair(jVal, ssq::Object(childArray)));
                    //curTable.set(name.c_str(), childArray);
                    curArray.push(childArray);
                }
                else if (jVal.is_boolean())
                {
                    bool bVal = jVal.get<bool>();
                    curArray.push(bVal);
                }
                else if (jVal.is_string())
                {
                    std::string strVal = jVal.get<std::string>();
                    curArray.push(toSqStringFromUtf8(strVal));
                }
                else if (jVal.is_number())
                {
                    double dVal = jVal.get<double>();
                    curArray.push(float(dVal));
                }
                else
                {
                    std::ostringstream oss;
                    oss << fileName << ": " << ": error: unknown JSON node type\n";
                    throw std::runtime_error(oss.str());
                }

            }
        }

    }

}

//----------------------------------------------------------------------------
inline
ssq::Object parseJsonNodeToSqurrel(HSQUIRRELVM hVm, nlohmann::json j, const std::string &fileName)
{
    std::queue< std::pair<nlohmann::json, ssq::Object> > q;

    // (void)q;
    // (void)hVm;
    // (void)j;
    // (void)fileName;

    ssq::Object rootObj;

    if (j.is_object())
    {
        //Table(HSQUIRRELVM vm);
        auto t = ssq::Table(hVm);
        rootObj = ssq::Object(t);
        q.emplace(std::make_pair(j, rootObj));
    }
    else if (j.is_array())
    {
        auto a = ssq::Array(hVm);
        rootObj = ssq::Object(a);
        q.emplace(std::make_pair(j, rootObj));
    }
    else
    {
        std::ostringstream oss;
        oss << fileName << ": " << ": error: root JSON element must be array or object\n";
        // if (!tmpJson.empty())
        // {
        //     oss << "JSON:" << "\n";
        //     oss << tmpJson << "\n";
        // }

        throw std::runtime_error(oss.str());
    
    }

    //HSQOBJECT& getRaw();
    //const HSQUIRRELVM& getHandle() const;

    parseJsonObjectsToSqurrel(hVm, q, fileName);

    return rootObj;
}

//----------------------------------------------------------------------------
inline
ssq::Object parseJsonNodeToSqurrel(HSQUIRRELVM hVm, nlohmann::json j, const std::wstring &fileName)
{
    return parseJsonNodeToSqurrel(hVm, j, umba::toUtf8(fileName));
}



//----------------------------------------------------------------------------
// Коды лень тащить из marty_virtual_fsб используем просто int, помним, что 0 - это ока
inline
ssq::Table parseJsonTextToSqurrel(HSQUIRRELVM hVm, const std::string &jsonStr, const std::string &fileName, int readFileResult = 0)
{
    auto resTable = ssq::Table(hVm);

    if (readFileResult)
    {
        resTable.set(_SC("status" ), readFileResult);
        resTable.set(_SC("message"), toSqStringFromUtf8("File error"));
        return resTable;
    }

    try
    {
        nlohmann::json j = readGenericJsonFromUtfString(jsonStr, fileName);
        resTable.set(_SC("data"), parseJsonNodeToSqurrel(hVm, j, fileName));
        resTable.set(_SC("status" ), int(0));

        return resTable;
    }
    catch(const std::exception &e)
    {
        resTable.set(_SC("status" ), int(0x0B)); // invalidFormat
        resTable.set(_SC("message"), toSqStringFromUtf8(e.what()));
        umba::gmesg(e.what());
        return resTable;
    }
    catch(...)
    {
        resTable.set(_SC("status" ), int(0x01)); // genericError
        resTable.set(_SC("message"), toSqStringFromUtf8("Unknown JSON parsing error"));
        umba::gmesg("Unknown JSON parsing error");
        return resTable;
    }

}

inline
ssq::Table parseJsonTextToSqurrel(HSQUIRRELVM hVm, const std::string &jsonStr, const std::wstring &fileName, int readFileResult = 0)
{
    return parseJsonTextToSqurrel(hVm, jsonStr, umba::toUtf8(fileName), readFileResult);
}





} // namespace json_helpers


} // namespace marty_simplesquirrel {



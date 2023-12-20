#pragma once


#include "simplesquirrel.h"

#include <utility>

// marty_simplesquirrel::

namespace marty_simplesquirrel {


struct ScriptInfo
{
    ssq::sqstring    scriptText;
    ssq::sqstring    scriptFile;

}; // struct ScriptInfo


struct LoadedScripts
{
    std::vector<ssq::Script>   scripts;

    LoadedScripts() = default;
    LoadedScripts(const LoadedScripts &) = delete;
    LoadedScripts& operator=(const LoadedScripts &) = delete;
    LoadedScripts(LoadedScripts &&) = default;
    LoadedScripts& operator=(LoadedScripts &&) = default;

    static
    LoadedScripts load(ssq::VM &vm, const std::vector<ScriptInfo> &scriptInfos)
    {
        LoadedScripts ls;

        for(const auto &src: scriptInfos)
        {
            ls.scripts.emplace_back( vm.compileSource(src.scriptText.c_str(), src.scriptFile.c_str()) );
        }

        for(auto &l: ls.scripts)
        {
            vm.run(*&l);
        }

        return std::move(ls);
    }

}; // struct LoadedScripts


struct VmWithScripts
{
    ssq::VM           vm           ;
    LoadedScripts     loadedScripts;

    VmWithScripts() : vm(1024, 0), loadedScripts() {}
    VmWithScripts(const VmWithScripts &) = delete;
    VmWithScripts& operator=(const VmWithScripts &) = delete;
    VmWithScripts(VmWithScripts &&) = default;
    VmWithScripts& operator=(VmWithScripts &&) = default;

    VmWithScripts( size_t stackSize, ssq::Libs::Flag flags
                 , const std::vector<ScriptInfo> &scriptInfos
                 )
    : vm(stackSize, flags)
    , loadedScripts()
    {
        loadedScripts = LoadedScripts::load(vm, scriptInfos);
    }

    template<typename VmInitProc>
    VmWithScripts( size_t stackSize, ssq::Libs::Flag flags
                 , const std::vector<ScriptInfo> &scriptInfos
                 , VmInitProc vmInitProc
                 )
    : vm(stackSize, flags)
    , loadedScripts()
    {
        vmInitProc(vm);
        loadedScripts = LoadedScripts::load(vm, scriptInfos);
    }



}; // struct VmWithScripts




} // namespace marty_simplesquirrel


#pragma once


#include "simplesquirrel.h"
//
#include "squirrel_interface.h"
//
#include <utility>
//
#include"marty_assets_manager/types.h"

// marty_simplesquirrel::

//----------------------------------------------------------------------------
namespace marty_simplesquirrel {




//----------------------------------------------------------------------------
struct ScriptInfo
{
    ssq::sqstring    scriptText;
    ssq::sqstring    scriptFile;

}; // struct ScriptInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct LoadedScripts
{
    std::vector<ssq::Script>   scripts;

    LoadedScripts() = default;
    LoadedScripts(const LoadedScripts &) = delete;
    LoadedScripts& operator=(const LoadedScripts &) = delete;
    LoadedScripts(LoadedScripts &&) = default;
    LoadedScripts& operator=(LoadedScripts &&) = default;

    void clear()
    {
        scripts.clear();
    }

    void loadMoreScripts(ssq::VM &vm, const std::vector<ScriptInfo> &scriptInfos)
    {
        std::size_t sz = scripts.size();

        for(const auto &src: scriptInfos)
        {
            scripts.emplace_back( vm.compileSource(src.scriptText.c_str(), src.scriptFile.c_str()) );
        }

        for(; sz!=scripts.size(); ++sz)
        {
            vm.run(scripts[sz]);
        }

    }

    static
    LoadedScripts getLoadedScripts(ssq::VM &vm, const std::vector<ScriptInfo> &scriptInfos)
    {
        LoadedScripts ls;
        ls.loadMoreScripts(vm, scriptInfos);
        return std::move(ls);
    }

}; // struct LoadedScripts

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct VmWithScripts
{
    ssq::VM                    vm           ;
    std::vector<ScriptInfo>    scripts      ; // Пусть лежит тут на всякий случай
    LoadedScripts              loadedScripts;

    //------------------------------
    VmWithScripts() : vm(128, 0), loadedScripts() {}
    VmWithScripts(const VmWithScripts &) = delete;
    VmWithScripts& operator=(const VmWithScripts &) = delete;
    VmWithScripts(VmWithScripts &&) = default;
    VmWithScripts& operator=(VmWithScripts &&) = default;

    //------------------------------
    void clear()
    {
        loadedScripts.clear();
        scripts.clear();
        vm.destroy();
    }


    VmWithScripts( size_t stackSize, ssq::Libs::Flag flags
                 , const std::vector<ScriptInfo> &scriptInfos
                 )
    : vm(stackSize, flags)
    , scripts(scriptInfos)
    , loadedScripts()
    {
        loadedScripts = LoadedScripts::getLoadedScripts(vm, scriptInfos);
    }

    //------------------------------
    VmWithScripts( size_t stackSize, ssq::Libs::Flag flags
                 )
    : vm(stackSize, flags)
    , scripts()
    , loadedScripts()
    {
    }

    //------------------------------
    HSQUIRRELVM getVmHandle() const
    {
        return vm.getHandle();
    }

    //------------------------------
    template<typename VmInitProc>
    VmWithScripts( size_t stackSize, ssq::Libs::Flag flags
                 , const std::vector<ScriptInfo> &scriptInfos
                 , VmInitProc vmInitProc
                 )
    : vm(stackSize, flags)
    , scripts(scriptInfos)
    , loadedScripts()
    {
        vmInitProc(vm);
        loadedScripts = LoadedScripts::getLoadedScripts(vm, scriptInfos);
    }

    //------------------------------
    template<typename VmInitProc>
    VmWithScripts( size_t stackSize, ssq::Libs::Flag flags
                 , const std::vector<ScriptInfo>                          &predefinedScripts
                 , const marty_assets_manager::NutProjectT<ssq::sqstring> &nutProject
                 , VmInitProc vmInitProc
                 )
    : vm(stackSize, flags)
    , scripts() // scriptInfos
    , loadedScripts()
    {
        //std::vector<ScriptInfo>

        scripts = predefinedScripts;

        std::vector<ssq::sqstring>::const_iterator nit = nutProject.nuts.begin();
        std::vector<ssq::sqstring>::const_iterator dit = nutProject.nutsData.begin();

        for(; nit!=nutProject.nuts.end() && dit!=nutProject.nutsData.end(); ++nit, ++dit)
        {
            scripts.emplace_back(ScriptInfo{*dit, *nit});
        }

        vmInitProc(vm);
        loadedScripts = LoadedScripts::getLoadedScripts(vm, scriptInfos);
        
        //NutProjectT<ssq::sqstring>
    }

    //------------------------------
    void loadScripts(const std::vector<ScriptInfo> &newScripts)
    {
        scripts.insert(scripts.end(), newScripts.begin(), newScripts.end());
        loadedScripts.loadMoreScripts(vm, newScripts);
    }

    //------------------------------
    void loadScript(const ScriptInfo &newScript)
    {
        std::vector<ScriptInfo> newScripts;
        newScripts.emplace_back(newScript);
        loadScripts(newScripts);
    }

    //------------------------------
    void loadScript(const ssq::sqstring &scriptText, const ssq::sqstring &scriptFile)
    {
        loadScript(ScriptInfo{scriptText, scriptFile});
    }

    //------------------------------
    void loadNutProjectScripts(const marty_assets_manager::NutProjectT<ssq::sqstring> &nutProject)
    {
        std::vector<ScriptInfo> newScripts;
        std::vector<ssq::sqstring>::const_iterator nit = nutProject.nuts.begin();
        std::vector<ssq::sqstring>::const_iterator dit = nutProject.nutsData.begin();

        for(; nit!=nutProject.nuts.end() && dit!=nutProject.nutsData.end(); ++nit, ++dit)
        {
            newScripts.emplace_back(ScriptInfo{*dit, *nit});
        }

        scripts.insert(scripts.end(), newScripts.begin(), newScripts.end());

        loadedScripts.loadMoreScripts(vm, newScripts);
    
    }

    //------------------------------
    template<typename InterfaceClassType>
    InterfaceClassType extractGlobalInterface() const
    {
        return InterfaceClassType::extract(vm);
    }

    //------------------------------
    template<typename InterfaceClassType>
    InterfaceClassType extractGlobalInterface(const ssq::sqstring &nameOverride) const
    {
        return InterfaceClassType::extract(vm, nameOverride);
    }

    //------------------------------
    template<typename InterfaceClassType>
    InterfaceClassType extractObjectInterface(const ssq::Object &o) const
    {
        return InterfaceClassType::extract(vm, o);
    }


}; // struct VmWithScripts

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace marty_simplesquirrel




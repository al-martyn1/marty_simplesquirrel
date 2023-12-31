#pragma once


#include "simplesquirrel.h"

// marty_simplesquirrel::

namespace marty_simplesquirrel {


struct SquirrelClosure
{

    ssq::sqstring                  name; // clossure name

protected:

    std::optional<ssq::Function>   optClosure;

    // Можно хранить какой-то указатель на содержащий эти кложи интерфейс, но это мы что-то на спичках сэкономим
    // В vm.callFunc передаём 
    ssq::Object                    ssqThis; // vm, table or class

    // не работает
    //std::optional<ssq::VM>         optVm  ;
    //ssq::VM                        vm     ;

    ssq::VM                       *pVm = 0;
    //HSQUIRRELVM                      hvm = 0;

    // template<typename ObjType>
    // void resetObject(ObjType &rst)
    // {
    //     ObjType cleanObj;
    //  
    // }


public:

    void swap(SquirrelClosure &other)
    {
        std::swap(name       , other.name);
        std::swap(optClosure , other.optClosure);
        std::swap(ssqThis    , other.ssqThis);
        std::swap(pVm        , other.pVm);
        //std::swap(hvm        , other.hvm);
    }

    SquirrelClosure() = delete;
    // SquirrelClosure(const ssq::sqstring &n) : name(n), optClosure(std::nullopt), ssqThis(), optVm(std::nullopt) {}
    //SquirrelClosure(const ssq::sqstring &n) : name(n), optClosure(std::nullopt), ssqThis(), hvm(0) {}
    SquirrelClosure(const ssq::sqstring &n) : name(n), optClosure(std::nullopt), ssqThis(), pVm(0) {}

    SquirrelClosure(const SquirrelClosure &) = delete;
    SquirrelClosure& operator=(const SquirrelClosure &) = delete;

    // SquirrelClosure(SquirrelClosure &&) = default;
    // SquirrelClosure& operator=(SquirrelClosure &&) = default;

    SquirrelClosure(SquirrelClosure &&other)
    {
        swap(other);
        //reset();
    }

    SquirrelClosure& operator=(SquirrelClosure &&other)
    {
        swap(other);
        return *this;
    }

    //static
    //SquirrelClosure getFromSquirrelObject(const SquirrelClosure &closure, ssq::Object &obj)
    SquirrelClosure getFromSquirrelObject(const ssq::VM &vm, const ssq::Object &obj) const
    {
        SquirrelClosure boundClosure = SquirrelClosure(name);
        //boundClosure.optVm           = std::optional<ssq::VM>(std::in_place, vm);
        //boundClosure.optVm           = std::make_optional<ssq::VM>(vm);
        boundClosure.pVm             = const_cast<ssq::VM*>(&vm);
        //boundClosure.hvm             = vm.getHandle();
        boundClosure.optClosure      = marty_simplesquirrel::findFuncOptionalEx(obj, name, boundClosure.ssqThis);
        return boundClosure;
    }

    bool isReadyForCall() const
    {
        if (!optClosure.has_value())
        {
            return false;
        }

        if (!pVm)
        {
            return false;
        }

        // if (!hvm)
        // {
        //     return false;
        // }

        return true;
    }


    template <typename ... Args>
    ssq::Object operator() (Args && ... args)
    {
        if (!optClosure.has_value())
        {
            // Тоже неплохая идея использовать std::bad_optional_access
            throw std::bad_optional_access( /* "Failed to call '" + encoding::toUtf8(name) + "' squirrel closure" */ );
        }

        //if (!optVm.has_value())
        if (!pVm)
        {
            // Тоже неплохая идея использовать std::bad_optional_access
            throw std::bad_optional_access( /* "Failed to call '" + encoding::toUtf8(name) + "' squirrel closure" */ );
        }

        // if (!hvm)
        // {
        //     // Тоже неплохая идея использовать std::bad_optional_access
        //     throw std::bad_optional_access( /* "Failed to call '" + encoding::toUtf8(name) + "' squirrel closure" */ );
        // }

        // тут ещё проверки на всякую валидность
        //return ssq::VM(ssqThis.getHandle()).callFunc(optClosure.value(), ssqThis, std::forward<Args>(args) ...);
        //return optVm.value().callFunc(optClosure.value(), ssqThis, std::forward<Args>(args) ...);
        return pVm->callFunc(optClosure.value(), ssqThis, std::forward<Args>(args) ...);
        
    }

}; // struct SquirrelClosure



} // namespace marty_simplesquirrel


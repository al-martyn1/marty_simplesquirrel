#pragma once

#include "squirrel_closure.h"


#define MARTY_SIMPLESQUIRREL_IMPLEMENT_SQUIRREL_INTERFACE_CTORS_EX_CONST_CHAR_NAME(InterfaceClassName, Ns) \
                InterfaceClassName() : marty_simplesquirrel::SquirrelInterfaceBase(_SC(Ns)) {}             \
                                                                                                           \
                InterfaceClassName(const InterfaceClassName &) = delete;                                   \
                InterfaceClassName& operator=(const InterfaceClassName &) = delete;                        \
                                                                                                           \
                InterfaceClassName(InterfaceClassName &&) = default;                                       \
                InterfaceClassName& operator=(InterfaceClassName &&) = default


#define MARTY_SIMPLESQUIRREL_IMPLEMENT_SQUIRREL_INTERFACE_CTORS(InterfaceClassName)                    \
                InterfaceClassName() : marty_simplesquirrel::SquirrelInterfaceBase(ssq::sqstring()) {} \
                                                                                                       \
                InterfaceClassName(const InterfaceClassName &) = delete;                               \
                InterfaceClassName& operator=(const InterfaceClassName &) = delete;                    \
                                                                                                       \
                InterfaceClassName(InterfaceClassName &&) = default;                                   \
                InterfaceClassName& operator=(InterfaceClassName &&) = default



// marty_simplesquirrel::

namespace marty_simplesquirrel {



struct SquirrelInterfaceBase
{
    ssq::sqstring                  name; // table/class name

    // Тут пачка ctor/op=
    SquirrelInterfaceBase() = delete;
    SquirrelInterfaceBase(const ssq::sqstring &n) : name(n) {}

    SquirrelInterfaceBase(const SquirrelInterfaceBase &) = delete;
    SquirrelInterfaceBase& operator=(const SquirrelInterfaceBase &) = delete;

    SquirrelInterfaceBase(SquirrelInterfaceBase &&) = default;
    SquirrelInterfaceBase& operator=(SquirrelInterfaceBase &&) = default;


    template<typename InterfaceType> 
    static
    void getFromSquirrelObjectImpl(ssq::VM &vm, InterfaceType &&iface, ssq::Object &obj, const std::vector<SquirrelClosure*> &closures)
    //void getFromSquirrelObjectImpl(ssq::Object &obj, IterType b, IterType e)
    {
        ssq::Object objFound;
        if (!marty_simplesquirrel::findObjectByFullQualifiedName(obj, iface.name, objFound))
        {
            // Пока кинем так
            throw std::bad_optional_access( /* "Failed to extract '" + encoding::toUtf8(iface.name) + "' squirrel interface" */ );
        }

        for(auto &c : closures)
        {
            *c = c->getFromSquirrelObject(vm, objFound);
        }

        //return std::move(iface);
    }

}; // struct SquirrelInterfaceBase



} // namespace marty_simplesquirrel


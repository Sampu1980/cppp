//
//
// ZException.cpp
//
//
// $Id: //swdepot/main/src_ne/component/Util/ZException.cpp#4 $ 
// $Change: 30874 $ 
// $DateTime: 2003/11/19 17:15:33 $ 
//
// Copyright(c) Infinera 2002
//
//

#include <string.h>
#include "Util/ZException.h"

ZException::ZException(const char* const& module, 
                       const char* const& what, 
                       const char* const& msg, 
                       const char* const& pFilename,
                       int linenum)
: mpFilename(pFilename),
  mLinenum(linenum),
  mModule(module),
  mWhat(what),
  mMsg(msg)
{
    // XXX Get stack trace
    mBt.Capture();
}

ZException::ZException(const ZException& ex)
: mpFilename(ex.mpFilename),
  mLinenum(ex.mLinenum),
  mModule(ex.mModule),
  mWhat(ex.mWhat),
  mMsg(ex.mMsg)
{
    // XXX Copy stack trace
    mBt = ex.mBt;
}

ZException::~ZException()
{
    // XXX anything to free
}

bool
ZException::IsModule(const char* pModule) const
{
    return !strcmp(pModule, Module());
}

bool
ZException::IsWhat(const char* pWhat) const
{
    return !strcmp(pWhat, What());
}

const char*
ZException::Module() const
{
    return mModule.c_str();
}

const char*
ZException::What() const
{
    return mWhat.c_str();
}

const char*
ZException::Msg() const
{
    return mMsg.c_str();
}

const char*
ZException::Filename() const
{
    return mpFilename;
}

int
ZException::Linenum() const
{
    return mLinenum;
}

ZException&
ZException::operator=(const ZException& ex)
{
    mpFilename = ex.mpFilename;
    mLinenum   = ex.mLinenum;
    mModule    = ex.mModule;
    mWhat      = ex.mWhat;
    mMsg       = ex.mMsg;

    // XXX Copy stack trace

    mBt = ex.mBt;

    return *this;
}

std::ostream& operator<< ( std::ostream& os, ZException& ex )
{
    os << "ZEXCEPTION:" << ex.mModule << ":" << ex.mWhat << ":" << ex.mMsg << ":";
    
    if (ex.mpFilename)
    {
        os << ex.mpFilename;
    }
    
    //os << ":" << ex.mLinenum << std::endl
       //<< ex.GetBackTrace();
    return os;
}

const util::Backtrace& 
ZException::GetBackTrace() const 
{
    return mBt;
}



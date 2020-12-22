//
// Util/RefCountObject.cpp
//
// Copyright(c) Infinera 2002-2015
//

#include <Util/RefCountObject.h>
#include <Util/Dbc.h>
#include <Q2LinAtomic.h>

#include <atomic.h>

#include <iostream>

using namespace std;

RefCountObject::RefCountObject()
: mCount(1)
{
}

RefCountObject::~RefCountObject()
{
}

void
RefCountObject::Dump(std::ostream& os, int level, int format) const
{
    REQUIRE_STRM(mCount > 0, "","Count=" << mCount);

    ZObject::Dump(os, level);
    os << "RefCountObject : count = " << mCount << endl;
}

ostream&
operator << (ostream& os, const RefCountObject& rRefCountObject)
{
    REQUIRE_STRM(rRefCountObject.mCount > 0, "","Count=" << rRefCountObject.mCount);

    rRefCountObject.Dump(os);
    return os;
}

void
RefCountObject::AddRef()
{
    REQUIRE_STRM(mCount > 0, "","Count=" << mCount);

    atomic_add( (unsigned*)&mCount, 1 );

}

void
RefCountObject::Destroy()
{
    int old_count;

    REQUIRE_STRM(mCount > 0, "","Count=" << mCount);

    old_count = (int)atomic_sub_value( (unsigned*)&mCount, 1 );

    REQUIRE_STRM(old_count > 0, "","old_count=" << old_count);

    if (old_count <= 1)
    {
        delete this;
    }
}


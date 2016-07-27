#include "StdAfx.h"
#include "Routines.h"


void AtomicIncrement(long* var)
{
    InterlockedIncrement ( var);
}
void AtomicDecrement(long* var)
{
    InterlockedDecrement(var);
}
void AtomicExchange(long* var, long val)
{
    InterlockedExchange (var, val);
}

bool CompareAndSwap(long* var, long newVal, long oldVal)
{
    return  oldVal ==  InterlockedCompareExchange(var, newVal, oldVal);
}


#include "StdAfx.h"
#include "Routines.h"


void AtomicIncrement(long* var)
{
    __sync_add_and_fetch(var, 1);
}
void AtomicDecrement(long* var)
{
    __sync_add_and_fetch(var, -1);
}
void AtomicExchange(long* var, long val)
{
    __sync_lock_test_and_set (var, val);
}

bool CompareAndSwap(long* var, long newVal, long oldVal)
{
    return oldVal == __sync_val_compare_and_swap(var, newVal, oldVal);
}


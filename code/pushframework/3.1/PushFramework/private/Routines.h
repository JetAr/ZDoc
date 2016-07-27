#pragma once


#include "../include/Plateform.h"

void AtomicIncrement(long* var);
void AtomicDecrement(long* var);
void AtomicExchange(long* var, long val);

bool CompareAndSwap(long* var, long newVal, long oldVal);



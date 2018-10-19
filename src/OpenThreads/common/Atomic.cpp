/* -*-c++-*- OpenThreads library, Copyright (C) 2008  The Open Thread Group
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

//forward compatibility for ot21: export the functions from dll as well as allowing inline versions in new binaries
#include <OpenThreads/Config>
#if defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
# define _OPENTHREADS_ATOMIC_USE_LIBRARY_ROUTINES
#endif


#include <OpenThreads/Atomic>



namespace OpenThreads {

#if defined(_OPENTHREADS_ATOMIC_USE_LIBRARY_ROUTINES)

// Non inline implementations for two special cases:
// * win32
// * i386 gcc
//
// On win32 we do not want to pull windows.h in the Atomic header.
// windows.h just pulls too much stuff that disturbs sources.
//
// On i386 gcc, we have that nice builtins available but only with some compiler
// architectural compiler flags enabled. This way we can make sure that the
// compiler flags are like we had them in the configure test. If the functions
// are inline, we do not need what compiler flags the Atomic header will see.

unsigned
Atomic::operator++()
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_add_and_fetch(&_value, 1);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    return _InterlockedIncrement(&_value);
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicIncrement32(&_value);
#else
# error This implementation should happen inline in the include file
#endif
}

unsigned
Atomic::operator--()
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_sub_and_fetch(&_value, 1);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    return _InterlockedDecrement(&_value);
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicDecrement32(&_value);
#else
# error This implementation should happen inline in the include file
#endif
}

unsigned
Atomic::AND(unsigned value)
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_fetch_and_and(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    return _InterlockedAnd(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicAnd32((uint32_t)value, (uint32_t *)&_value);
#else
# error This implementation should happen inline in the include file
#endif
}

unsigned
Atomic::OR(unsigned value)
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_fetch_and_or(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    return _InterlockedOr(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicOr32((uint32_t)value, (uint32_t *)&_value);
#else
# error This implementation should happen inline in the include file
#endif
}

unsigned
Atomic::XOR(unsigned value)
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_fetch_and_xor(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    return _InterlockedXor(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicXor32((uint32_t)value, (uint32_t *)&_value);
#else
# error This implementation should happen inline in the include file
#endif
}


unsigned
Atomic::exchange(unsigned value)
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_lock_test_and_set(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    return _InterlockedExchange(&_value, value);
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicCompareAndSwap32(_value, value, &_value);
#else
# error This implementation should happen inline in the include file
#endif
}


Atomic::operator unsigned() const
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    __sync_synchronize();
    return _value;
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    _ReadBarrier();
    return _value;
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    OSMemoryBarrier();
    return static_cast<unsigned const volatile>(_value);
#else
# error This implementation should happen inline in the include file
#endif
}

bool
AtomicPtr::assign(void* ptrNew, const void* const ptrOld)
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    return __sync_bool_compare_and_swap(&_ptr, (void*)ptrOld, ptrNew);
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    //return ptrOld == InterlockedCompareExchangePointer((PVOID volatile*)&_ptr, (PVOID)ptrNew, (PVOID)ptrOld);
  #if defined(_WIN64)
    return ptrOld == (const void*)_InterlockedCompareExchange64((volatile intptr_t*)&_ptr, (intptr_t)ptrNew, (intptr_t)ptrOld);
  #else
    return ptrOld == (const void*)_InterlockedCompareExchange((volatile intptr_t*)&_ptr, (intptr_t)ptrNew, (intptr_t)ptrOld);
#endif
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    return OSAtomicCompareAndSwapPtr((void *)ptrOld, (void *)ptrNew, (void* volatile *)&_ptr);
#else
# error This implementation should happen inline in the include file
#endif
}

void*
AtomicPtr::get() const
{
#if defined(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS)
    __sync_synchronize();
    return _ptr;
#elif defined(_OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED)
    _ReadBarrier();
    return _ptr;
#elif defined(_OPENTHREADS_ATOMIC_USE_BSD_ATOMIC)
    OSMemoryBarrier();
    return _ptr;
#else
# error This implementation should happen inline in the include file
#endif
}

#endif // defined(_OPENTHREADS_ATOMIC_USE_LIBRARY_ROUTINES)

}

/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2012 Sandro Santilli <strk@kbt.io>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/util/Interrupt.h>
#include <geos/util/GEOSException.h> // for inheritance

namespace {

// Callback and request status for interruption of any single thread
bool requested = false;
thread_local bool requested_for_thread = false;

// Callback and request status for interruption of a the current thread
geos::util::Interrupt::Callback* callback = nullptr;
thread_local geos::util::Interrupt::ThreadCallback* callback_thread = nullptr;
thread_local void* callback_thread_data = nullptr;

}

namespace geos {
namespace util { // geos::util

class GEOS_DLL InterruptedException: public GEOSException {
public:
    InterruptedException() :
        GEOSException("InterruptedException", "Interrupted!") {}
};

void
Interrupt::request()
{
    requested = true;
}

void
Interrupt::requestForCurrentThread()
{
    requested_for_thread = true;
}

void
Interrupt::cancel()
{
    requested = false;
    requested_for_thread = false;
}

bool
Interrupt::check()
{
    return requested || requested_for_thread;
}

Interrupt::Callback*
Interrupt::registerCallback(Interrupt::Callback* cb)
{
    Callback* prev = callback;
    callback = cb;
    return prev;
}

Interrupt::ThreadCallback*
Interrupt::registerThreadCallback(ThreadCallback* cb, void* data)
{
    ThreadCallback* prev = callback_thread;
    callback_thread = cb;
    callback_thread_data = data;
    return prev;
}

void
Interrupt::process()
{
    if(callback) {
        (*callback)();
    }
    if(callback_thread) {
        (*callback_thread)(callback_thread_data);
    }
    if(check()) {
        interrupt();
    }
}


void
Interrupt::interrupt()
{
    requested = false;
    requested_for_thread = false;
    throw InterruptedException();
}


} // namespace geos::util
} // namespace geos


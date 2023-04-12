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

#pragma once

#include <geos/export.h>

namespace geos {
namespace util { // geos::util

#define GEOS_CHECK_FOR_INTERRUPTS() geos::util::Interrupt::process()

/** \brief Used to manage interruption requests and callbacks. */
class GEOS_DLL Interrupt {

public:

    typedef void (Callback)(void);
    typedef void (ThreadCallback)(void*);

    /**
     * Request interruption of operations
     *
     * Operations will be terminated by a GEOSInterrupt
     * exception at first occasion, by the first thread
     * to check for an interrupt request.
     */
    static void request();

    /**
     * Request interruption of operations in the current thread
     *
     * Operations in the current thread will be terminated by
     * a GEOSInterrupt at first occasion.
     */
    static void requestForCurrentThread();

    /** Cancel a pending interruption request */
    static void cancel();

    /** Check if an interruption request is pending */
    static bool check();

    /** \brief
     * Register a callback that will be invoked by all threads
     * before checking for interruption requests.
     *
     * NOTE that interruption request checking may happen
     * frequently so the callback should execute quickly.
     *
     * The callback can be used to call Interrupt::request()
     * or Interrupt::requestForCurrentThread().
     */
    static Callback* registerCallback(Callback* cb);

    /** \brief
     * Register a callback that will be invoked the current thread
     * before checking for interruption requests.
     *
     * NOTE that interruption request checking may happen
     * frequently so the callback should execute quickly.
     *
     * The callback can be used to call Interrupt::request()
     * or Interrupt::requestForCurrentThread().
     */
    static ThreadCallback* registerThreadCallback(ThreadCallback* cb, void* data);

    /**
     * Invoke the callback, if any. Process pending interruption, if any.
     *
     */
    static void process();

    /* Perform the actual interruption (simply throw an exception) */
    static void interrupt();

};


} // namespace geos::util
} // namespace geos


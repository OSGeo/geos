/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/noding/SegmentSetMutualIntersector.h> // inherited
#include <geos/index/chain/MonotoneChainOverlapAction.h> // inherited
#include <geos/index/chain/MonotoneChain.h> // inherited
#include <geos/index/strtree/TemplateSTRtree.h> // inherited

namespace geos {
namespace index {
class SpatialIndex;
}
namespace noding {
class SegmentString;
class SegmentIntersector;
}
}

namespace geos {
namespace noding { // geos::noding

/** \brief
 * Intersects two sets of [SegmentStrings](@ref SegmentString) using a index based
 * on [MonotoneChains](@ref index::chain::MonotoneChain) and a
 * [SpatialIndex](@ref index::SpatialIndex).
 *
 * @version 1.7
 */
class MCIndexSegmentSetMutualIntersector : public SegmentSetMutualIntersector {
public:

    MCIndexSegmentSetMutualIntersector(double p_tolerance)
        : overlapTolerance(p_tolerance)
    {}

    MCIndexSegmentSetMutualIntersector()
        : MCIndexSegmentSetMutualIntersector(0.0)
    {}

    ~MCIndexSegmentSetMutualIntersector() override
    {};

    index::SpatialIndex*
    getIndex()
    {
        return &index;
    }

    void setBaseSegments(SegmentString::ConstVect* segStrings) override;

    void process(SegmentString::ConstVect* segStrings) override;

    void process(SegmentString::ConstVect* segStrings, SegmentIntersector* si);

    class SegmentOverlapAction : public index::chain::MonotoneChainOverlapAction {
    private:
        SegmentIntersector& si;

        // Declare type as noncopyable
        SegmentOverlapAction(const SegmentOverlapAction& other) = delete;
        SegmentOverlapAction& operator=(const SegmentOverlapAction& rhs) = delete;

    public:
        SegmentOverlapAction(SegmentIntersector& p_si) :
            index::chain::MonotoneChainOverlapAction(), si(p_si)
        {}

        void overlap(const index::chain::MonotoneChain& mc1, std::size_t start1,
                     const index::chain::MonotoneChain& mc2, std::size_t start2) override;
    };

    /**
     * Disable copy construction and assignment. Apparently needed to make this
     * class compile under MSVC. (See https://stackoverflow.com/q/29565299)
     */
    MCIndexSegmentSetMutualIntersector(const MCIndexSegmentSetMutualIntersector&) = delete;
    MCIndexSegmentSetMutualIntersector& operator=(const MCIndexSegmentSetMutualIntersector&) = delete;

private:

    typedef std::vector<index::chain::MonotoneChain> MonoChains;

    /*
     * The index::SpatialIndex used should be something that supports
     * envelope (range) queries efficiently (such as a index::quadtree::Quadtree
     * or index::strtree::STRtree).
     */
    index::strtree::TemplateSTRtree<const index::chain::MonotoneChain*> index;

    const double overlapTolerance;

    /* memory management helper, holds MonotoneChain objects used
     * in the SpatialIndex. It's cleared when the SpatialIndex is
     */
    std::once_flag indexBuilt;
    MonoChains indexChains;

    void intersectChains(const MonoChains& chains, SegmentIntersector& segmentIntersector);

    void addChains(const SegmentString* segStr, MonoChains& chains) const;

};

} // namespace geos::noding
} // namespace geos


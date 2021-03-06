// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_IS_VALID_HAS_SPIKES_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_IS_VALID_HAS_SPIKES_HPP

#include <algorithm>

#include <boost/range.hpp>

#include <boost/geometry/core/point_type.hpp>

#include <boost/geometry/util/range.hpp>

#include <boost/geometry/views/closeable_view.hpp>

#include <boost/geometry/algorithms/equals.hpp>
#include <boost/geometry/algorithms/detail/point_is_spike_or_equal.hpp>


namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace is_valid
{

template <typename Point>
struct equal_to
{
    Point const& m_point;

    equal_to(Point const& point)
        : m_point(point)
    {}

    template <typename OtherPoint>
    inline bool operator()(OtherPoint const& other) const
    {
        return geometry::equals(m_point, other);
    }
};

template <typename Point>
struct not_equal_to
{
    Point const& m_point;

    not_equal_to(Point const& point)
        : m_point(point)
    {}

    template <typename OtherPoint>
    inline bool operator()(OtherPoint const& other) const
    {
        return !geometry::equals(other, m_point);
    }
};



template <typename Range, closure_selector Closure>
struct has_spikes
{
    static inline bool apply(Range const& range)
    {
        typedef not_equal_to<typename point_type<Range>::type> not_equal;

        typedef typename closeable_view<Range const, Closure>::type view_type;
        typedef typename boost::range_iterator<view_type const>::type iterator; 

        view_type const view(range);

        iterator prev = boost::begin(view);

        iterator cur = std::find_if(prev, boost::end(view), not_equal(*prev));
        if ( cur == boost::end(view) )
        {
            // the range has only one distinct point, so it
            // cannot have a spike
            return false;
        }

        iterator next = std::find_if(cur, boost::end(view), not_equal(*cur));
        if ( next == boost::end(view) )
        {
            // the range has only two distinct points, so it
            // cannot have a spike
            return false;
        }

        while ( next != boost::end(view) )
        {
            if ( geometry::detail::point_is_spike_or_equal(*prev,
                                                           *next,
                                                           *cur) )
            {
                return true;
            }
            prev = cur;
            cur = next;
            next = std::find_if(cur, boost::end(view), not_equal(*cur));
        }

        if ( geometry::equals(range::front(view), range::back(view)) )
        {
            iterator cur = boost::begin(view);
            typename boost::range_reverse_iterator
                <
                    view_type const
                >::type prev = std::find_if(boost::rbegin(view),
                                            boost::rend(view),
                                            not_equal(range::back(view)));
            iterator next =
                std::find_if(cur, boost::end(view), not_equal(*cur));
            return detail::point_is_spike_or_equal(*prev, *next, *cur);
        }

        return false;
    }
};



}} // namespace detail::is_valid
#endif // DOXYGEN_NO_DETAIL


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_IS_VALID_HAS_SPIKES_HPP

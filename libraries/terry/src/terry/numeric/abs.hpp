#ifndef _TERRY_NUMERIC_ABS_HPP_
#define _TERRY_NUMERIC_ABS_HPP_

#include <terry/globals.hpp>

#include <cmath>

namespace terry {
namespace numeric {

template <typename Channel, typename ChannelR>
struct channel_abs_t : public std::unary_function<Channel, ChannelR>
{
	GIL_FORCEINLINE
	ChannelR operator()( typename channel_traits<Channel>::const_reference ch ) const
	{
		typedef typename floating_channel_type_t<ChannelR>::type T;
		return std::abs( T( ch ) );
	}
};

template <typename PixelRef, typename PixelR = PixelRef> // models pixel concept
struct pixel_abs_t
{
	GIL_FORCEINLINE
	PixelR operator()(const PixelRef & p ) const
	{
		PixelR result;
		boost::gil::static_transform( p, result, channel_abs_t<typename channel_type<PixelRef>::type, typename channel_type<PixelR>::type > ( ) );
		return result;
	}
};


}
}

#endif

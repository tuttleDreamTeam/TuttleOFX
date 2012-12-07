#include "AwaPlugin.hpp"

#include <terry/filter/gaussianKernel.hpp>
#include <terry/filter/convolve.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/pixel.hpp>


namespace tuttle {
namespace plugin {
namespace awa {

template<typename Locator>
struct AwaFilteringFunctor {
public:
	explicit AwaFilteringFunctor(Locator loc)
	: //up(loc.cache_location(0,1)),
	  //down(loc.cache_location(0,-1)){};
	  //left(loc.cache_location(-1, 0)),
	  //right(loc.cache_location(1, 0)),
	  a(loc.cache_location(-1, -1)),
	  b(loc.cache_location(-1, 0)),
	  c(loc.cache_location(-1, 1)),
	  d(loc.cache_location(0, -1)),
	  e(loc.cache_location(0, 0)),
	  f(loc.cache_location(0, 1)),
	  g(loc.cache_location(1, -1)),
	  h(loc.cache_location(1, 0)),
	  i(loc.cache_location(1, 1)){};

	typename Locator::value_type operator()(Locator loc) {
		typedef typename Locator::value_type pixel_type;
		typedef typename terry::channel_type<Locator>::type channel_type;
		//pixel_type src_left  = loc[left];
		//pixel_type src_right = loc[right];
		//pixel_type src_up  = loc[up];
		//pixel_type src_down = loc[down];
		pixel_type src_a = loc[a];
		pixel_type src_b = loc[b];
		pixel_type src_c = loc[c];
		pixel_type src_d = loc[d];
		pixel_type src_e = loc[e];
		pixel_type src_f = loc[f];
		pixel_type src_g = loc[g];
		pixel_type src_h = loc[h];
		pixel_type src_i = loc[i];
		pixel_type dst;
		
		for( int ch = 0; ch < terry::num_channels<Locator>::value; ++ch )
			dst[ch] = ((src_a[ch] + src_b[ch] + src_c[ch] 
				      + src_d[ch] + src_e[ch] + src_f[ch] 
				      + src_g[ch] + src_h[ch] + src_i[ch] )/9);
		return dst;
	}

private:
	//typename Locator::cached_location_t left;
	//typename Locator::cached_location_t right;
	//typename Locator::cached_location_t up;
	//typename Locator::cached_location_t down;
	typename Locator::cached_location_t a;
	typename Locator::cached_location_t b;
	typename Locator::cached_location_t c;
	typename Locator::cached_location_t d;
	typename Locator::cached_location_t e;
	typename Locator::cached_location_t f;
	typename Locator::cached_location_t g;
	typename Locator::cached_location_t h;
	typename Locator::cached_location_t i;
};

template<class View>
AwaProcess<View>::AwaProcess( AwaPlugin& effect )
	: ImageGilFilterProcessor<View>( effect, eImageOrientationIndependant )
	, _plugin( effect )
{}

template <class View>
void AwaProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

	//	TUTTLE_COUT_X( 20, "_" );
	//	TUTTLE_COUT_VAR( _params._size );
	//	TUTTLE_COUT_VAR2( _params._gilKernelX.size(), _params._gilKernelY.size() );
	//	std::cout << "x [";
	//	std::for_each(_params._gilKernelX.begin(), _params._gilKernelX.end(), std::cout << boost::lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
	//	std::cout << "y [";
	//	std::for_each(_params._gilKernelY.begin(), _params._gilKernelY.end(), std::cout << boost::lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void AwaProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	using namespace terry::filter;
	
	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	
	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );

	//const Point proc_tl( procWindowRoW.x1 - this->_srcPixelRod.x1, procWindowRoW.y1 - this->_srcPixelRod.y1 );
	/*
	const std::size_t alignment = 2;
 
	rgb32f_planar_image_t srcWork( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_planar_view_t  srcWorkV = view( srcWork );
	copy_and_convert_pixels( src, srcWorkV );

	rgb32f_planar_image_t dstWork( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_planar_view_t  dstWorkV = view( dstWork );
	
	boost::gil::transform_pixels ( srcWorkV, dstWorkV, awaFiltering<rgb32f_pixel_t>() );	
	
	copy_and_convert_pixels( dstWorkV, dst );
	*/
	//boost::gil::transform_pixels ( src, dst, AwaFiltering< typename View::value_type>() );
	boost::gil::transform_pixel_positions( src, dst, AwaFilteringFunctor< typename View::locator>( src.xy_at(0, 0) ) );
	
	//TUTTLE_COUT(_params._alpha);
	
	/*
	if( _params._size.x == 0 )
	{
		correlate_cols_auto<Pixel>( this->_srcView, _params._gilKernelY, dst, proc_tl, _params._boundary_option );
	}
	else if( _params._size.y == 0 )
	{
		correlate_rows_auto<Pixel>( this->_srcView, _params._gilKernelX, dst, proc_tl, _params._boundary_option );
	}
	else
	{
		correlate_rows_cols_auto<Pixel, OfxAllocator>(
			this->_srcView, _params._gilKernelX, _params._gilKernelY, dst, proc_tl, _params._boundary_option );
	}
	*/
}

}
}
}


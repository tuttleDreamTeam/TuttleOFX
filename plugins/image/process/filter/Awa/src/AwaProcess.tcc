#include "AwaPlugin.hpp"

#include <terry/numeric/operations.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/assign_minmax.hpp>
#include <terry/numeric/minmax.hpp>
#include <terry/numeric/init.hpp>
#include <terry/numeric/pow.hpp>
#include <terry/numeric/sqrt.hpp>
#include <terry/numeric/abs.hpp>
#include <terry/filter/gaussianKernel.hpp>
#include <terry/filter/convolve.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/pixel.hpp>
#include <boost/math/constants/constants.hpp>


namespace tuttle {
namespace plugin {
namespace awa {

template<typename Locator>
struct AwaFilteringFunctor {
public:
	explicit AwaFilteringFunctor(Locator loc)
	: center      (loc.cache_location( 0,  0)),
	  left        (loc.cache_location(-1,  0)),
	  right       (loc.cache_location( 1,  0)),
	  bottom      (loc.cache_location( 0, -1)),
	  top         (loc.cache_location( 0,  1)),
	  topLeft     (loc.cache_location(-1, -1)),
	  bottomLeft  (loc.cache_location(-1,  1)),
	  bottomRight (loc.cache_location( 1, -1)),
	  topRight    (loc.cache_location( 1,  1))
	{
	};

	typename Locator::value_type operator()(Locator loc)
	{
		typedef typename Locator::value_type pixel_type;
		pixel_type dst;
		
		dst = ( loc[topLeft] + loc[top] + loc[topRight] + loc[left] + loc[center] + loc[right] + loc[bottomLeft] + loc[bottom] + loc[bottomRight] ) / 9.0;

		return dst;
	}

private:
	typename Locator::cached_location_t center;
	typename Locator::cached_location_t left;
	typename Locator::cached_location_t right;
	typename Locator::cached_location_t bottom;
	typename Locator::cached_location_t top;
	typename Locator::cached_location_t topLeft;
	typename Locator::cached_location_t bottomLeft;
	typename Locator::cached_location_t bottomRight;
	typename Locator::cached_location_t topRight;
	
};

template<class View>
AwaProcess<View>::AwaProcess( AwaPlugin& effect )
	: ImageGilFilterProcessor<View>( effect, eImageOrientationIndependant )
	, _plugin( effect )
{
	this->setNoMultiThreading();
}

template <class View>
void AwaProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
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
	
	TUTTLE_COUT("TEMPLATE VIEW");
}

template<>
void AwaProcess<boost::gil::rgba32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	using namespace terry::filter;
	using namespace terry::numeric;

	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	
	boost::gil::rgba32f_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );	
	
	boost::gil::rgba32f_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );
	
// 	boost::gil::rgba32f_view_t noise = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
// 	                          procWindowSize.x,
// 	                          procWindowSize.y );

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
	//boost::gil::transform_pixel_positions( src, dst, AwaFilteringFunctor< typename View::locator>( src.xy_at(0, 0) ) );

	typedef rgba32f_pixel_t P;

	double alpha = _params._alpha;
	P ones;
	pixel_ones_t< P >()( ones );

	P epsilon( _params._epsilonR, _params._epsilonG, _params._epsilonB, 1.0 );
	P epsilon2 = pixel_pow_t< P, 2 >()( epsilon );

	P K;
	P N;
	P p;
	P sum   = pixel_zeros< P >();
	P noise = pixel_zeros< P >();
	P d;
	P w;
	double laplace[3][3] =
		{
			{  1.0, -2.0,  1.0 },
			{ -2.0,  4.0, -2.0 },
			{  1.0, -2.0,  1.0 }
		};
	
	
	/// Noise estimation
	for( int y = 1; y < src.height()-1; y++ )
	{
		for( int x = 1; x < src.width()-1; x++ )
		{
			N = pixel_zeros< P >();

			for( int i = 0; i <= 2; i++ )
			{
				for( int j = 0; j <= 2; j++ )
				{
					//N += laplace[i][j] * src( x + i, y + j );
					pixel_plus_assign_t<P, P>( )( pixel_multiplies_scalar_t<P, double>() ( src( x + i, y + j ), laplace[i][j] ), sum );
				}
				pixel_plus_assign_t<P, P>( )( pixel_abs_t< P >()( N ), sum ); // sum += abs( N );
			}
		}

		double normalizeFactor = std::sqrt( boost::math::constants::pi<double>() / 2.0 ) / ( 6.0 * ( src.width() - 2.0 ) * ( src.height() - 2.0 ) );

		noise = pixel_multiplies_scalar_t<P, double>() ( sum, normalizeFactor );
	}
	//TUTTLE_COUT_VAR3( noise[0], noise[1], noise[2] );

	/// AWA process
	for( int y = 1; y < src.height()-1; y++ )
	{
		for( int x = 1; x < src.width()-1; x++ )
		{
			K = pixel_zeros< P >();
			p = pixel_zeros< P >();
			w = pixel_zeros< P >();

			d = src( x, y );

			for( int i = 0; i <= 2; i++ )
			{
				for( int j = 0; j <= 2; j++ )
				{
					// d[i][j] = src(x,y) - src( x+j, y+j )
					// here: d[i][j] -= src( x+j, y+j )
					pixel_minus_assign_t<P, P>( )( src( x + i, y + j ), d );

					// K += 1 / ( 1+ alpha * max( epsilon^2, d[i][j]^2 ) )
					P d2 = pixel_pow_t< P, 2 >()( d );

					pixel_assign_max_t< P, P >()( epsilon2, d2 );
					P pMax = pixel_multiplies_scalar_t< P, double >( )( d2 , alpha );
					pixel_plus_assign_t< P, P >( )( ones, pMax );

					// one = one / pMax
					pMax = pixel_divides_t< P, P, P >()( ones, pMax );
					pixel_minus_assign_t< P, P >( )( pMax , K );
				}
			}
			K = pixel_divides_t< P, P, P >()( ones, K );
		
			for( int i = 0; i <= 2; i++ )
			{
				for( int j = 0; j <= 2; j++ )
				{
					P noise2 = pixel_pow_t< P, 2 >()( noise );
					P d2 = pixel_pow_t< P, 2 >()( d );

					pixel_assign_max_t< P, P >()( noise2, d2 );

					P pMax = pixel_multiplies_scalar_t< P, double >( )( d2 , alpha );
					pixel_plus_assign_t< P, P >( )( ones, pMax );
					pMax = pixel_divides_t< P, P, P >()( ones, pMax );

					w = pixel_divides_t< P, P, P >()( K, pMax );

					pixel_plus_assign_t<P, P>( )( pixel_multiplies_t< P, P, P >()( w, src( x + i, y + j ) ), p );
				}
			}

			dst( x, y ) = p;
		}
	}
	
	// to replace matlab's "padarray" (temporary solution):
	
	for( int x = 0; x < src.width(); x++ )
	{
		dst( x, 0 ) = src( x, 0 );
		dst( x, src.height() - 1 ) = src( x, src.height() - 1 );
	}
	
	for( int y = 0; y < src.height(); y++ )
	{
		dst( 0, y ) = src( 0, y );
		dst( src.width() - 1, y ) = src( src.width() - 1, y );
	}
	
}




}
}
}


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

#include <math.h>



namespace tuttle {
namespace plugin {
namespace awa {


/*
 template <typename SrcView, typename DstImage>
  void create_with_margin(const SrcView& src, DstImage& result) {
    result.recreate(src.width()+1, src.height()+1);
    copy_pixels(src, subimage_view(view(result), 1,1,src.width(),src.height()));
}*/

template<typename Locator>
struct AwaFilteringFunctor {
public:
  
    explicit AwaFilteringFunctor(Locator loc)
      : center (loc.cache_location( 0, 0)),
	left (loc.cache_location(-1, 0)),
	right (loc.cache_location( 1, 0)),
	bottom (loc.cache_location( 0, -1)),
	top (loc.cache_location( 0, 1)),
	topLeft (loc.cache_location(-1, -1)),
	bottomLeft (loc.cache_location(-1, 1)),
	bottomRight (loc.cache_location( 1, -1)),
	topRight (loc.cache_location( 1, 1))
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

template<class View, class P>
void noiseEstimationProcess( View& src, P& noise)
{
  using namespace terry;
  using namespace terry::numeric;

  P laplacian ;
  P sum   = pixel_zeros< P >(); // laplacian
  noise = pixel_zeros< P >();	// noise estimation
  
  double laplace_mask[3][3] = {{1.0, -2.0, 1.0},
				{-2.0, 4.0, -2.0},
				{1.0, -2.0, 1.0}};
  
  for(int y = 1; y < src.height()-1; y++ )
  {
      for(int x = 1; x < src.width()-1; x++ )
      {
	  laplacian = pixel_zeros< P >();

	  for(int i = 0; i <= 2; i++ )
	  {
	      for(int j = 0; j <= 2; j++ )
	      {      
		//laplacian += laplace_mask[i][j] * src( x + i, y + j ); 
		pixel_plus_assign_t<P, P>( )( pixel_multiplies_scalar_t<P, double>() ( src( x + i -1, y + j -1 ), laplace_mask[i-1][j-1] ), laplacian );		
	      }
	  } 		
	  pixel_plus_assign_t<P, P>( )( pixel_abs_t< P >()( laplacian ), sum ); // sum += abs( laplacian );
      }
  }
  
  //noise = (sqrt(pi/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * sum ;	 
  double normalizeFactor = std::sqrt( boost::math::constants::pi<double>() / 2.0 ) / ( 6.0 * ( src.width() - 2.0 ) * ( src.height() - 2.0 ) ); 
  
  noise = pixel_multiplies_scalar_t<P, double>() ( sum, normalizeFactor );
  
  TUTTLE_COUT_VAR3( noise[0], noise[1], noise[2] );
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
	
	typedef rgba32f_pixel_t P;
	
	P ones ;
	pixel_ones_t< P >()( ones ) ; // { 1.0, 1.0, 1.0, 1.0}
	
	float alpha = _params._alpha ;
	P epsilon( _params._epsilonR, _params._epsilonG, _params._epsilonB, 1.0 );
	P epsilon2 = pixel_pow_t< P, 2 >()( epsilon ); // epsilon²
	
	P K ; // Normalization constant
	P p ; // Output/Denoised pixel
	P noise ; // Noise Estimation
	
	noiseEstimationProcess( src, noise) ;
	P noise2 = pixel_pow_t< P, 2 >()( noise ); // noise²
	
	TUTTLE_COUT_VAR3( noise[0], noise[1], noise[2] );
		
	// AWA process
	
	P diff ; // difference between current pixel and pixel of the support (neighbors)
	P diff2 ; // diff²
	P weight ;	// Weight of the neighbors pixels
	
	for(int y = 1; y < src.height()-1; y++ )
	{
	    for(int x = 1; x < src.width()-1; x++ )
	    {
		K = pixel_zeros< P >();		
		weight = pixel_zeros< P >();
		
		for(int i = 0; i <= 2; i++ )
		{
		    for(int j = 0; j <= 2; j++ )
		    {
 			// diff = src(x,y) - src(x+i-1,y+j-1)
			diff = pixel_minus_t< P, P, P >( )( src( x, y ), src( x + i - 1, y + j - 1 ) );
			
			
			// K +=  1 / (1+alpha * ( max( epsilon², diff² ) )) ;	
			diff2 = pixel_pow_t< P, 2 >()( diff );
			
			
			if (  _params._noiseEstimation )
			  pixel_assign_max_t< P, P >()( noise2, diff2 );
			else 
			  pixel_assign_max_t< P, P >()( epsilon2, diff2 );			
			
			P pMax = pixel_multiplies_scalar_t< P, double >( )( diff2 , alpha );
			pixel_plus_assign_t< P, P >( )( ones, pMax );	
			
			pMax = pixel_divides_t< P, P, P >()( ones, pMax );
			pixel_plus_assign_t< P, P >( )( pMax , K );
		    }    
		}
		
		K = pixel_divides_t< P, P, P >()( ones, K );	// K = 1/K
	
		p = pixel_zeros< P >();
		
		for(int i = 0; i <= 2; i++ )
		{
		    for(int j = 0; j <= 2; j++ )
		    {
// 			weight = K / ( 1 / (1+alpha * ( max( noise², diff² ) ))) ;
			diff = pixel_minus_t< P, P, P >( )( src( x, y ), src( x + i - 1, y + j - 1 ) );	
			diff2 = pixel_pow_t< P, 2 >()( diff );

 			if (  _params._noiseEstimation )
			  pixel_assign_max_t< P, P >()( noise2, diff2 );
			else
			  pixel_assign_max_t< P, P >()( epsilon2, diff2 );

			P pMax = pixel_multiplies_scalar_t< P, double >( )( diff2 , alpha );
			pixel_plus_assign_t< P, P >( )( ones, pMax );
			pMax = pixel_divides_t< P, P, P >()( ones, pMax );

			weight = pixel_divides_t< P, P, P >()( K, pMax );					

			//p += w * src(x+i-1,y+j-1) ;
			pixel_plus_assign_t<P, P>( )( pixel_multiplies_t< P, P, P >()( weight, src( x + i - 1, y + j - 1 ) ), p );	
		    }    
		}


 		// dst(x,y) = p
		pixel_assigns_t< P, P >()( p, dst( x, y ) );
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


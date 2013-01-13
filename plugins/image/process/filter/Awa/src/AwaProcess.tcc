#include "AwaPlugin.hpp"

#include <terry/filter/gaussianKernel.hpp>
#include <terry/filter/convolve.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/pixel.hpp>

#include <math.h>
//#define M_PI        3.14159265358979323846264338327950288   /* pi */


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

}

template <class View>
void AwaProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
	this->setNoMultiThreading();

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
	
	TUTTLE_COUT("TEMPLATE VIEW");
}

template<>
void AwaProcess<boost::gil::rgba32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	using namespace terry::filter;
	
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
	
	float alpha = _params._alpha ;
	float epsilonR = _params._epsilonR ;
	float epsilonG = _params._epsilonG ;
	float epsilonB = _params._epsilonB ;
	
	float K[3] ;
	float N[3] ;
	float p[3] ;
	float somme[3] = { 0.0, 0.0, 0.0} ;
	float noise[3] = { 0.0, 0.0, 0.0} ;
	float d[3][3][3];
	float w[3][3][3];
	float g[3][3][3];
	float laplace[3][3] = {{1.0, -2.0, 1.0},{-2.0, 4.0, -2.0},{1.0, -2.0, 1.0}};
	
	
	// Noise estimation
	
	for(int y = 1; y < src.height()-1; y++ )
	{
	    for(int x = 1; x < src.width()-1; x++ )
	    {
		N[0] = 0.0 ;
		N[1] = 0.0 ;
		N[2] = 0.0 ;

		
		for(int i = -1; i <= 1; i++ )
		{
		    for(int j = -1; j <= 1; j++ )
		    {
			N[0] = N[0] + (laplace[i+1][j+1] *  get_color( src(x+i+1,y+j+1), red_t() )) ;
			N[1] = N[1] + (laplace[i+1][j+1] *  get_color( src(x+i+1,y+j+1), green_t() )) ;
			N[2] = N[2] + (laplace[i+1][j+1] *  get_color( src(x+i+1,y+j+1), blue_t() )) ;
			
		    }
		    
		    somme[0] = somme[0] + std::abs(N[0]) ;
		    somme[1] = somme[1] + std::abs(N[1]) ;
		    somme[2] = somme[2] + std::abs(N[2]) ;
 		}
	    }
	    
	    noise[0] = (std::sqrt(M_PI/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * (somme[0]) ; 
	    noise[1] = (std::sqrt(M_PI/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * (somme[1]) ;
	    noise[2] = (std::sqrt(M_PI/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * (somme[2]) ;	    
	}
// 	TUTTLE_COUT(noise[0]);
// 	TUTTLE_COUT(noise[1]);
// 	TUTTLE_COUT(noise[2]);
		
	// AWA process
	
	for(int y = 1; y < src.height()-1; y++ )
	{
	    for(int x = 1; x < src.width()-1; x++ )
	    {
		K[0] = 0. ;
		K[1] = 0. ;
		K[2] = 0. ;
		

		for(int i = 0; i < 3; i++ )
		{
		    for(int j = 0; j < 3; j++ )
		    {
			for(int k = 0; k < 3; k++ )
			{
			    d[i][j][k] = 0.0;
			    w[i][j][k] = 0.0;
			    g[i][j][k] = 0.0;
			}
		    } 
		}
		
		for(int i = -1; i <= 1; i++ )
		{
		    for(int j = -1; j <= 1; j++ )
		    {
			d[i+1][j+1][0] = get_color( src(x,y), red_t() ) - get_color( src(x+i+1,y+j+1), red_t() ) ;
			d[i+1][j+1][1] = get_color( src(x,y), green_t() ) - get_color( src(x+i+1,y+j+1), green_t() );
			d[i+1][j+1][2] = get_color( src(x,y), blue_t() ) - get_color( src(x+i+1,y+j+1), blue_t() );
			
			K[0] = K[0]+ ( 1 / (1+alpha * ( std::max( epsilonR*epsilonR, d[i+1][j+1][0]*d[i+1][j+1][0] ) ))) ;
			K[1] = K[1]+ ( 1 / (1+alpha * ( std::max( epsilonG*epsilonG, d[i+1][j+1][1]*d[i+1][j+1][1] ) ))) ;
			K[2] = K[2]+ ( 1 / (1+alpha * ( std::max( epsilonB*epsilonB, d[i+1][j+1][2]*d[i+1][j+1][2] ) ))) ;
			
// 			K[0] = K[0]+ ( 1 / (1+alpha * ( std::max( noise[0]*noise[0], d[i+1][j+1][0]*d[i+1][j+1][0] ) ))) ;
// 			K[1] = K[1]+ ( 1 / (1+alpha * ( std::max( noise[1]*noise[1], d[i+1][j+1][1]*d[i+1][j+1][1] ) ))) ;
// 			K[2] = K[2]+ ( 1 / (1+alpha * ( std::max( noise[2]*noise[2], d[i+1][j+1][2]*d[i+1][j+1][2] ) ))) ;
		    }    
		}
		K[0] = 1/K[0];
		K[1] = 1/K[1];
		K[2] = 1/K[2];
	
		p[0] = 0. ;
		p[1] = 0. ;
		p[2] = 0. ;
		
		for(int i = -1; i <= 1; i++ )
		{
		    for(int j = -1; j <= 1; j++ )
		    {
		      
			w[i+1][j+1][0] = K[0] / ( 1 / (1+alpha * ( std::max( noise[0]*noise[0], d[i+1][j+1][0]*d[i+1][j+1][0] ) ))) ;
			w[i+1][j+1][1] = K[1] / ( 1 / (1+alpha * ( std::max( noise[1]*noise[1], d[i+1][j+1][1]*d[i+1][j+1][1] ) ))) ;
			w[i+1][j+1][2] = K[2] / ( 1 / (1+alpha * ( std::max( noise[2]*noise[2], d[i+1][j+1][2]*d[i+1][j+1][2] ) ))) ;
				
			g[i+1][j+1][0] = get_color( src(x+i+1,y+j+1), red_t() ) ;
			g[i+1][j+1][1] = get_color( src(x+i+1,y+j+1), green_t() ) ;
			g[i+1][j+1][2] = get_color( src(x+i+1,y+j+1), blue_t() ) ;
			
			p[0] += w[i+1][j+1][0] * g[i+1][j+1][0] ;
			p[1] += w[i+1][j+1][1] * g[i+1][j+1][1] ;
			p[2] += w[i+1][j+1][2] * g[i+1][j+1][2] ;
			
		    }    
		}

		get_color( dst(x,y), red_t() )   = p[0];
		get_color( dst(x,y), green_t() ) = p[1];
		get_color( dst(x,y), blue_t() )  = p[2];
	   
	    } 
	}
	
	// to replace matlab's "padarray" (temporary solution):
	
	for( int x = 0; x < src.width(); x++ )
	{
	    get_color( src(x,0), red_t() )   = get_color( src(x,0), red_t() ) ;
	    get_color( src(x,0), green_t() ) = get_color( src(x,0),  green_t() ) ;
	    get_color( src(x,0), blue_t() )  = get_color( src(x,0), blue_t() ) ;
	    get_color( src(x,src.height()-1), red_t() )   = get_color( src(x,src.height()-1), red_t() ) ;
	    get_color( src(x,src.height()-1), green_t() ) = get_color( src(x,src.height()-1),  green_t() ) ;
	    get_color( src(x,src.height()-1), blue_t() )  = get_color( src(x,src.height()-1), blue_t() ) ;
	}
	
	for( int y = 0; y < src.height(); y++ )
	{
	    get_color( src(0,y), red_t() )   = get_color( src(0,y), red_t() ) ;
	    get_color( src(0,y), green_t() ) = get_color( src(0,y), green_t() ) ;
	    get_color( src(0,y), blue_t() )  = get_color( src(0,y), blue_t() ) ;
	    get_color( src(src.width(),y), red_t() )   = get_color( src(src.width(),y), red_t() ) ;
	    get_color( src(src.width(),y), green_t() ) = get_color( src(src.width(),y),  green_t() ) ;
	    get_color( src(src.width(),y), blue_t() )  = get_color( src(src.width(),y), blue_t() ) ;
	}
	
}




}
}
}


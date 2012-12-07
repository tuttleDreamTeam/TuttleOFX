#include "AwaPlugin.hpp"

#include <terry/filter/gaussianKernel.hpp>
#include <terry/filter/convolve.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/pixel.hpp>


namespace tuttle {
namespace plugin {
namespace awa {

float max(float a, float b){
  if ( a > b ) return a ;
  else return b ;
}  
  
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
	
	float alpha = 0.5 ;
	float epsilon = 0.01 ;
	float K[3] ;
	float p[3] ;
	float d[3][3][3];
	float w[3][3][3];
	float g[3][3][3];
	
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
			d[i+2][j+2][0] = get_color( src(y,x), red_t() ) - get_color( src(y+i,x+j), red_t() ) ;
			d[i+2][j+2][1] = get_color( src(y,x), green_t() ) - get_color( src(y+i,x+j), green_t() );
			d[i+2][j+2][2] = get_color( src(y,x), blue_t() ) - get_color( src(y+i,x+j), blue_t() );
			
			K[0] = K[0]+ ( 1 / (1+alpha * ( max( epsilon*epsilon, d[i+2][j+2][0]*d[i+2][j+2][0] ) ))) ;
			K[1] = K[1]+ ( 1 / (1+alpha * ( max( epsilon*epsilon, d[i+2][j+2][1]*d[i+2][j+2][1] ) ))) ;
			K[2] = K[2]+ ( 1 / (1+alpha * ( max( epsilon*epsilon, d[i+2][j+2][2]*d[i+2][j+2][2] ) ))) ;
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
			w[i+2][j+2][0] = K[0] / ( 1 / (1+alpha * ( max( epsilon*epsilon, d[i+2][j+2][0]*d[i+2][j+2][0] ) ))) ;
			w[i+2][j+2][1] = K[1] / ( 1 / (1+alpha * ( max( epsilon*epsilon, d[i+2][j+2][1]*d[i+2][j+2][1] ) ))) ;
			w[i+2][j+2][2] = K[2] / ( 1 / (1+alpha * ( max( epsilon*epsilon, d[i+2][j+2][2]*d[i+2][j+2][2] ) ))) ;
			
			g[i+2][j+2][0] = get_color( src(y+i,x+j), red_t() ) ;
			g[i+2][j+2][1] = get_color( src(y+i,x+j), green_t() ) ;
			g[i+2][j+2][2] = get_color( src(y+i,x+j), blue_t() ) ;
			
			p[0] = p[0] + w[i+2][j+2][0] * g[i+2][j+2][0] ;
			p[1] = p[1] + w[i+2][j+2][1] * g[i+2][j+2][1] ;
			p[2] = p[2] + w[i+2][j+2][2] * g[i+2][j+2][2] ;
		    }    
		}

		
		
		get_color( dst(x,y), red_t() )   = get_color( src(x,y), red_t() ) ; 
		get_color( dst(x,y), green_t() ) = get_color( src(x,y), green_t() ) ; 
		get_color( dst(x,y), blue_t() )  = get_color( src(x,y), blue_t() ) ; 
	   
	    } 
	}
	
	
}




}
}
}


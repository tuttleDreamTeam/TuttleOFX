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
//	boost::gil::transform_pixels ( src, dst, AwaFiltering< typename View::value_type>() );
	//boost::gil::rgba32f_view_t::xy_locator src_loc = src.xy_at(0,0);
	//boost::gil::transform_pixel_positions(src, dst, AwaFilteringFunctor<typename View::locator>(src.xy_at(10,10)));
	
	typedef rgba32f_pixel_t P;
	
	P ones ;
	pixel_ones_t< P >()( ones ) ; // { 1.0, 1.0, 1.0, 1.0}
	
	float alpha = _params._alpha ;
// 	float epsilonR = _params._epsilonR ;
// 	float epsilonG = _params._epsilonG ;
// 	float epsilonB = _params._epsilonB ;
	P epsilon( _params._epsilonR, _params._epsilonG, _params._epsilonB, 1.0 );
	P epsilon2 = pixel_pow_t< P, 2 >()( epsilon ); // epsilon²
	
	P K ;
	P N ;
	P p ;
	P sum   = pixel_zeros< P >();
	
 	P noise = pixel_zeros< P >();
	
// 	float d[3][3][3];
// 	float w[3][3][3];
// 	float g[3][3][3];
// moved in awa process part
	
	double laplace[3][3] = {{1.0, -2.0, 1.0},
				{-2.0, 4.0, -2.0},
				{1.0, -2.0, 1.0}};
	
	
	// Noise estimation
	
	for(int y = 1; y < src.height()-1; y++ )
	{
	    for(int x = 1; x < src.width()-1; x++ )
	    {
		N = pixel_zeros< P >();

		for(int i = 0; i <= 2; i++ )
		{
		    for(int j = 0; j <= 2; j++ )
		    {
// 			N[0] = N[0] + (laplace[i+1][j+1] *  get_color( src(x+i+1,y+j+1), red_t() )) ;
// 			N[1] = N[1] + (laplace[i+1][j+1] *  get_color( src(x+i+1,y+j+1), green_t() )) ;
// 			N[2] = N[2] + (laplace[i+1][j+1] *  get_color( src(x+i+1,y+j+1), blue_t() )) ;
		      
		      //N += laplace[i][j] * src( x + i, y + j ); 
		      // GIL_FORCEINLINE PixelDst & 	operator() (const PixelSrc &p1, PixelDst &p2) constants
		      // p2 += p1
		      pixel_plus_assign_t<P, P>( )( pixel_multiplies_scalar_t<P, double>() ( src( x + i, y + j ), laplace[i][j] ), N );
			
		    }
		    
// 		    somme[0] = somme[0] + std::abs(N[0]) ;
// 		    somme[1] = somme[1] + std::abs(N[1]) ;
// 		    somme[2] = somme[2] + std::abs(N[2]) ;
		    pixel_plus_assign_t<P, P>( )( pixel_abs_t< P >()( N ), sum ); // sum += abs( N );
 		}
	    }
	    
/*	    noise[0] = (std::sqrt(M_PI/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * (somme[0]) ; 
	    noise[1] = (std::sqrt(M_PI/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * (somme[1]) ;
	    noise[2] = (std::sqrt(M_PI/2.0))*(1.0/(6.0*(src.width() - 2.0)*(src.height() - 2.0))) * (somme[2]) ;	*/  
	    double normalizeFactor = std::sqrt( boost::math::constants::pi<double>() / 2.0 ) / ( 6.0 * ( src.width() - 2.0 ) * ( src.height() - 2.0 ) );
	    
	    noise = pixel_multiplies_scalar_t<P, double>() ( sum, normalizeFactor );
	}
	P noise2 = pixel_pow_t< P, 2 >()( noise );
	noise = pixel_zeros< P >();
	
	TUTTLE_COUT_VAR3( noise[0], noise[1], noise[2] );
// 	TUTTLE_COUT(noise[0]);
// 	TUTTLE_COUT(noise[1]);
// 	TUTTLE_COUT(noise[2]);
		
	// AWA process
	
	P diff ; // variable d
	P diff2 ; // variable d²
	P w ;
	P g ;
	
	for(int y = 1; y < src.height()-1; y++ )
	{
	    for(int x = 1; x < src.width()-1; x++ )
	    {
// 		K[0] = 0. ;
// 		K[1] = 0. ;
// 		K[2] = 0. ;
		K = pixel_zeros< P >();		

// 		for(int i = 0; i < 3; i++ )
// 		{
// 		    for(int j = 0; j < 3; j++ )
// 		    {
// 			for(int k = 0; k < 3; k++ )
// 			{
// 			    d[i][j][k] = 0.0;
// 			    w[i][j][k] = 0.0;
// 			    g[i][j][k] = 0.0;
// 			}
// 		    } 
// 		}
		w = pixel_zeros< P >();
		g = pixel_zeros< P >();
		
		for(int i = 0; i <= 2; i++ )
		{
		    for(int j = 0; j <= 2; j++ )
		    {
// 			d[i][j][0] = get_color( src(x,y), red_t() ) - get_color( src(x+i,y+j), red_t() ) ;
// 			d[i][j][1] = get_color( src(x,y), green_t() ) - get_color( src(x+i,y+j), green_t() );
// 			d[i][j][2] = get_color( src(x,y), blue_t() ) - get_color( src(x+i,y+j), blue_t() );
			diff = pixel_minus_t< P, P, P >( )( src( x, y ), src( x + i - 1, y + j - 1 ) );
			
			
// 			K[0] = K[0]+ ( 1 / (1+alpha * ( std::max( epsilonR*epsilonR, d[i][j][0]*d[i][j][0] ) ))) ;
// 			K[1] = K[1]+ ( 1 / (1+alpha * ( std::max( epsilonG*epsilonG, d[i][j][1]*d[i][j][1] ) ))) ;
// 			K[2] = K[2]+ ( 1 / (1+alpha * ( std::max( epsilonB*epsilonB, d[i][j][2]*d[i][j][2] ) ))) ;	// epsilon -> noise		
			
			diff2 = pixel_pow_t< P, 2 >()( diff );
			
			// struct terry::numeric::pixel_assign_max_t< PixelSrc, PixelDst >
			// p2 = max( p1, p2 )
			//pixel_assign_max_t< P, P >()( epsilon2, diff2 );
			pixel_assign_max_t< P, P >()( noise2, diff2 );
			
			P pMax = pixel_multiplies_scalar_t< P, double >( )( diff2 , alpha );
			pixel_plus_assign_t< P, P >( )( ones, pMax );	
			
			pMax = pixel_divides_t< P, P, P >()( ones, pMax );
			pixel_plus_assign_t< P, P >( )( pMax , K );
		    }    
		}
		
// 		K[0] = 1/K[0];
// 		K[1] = 1/K[1];
// 		K[2] = 1/K[2];
		K = pixel_divides_t< P, P, P >()( ones, K );
	
// 		p[0] = 0. ;
// 		p[1] = 0. ;
// 		p[2] = 0. ;
		p = pixel_zeros< P >();
		
		for(int i = 0; i <= 2; i++ )
		{
		    for(int j = 0; j <= 2; j++ )
		    {
		      
// 			w[i+1][j+1][0] = K[0] / ( 1 / (1+alpha * ( std::max( noise[0]*noise[0], d[i+1][j+1][0]*d[i+1][j+1][0] ) ))) ;
// 			w[i+1][j+1][1] = K[1] / ( 1 / (1+alpha * ( std::max( noise[1]*noise[1], d[i+1][j+1][1]*d[i+1][j+1][1] ) ))) ;
// 			w[i+1][j+1][2] = K[2] / ( 1 / (1+alpha * ( std::max( noise[2]*noise[2], d[i+1][j+1][2]*d[i+1][j+1][2] ) ))) ; // epsilon ~ noise
			P diff = pixel_minus_t< P, P, P >( )( src( x, y ), src( x + i - 1, y + j - 1 ) );	// stockage de d ??
			P d2 = pixel_pow_t< P, 2 >()( diff );

// 			pixel_assign_max_t< P, P >()( epsilon2, d2 );
			pixel_assign_max_t< P, P >()( noise2, d2 );

			P pMax = pixel_multiplies_scalar_t< P, double >( )( d2 , alpha );
			pixel_plus_assign_t< P, P >( )( ones, pMax );
			pMax = pixel_divides_t< P, P, P >()( ones, pMax );

			w = pixel_divides_t< P, P, P >()( K, pMax );					
			
// 			g[i+1][j+1][0] = get_color( src(x+i+1,y+j+1), red_t() ) ;
// 			g[i+1][j+1][1] = get_color( src(x+i+1,y+j+1), green_t() ) ;
// 			g[i+1][j+1][2] = get_color( src(x+i+1,y+j+1), blue_t() ) ;
			
// 			p[0] += w[i+1][j+1][0] * g[i+1][j+1][0] ;
// 			p[1] += w[i+1][j+1][1] * g[i+1][j+1][1] ;
// 			p[2] += w[i+1][j+1][2] * g[i+1][j+1][2] ;
			pixel_plus_assign_t<P, P>( )( pixel_multiplies_t< P, P, P >()( w, src( x + i - 1, y + j - 1 ) ), p );	
		    }    
		}

// 		get_color( dst(x,y), red_t() )   = p[0];
// 		get_color( dst(x,y), green_t() ) = p[1];
// 		get_color( dst(x,y), blue_t() )  = p[2];
		pixel_assigns_t< P, P >()( p, dst( x, y ) );
	    } 
	}
	
	// to replace matlab's "padarray" (temporary solution):
	
	//...
	
}




}
}
}


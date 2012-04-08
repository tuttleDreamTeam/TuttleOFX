#include "ToneMappingDefinitions.hpp"
#include "ToneMappingProcess.hpp"
#include "ToneMappingPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>
#include <terry/typedefs.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/color/invert.hpp>
#include <terry/channel_view.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/rgba.hpp>

#include <terry/globals.hpp>
#include <terry/copy.hpp>
#include <terry/colorspace/gradation.hpp>

#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

#include <tmo_drago03.h>

namespace tuttle {
namespace plugin {
namespace toneMapping {

/*
//--- 7 digits approximation of precise values
static const float rgb2xyzD65Mat[3][3] =
{ { 0.412424f, 0.357579f, 0.180464f },
  { 0.212656f, 0.715158f, 0.072186f },
  { 0.019332f, 0.119193f, 0.950444f } };

static const float xyz2rgbD65Mat[3][3] =
{ {  3.240708, -1.537259, -0.498570 },
  { -0.969257,  1.875995,  0.041555 },
  {  0.055636, -0.203996,  1.057069 } };
*/

template < typename SrcP, typename DstP > 
struct convertRgbToXYZ
{
    DstP operator() ( const SrcP& src ) {
        DstP dst;
        using namespace boost::gil;
        bits32f temp_red   = channel_convert<bits32f>( get_color( src, red_t()   ));
        bits32f temp_green = channel_convert<bits32f>( get_color( src, green_t() ));
        bits32f temp_blue  = channel_convert<bits32f>( get_color( src, blue_t()  ));

        bits32f x, y, z;
        x = temp_red * 0.4124240f + temp_green * 0.3575790f + temp_blue * 0.1804640f;
        y = temp_red * 0.2126560f + temp_green * 0.7151580f + temp_blue * 0.0721856f;
        z = temp_red * 0.0193324f + temp_green * 0.1191930f + temp_blue * 0.9504440f;

        get_color( dst, red_t() )	= x;
        get_color( dst, green_t() )	= y;
        get_color( dst, blue_t() )	= z;
        return dst;
    }
};

template < typename SrcP, typename DstP > 
struct convertXYZToRgb
{
    DstP operator() ( const SrcP& src ) {
        DstP dst;
        using namespace boost::gil;
        bits32f temp_x  = channel_convert<bits32f>( get_color( src, red_t()   ));
        bits32f temp_y  = channel_convert<bits32f>( get_color( src, green_t() ));
        bits32f temp_z  = channel_convert<bits32f>( get_color( src, blue_t()  ));

        bits32f r, g, b;
        r = temp_x * 3.240708 + temp_y * -1.537259 + temp_z * -0.498570;
        g = temp_x * -0.969257 + temp_y * 1.875995 + temp_z * 0.041555;
        b = temp_x * 0.055636 + temp_y * -0.203996 + temp_z * 1.057069;

        get_color( dst, red_t() )	= r;
        get_color( dst, green_t() )	= g;
        get_color( dst, blue_t() )	= b;
        return dst;
    }
};

using namespace boost::gil;

template<class View>
ToneMappingProcess<View>::ToneMappingProcess( ToneMappingPlugin& effect )
	: ImageGilFilterProcessor<View>( effect, eImageOrientationIndependant )
	, _plugin( effect )
{
	this-> setNoMultiThreading();
}

template<class View>
void ToneMappingProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.renderScale );

}

template<class View>
void ToneMappingProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );

//	copy_pixels( src, dst );// test: Output=Input

        const std::size_t alignment = 2;

	rgb32f_planar_image_t srcWork( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_planar_view_t  srcWorkV = view( srcWork );

        copy_pixels( src, srcWorkV );

	rgb32f_image_t xyzImg( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_view_t  xyzView = view( xyzImg );

	boost::gil::transform_pixels ( srcWorkV, xyzView, convertRgbToXYZ<rgb32f_pixel_t, rgb32f_pixel_t>() );
	
	boost::gil::transform_pixels ( xyzView, srcWorkV, convertXYZToRgb<rgb32f_pixel_t, rgb32f_pixel_t>() );
     
	//copy_pixels( srcWorkV, dst );
/*   	
        float* redPtr   = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 0 ) );
        float* greenPtr = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 1 ) );
        float* bluePtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 2 ) );
	
	float* XPtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( xyzView, 0 ) );
        float* YPtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( xyzView, 1 ) ); // @param Y [in] image luminance values
        float* ZPtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( xyzView, 2 ) );

	float* LPtr; // @param L [out] tone mapped values

	float avLum, maxLum;
	calculateLuminance( src.width(), src.heigth(), YPtr, avLum, maxLum );

	tmo_drago03(src.width(), src.heigth(), YPtr, LPtr, maxLum, avLum, 0.85 );

	for( int x=0 ; x<src.width() ; x++ )
	      for( int y=0 ; y<src.heigth() ; y++ )
	      {
		float scale = (*LPtr)(x,y) / (*YPtr)(x,y);
		(*XPtr)(x,y) *= scale;
		(*YPtr)(x,y) *= scale;
		(*ZPtr)(x,y) *= scale;

	      }

*/

}




void test( const OfxRectI& procWindowRoW )
{
}   


        



}
}
}

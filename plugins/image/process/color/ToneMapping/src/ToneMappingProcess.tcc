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

namespace tuttle {
namespace plugin {
namespace toneMapping {

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
/*
        const std::size_t alignment = 2;
        rgb32f_planar_image_t srcWork( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_planar_view_t  srcWorkV = view( srcWork );


        copy_pixels( src, srcWorkV );


        float* redPtr   = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 0 ) );
        float* greenPtr = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 1 ) );
        float* bluePtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 2 ) );


	rgb32f_image_t xyzImg( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_view_t  xyzView = view( xyzImg );
        boost::gil::transform_pixels ( src, xyzView, convertRgbToXYZ<rgb32f_pixel_t, rgb32f_pixel_t>() );

*/


}




void test( const OfxRectI& procWindowRoW )
{
}   


        



}
}
}

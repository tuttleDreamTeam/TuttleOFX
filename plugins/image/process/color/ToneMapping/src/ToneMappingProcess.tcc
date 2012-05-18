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
#include <tmo_pattanaik00.h>
#include <tmo_reinhard05.h>
#include <tmo_reinhard02.h>
#include <tmo_durand02.h>
#include <tmo_fattal02.h>

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

	copy_pixels(src, dst);
	TUTTLE_COUT("pass here: template View...");
	switch(  _params._toneoperator )
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		default:break;
	}
}

template<>
void ToneMappingProcess<rgba32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	rgba32f_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	rgba32f_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );

	TUTTLE_COUT("pass here: rgba32f...");
	copy_pixels( src, dst );

	unsigned int w = src.width();
	unsigned int h = src.height();
	
//Creation of the work view and xyz view
        const std::size_t alignment = 2;
 
	rgb32f_planar_image_t srcWork( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_planar_view_t  srcWorkV = view( srcWork );
	copy_and_convert_pixels( src, srcWorkV );

	rgb32f_planar_image_t xyzImg( procWindowSize.x, procWindowSize.y, alignment );
        rgb32f_planar_view_t  xyzView = view( xyzImg );

	boost::gil::transform_pixels ( srcWorkV, xyzView, convertRgbToXYZ<rgb32f_pixel_t, rgb32f_pixel_t>() );	
		
//Creation of the RGB pointers	
	float* R  = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 0 ) );
     	float* G  = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 1 ) );
        float* B  = reinterpret_cast<float*>( planar_view_get_raw_data ( srcWorkV, 2 ) );
//Creation of the RGB pointers and new luminance L pointer
	float* XPtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( xyzView, 0 ) );
	float* YPtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( xyzView, 1 ) ); // Y [in] image luminance values
	float* ZPtr  = reinterpret_cast<float*>( planar_view_get_raw_data ( xyzView, 2 ) );	
	float* LPtr = new float[src.size()] ; // L [out] tone mapped values

	VisualAdaptationModel* am = new VisualAdaptationModel();

	
		switch(  _params._toneoperator )
	{
		case 0:
		  //drago03
		  float avLum, maxLum;	// Average luminance and maximum luminance of the source
		  calculateLuminance( w, h, YPtr, avLum, maxLum );

		  tmo_drago03(w, h, YPtr, LPtr, maxLum, avLum, _params._Bias );

		  for (unsigned int i=0 ; i<w*h ; i++ )
		  { 
			  float scale = LPtr[i] / YPtr[i] ;
			  XPtr[i]	*= scale  ;
			  YPtr[i]	*= scale  ;
			  ZPtr[i]	*= scale  ;
		  }	// Rescaling of the 3 components with the new luminance values

		  boost::gil::transform_pixels ( xyzView, srcWorkV, convertXYZToRgb<rgb32f_pixel_t, rgb32f_pixel_t>() ); // Back 2 RGB
		  copy_and_convert_pixels( srcWorkV, dst );	
				  
		  break;
		case 1:
		  //pattanaik00

		  if(_params._Mult!=1.0f )
		  {//multiplyChannels
		    float mult = _params._Mult/10 ;
		  TUTTLE_COUT("Multiply Channels");
		    for (unsigned int i=0 ; i<w*h ; i++ )
		    { 
			    XPtr[i]	*= mult  ;
			    YPtr[i]	*= mult  ;
			    ZPtr[i]	*= mult  ;
		    }
		    boost::gil::transform_pixels ( xyzView, srcWorkV, convertXYZToRgb<rgb32f_pixel_t, rgb32f_pixel_t>() );
		   }
		   
		   if( !_params._processLocal )
		    {
		      TUTTLE_COUT("Global");		    
		      am->setAdaptation(_params._Cone, _params._Rod);
		    }
		    tmo_pattanaik00( w, h, R, G, B, YPtr, am, _params._processLocal );
		    copy_and_convert_pixels( srcWorkV, dst );
		  
		  break;
		case 2:
		  //reinhard05
		  tmo_reinhard05(w, h, R, G, B, YPtr, _params._Brightness, _params._ChromaticAdaptation, _params._LightAdaptation);
		  copy_and_convert_pixels( srcWorkV, dst ); 
		  break;
		  
		case 3:
		  //reinhard02
		 /* use_scales true: local version, false: global version of TMO
		  * key maps log average luminance to this value (default: 0.18)
		  * phi sharpening parameter (defaults to 1 - no sharpening)
		  * num number of scales to use in computation (default: 8)
		  * low size in pixels of smallest scale (should be kept at 1)
		  * high size in pixels of largest scale (default 1.6^8 = 43) */ 
		  tmo_reinhard02( w, h, YPtr, LPtr, 1, _params._key, _params._phi, 8, 1, 43, 0/*temporal_coherent*/ );
		  
		  		  for (unsigned int i=0 ; i<w*h ; i++ )
		  { 
			  float scale = LPtr[i] / YPtr[i] ;
			  XPtr[i]	*= scale  ;
			  YPtr[i]	*= scale  ;
			  ZPtr[i]	*= scale  ;
		  }	// Rescaling of the 3 components with the new luminance values

		  boost::gil::transform_pixels ( xyzView, srcWorkV, convertXYZToRgb<rgb32f_pixel_t, rgb32f_pixel_t>() ); // Back 2 RGB
		  copy_and_convert_pixels( srcWorkV, dst );
		  
		  break;
		case 4:
		//Durand02 operations 	
		//_____NOT WORKING_____(core dumped errors) 	
		//tmo_durand02(w, h, XPtr, YPtr, ZPtr, _params._SpatialKernelSigma, _params._RangeKernelSigma, _params._BaseContrast, 1);
		  break;
		case 5: 
		   	
		  tmo_fattal02(w, h,  YPtr, LPtr , _params._Alpha, _params._Beta, _params._NoiseReduction);
		  	
		   for (unsigned int i=0 ; i<w*h ; i++ )
		  { 
			  //float scale = LPtr[i] / YPtr[i] ;
			  //(*X)(x,y) = powf( (*X)(x,y)/(*Y)(x,y), opt_saturation ) * (*L)(x,y);
			  //(*Z)(x,y) = powf( (*Z)(x,y)/(*Y)(x,y), opt_saturation ) * (*L)(x,y);
			  //(*Y)(x,y) = (*L)(x,y);
			  XPtr[i]= powf( XPtr[i]/YPtr[i], _params._ColorSaturation ) * LPtr[i];
			  ZPtr[i]= powf( ZPtr[i]/YPtr[i], .8f ) * LPtr[i];
			  YPtr[i]= LPtr[i]  ;
		  }	// Rescaling of the 3 components with the new luminance values
		
		  boost::gil::transform_pixels ( xyzView, srcWorkV, convertXYZToRgb<rgb32f_pixel_t, rgb32f_pixel_t>() ); // Back 2 RGB
		  copy_and_convert_pixels( srcWorkV, dst ); 	
		  
		  break;
		case 6: break;
		case 7: break;
		default:break;
	}/**/
}


template<>
void ToneMappingProcess<rgb32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	rgb32f_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	rgb32f_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );

	copy_pixels( src, dst );
	TUTTLE_COUT("pass here: rgb32f...");
	switch(  _params._toneoperator )
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		default:break;
	}
}



template<>
void ToneMappingProcess<boost::gil::gray32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	gray32f_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	gray32f_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	copy_pixels( src, dst );
	TUTTLE_COUT("pass here: gray...");
		switch(  _params._toneoperator )
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		default:break;
	}
}

template<>
void ToneMappingProcess<boost::gil::gray32_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	gray32_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	gray32_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	copy_pixels( src, dst );
	TUTTLE_COUT("pass here: gray...");
		switch(  _params._toneoperator )
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		default:break;
	}
}

template<>
void ToneMappingProcess<boost::gil::gray16_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	gray16_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	gray16_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	copy_pixels( src, dst );
	TUTTLE_COUT("pass here: gray...");
		switch(  _params._toneoperator )
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		default:break;
	}
}

template<>
void ToneMappingProcess<boost::gil::gray8_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	gray8_view_t src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	gray8_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x,
	                          procWindowSize.y );
	copy_pixels( src, dst );
	TUTTLE_COUT("pass here: gray...");
		switch(  _params._toneoperator )
	{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		default:break;
	}
}

void test( const OfxRectI& procWindowRoW )
{
}   



}
}
}

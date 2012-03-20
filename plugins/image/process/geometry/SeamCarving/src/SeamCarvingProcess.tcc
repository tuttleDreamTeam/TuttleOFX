#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <terry/sampler/resample_progress.hpp>
#include <terry/geometry/affine.hpp>

/*template<typename P>
void min3( P x1, P x2, P x3, char dir, P minValue )
{
    minValue = x2;
    if( x1 < minValue )
    {
	minValue = x1;
	dir = -1;
    }
    else if( x3 < minValue)
    {
	minValue = x3;
	dir = 1;
    }
    else
    {
	dir = 0;
    }
 
}*/

namespace tuttle {
namespace plugin {
namespace seamcarving {

template<class View>
SeamCarvingProcess<View>::SeamCarvingProcess( SeamCarvingPlugin &effect )
: ImageGilFilterProcessor<View>( effect , eImageOrientationFromTopToBottom )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void SeamCarvingProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}



/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void SeamCarvingProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<>
void SeamCarvingProcess<boost::gil::gray32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
  	using namespace terry;
	using namespace terry::sampler;

	//OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	TUTTLE_COUT("titi est un BG!");
	
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
								procWindowRoW.y2 - procWindowRoW.y1 };

	gray32f_view_t src = subimage_view( this->_srcView, procWindowOutput.x1,
							 procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	gray32f_view_t dst = subimage_view( this->_dstView, procWindowOutput.x1,
							 procWindowOutput.y1, procWindowSize.x, procWindowSize.y );

	//gray32f_image_t cumulSum (src.width(), src.height(), 0);
	//gray32f_view_t viewCumulSum <gray32f_pixel_t>( cumulSum);
	
	//copy_pixels( src, dst );
	
	// get_color(dst,black_t())   = (std::min)(get_color(dst,cyan_t()),
	
	for(int x= 0; x< src.width(); x++ )
	{
	  dst(x,0) = src(x,0);
	}
	
	for(int y= 1; y< src.height(); y++ )
	{
	    for(int x= 0; x< src.width(); x++ )
	    {
		char dir;
		//::min3( get_color( src(x-1,y-1), gray_color_t() ), get_color( src(x,y-1), gray_color_t() ), get_color( src(x+1,y-1), gray_color_t() ), dir, get_color( dst(x,y), gray_color_t() ) );
		//dst(x,y) = (std::min)( gt_color( src(x,y-1), gray_color_t() ), get_color( src(x-1,y-1), gray_color_t() ) );
		float x1 = get_color( src(x-1,y-1), gray_color_t() );
		float x2 = get_color( src(x,y-1), gray_color_t() );
		float x3 = get_color( src(x+1,y-1), gray_color_t() );
		
		float minValue = x2;
		if( x1 < minValue )
		{
		    minValue = x1;
		    dir = -1;
		}
		else if( x3 < minValue)
		{
		    minValue = x3;
		    dir = 1;
		}
		else
		{
		    dir = 0;
		}
		dst(x,y) = minValue;
	    }		  
	}
}


}
}
}

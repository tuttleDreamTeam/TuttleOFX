#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <terry/geometry/affine.hpp>
//#include <gray.hpp>

namespace tuttle {
namespace plugin {
namespace seamcarving {

template<class MapView, class View>
SeamCarvingProcess<MapView, View>::SeamCarvingProcess( SeamCarvingPlugin &effect )
        : ImageGilFilterProcessor<View>( effect , eImageOrientationFromTopToBottom )
	, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class MapView, class View>
void SeamCarvingProcess<MapView, View>::setup( const OFX::RenderArguments& args )
{
        ImageGilFilterProcessor<View>::setup( args );
        _params = _plugin.getProcessParams( args.renderScale );

	// clip Map
        _mapImg = _plugin._mapClip->fetchImage( args.time );
        if( _mapImg == NULL )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
        if( _mapImg->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );

        _mapPixelRod = _plugin._mapClip->getPixelRod( args.time, args.renderScale );
        _mapView = tuttle::plugin::getGilView<MapView>( _mapImg, _mapPixelRod, eImageOrientationFromTopToBottom );

        //TUTTLE_COUT_VAR( _srcPixelRod );
        //TUTTLE_COUT_VAR( _mapPixelRod );
}

template<class MapView>
void processVerticalCumulSum( MapView& map, boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum )
{
	using namespace terry;
	TUTTLE_COUT("CumulSum");
	
	float currentP;
	float topLeft;
	float top;
	float topRight;
	float minValue;
	int dir;

	for(int y = 1; y < map.height(); y++ )
	{
		for(int x = 0; x < map.width(); x++ )
		{
			currentP = get_color( map(x,y), gray_color_t() );
			topLeft  = get_color( viewCumulSum(x-1,y-1), gray_color_t() );
			top      = get_color( viewCumulSum(x  ,y-1), gray_color_t() );
			topRight = get_color( viewCumulSum(x+1,y-1), gray_color_t() );
			minValue = top;
			dir = 0;
		
			if( topLeft < minValue )
			{
				//TUTTLE_COUT("dir = -1");
				minValue = topLeft;
				dir = -1;
			}
			else if( topRight < minValue)
			{
				//TUTTLE_COUT("dir = 1");
				minValue = topRight;
				dir = 1;
			}
			get_color( viewCumulSum(x,y), gray_color_t() ) = currentP + minValue;
			//TUTTLE_COUT(minValue);
			get_color( viewDirMap(x,y), gray_color_t() ) = dir;
	
		}
	}
	//copy_and_convert_pixels( viewCumulSum, map );
}

void processFoundVerticalSeam( boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, std::vector<int>& seamPosition )
{

}

template<class MapView>
void processRemoveSeam( MapView& map, std::vector<int>& seamPosition )
{

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class MapView, class View>
void SeamCarvingProcess<MapView, View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;

	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
        OfxPointI procWindowSize = { procWindowOutput.x2 - procWindowOutput.x1, procWindowOutput.y2 - procWindowOutput.y1 };
	
        //TUTTLE_COUT( procWindowOutput );
        //TUTTLE_COUT( procWindowSize );
	
        View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
        MapView map = subimage_view( _mapView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
	gray32f_image_t cumulSum ( map.width(), map.height(), 0 );
	gray32f_view_t viewCumulSum ( view(cumulSum) );
	
	gray32f_image_t dirMap ( map.width(), map.height(), 0 );
	gray32f_view_t viewDirMap ( view( dirMap ) );

	if( _params._showMap )
        {
                copy_and_convert_pixels( map, dst );
	}
	else
	{
                TUTTLE_COUT("show resized");
                processVerticalCumulSum( map, viewDirMap, viewCumulSum);
		copy_and_convert_pixels( viewCumulSum, dst );
        }
}

}
}
}

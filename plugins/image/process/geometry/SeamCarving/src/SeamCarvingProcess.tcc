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
}

template<class MapView>
void ProcessVerticalCumulSum( MapView& verticalMap, boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, int widthProcess)
{
	using namespace terry;
	float currentP;
	float topLeft;
	float top;
	float topRight;
	float minValue;
	int dir;
	
	for(int x = 0; x < widthProcess; x++ )
	{
		viewCumulSum(x,0) = verticalMap(x,0);
	}
	for(int y = 1; y < verticalMap.height(); y++ )
	{
		//First Column
		currentP = get_color( verticalMap(0,y), gray_color_t() );
		top      = get_color( viewCumulSum(0  ,y-1), gray_color_t() );
		topRight = get_color( viewCumulSum(1,y-1), gray_color_t() );
			
		if( topRight < top )
		{
			minValue = topRight;
			dir = 1;
		}
		else
		{
			minValue = top;
			dir = 0;
		}
		viewCumulSum(0,y) = currentP + minValue;
		viewDirMap(0,y) = dir;
		
		//Last Column
		currentP = get_color( verticalMap(widthProcess-1,y), gray_color_t() );
		top      = get_color( viewCumulSum(widthProcess-1  ,y-1), gray_color_t() );
		topLeft  = get_color( viewCumulSum(widthProcess-2,y-1), gray_color_t() );			
		if( topLeft < top )
		{
			minValue = topLeft;
			dir = -1;
		}
		else
		{
			minValue = top;
			dir = 0;
		}
		viewCumulSum(widthProcess-1,y) = currentP + minValue;
		viewDirMap(widthProcess-1,y) = dir;
		
		//Other Column
		for(int x = 1; x < widthProcess-1; x++ )
		{
			currentP = get_color( verticalMap(x,y), gray_color_t() );
			topLeft  = get_color( viewCumulSum(x-1,y-1), gray_color_t() );
			top      = get_color( viewCumulSum(x  ,y-1), gray_color_t() );
			topRight = get_color( viewCumulSum(x+1,y-1), gray_color_t() );
			minValue = top;
			dir = 0;
		
			if( topLeft < minValue )
			{
				minValue = topLeft;
				dir = -1;
			}
			else if( topRight < minValue)
			{
				minValue = topRight;
				dir = 1;
			}
			viewCumulSum(x,y) = currentP + minValue;
			viewDirMap(x,y) = dir;	
		} 
	}
}

void ProcessFoundVerticalSeam( boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, std::vector<int>& seamPosition, int widthProcess )
{
	using namespace terry;
	/* Find the last seam element. (pixel to remove in the last arrow of the image)*/
	float min = get_color( viewCumulSum(0, viewCumulSum.height()-1), gray_color_t() ) ;
	int index = 0;
	for(int x = 1; x < widthProcess; x++ )
	{
		if ( get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) < min )
		{
			min = get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) ;
			index = x;
		}
	}
	seamPosition.at(viewCumulSum.height()-1) = index;
	/* Find the other seam elements */
	for(int y = 0 ; y <= viewCumulSum.height()-2 ; y++ )
	{
		seamPosition.at(viewCumulSum.height()-2 - y) = seamPosition.at(viewCumulSum.height()- 1 - y) + get_color( viewDirMap( seamPosition.at(viewCumulSum.height()- 1 - y) ,viewCumulSum.height()- 1 - y ), gray_color_t() ) ;
	}
}

template<class View, class MapView>
void ProcessRemoveSeam( View& processingSrc, MapView& verticalMap, std::vector<int>& seamPosition)
{
	using namespace terry;
	for (int y = 0; y < processingSrc.height(); y++)
	{
		for (int x = seamPosition.at(y); x < processingSrc.width()-1; x++)
		{
			processingSrc(x,y) = processingSrc(x+1,y);
			verticalMap(x,y) = verticalMap(x+1,y);
		}
	}
// 	TUTTLE_COUT("The seam is removed");
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
	
	/* Affichage de src et dst */
	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
	TUTTLE_COUT_VAR(procWindowOutput.x1);
	TUTTLE_COUT_VAR(procWindowOutput.y1);
	TUTTLE_COUT_VAR(procWindowSize.x);
	TUTTLE_COUT_VAR(procWindowSize.y);
	
	/* Affichage de verticalMap --> gradient */
        MapView verticalMap = subimage_view( _mapView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
	/* Affichage de CumulSum image */
	gray32f_image_t cumulSum ( verticalMap.width(), verticalMap.height(), 0 );
	gray32f_view_t viewCumulSum ( view(cumulSum) );
	
	/* Affichage de direction map */
	gray32f_image_t dirMap ( verticalMap.width(), verticalMap.height(), 0 );
	gray32f_view_t viewDirMap ( view( dirMap ) );
	
	/* Image temporaire pendant SeamCarving Process : */
	View processingSrc = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	
	std::vector<int> seamPosition( viewCumulSum.height() ); 	// vecteur seam

	
	int iteration_x = src.width() - _params._outputSize.x;
	int widthProcess = 0;

	
	
	if( _params._showMap )
        {
		copy_and_convert_pixels( verticalMap, dst );
		TUTTLE_COUT("showMap active");
	}
	/*else if( _params._showSource )
        {
		copy_and_convert_pixels( src, dst );
		TUTTLE_COUT("showSource active");
	}*/
	else
	{
		TUTTLE_COUT("show Seam Carving");
		for (int i = 0; i < iteration_x; i++)
		{
			widthProcess = src.width() - i;
			ProcessVerticalCumulSum( verticalMap, viewDirMap, viewCumulSum, widthProcess);
			ProcessFoundVerticalSeam( viewDirMap, viewCumulSum, seamPosition, widthProcess);
			ProcessRemoveSeam(processingSrc, verticalMap, seamPosition);
		}
		copy_and_convert_pixels( processingSrc, dst);
        }
}

}
}
}

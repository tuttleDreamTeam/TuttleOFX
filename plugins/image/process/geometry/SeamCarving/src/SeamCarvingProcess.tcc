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
	TUTTLE_COUT("CumulSum Test");
	
	float currentP;
	float topLeft;
	float top;
	float topRight;
	float minValue;
	int dir;
	
	for(int x = 0; x < map.width(); x++ )
	{
		viewCumulSum(x,0) = map(x,0);
	}

	for(int y = 1; y < map.height(); y++ )
	{
		//First Column
		currentP = get_color( map(0,y), gray_color_t() );
		top      = get_color( viewCumulSum(0  ,y-1), gray_color_t() );
		topRight = get_color( viewCumulSum(1,y-1), gray_color_t() );
			
		if( topRight < top )
		{
			//TUTTLE_COUT("dir = -1");
			minValue = topRight;
			dir = 1;
		}
		else
		{
			//TUTTLE_COUT("dir = 1");
			minValue = top;
			dir = 0;
		}
		get_color( viewCumulSum(0,y), gray_color_t() ) = currentP + minValue;
		get_color( viewDirMap(0,y), gray_color_t() ) = dir;
		
		//Last Column
		currentP = get_color( map(map.width()-1,y), gray_color_t() );
		top      = get_color( viewCumulSum(map.width()-1  ,y-1), gray_color_t() );
		topLeft  = get_color( viewCumulSum(map.width()-2,y-1), gray_color_t() );
			
		if( topLeft < top )
		{
			//TUTTLE_COUT("dir = -1");
			minValue = topLeft;
			dir = -1;
		}
		else
		{
			//TUTTLE_COUT("dir = 1");
			minValue = top;
			dir = 0;
		}
		get_color( viewCumulSum(map.width()-1,y), gray_color_t() ) = currentP + minValue;
		get_color( viewDirMap(map.width()-1,y), gray_color_t() ) = dir;
		
		//Other Column
		for(int x = 1; x < map.width()-1; x++ )
		{
			currentP = get_color( map(x,y), gray_color_t() );
			//TUTTLE_COUT_VAR(currentP);
			topLeft  = get_color( viewCumulSum(x-1,y-1), gray_color_t() );
			//TUTTLE_COUT_VAR(topLeft);
			top      = get_color( viewCumulSum(x  ,y-1), gray_color_t() );
			topRight = get_color( viewCumulSum(x+1,y-1), gray_color_t() );
			//TUTTLE_COUT_VAR(topRight);
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
			//TUTTLE_COUT( get_color( viewDirMap(x,y), gray_color_t() ) );
		} 
	}
	//copy_and_convert_pixels( viewCumulSum, map );
// 	TUTTLE_COUT("Last arrow of ViewCumulSum: ");
// 	for(int x = 0; x < viewCumulSum.width(); x++ )
// 	{
// 		float var = get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) ;
// 		TUTTLE_COUT(var);
// 	}
}

void printCumulSum( boost::gil::gray32f_view_t& viewCumulSum )
{
	using namespace terry;
	TUTTLE_COUT("WeightedMap");

  
}

void processFoundVerticalSeam( boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum, std::vector<int>& seamPosition )
//void processFoundVerticalSeam( boost::gil::gray32f_view_t& viewDirMap, boost::gil::gray32f_view_t& viewCumulSum )
{
	using namespace terry;
	
	TUTTLE_COUT("Find Seam");	
	/* Find the last seam element. (pixel to remove in the last arrow of the image)*/
	
// 	TUTTLE_COUT("Last arrow: ");
// 	for(int x = 0; x < viewCumulSum.width(); x++ )
// 	{
// 		float var = get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) ;
// 		TUTTLE_COUT(var);
// 	}
	
	float min = get_color( viewCumulSum(0, viewCumulSum.height()-1), gray_color_t() ) ;
	int index = 0;
	TUTTLE_COUT_VAR(min);
	TUTTLE_COUT_VAR(index);
	
	for(int x = 1; x < viewCumulSum.width(); x++ )
	{
		if ( get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) < min )
		{
			min = get_color( viewCumulSum(x, viewCumulSum.height()-1), gray_color_t() ) ;
			index = x;
		}
	}
	
	seamPosition.at(viewCumulSum.height()-1) = index;
	TUTTLE_COUT("OK");
	TUTTLE_COUT("Index of the Pixel to remove in the last arrow of the image: ");
	TUTTLE_COUT_VAR(viewCumulSum.height()-1);

	
	/* Find the other seam elements */
	//TUTTLE_COUT(seamPosition.at(0));
	/*for(int y = viewCumulSum.height()-2 ; y== 0 ; y-- )
	{
		seamPosition.at(y) = seamPosition.at(y+1) + get_color( viewDirMap(seamPosition.at(y+1),y+1 ), gray_color_t() );
	}*/
	for(int y = 0 ; y <= viewCumulSum.height() - 2 ; y++ )
	{
		//TUTTLE_COUT_VAR(viewCumulSum.height()- 2 - y);
		seamPosition.at(viewCumulSum.height()- 2 - y) = seamPosition.at(viewCumulSum.height()- 1 - y) + get_color( viewDirMap( seamPosition.at(viewCumulSum.height()- 1 - y) ,viewCumulSum.height()- 1 - y ), gray_color_t() ) ;
	}
		
	TUTTLE_COUT("The seam is: ");
	for(int x = 0; x < viewCumulSum.height(); x++ )
	{
		TUTTLE_COUT_VAR(seamPosition.at(x));
	}
	//TESTER DIRMAP

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
	
	std::vector<int> seamPosition( viewCumulSum.height() );

	if( _params._showMap )
        {
		copy_and_convert_pixels( map, dst );
		TUTTLE_COUT("showMap active");
	}
	else
	{
		TUTTLE_COUT("show Seam Carving");
		processVerticalCumulSum( map, viewDirMap, viewCumulSum);
		processFoundVerticalSeam( viewDirMap, viewCumulSum, seamPosition);
		copy_and_convert_pixels( viewCumulSum, dst );
		
        }
}

}
}
}

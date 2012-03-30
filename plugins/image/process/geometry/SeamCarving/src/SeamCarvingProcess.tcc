#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <terry/sampler/resample_progress.hpp>
#include <terry/geometry/affine.hpp>

/*template<typename P>
void min3( P x1, P x2, P x3, char& dirValue, P& minValue )
{
    minValue = x2;
    if( x1 < minValue )
    {
	minValue = x1;
	dirValue = -1;
    }
    else if( x3 < minValue)
    {
	minValue = x3;
	dirValue = 1;
    }
    else
    {
	dirValue = 0;
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
	using namespace terry;
	using namespace terry::sampler;

	TUTTLE_COUT("titi est un RGB!");
	
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
								procWindowRoW.y2 - procWindowRoW.y1 };

	View src = subimage_view( this->_srcView, procWindowOutput.x1,
							 procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1,
							 procWindowOutput.y1, procWindowSize.x, procWindowSize.y );
		
	
	/*for(int y(0); y < src.height()-1; y++ )
	{
	  for(int x(0); x < src.width(); x++ )
	  {
	    dir(x,y) = 0;	 
	  }	 
	}*/
	
	for(int x = 0; x < src.width(); x++ )
	{
		dst(0,x) = src(0,x);
	}
	for(int y = 1; y < src.height(); y++ )
	{
               for(int x = 0; x < src.width(); x++ )
               {

                       float currentP = get_color( src(x,y), red_t() );

                       float topLeft  = get_color( dst(x-1,y-1), red_t() );
                       float top      = get_color( dst(x  ,y-1), red_t() );
                       float topRight = get_color( dst(x+1,y-1), red_t() );

                       float minValue = top;
                       char  dir      = 0 ;
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
                       get_color( dst(x,y), red_t() ) = currentP + minValue;
                       get_color( dst(x,y), green_t() ) = currentP + minValue;
                       get_color( dst(x,y), blue_t() ) = currentP + minValue;
               }
       }
	
	/*for(int y(1); y < src.height(); y++ )
	{
	    char dirValue;
	    float minValue;
	    int position(0);
	    float x1 = get_color( dst( y-1, position), gray_color_t() );
	    float x2 = get_color( dst( y-1, position+1 ), gray_color_t() );
	    	    
	    //First Column

	    if( x1 < x2 )
	    {
	      minValue = x1;
	      dirValue = 0;
	    }
	    else 
	    {
	      minValue = x2;
	      dirValue = 1;
	    }
	    get_color( dst( y, position ), gray_color_t() ) = get_color( src( y, position ), gray_color_t() )  + minValue;
	    //dir( y-1, position ) = dirValue;
	    
	   
	    //Last Column
	    position = src.width();
	    
	    x1 = get_color( dst( y-1, position-1 ), gray_color_t() );
	    x2 = get_color( dst( y-1, position), gray_color_t() );
	    
	    if( x1 < x2 )
	    {
	      minValue = x1;
	      dirValue = -1;
	    }
	    else 
	    {
	      minValue = x2;
	      dirValue = 0;
	    }
	    //dst( y, position ) = src( y, position ) + minValue;
	    get_color( dst( y, position ), gray_color_t() ) = get_color( src( y, position ), gray_color_t() )  + minValue;
	    //dir( y-1, position ) = dirValue;
	    
	    //Other columns
	    for(int x(1); x < src.width()-1; x++ )
	    {
		char dirValue;
		
		//::min3( get_color( src(x-1,y-1), gray_color_t() ), get_color( src(x,y-1), gray_color_t() ), get_color( src(x+1,y-1), gray_color_t() ), dir, get_color( dst(x,y), gray_color_t() ) );
		//dst(x,y) = (std::min)( gt_color( src(x,y-1), gray_color_t() ), get_color( src(x-1,y-1), gray_color_t() ) );
			
		float x1 = get_color( dst( y-1, x-1 ), gray_color_t() );
		float x2 = get_color( dst( y-1, x ), gray_color_t() );
		float x3 = get_color( dst( y-1, x+1 ), gray_color_t() );
		
		float minValue = x2;	
		if( x1 < minValue )
		{
		    minValue = x1;
		    dirValue = -1;
		}
		else if( x3 < minValue)
		{
		    minValue = x3;
		    dirValue = 1;
		}
		else
		{
		    dirValue = 0;
		}
		//dst( y, x ) = src( y, x ) + minValue;
		get_color( dst( y, x ), gray_color_t() ) = get_color( src( y, x ), gray_color_t() )  + minValue;
		//dir( y-1, x ) = dirValue;
	    }*/		  
	
  

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */

template<>
void SeamCarvingProcess<boost::gil::gray32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
       TUTTLE_COUT("titi 32f!");
}

template<>
void SeamCarvingProcess<boost::gil::gray32_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
       TUTTLE_COUT("titi 32");
}

template<>
void SeamCarvingProcess<boost::gil::gray16_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
       TUTTLE_COUT("titi 16");
}

template<>
void SeamCarvingProcess<boost::gil::gray8_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
       TUTTLE_COUT("titi 8");
}


}
}
}

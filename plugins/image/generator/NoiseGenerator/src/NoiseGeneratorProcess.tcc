#include "NoiseGeneratorProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace noisegenerator {

template<class View>
NoiseGeneratorProcess<View>::NoiseGeneratorProcess( NoiseGeneratorPlugin& instance )
: ImageGilProcessor<View>( instance, eImageOrientationFromTopToBottom )
, _plugin( instance )
{}

template<class View>
NoiseGeneratorParams<View> NoiseGeneratorProcess<View>::getParams()
{
	using namespace boost::gil;
	NoiseGeneratorParams<View> params;
	

	OfxRGBAColourD c1 = _plugin._color1->getValue();
	params._sigma = _plugin._sigma->getValue();
	color_convert( rgba32f_pixel_t( c1.r, c1.g, c1.b, c1.a ), params._color1 );

	
	
	return params;
}

template<class View>
void NoiseGeneratorProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilProcessor<View>::setup( args );

	boost::function_requires<PixelLocatorConcept<Locator> >();
	gil_function_requires < StepIteratorConcept<typename Locator::x_iterator> >();

	// params
	NoiseGeneratorParams<View> params = getParams();

	OfxRectD rod = _plugin._clipDst->getCanonicalRod( args.time );
	Point dims( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	int yshift = boost::numeric_cast<int>( ( dims.x - dims.y ) * 0.5 );

	// create a squared noisegenerator
	NoiseGeneratorVirtualView noise( Point( dims.x, dims.x ), Locator( Point( 0, 0 ), Point( 1, 1 ), NoiseGeneratorFunctorT( params._color1, params._sigma) ) );
	// create a subview depending on the image ratio
	_srcView = subimage_view<>( noise, 0, yshift, boost::numeric_cast<int>( dims.x ), boost::numeric_cast<int>( dims.y ) );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void NoiseGeneratorProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename NoiseGeneratorVirtualView::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it                    = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			*dst_it = *src_it;
		}
		if( this->progressForward() )
			return;
	}
}

}
}
}

#include "NoiseGeneratorProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include "NoiseGeneratorPlugin.hpp"

#include "NoiseGeneratorDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

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
	using namespace terry;
	using namespace boost::gil;
	NoiseGeneratorParams<View> params;
	
	params._color  		= static_cast<ENoiseGeneratorColor>( _plugin._color->getValue() );

	params._sigma 		= _plugin._sigma->getValue();
	params._nu 		= _plugin._nu->getValue();

	params._r_weight 	= _plugin._r_weight->getValue();
	params._g_weight 	= _plugin._g_weight->getValue();
	params._b_weight	 = _plugin._b_weight->getValue();
	params._bw_weight 	= _plugin._bw_weight->getValue();

	
	return params;
}

template<class View>
 void NoiseGeneratorProcess<View>::updateParameters()
{
	_plugin._sigma  ->setIsSecretAndDisabled( true );
	
	_nu        	->setIsSecretAndDisabled( true );
	_r_weight      	->setIsSecretAndDisabled( true );
	_g_weight       ->setIsSecretAndDisabled( true );
	_b_weight	->setIsSecretAndDisabled( true );
	_bw_weight	->setIsSecretAndDisabled( true );
	
	switch( _plugin._color->getValue() )
	{
		case 0: // nb noise
			_nu		->setIsSecretAndDisabled( false );
			_sigma		->setIsSecretAndDisabled( false );
			_bw_weight	->setIsSecretAndDisabled( false );
			break;
		case 1: // color noise
			_nu		->setIsSecretAndDisabled( false );
			_sigma		->setIsSecretAndDisabled( false );
			_r_weight	->setIsSecretAndDisabled( false );
			_g_weight	->setIsSecretAndDisabled( false );
			_b_weight	->setIsSecretAndDisabled( false );
			break;
			default:
			break;
	}
}

template<class View>
void NoiseGeneratorProcess<View>::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
if( paramName == kParamDefault )
	{
		TUTTLE_COUT("pass here: default...");
		switch( _plugin._color->getValue() )
			{
				case 0: // nb noise
					_nu		->setValue( 0 );
					_sigma		->setValue( 0 );
					_bw_weight	->setValue( 0 );
					break;
				case 1: // color noise
					_nu		->setValue( 0 );
					_sigma		->setValue( 0);
					_r_weight	->setValue( 0 );
					_g_weight	->setValue( 0 );
					_b_weight	->setValue( 0 );
					break;
					default:
					break;
			}
		}			
	updateParameters();
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

	// create a noise
	NoiseGeneratorVirtualView noise( Point( dims.x, dims.x ), Locator( Point( 0, 0 ), Point( 1, 1 ), NoiseGeneratorFunctorT( params._sigma, params._nu, params._color, params._r_weight, params._g_weight, params._b_weight, params._bw_weight, params._nb) ) );


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

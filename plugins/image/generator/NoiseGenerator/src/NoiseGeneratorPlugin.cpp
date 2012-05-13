#include "NoiseGeneratorPlugin.hpp"
#include "NoiseGeneratorProcess.hpp"
#include "NoiseGeneratorDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace noisegenerator {

NoiseGeneratorPlugin::NoiseGeneratorPlugin( OfxImageEffectHandle handle )
	: GeneratorPlugin( handle )
{
	
	
	_sigma  = fetchDoubleParam( kNoiseGeneratorSigma );
	_nu = fetchDoubleParam( kNoiseGeneratorNu );
	
	_color     = fetchChoiceParam( kNoiseGeneratorColor );

	_r_weight  = fetchDoubleParam( kNoiseGeneratorRweight );
	_g_weight  = fetchDoubleParam( kNoiseGeneratorGweight );
	_b_weight  = fetchDoubleParam( kNoiseGeneratorBweight );
	_bw_weight  = fetchDoubleParam( kNoiseGeneratorBW );

	_nb  = fetchDoubleParam( kNoiseGeneratorNB );


}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */



void NoiseGeneratorPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<NoiseGeneratorProcess>( *this, args );
}

void NoiseGeneratorPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	GeneratorPlugin::getClipPreferences( clipPreferences );

	if( getExplicitConversion() == eParamGeneratorExplicitConversionAuto )
	{
		clipPreferences.setClipBitDepth( *_clipDst, OFX::eBitDepthFloat );
	}
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
}

}
}
}

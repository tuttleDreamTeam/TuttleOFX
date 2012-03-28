#include "NoiseGeneratorPluginFactory.hpp"
#include "NoiseGeneratorPlugin.hpp"
#include "NoiseGeneratorDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace noisegenerator {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void NoiseGeneratorPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleNoiseGenerator", "NoiseGenerator",
			"NoiseGenerator" );
	desc.setPluginGrouping( "tuttle/image/generator" );

	desc.setDescription(
"NoiseGenerator / generation de bruit"

);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGenerator );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void NoiseGeneratorPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						   OFX::EContext               context )
{
	describeGeneratorParamsInContext( desc, context );

	OFX::RGBAParamDescriptor* color1 = desc.defineRGBAParam( kNoiseGeneratorColor1 );
	color1->setDefault( 0, 0, 0, 1 );
	color1->setLabel( "color1" );

	OFX::DoubleParamDescriptor* sigma = desc.defineDoubleParam( kNoiseGeneratorSigma );
	sigma->setDefault( 1 );
	sigma->setRange(0.00000001, 50); //find the max lim for a double
	sigma->setLabel( "sigma" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* NoiseGeneratorPluginFactory::createInstance( OfxImageEffectHandle handle,
							     OFX::EContext        context )
{
	return new NoiseGeneratorPlugin( handle );
}

}
}
}

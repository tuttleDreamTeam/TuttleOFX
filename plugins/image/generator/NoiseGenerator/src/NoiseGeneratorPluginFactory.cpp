#include "NoiseGeneratorPluginFactory.hpp"
#include "NoiseGeneratorPlugin.hpp"
#include "NoiseGeneratorDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <math.h>

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

	

	OFX::DoubleParamDescriptor* sigma = desc.defineDoubleParam( kNoiseGeneratorSigma );
	/*sigma->setDefault( 0.13);
	sigma->setRange(0.00000001, 0.13); //find the max lim for a double
	sigma->setLabel( "sigma" );
	sigma->setHint( "Sigma value of the gaussian" );

OFX::DoubleParamDescriptor* nu = desc.defineDoubleParam( kNoiseGeneratorNu );
	nu->setDefault( 10 );
	nu->setRange(0, 10); //find the max lim for a double
	nu->setLabel( "nu" );
	nu->setHint( "Nu value of the gaussian" );

OFX::ChoiceParamDescriptor* color = desc.defineChoiceParam( kNoiseGeneratorColor );
	color->setLabel( "Color" );
	color->appendOption( kNoiseGeneratorColorGrey );
	color->appendOption( kNoiseGeneratorColorRGB );
	color->setDefault( 0 );
	color->setHint( "Set the color of the noise" );*/

sigma->setDefault( 0.13);
	sigma->setRange(0.00000001, 10000); //find the max lim for a double
	sigma->setLabel( "sigma" );
	sigma->setHint( "Sigma value of the gaussian" );

OFX::DoubleParamDescriptor* nu = desc.defineDoubleParam( kNoiseGeneratorNu );
	nu->setDefault( 10 );
	nu->setRange(-10000, 10000); //find the max lim for a double
	nu->setLabel( "nu" );
	nu->setHint( "Nu value of the gaussian" );

OFX::ChoiceParamDescriptor* color = desc.defineChoiceParam( kNoiseGeneratorColor );
	color->setLabel( "Color" );
	color->appendOption( kNoiseGeneratorColorGrey );
	color->appendOption( kNoiseGeneratorColorRGB );
	color->setDefault( 0 );
	color->setHint( "Set the color of the noise" );

OFX::DoubleParamDescriptor* r_weight = desc.defineDoubleParam( kNoiseGeneratorRweight );
	r_weight->setDefault( 0 );
	r_weight->setRange(0, 10); //find the max lim for a double
	r_weight->setLabel( "r_weight" );
	r_weight->setHint( "r_weight value" );

OFX::DoubleParamDescriptor* g_weight = desc.defineDoubleParam( kNoiseGeneratorGweight );
	g_weight->setDefault( 0 );
	g_weight->setRange(0, 10); //find the max lim for a double
	g_weight->setLabel( "g_weight" );
	g_weight->setHint( "g_weight value" );

OFX::DoubleParamDescriptor* b_weight = desc.defineDoubleParam( kNoiseGeneratorBweight );
	b_weight->setDefault( 0 );
	b_weight->setRange(0, 10); //find the max lim for a double
	b_weight->setLabel( "b_weight" );
	b_weight->setHint( "b_weight value" );

OFX::DoubleParamDescriptor* bw_weight = desc.defineDoubleParam( kNoiseGeneratorBW );
	bw_weight->setDefault( 0 );
	bw_weight->setRange(0, 10); //find the max lim for a double
	bw_weight->setLabel( "bw" );
	bw_weight->setHint( "bw value" );

OFX::DoubleParamDescriptor* nb = desc.defineDoubleParam( kNoiseGeneratorNB );
	nb->setDefault( 0 );
	nb->setRange(0, 1); //find the max lim for a double
	nb->setLabel( "nb" );
	nb->setHint( "nb value" );
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

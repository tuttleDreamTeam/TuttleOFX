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
OFX::ChoiceParamDescriptor* color = desc.defineChoiceParam( kNoiseGeneratorColor );
	color->setLabel( "Color" );
	color->appendOption( kNoiseGeneratorColorGrey );
	color->appendOption( kNoiseGeneratorColorRGB );
	color->setDefault( 0 );
	color->setHint( "Set the color of the noise" );
	color->setDefault(eNoiseGeneratorColorGrey);

OFX::GroupParamDescriptor* color_grey = desc.defineGroupParam(  kNoiseGeneratorColorGrey );
	color_grey->setLabel( "grey" );
	color_grey->setOpen(true );

OFX::GroupParamDescriptor* color_rgb = desc.defineGroupParam(  kNoiseGeneratorColorRGB );
	color_rgb->setLabel( "rgb" );
	color_rgb->setOpen(false );

OFX::DoubleParamDescriptor* sigma = desc.defineDoubleParam( kNoiseGeneratorSigma );
	sigma->setDefault( 1);
	sigma->setRange(0.00000001, std::numeric_limits<double>::max()); 
	sigma->setLabel( "sigma" );
	sigma->setHint( "Sigma value of the gaussian" );

OFX::DoubleParamDescriptor* nu = desc.defineDoubleParam( kNoiseGeneratorNu );
	nu->setDefault( 0 );
	nu->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max()); 
	nu->setLabel( "nu" );
	nu->setHint( "Nu value of the gaussian" );

OFX::DoubleParamDescriptor* r_weight = desc.defineDoubleParam( kNoiseGeneratorRweight );
	r_weight->setDefault( 0 );
	r_weight->setRange(0, std::numeric_limits<double>::max()); 
	r_weight->setLabel( "r_weight" );
	r_weight->setHint( "r_weight value" );
	r_weight->setParent( color_rgb );

OFX::DoubleParamDescriptor* g_weight = desc.defineDoubleParam( kNoiseGeneratorGweight );
	g_weight->setDefault( 0 );
	g_weight->setRange(0, std::numeric_limits<double>::max()); 
	g_weight->setLabel( "g_weight" );
	g_weight->setHint( "g_weight value" );
	g_weight->setParent( color_rgb );

OFX::DoubleParamDescriptor* b_weight = desc.defineDoubleParam( kNoiseGeneratorBweight );
	b_weight->setDefault( 0 );
	b_weight->setRange(0, std::numeric_limits<double>::max()); 
	b_weight->setLabel( "b_weight" );
	b_weight->setHint( "b_weight value" );
	b_weight->setParent( color_rgb );

OFX::DoubleParamDescriptor* bw_weight = desc.defineDoubleParam( kNoiseGeneratorBW );
	bw_weight->setDefault( 0 );
	bw_weight->setRange(0, std::numeric_limits<double>::max()); 
	bw_weight->setLabel( "bw" );
	bw_weight->setHint( "bw value" );
	bw_weight->setParent( color_grey );
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

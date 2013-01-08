#include "AwaPluginFactory.hpp"
#include "AwaPlugin.hpp"
#include "AwaDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <limits>

namespace tuttle {
namespace plugin {
namespace awa {

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void AwaPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleAwa", "Awa",
	                "Awa" );
	desc.setPluginGrouping( "tuttle/image/process/filter" );

	desc.setDescription(
"A Adaptive Weighted Average filter."
"\n"
"\n"
"It is a widely used effect in graphics software, typically to reduce image "
"noise and reduce detail. The visual effect of this blurring technique is a "
"smooth blur resembling that of viewing the image through a translucent "
"screen, distinctly different from the bokeh effect produced by an "
"out-of-focus lens or the shadow of an object under usual illumination. "
"Gaussian smoothing is also used as a pre-processing stage in computer vision "
"algorithms in order to enhance image structures at different scales—see "
"scale-space representation and scale-space implementation."
"\n"
"Mathematically, applying a Gaussian blur to an image is the same as "
"convolving the image with a Gaussian function; this is also known as a "
"two-dimensional Weierstrass transform. Applying a Gaussian blur has the "
"effect of reducing the image's high-frequency components; "
"a Gaussian blur is thus a low pass filter."
"\n"
"\n"
"http://en.wikipedia.org/wiki/Gaussian_blur"
);

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void AwaPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                           OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );
	
	OFX::DoubleParamDescriptor* alpha = desc.defineDoubleParam( kParamAlpha );
	alpha->setLabel			( "alpha" );
	alpha->setDefault		( 0.5 );
	alpha->setRange			( 0.0, std::numeric_limits<double>::max() );
	alpha->setDisplayRange		( 0.001, 10.0 );
	alpha->setHint			( "alpha: \"penalty\" parameter that determines the sensitivity according to the weight." );

	OFX::DoubleParamDescriptor* epsilonR = desc.defineDoubleParam( kParamEpsilonR );
	epsilonR->setLabel		( "epsilon red" );
	epsilonR->setDefault		( 0.01 );
	epsilonR->setRange		( 0.0, std::numeric_limits<double>::max() );
	epsilonR->setDisplayRange	( 0.0, 1.0 );
	epsilonR->setHint		( "Noise variance in the red channel." );
	
	OFX::DoubleParamDescriptor* epsilonG = desc.defineDoubleParam( kParamEpsilonG );
	epsilonG->setLabel		( "epsilon green" );
	epsilonG->setDefault		( 0.01 );
	epsilonG->setRange		( 0.0, std::numeric_limits<double>::max() );
	epsilonG->setDisplayRange	( 0.0, 1.0 );
	epsilonG->setHint		( "Noise variance in the green channel." );
	
	OFX::DoubleParamDescriptor* epsilonB = desc.defineDoubleParam( kParamEpsilonB );
	epsilonB->setLabel		( "epsilon blue" );
	epsilonB->setDefault		( 0.01 );
	epsilonB->setRange		( 0.0, std::numeric_limits<double>::max() );
	epsilonB->setDisplayRange	( 0.0, 1.0 );
	epsilonB->setHint		( "Noise variance in the blue channel." );
		
	OFX::Double2DParamDescriptor* size = desc.defineDouble2DParam( kParamSize );
	size->setLabel( "Size" );
	size->setDefault( 3, 3 );
	size->setRange( 0.0, 0.0, std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
	size->setDisplayRange( 0, 0, 10, 10 );
	size->setDoubleType( OFX::eDoubleTypeScale );

	OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam( kParamBorder );
	border->setLabel( "Border" );
	border->appendOption( kParamBorderMirror );
	border->appendOption( kParamBorderConstant );
	border->appendOption( kParamBorderBlack );
	border->appendOption( kParamBorderPadded );


	OFX::GroupParamDescriptor* advanced = desc.defineGroupParam( kParamGroupAdvanced );
	advanced->setLabel( "Advanced" );
	advanced->setOpen( false );

	OFX::BooleanParamDescriptor* normalizedKernel = desc.defineBooleanParam( kParamNormalizedKernel );
	normalizedKernel->setLabel( "Normalized kernel" );
	normalizedKernel->setHint( "Use a normalized kernel to compute the gradient." );
	normalizedKernel->setDefault( true );
	normalizedKernel->setParent( advanced );

	OFX::DoubleParamDescriptor* kernelEpsilon = desc.defineDoubleParam( kParamKernelEpsilon );
	kernelEpsilon->setLabel( "Kernel espilon value" );
	kernelEpsilon->setHint( "Threshold at which we no longer consider the values of the function." );
	kernelEpsilon->setDefault( 0.01 );
	kernelEpsilon->setRange( std::numeric_limits<double>::epsilon(), 1.0 );
	kernelEpsilon->setDisplayRange( 0, 0.01 );
	kernelEpsilon->setParent( advanced );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* AwaPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                     OFX::EContext        context )
{
	return new AwaPlugin( handle );
}

}
}
}


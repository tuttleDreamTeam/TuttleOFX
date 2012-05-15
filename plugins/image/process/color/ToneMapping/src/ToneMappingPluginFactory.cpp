#include "ToneMappingPluginFactory.hpp"
#include "ToneMappingPlugin.hpp"
#include "ToneMappingDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ToneMappingPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleToneMapping", "ToneMapping",
	                "ToneMapping" );
	desc.setPluginGrouping( "tuttle/image/process/color" );
	desc.setDescription(
"ToneMapping"
"\n"
"\n"
"http://en.wikipedia.org/wiki/Tone_mapping"
"\n"
	/// @todo documentation: definition of ToneMapping
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
void ToneMappingPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                        OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* toneoperator = desc.defineChoiceParam( kParamToneOperator );
	toneoperator->setLabel( "Tone Mapping Operator" );
	toneoperator->setHint( "Tone Mapping Algorithm" );
	toneoperator->appendOption( kParamToneOperator_drago03 );
	toneoperator->appendOption( kParamToneOperator_pattanaik00 );
	toneoperator->appendOption( kParamToneOperator_reinhard05 );
	toneoperator->appendOption( kParamToneOperator_reinhard02 );
	toneoperator->appendOption( kParamToneOperator_durand02 );
	//toneoperator->appendOption( kParamToneOperator_ashikhmin );
	//toneoperator->appendOption( kParamToneOperator_fattal );
	//toneoperator->appendOption( kParamToneOperator_mantiuk );
	toneoperator->setDefault( eParamToneOperator_drago03 );

	OFX::GroupParamDescriptor* drago03Group = desc.defineGroupParam( kParamdrago03Group );
	drago03Group->setLabel( "Drago03" );
	drago03Group->setOpen( true );

	OFX::GroupParamDescriptor* pattanaik00Group = desc.defineGroupParam( kParampattanaik00Group );
	pattanaik00Group->setLabel( "Pattanaik00" );
	pattanaik00Group->setOpen(false );

	OFX::GroupParamDescriptor* reinhard05Group = desc.defineGroupParam( kParamreinhard05Group );
	reinhard05Group->setLabel( "Reinhard05" );
	reinhard05Group->setOpen( false );
	
	OFX::GroupParamDescriptor* durand02Group = desc.defineGroupParam( kParamdurand02Group );
	durand02Group->setLabel( "Durand02" );
	durand02Group->setOpen( false );
	

	
/**/
//drago03
	OFX::DoubleParamDescriptor* bias = desc.defineDoubleParam( kBias );
	bias->setLabel			( "Bias" );
	bias->setDefault		( 0.85 );
	bias->setRange			( 0.0, std::numeric_limits<double>::max() );
	bias->setDisplayRange		( 0.0, 1.0 );
	bias->setHint			( "Adjust the bias. The bias has an influence on the logarithmic function." );
	bias->setParent( drago03Group );

//pattanaik00
	OFX::DoubleParamDescriptor* mult = desc.defineDoubleParam( kMult );
	mult->setLabel			( "Multiplier" );
	mult->setDefault		( 1.0 );
	mult->setRange			( 0.0, std::numeric_limits<double>::max() );
	mult->setDisplayRange		( 0.0, 1000.0 );
	mult->setHint			( "Adjust the multiplier." );
	mult->setParent( pattanaik00Group );

	OFX::DoubleParamDescriptor* rod = desc.defineDoubleParam( kRod );
	rod->setLabel			( "Rod" );
	rod->setDefault			( 0.5 );
	rod->setRange			( 0.0, std::numeric_limits<double>::max() );
	rod->setDisplayRange		( 0.0, 1.0 );
	rod->setHint			( "Adjust the rod value." );
	rod->setParent( pattanaik00Group );

	OFX::DoubleParamDescriptor* cone = desc.defineDoubleParam( kCone );
	cone->setLabel			( "Cone" );
	cone->setDefault		( 0.5 );
	cone->setRange			( 0.0, std::numeric_limits<double>::max() );
	cone->setDisplayRange		( 0.0, 1.0 );
	cone->setHint			( "Adjust the cone value." );
	cone->setParent( pattanaik00Group );

	OFX::BooleanParamDescriptor* local = desc.defineBooleanParam( kParamProcessLocal );
	local->setLabel( "Process local" );
	local->setHint( "Apply the local algorithm." );
	local->setDefault( false );
	local->setParent( pattanaik00Group );

	OFX::BooleanParamDescriptor* autoconerod = desc.defineBooleanParam( kParamAutoConeRod );
	autoconerod->setLabel( "Process Auto Cone/Rod" );
	autoconerod->setHint( "Automatic computing of the rod and cone values" );
	autoconerod->setDefault( false );
	autoconerod->setParent( pattanaik00Group );
	
//reinhard05
	OFX::DoubleParamDescriptor* brightness = desc.defineDoubleParam( kBrightness );
	brightness->setLabel			( "Brightness" );
	brightness->setDefault			(0.0 );
	brightness->setRange			(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
	brightness->setDisplayRange		(-8.0, 8.0 );
	brightness->setHint			( "Adjust the base brightness." );
	brightness->setParent( reinhard05Group );

	OFX::DoubleParamDescriptor* chromaticAdaptation = desc.defineDoubleParam( kChromaticAdaptation );
	chromaticAdaptation->setLabel		( "Chromatic Adaptation" );
	chromaticAdaptation->setDefault		( 0.0 );
	chromaticAdaptation->setRange		( 0.0, std::numeric_limits<double>::max() );
	chromaticAdaptation->setDisplayRange	( 0.0, 1.0 );
	chromaticAdaptation->setHint		( "Adjust the Spatial Chromatic Adaptation." );
	chromaticAdaptation->setParent( reinhard05Group );

	OFX::DoubleParamDescriptor* lightAdaptation = desc.defineDoubleParam( kLightAdaptation );
	lightAdaptation->setLabel		( "Light Adaptation" );
	lightAdaptation->setDefault		( 0.0 );
	lightAdaptation->setRange		( 0.0, std::numeric_limits<double>::max() );
	lightAdaptation->setDisplayRange	( 0.0, 1.0 );
	lightAdaptation->setHint		( "Adjust the Light Adaptation." );
	lightAdaptation->setParent( reinhard05Group );

//durand02
	OFX::DoubleParamDescriptor* baseContrast = desc.defineDoubleParam( kBaseContrast );
	baseContrast->setLabel			( "Base Contrast" );
	baseContrast->setDefault		( 5.0 );
	baseContrast->setRange			( 0.0, std::numeric_limits<double>::max() );
	baseContrast->setDisplayRange		( 0.0, 10.0 );
	baseContrast->setHint			( "Adjust the base contrast." );
	baseContrast->setParent( durand02Group );

	OFX::DoubleParamDescriptor* spatialKernelSigma = desc.defineDoubleParam( kSpatialKernelSigma );
	spatialKernelSigma->setLabel		( "Spatial Kernel Sigma" );
	spatialKernelSigma->setDefault		( 2.0 );
	spatialKernelSigma->setRange		( 0.0, std::numeric_limits<double>::max() );
	spatialKernelSigma->setDisplayRange	( 0.0, 100.0 );
	spatialKernelSigma->setHint		( "Adjust the Spatial Kernel Sigma." );
	spatialKernelSigma->setParent( durand02Group );

	OFX::DoubleParamDescriptor* rangeKernelSigma = desc.defineDoubleParam( kRangeKernelSigma );
	rangeKernelSigma->setLabel		( "Range Kernel Sigma" );
	rangeKernelSigma->setDefault		( 2.0 );
	rangeKernelSigma->setRange		( 0.0, std::numeric_limits<double>::max() );
	rangeKernelSigma->setDisplayRange	( 0.0, 10.0 );
	rangeKernelSigma->setHint		( "Adjust the Range Kernel Sigma." );
	rangeKernelSigma->setParent( durand02Group );
	
	
	

	

	OFX::PushButtonParamDescriptor* bydefault = desc.definePushButtonParam( kParamDefault );
	bydefault->setLabel( "Default" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ToneMappingPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
	return new ToneMappingPlugin( handle );
}

}
}
}


#include "ToneMappingPluginFactory.hpp"
#include "ToneMappingPlugin.hpp"
#include "ToneMappingDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

static const bool kSupportTiles = true;

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

	OFX::ChoiceParamDescriptor* toneoperator = desc.defineChoiceParam( kParamToneOperator );
	toneoperator->setLabel( "Tone Mapping Operator" );
	toneoperator->setHint( "Tone Mapping Algorithm" );
	toneoperator->appendOption( kParamToneOperator_drago03 );
	toneoperator->appendOption( kParamToneOperator_pattanaik00 );
	toneoperator->appendOption( kParamToneOperator_durand02 );
	toneoperator->setDefault( eParamToneOperator_drago03 );
/*
	OFX::GroupParamDescriptor* drago03Group = desc.defineGroupParam( kParamdrago03Group );
	drago03Group->setLabel( "Drago03" );
	drago03Group->setOpen( true );

	OFX::GroupParamDescriptor* pattanaik00Group = desc.defineGroupParam( kParampattanaik00Group );
	pattanaik00Group->setLabel( "Pattanaik00" );
	pattanaik00Group->setOpen(false );

	OFX::GroupParamDescriptor* durand02Group = desc.defineGroupParam( kParamdurand02Group );
	durand02Group->setLabel( "Durand02" );
	durand02Group->setOpen( false );
*/
//drago03
	OFX::DoubleParamDescriptor* Bias = desc.defineDoubleParam( kBias );
	Bias->setLabel			( "Bias" );
	Bias->setDefault		( 0.85 );
	Bias->setRange			( 0.0, std::numeric_limits<double>::max() );
	Bias->setDisplayRange		( 0.0, 1.0 );
	Bias->setHint			( "Adjust the bias. The bias has an influence on the logarithmic function." );
//	Bias->setParent( drago03Group );

//pattanaik00
	OFX::DoubleParamDescriptor* Mult = desc.defineDoubleParam( kMult );
	Mult->setLabel			( "Multiplier" );
	Mult->setDefault		( 1.0 );
	Mult->setRange			( 0.0, std::numeric_limits<double>::max() );
	Mult->setDisplayRange		( 0.0, 1000.0 );
	Mult->setHint			( "Adjust the multiplier." );
//	Mult->setParent( pattanaik00Group );

	OFX::DoubleParamDescriptor* Rod = desc.defineDoubleParam( kRod );
	Rod->setLabel			( "Rod" );
	Rod->setDefault			( 0.5 );
	Rod->setRange			( 0.0, std::numeric_limits<double>::max() );
	Rod->setDisplayRange		( 0.0, 1.0 );
	Rod->setHint			( "Adjust the rod value." );
//	Rod->setParent( pattanaik00Group );

	OFX::DoubleParamDescriptor* Cone = desc.defineDoubleParam( kCone );
	Cone->setLabel			( "Cone" );
	Cone->setDefault		( 0.5 );
	Cone->setRange			( 0.0, std::numeric_limits<double>::max() );
	Cone->setDisplayRange		( 0.0, 1.0 );
	Cone->setHint			( "Adjust the cone value." );
//	Cone->setParent( pattanaik00Group );

	OFX::BooleanParamDescriptor* local = desc.defineBooleanParam( kParamProcessLocal );
	local->setLabel( "Process local" );
	local->setHint( "Apply the local algorithm." );
	local->setDefault( false );
//	local->setParent( pattanaik00Group );

	OFX::BooleanParamDescriptor* autoconerod = desc.defineBooleanParam( kParamAutoConeRod );
	autoconerod->setLabel( "Process Auto Cone/Rod" );
	autoconerod->setHint( "Automatic computing of the rod and cone values" );
	autoconerod->setDefault( false );
//	autoconerod->setParent( pattanaik00Group );

//durand02
	OFX::DoubleParamDescriptor* BaseContrast = desc.defineDoubleParam( kBaseContrast );
	BaseContrast->setLabel			( "Base Contrast" );
	BaseContrast->setDefault		( 5.0 );
	BaseContrast->setRange			( 0.0, std::numeric_limits<double>::max() );
	BaseContrast->setDisplayRange		( 0.0, 10.0 );
	BaseContrast->setHint			( "Adjust the base contrast." );
//	BaseContrast->setParent( durand02Group );

	OFX::DoubleParamDescriptor* SpatialKernelSigma = desc.defineDoubleParam( kSpatialKernelSigma );
	SpatialKernelSigma->setLabel		( "Spatial Kernel Sigma" );
	SpatialKernelSigma->setDefault		( 2.0 );
	SpatialKernelSigma->setRange		( 0.0, std::numeric_limits<double>::max() );
	SpatialKernelSigma->setDisplayRange	( 0.0, 100.0 );
	SpatialKernelSigma->setHint		( "Adjust the Spatial Kernel Sigma." );
//	SpatialKernelSigma->setParent( durand02Group );

	OFX::DoubleParamDescriptor* RangeKernelSigma = desc.defineDoubleParam( kRangeKernelSigma );
	RangeKernelSigma->setLabel		( "Range Kernel Sigma" );
	RangeKernelSigma->setDefault		( 2.0 );
	RangeKernelSigma->setRange		( 0.0, std::numeric_limits<double>::max() );
	RangeKernelSigma->setDisplayRange	( 0.0, 10.0 );
	RangeKernelSigma->setHint		( "Adjust the Range Kernel Sigma." );
//	RangeKernelSigma->setParent( durand02Group );

	OFX::PushButtonParamDescriptor* bydefault = desc.definePushButtonParam( kParamDefault );
	bydefault->setLabel( "Default" );
	

/*	OFX::ChoiceParamDescriptor* in = desc.defineChoiceParam( kParamIn );
	in->setLabel( "In" );
	in->setHint( "Input color gradation." );
	in->appendOption( kParamGradation_linear );
	in->appendOption( kParamGradation_sRGB );
	in->appendOption( kParamGradation_cineon );
	in->appendOption( kParamGradation_gamma );
	in->appendOption( kParamGradation_panalog );
	in->appendOption( kParamGradation_REDLog );
	in->appendOption( kParamGradation_ViperLog );
	in->appendOption( kParamGradation_REDSpace );
	in->appendOption( kParamGradation_AlexaLogC );

	//	in->appendOption( kParamGradation_rec709 );
	//	in->appendOption( kParamGradation_rec601 );
	in->setDefault( eParamGradation_linear );

	OFX::DoubleParamDescriptor* inGammaValue = desc.defineDoubleParam( kColorSpaceInGammaValue );
	inGammaValue->setLabel			( "Gamma" );
	inGammaValue->setDefault		( 1.0 );
	inGammaValue->setRange			( 0.0, std::numeric_limits<double>::max() );
	inGammaValue->setDisplayRange		( 0.0, 5.0 );
	inGammaValue->setHint			( "Adjust the Gamma." );

	OFX::DoubleParamDescriptor* inBlackPoint = desc.defineDoubleParam( kColorSpaceInBlackPoint );
	inBlackPoint->setLabel			( "Black Point" );
	inBlackPoint->setDefault		( 0.0 );
	inBlackPoint->setRange			( 0.0, 1.0 );
	inBlackPoint->setDisplayRange		( 0.0, 1.0 );
	inBlackPoint->setHint			( "Adjust the Black Point." );

	OFX::DoubleParamDescriptor* inWhitePoint = desc.defineDoubleParam( kColorSpaceInWhitePoint );
	inWhitePoint->setLabel			( "White Point" );
	inWhitePoint->setDefault		( 1.0 );
	inWhitePoint->setRange			( 0.0, 1.0 );
	inWhitePoint->setDisplayRange		( 0.0, 1.0 );
	inWhitePoint->setHint			( "Adjust the White Point." );

	OFX::DoubleParamDescriptor* inGammaSensito = desc.defineDoubleParam( kColorSpaceInGammaSensito );
	inGammaSensito->setLabel		( "Gamma Sensito" );
	inGammaSensito->setDefault		( 1.0 );
	inGammaSensito->setRange		( 0.0, std::numeric_limits<double>::max() );
	inGammaSensito->setDisplayRange		( 0.0, 5.0 );
	inGammaSensito->setHint			( "Adjust the Gamma Sensito." );


	OFX::ChoiceParamDescriptor* out = desc.defineChoiceParam( kParamOut );
	out->setLabel( "Out" );
	out->setHint( "Output color gradation." );
	out->appendOption( kParamGradation_linear );
	out->appendOption( kParamGradation_sRGB );
	out->appendOption( kParamGradation_cineon );
	out->appendOption( kParamGradation_gamma );
	out->appendOption( kParamGradation_panalog );
	out->appendOption( kParamGradation_REDLog );
	out->appendOption( kParamGradation_ViperLog );
	out->appendOption( kParamGradation_REDSpace );
	out->appendOption( kParamGradation_AlexaLogC );

	//	out->appendOption( kParamGradation_rec709 );
	//	out->appendOption( kParamGradation_rec601 );
	out->setDefault( eParamGradation_linear );

	OFX::DoubleParamDescriptor* outGammaValue = desc.defineDoubleParam( kColorSpaceOutGammaValue );
	outGammaValue->setLabel			( "Gamma" );
	outGammaValue->setDefault		( 1.0 );
	outGammaValue->setRange			( 0.0, std::numeric_limits<double>::max() );
	outGammaValue->setDisplayRange		( 0.0, 5.0 );
	outGammaValue->setHint			( "Adjust the Gamma." );

	OFX::DoubleParamDescriptor* outBlackPoint = desc.defineDoubleParam( kColorSpaceOutBlackPoint );
	outBlackPoint->setLabel			( "Black Point" );
	outBlackPoint->setDefault		( 0.0 );
	outBlackPoint->setRange			( 0.0, 1.0 );
	outBlackPoint->setDisplayRange		( 0.0, 1.0 );
	outBlackPoint->setHint			( "Adjust the Black Point." );

	OFX::DoubleParamDescriptor* outWhitePoint = desc.defineDoubleParam( kColorSpaceOutWhitePoint );
	outWhitePoint->setLabel			( "White Point" );
	outWhitePoint->setDefault		( 1.0 );
	outWhitePoint->setRange			( 0.0, 1.0 );
	outWhitePoint->setDisplayRange		( 0.0, 1.0 );
	outWhitePoint->setHint			( "Adjust the White Point." );

	OFX::DoubleParamDescriptor* outGammaSensito = desc.defineDoubleParam( kColorSpaceOutGammaSensito );
	outGammaSensito->setLabel		( "Gamma Sensito" );
	outGammaSensito->setDefault		( 1.0 );
	outGammaSensito->setRange		( 0.0, std::numeric_limits<double>::max() );
	outGammaSensito->setDisplayRange	( 0.0, 5.0 );
	outGammaSensito->setHint		( "Adjust the Gamma Sensito." );


	OFX::BooleanParamDescriptor* alpha = desc.defineBooleanParam( kParamProcessAlpha );
	alpha->setLabel( "Process alpha" );
	alpha->setHint( "Apply the conversion on alpha channel." );
	alpha->setDefault( false );

	OFX::PushButtonParamDescriptor* invert = desc.definePushButtonParam( kParamInvert );
	invert->setLabel( "Invert" );*/
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


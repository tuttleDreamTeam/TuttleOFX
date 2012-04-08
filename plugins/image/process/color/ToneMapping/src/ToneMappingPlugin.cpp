#include "ToneMappingPlugin.hpp"
#include "ToneMappingProcess.hpp"
#include "ToneMappingDefinitions.hpp"


#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

ToneMappingPlugin::ToneMappingPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramToneOperator    = fetchChoiceParam( kParamToneOperator );
/*
	_drago03Group = fetchGroupParam( kParamdrago03Group );
	_pattanaik00Group = fetchGroupParam( kParampattanaik00Group );
	_durand02Group = fetchGroupParam( kParamdurand02Group );
*/
//drago03
	_paramBias         = fetchDoubleParam( kBias    ); 
//pattanaik00
	_paramMult         = fetchDoubleParam( kMult    );
	_paramRod          = fetchDoubleParam( kRod    );
	_paramCone         = fetchDoubleParam( kCone    );

	_paramProcessLocal    = fetchBooleanParam( kParamProcessLocal );
	_paramAutoConeRod     = fetchBooleanParam( kParamAutoConeRod );
//durand02
	_paramBaseContrast         = fetchDoubleParam( kBaseContrast   );
	_paramSpatialKernelSigma   = fetchDoubleParam( kSpatialKernelSigma   );
	_paramRangeKernelSigma   = fetchDoubleParam( kRangeKernelSigma   );


/*	_paramIn              = fetchChoiceParam( kParamIn );
	_paramOut             = fetchChoiceParam( kParamOut );

	_paramInGamma         = fetchDoubleParam( kColorSpaceInGammaValue    );
	_paramOutGamma        = fetchDoubleParam( kColorSpaceOutGammaValue   );
	_paramInBlackPoint    = fetchDoubleParam( kColorSpaceInBlackPoint    );
	_paramOutBlackPoint   = fetchDoubleParam( kColorSpaceOutBlackPoint   );
	_paramInWhitePoint    = fetchDoubleParam( kColorSpaceInWhitePoint    );
	_paramOutWhitePoint   = fetchDoubleParam( kColorSpaceOutWhitePoint   );
	_paramInGammaSensito  = fetchDoubleParam( kColorSpaceInGammaSensito  );
	_paramOutGammaSensito = fetchDoubleParam( kColorSpaceOutGammaSensito );

	_paramProcessAlpha    = fetchBooleanParam( kParamProcessAlpha );*/
	updateParameters();
}

ToneMappingProcessParams<ToneMappingPlugin::Scalar> ToneMappingPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ToneMappingProcessParams<Scalar> params;
	params._toneoperator  = static_cast<EParamToneOperator>( _paramToneOperator->getValue() );
//drago03
	params._Bias    	= _paramBias        	->getValue();
//pattanaik00
	params._Mult    	= _paramMult        	->getValue();
	params._Rod	    	= _paramRod        	->getValue();
	params._Cone    	= _paramCone        	->getValue();

	params._processLocal    = _paramProcessLocal    ->getValue();
	params._autoConeRod     = _paramAutoConeRod     ->getValue();
//durand02
	params._BaseContrast   	= _paramBaseContrast    ->getValue();
	params._SpatialKernelSigma   	= _paramSpatialKernelSigma     ->getValue();
	params._RangeKernelSigma   	= _paramRangeKernelSigma     ->getValue();
	return params;
}

void ToneMappingPlugin::updateParameters()
{
	_paramBias        ->setIsSecretAndDisabled( true );
	_paramMult        ->setIsSecretAndDisabled( true );
	_paramRod         ->setIsSecretAndDisabled( true );
	_paramCone        ->setIsSecretAndDisabled( true );
	_paramProcessLocal	->setIsSecretAndDisabled( true );
	_paramAutoConeRod	->setIsSecretAndDisabled( true );
	_paramBaseContrast	->setIsSecretAndDisabled( true );
	_paramSpatialKernelSigma->setIsSecretAndDisabled( true );
	_paramRangeKernelSigma	->setIsSecretAndDisabled( true );

	switch( _paramToneOperator->getValue( ) )
	{
		case 0: // drago03
			_paramBias->setIsSecretAndDisabled( false );
			break;
		case 1: // pattanaik00
			_paramMult->setIsSecretAndDisabled( false );
			_paramRod->setIsSecretAndDisabled( false );
			_paramCone->setIsSecretAndDisabled( false );
			_paramProcessLocal->setIsSecretAndDisabled( false );
			_paramAutoConeRod->setIsSecretAndDisabled( false );
			break;
		case 2: // durand02
			_paramBaseContrast->setIsSecretAndDisabled( false );
			_paramSpatialKernelSigma->setIsSecretAndDisabled( false );
			_paramRangeKernelSigma	->setIsSecretAndDisabled( false );
			break;
		default:
			break;
	}
}


void ToneMappingPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamDefault )
	{
		switch( _paramToneOperator->getValue( ) )
			{
				case 0: // drago03
					_paramBias->setValue( 0.85 );
					break;
				case 1: // pattanaik00
					_paramMult->setValue( 1.0 );
					_paramRod->setValue( 0.5 );
					_paramCone->setValue( 0.5 );
					_paramProcessLocal->setValue( false );
					_paramAutoConeRod->setValue( false );
					break;
				case 2: // durand02
					_paramBaseContrast->setValue( 5.0 );
					_paramSpatialKernelSigma->setValue( 2.0 );
					_paramRangeKernelSigma->setValue( 2.0 );
					break;/**/
				default:
					break;
			}	
	}/**/
	updateParameters();
}

bool ToneMappingPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	/*
	ColorGradationProcessParams<Scalar> params = getProcessParams();
	if( params._in == params._out )
	{
		if( ( params._in == 2 ) &&
		    ( params._GammaValueIn != params._GammaValueOut ) )
			return false;
		if( ( params._in == 3 ) &&
		    ( params._BlackPointIn != params._BlackPointOut ) &&
		    ( params._WhitePointIn != params._WhitePointOut ) &&
		    ( params._GammaSensitoIn != params._GammaSensitoOut ) )
			return false;
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
	*/
	return false; // by default, we are not an identity operation
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */

/*void ToneMappingPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<ToneMappingProcess>( *this, args );
}*/
void ToneMappingPlugin::render( const OFX::RenderArguments &args )
{
        // instantiate the render code based on the pixel depth of the dst clip
        OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
        OFX::EPixelComponent components = _clipDst->getPixelComponents();
        switch( components )
        {
                // cas du RGBA, a choisir ceux que vous voulez
                // et dis à gil d'utiliser uniquement les vues rgba32f_view_t, rgba32_view_t rgb16_view_t et rgb8_view_t
                // et la fonction multiThreadProcessImages (dans le .tcc) sera compilé uniquement avec ces templates
                case OFX::ePixelComponentRGBA:
                {
                        doGilRender<ToneMappingProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
                        return;
                }
                case OFX::ePixelComponentRGB:
		{
                        doGilRender<ToneMappingProcess, false, boost::gil::rgb_layout_t>( *this, args, bitDepth );
                        return;
                }
                case OFX::ePixelComponentAlpha:
                case OFX::ePixelComponentCustom:
                case OFX::ePixelComponentNone:
                {
                        BOOST_THROW_EXCEPTION( exception::Unsupported()
                                << exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
                }
        }
        BOOST_THROW_EXCEPTION( exception::Unknown() );
}

}
}
}

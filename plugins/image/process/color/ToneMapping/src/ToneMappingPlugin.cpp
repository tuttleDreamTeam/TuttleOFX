#include "ToneMappingPlugin.hpp"
#include "ToneMappingProcess.hpp"
#include "ToneMappingDefinitions.hpp"

//#include "ToneMappingPluginFactory.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

ToneMappingPlugin::ToneMappingPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramToneOperator    = fetchChoiceParam( kParamToneOperator );

	_drago03Group = fetchGroupParam( kParamdrago03Group );
	_pattanaik00Group = fetchGroupParam( kParampattanaik00Group );
	_reinhard05Group = fetchGroupParam( kParamreinhard05Group );
	_reinhard05Group = fetchGroupParam( kParamreinhard02Group );
	_durand02Group = fetchGroupParam( kParamdurand02Group );
	_fattal02Group = fetchGroupParam( kParamfattal02Group );
/**/
//drago03
	_paramBias         = fetchDoubleParam( kBias    ); 
//pattanaik00
	_paramMult         = fetchDoubleParam( kMult    );
	_paramRod          = fetchDoubleParam( kRod    );
	_paramCone         = fetchDoubleParam( kCone    );

	_paramProcessLocal    = fetchBooleanParam( kParamProcessLocal );
	_paramAutoConeRod     = fetchBooleanParam( kParamAutoConeRod );
	
//reinhard05
	_paramBrightness        = fetchDoubleParam( kBrightness); 
	_paramChromaticAdaptation        = fetchDoubleParam( kChromaticAdaptation); 
	_paramLightAdaptation        = fetchDoubleParam( kLightAdaptation); 
	
//reinhard02
	_paramKey        = fetchDoubleParam( kKey); 
	_paramPhi        = fetchDoubleParam( kPhi); 
	
//durand02
	_paramBaseContrast         = fetchDoubleParam( kBaseContrast   );
	_paramSpatialKernelSigma   = fetchDoubleParam( kSpatialKernelSigma   );
	_paramRangeKernelSigma   = fetchDoubleParam( kRangeKernelSigma   );
	
//fattal02
	_paramAlpha        = fetchDoubleParam( kAlpha   );
	_paramBeta  = fetchDoubleParam( kBeta   );
	_paramColorSaturation  = fetchDoubleParam( kColorSaturation  );
	_paramNoiseReduction  = fetchDoubleParam( kNoiseReduction  );
	
	
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
	
//reinhard05
	params._Brightness	        = _paramBrightness	        ->getValue();
	params._ChromaticAdaptation	= _paramChromaticAdaptation	->getValue();
	params._LightAdaptation  	= _paramLightAdaptation	        ->getValue();

//reinhard02
	params._key	        = _paramKey	        ->getValue();
	params._phi		= _paramPhi		->getValue();
	
//durand02
	params._BaseContrast   	        = _paramBaseContrast           ->getValue();
	params._SpatialKernelSigma   	= _paramSpatialKernelSigma     ->getValue();
	params._RangeKernelSigma   	= _paramRangeKernelSigma       ->getValue();
	
	
	
//fattal2
	params._Alpha  	        	= _paramAlpha  			->getValue();
	params._Beta   			= _paramBeta     		->getValue();
	params._ColorSaturation 	= _paramColorSaturation       	->getValue();
	params._NoiseReduction  	= _paramNoiseReduction       	->getValue();


	return params;
}

void ToneMappingPlugin::updateParameters()
{
	_paramBias        ->setIsSecretAndDisabled( true );
	
	_paramMult        ->setIsSecretAndDisabled( true );
	_paramRod         ->setIsSecretAndDisabled( true );
	_paramCone        ->setIsSecretAndDisabled( true );
	_paramProcessLocal	   ->setIsSecretAndDisabled( true );
	_paramAutoConeRod	   ->setIsSecretAndDisabled( true );
	
	_paramBrightness           ->setIsSecretAndDisabled( true );
	_paramChromaticAdaptation  ->setIsSecretAndDisabled( true );
	_paramLightAdaptation      ->setIsSecretAndDisabled( true );

	_paramKey        ->setIsSecretAndDisabled( true );
	_paramPhi       ->setIsSecretAndDisabled( true );
	
	_paramBaseContrast	   ->setIsSecretAndDisabled( true );
	_paramSpatialKernelSigma  ->setIsSecretAndDisabled( true );
	_paramRangeKernelSigma	   ->setIsSecretAndDisabled( true );
	
	_paramAlpha	  	->setIsSecretAndDisabled( true );
	_paramBeta 		->setIsSecretAndDisabled( true );
	_paramColorSaturation	->setIsSecretAndDisabled( true );
	_paramNoiseReduction	->setIsSecretAndDisabled( true );

//OFX Groups
	//drago03Group->setOpen( false );
	//pattanaik00Group->setOpen(false );
	//durand02Group->setOpen( false );
	
	switch( _paramToneOperator->getValue( ) )
	{
		case 0: // drago03
			_paramBias->setIsSecretAndDisabled( false );
			//drago03Group->setOpen( true );
			break;
		case 1: // pattanaik00
			_paramMult->setIsSecretAndDisabled( false );
			_paramRod->setIsSecretAndDisabled( false );
			_paramCone->setIsSecretAndDisabled( false );
			_paramProcessLocal->setIsSecretAndDisabled( false );
			_paramAutoConeRod->setIsSecretAndDisabled( false );
			//pattanaik00Group->setOpen( true );
			break;
		case 2: //reinhard05
			_paramBrightness->setIsSecretAndDisabled( false );
			_paramChromaticAdaptation->setIsSecretAndDisabled( false );
			_paramLightAdaptation->setIsSecretAndDisabled( false );
			//reinhard05Group->setOpen( true )
			 break;
		case 3://reinhard02
			_paramKey        ->setIsSecretAndDisabled( false );
			_paramPhi       ->setIsSecretAndDisabled( false );		  
		  break;
		case 4: // durand02
			_paramBaseContrast->setIsSecretAndDisabled( false );
			_paramSpatialKernelSigma->setIsSecretAndDisabled( false );
			_paramRangeKernelSigma->setIsSecretAndDisabled( false );
			//durand02Group->setOpen( true );
			break;
		case 5: //fattal02
			_paramAlpha->setIsSecretAndDisabled( false );
			_paramBeta->setIsSecretAndDisabled( false );
			_paramColorSaturation->setIsSecretAndDisabled( false );
			_paramNoiseReduction->setIsSecretAndDisabled( false );
		  
		  
		  break;
		case 6: break;
		case 7: break;
		default:
			break;
	}
}


void ToneMappingPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamDefault )
	{
		TUTTLE_COUT("pass here: default...");
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
				case 2: //reinhard05
					_paramBrightness->setValue( 0.0 );
					_paramChromaticAdaptation->setValue( 0.0 );
					_paramLightAdaptation->setValue( 0.0 );
					break;
				case 3: //reinhard02
					_paramKey->setValue( 0.18 );
					_paramPhi->setValue( 1.0 );
				  break;
				case 4: // durand02
					_paramBaseContrast->setValue( 5.0 );
					_paramSpatialKernelSigma->setValue( 2.0 );
					_paramRangeKernelSigma->setValue( 2.0 );
					break;/**/
				case 5: // fattal02
					_paramAlpha->setValue( 1.0 );
					_paramBeta->setValue( 0.9);
					_paramColorSaturation->setValue( 0.8 );
					_paramNoiseReduction->setValue( 0.0 );
				  
				  break;
				  
				case 6: break;
				case 7: break;
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
                case OFX::ePixelComponentRGB:
		{
                        doGilRender<ToneMappingProcess, false, boost::gil::rgb_layout_t>( *this, args, bitDepth );
                        return;
                }
		case OFX::ePixelComponentRGBA:
                {
                        doGilRender<ToneMappingProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
                        return;
                }/**/
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

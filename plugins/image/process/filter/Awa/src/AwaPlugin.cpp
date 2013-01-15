#include "AwaPlugin.hpp"
#include "AwaProcess.hpp"
#include "AwaDefinitions.hpp"

#include <terry/point/operations.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace awa {
  
  static const bool kSupportTiles = false;

AwaPlugin::AwaPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramAlpha   = fetchDoubleParam( kParamAlpha );
	_paramEpsilonR   = fetchDoubleParam( kParamEpsilonR );
	_paramEpsilonG   = fetchDoubleParam( kParamEpsilonG );
	_paramEpsilonB   = fetchDoubleParam( kParamEpsilonB );
	
	_paramNoiseEstimation = fetchBooleanParam( kParamNoiseEstimation );
  
	_paramSize   = fetchDouble2DParam( kParamSize );
	_paramBorder = fetchChoiceParam( kParamBorder );
	_paramNormalizedKernel = fetchBooleanParam( kParamNormalizedKernel );
	_paramKernelEpsilon = fetchDoubleParam( kParamKernelEpsilon );
	
	_paramEpsilonR        ->setIsSecretAndDisabled( _paramNoiseEstimation->getValue() );
	_paramEpsilonG        ->setIsSecretAndDisabled( _paramNoiseEstimation->getValue() );
	_paramEpsilonB        ->setIsSecretAndDisabled( _paramNoiseEstimation->getValue() );
}

AwaProcessParams<AwaPlugin::Scalar> AwaPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	using namespace terry::filter;
	
	AwaProcessParams<Scalar> params;
	
	params._alpha   = _paramAlpha->getValue();
	params._epsilonR = _paramEpsilonR->getValue();
	params._epsilonG = _paramEpsilonG->getValue();
	params._epsilonB = _paramEpsilonB->getValue();
	
	params._noiseEstimation = _paramNoiseEstimation->getValue();
	
	params._size   = ofxToGil( _paramSize->getValue() ) * ofxToGil( renderScale  );
	params._border = static_cast<EParamBorder>( _paramBorder->getValue() );

	const bool normalizedKernel = _paramNormalizedKernel->getValue();
	const double kernelEpsilon = _paramKernelEpsilon->getValue();

	params._gilKernelX = buildGaussian1DKernel<Scalar>( params._size.x, normalizedKernel, kernelEpsilon );
	params._gilKernelY = buildGaussian1DKernel<Scalar>( params._size.y, normalizedKernel, kernelEpsilon );
	
	params._boundary_option = convolve_option_extend_mirror;
	switch( params._border )
	{
		case eParamBorderMirror:
			params._boundary_option = convolve_option_extend_mirror;
			break;
		case eParamBorderConstant:
			params._boundary_option = convolve_option_extend_constant;
			break;
		case eParamBorderBlack:
			params._boundary_option = convolve_option_extend_zero;
			break;
		case eParamBorderPadded:
			params._boundary_option = convolve_option_extend_padded;
			break;
	}
	return params;
}


   void AwaPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
   {
      if( paramName == kParamNoiseEstimation )
      {
	_paramEpsilonR        ->setIsSecretAndDisabled( _paramNoiseEstimation->getValue() );
	_paramEpsilonG        ->setIsSecretAndDisabled( _paramNoiseEstimation->getValue() );
	_paramEpsilonB        ->setIsSecretAndDisabled( _paramNoiseEstimation->getValue() );
      }
//     if( paramName == kHelpButton )
//     {
//         sendMessage( OFX::Message::eMessageMessage,
//                      "", // No XML resources
//                      kHelpString );
//     }
   }
 

bool AwaPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	AwaProcessParams<Scalar> params = getProcessParams();

	switch( params._border )
	{
		case eParamBorderPadded:
			rod.x1 = srcRod.x1 + params._gilKernelX.left_size();
			rod.y1 = srcRod.y1 + params._gilKernelY.left_size();
			rod.x2 = srcRod.x2 - params._gilKernelX.right_size();
			rod.y2 = srcRod.y2 - params._gilKernelY.right_size();
			return true;
		default:
			break;
	}
	return false;
}

void AwaPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	AwaProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod                  = _clipSrc->getCanonicalRod( args.time );

	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - params._gilKernelX.left_size();
	srcRoi.y1 = srcRod.y1 - params._gilKernelY.left_size();
	srcRoi.x2 = srcRod.x2 + params._gilKernelX.right_size();
	srcRoi.y2 = srcRod.y2 + params._gilKernelY.right_size();
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}

bool AwaPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	AwaProcessParams<Scalar> params = getProcessParams( args.renderScale );
	if( params._size.x != 0 || params._size.y != 0 )
		return false;

	identityClip = _clipSrc;
	identityTime = args.time;
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AwaPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<AwaProcess>( *this, args );
}

}
}
}

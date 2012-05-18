#ifndef _TUTTLE_PLUGIN_TONEMAPPING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TONEMAPPING_PLUGIN_HPP_

#include "ToneMappingDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

template<typename Scalar>
struct ToneMappingProcessParams
{
	EParamToneOperator   _toneoperator;	

//drago03
	double            _Bias;
//pattanaik00
	double            _Mult;
	double            _Rod;
	double            _Cone;

	bool              _processLocal;
	bool              _autoConeRod;
	
//reinhard05
	double            _Brightness;
	double            _ChromaticAdaptation;
	double            _LightAdaptation;	
	
//reinhard02
	double            _key;
	double            _phi;
	
//durand02
	double            _BaseContrast;
	double            _SpatialKernelSigma;
	double            _RangeKernelSigma;

//fattal02
	double            _Alpha;
	double            _Beta;
	double            _NoiseReduction;
	
  
};

/**
 * @brief ToneMapping plugin
 */
class ToneMappingPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;

public:
	ToneMappingPlugin( OfxImageEffectHandle handle );

public:
	ToneMappingProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render( const OFX::RenderArguments& args );

private:
	void updateParameters();

public:
	OFX::ChoiceParam*       _paramToneOperator;

	OFX::GroupParam* 	_drago03Group;
	OFX::GroupParam* 	_pattanaik00Group;
	OFX::GroupParam* 	_reinhard05Group;
	OFX::GroupParam* 	_reinhard02Group;
	OFX::GroupParam* 	_durand02Group;
	OFX::GroupParam* 	_fattal02Group;

/**/
//drago03
	OFX::DoubleParam*       _paramBias;

//pattanaik00
	OFX::DoubleParam*       _paramMult;
	OFX::DoubleParam*       _paramRod;
	OFX::DoubleParam*       _paramCone;

	OFX::BooleanParam*      _paramProcessLocal;
	OFX::BooleanParam*      _paramAutoConeRod;
	
//reinhard05	
	OFX::DoubleParam*       _paramBrightness;
	OFX::DoubleParam*       _paramChromaticAdaptation;
	OFX::DoubleParam*       _paramLightAdaptation;
	
//reinhard02	
	OFX::DoubleParam*       _paramKey;
	OFX::DoubleParam*       _paramPhi;

//durand02
	OFX::DoubleParam*       _paramBaseContrast;
	OFX::DoubleParam*       _paramSpatialKernelSigma;
	OFX::DoubleParam*       _paramRangeKernelSigma;
	
//fattal02
	OFX::DoubleParam*       _paramAlpha;
	OFX::DoubleParam*       _paramBeta;
	OFX::DoubleParam*       _paramNoiseReduction;
};

}
}
}

#endif

#ifndef _TUTTLE_PLUGIN_TONEMAPPING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TONEMAPPING_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

const std::string kParamToneOperator  = "operator";

//a group of parameters for each algo ???
static const std::string kParamdrago03Group = "drago03Group";
static const std::string kParampattanaik00Group = "pattanaik00Group";
static const std::string kParamdurand02Group = "durand02Group";
/**/
const std::string kParamDefault              = "reset parameters with default values";

const std::string kParamToneOperator_drago03    = "Drago03";
const std::string kParamToneOperator_pattanaik00    = "Pattanaik00";
const std::string kParamToneOperator_reinhard04    = "Reinhard04";
const std::string kParamToneOperator_reinhard02    = "Reinhard02";
const std::string kParamToneOperator_durand02   = "Durand02";
const std::string kParamToneOperator_ashikhmin  = "Ashikhmin";
const std::string kParamToneOperator_fattal  = "Fattal";
const std::string kParamToneOperator_mantiuk   = "Mantiuk";

enum EParamToneOperator
{
	eParamToneOperator_drago03 = 0,
	eParamToneOperator_pattanaik00,
	eParamToneOperator_reinhard04,
	eParamToneOperator_reinhard02,
	eParamToneOperator_durand02,
	eParamToneOperator_ashikhmin,
	eParamToneOperator_fattal,
	eParamToneOperator_mantiuk,
};

//drago03
static const std::string kBias	= "Bias";
//pattanaik00
static const std::string kMult	= "Multiplier";
static const std::string kRod	= "Rod";
static const std::string kCone	= "Cone";

const std::string kParamProcessLocal	= "processLocal";
const std::string kParamAutoConeRod	= "autoConeRod";
//durand02
static const std::string kBaseContrast	= "BaseContrast";
static const std::string kSpatialKernelSigma	= "SpatialKernelSigma";
static const std::string kRangeKernelSigma	= "RangeKernelSigma";


}
}
}

#endif

#ifndef _TUTTLE_PLUGIN_TONEMAPPING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TONEMAPPING_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

const std::string kParamToneOperator  = "operator";

const std::string kParamToneOperator_drago03    = "Drago03";
const std::string kParamToneOperator_pattanaik00    = "Pattanaik00";
const std::string kParamToneOperator_durand02   = "Durand02";

enum EParamToneOperator
{
	eParamToneOperator_drago03 = 0,
	eParamToneOperator_pattanaik00,
	eParamToneOperator_durand02
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

/*
const std::string kParamIn  = "in";
const std::string kParamOut = "out";

const std::string kParamGradation_linear    = "Linear";
const std::string kParamGradation_sRGB      = "sRGB";
const std::string kParamGradation_cineon    = "Cineon";
const std::string kParamGradation_gamma     = "Gamma";
const std::string kParamGradation_panalog   = "Panalog";
const std::string kParamGradation_REDLog    = "REDLog";
const std::string kParamGradation_ViperLog  = "ViperLog";
const std::string kParamGradation_REDSpace  = "REDSpace";
const std::string kParamGradation_AlexaLogC = "AlexaLogC";

//const std::string kParamGradation_rec709  = "rec709";
//const std::string kParamGradation_rec601  = "rec601";
*/

/*
enum EParamGradation
{
	eParamGradation_linear = 0,
	eParamGradation_sRGB,
	eParamGradation_cineon,
	eParamGradation_gamma,
	eParamGradation_panalog,
	eParamGradation_REDLog,
	eParamGradation_ViperLog,
	eParamGradation_REDSpace,
	eParamGradation_AlexaLogC
	//	eParamGradation_rec709,
	//	eParamGradation_rec601
};

static const std::string kColorSpaceInGammaValue	= "inGammaValue";
static const std::string kColorSpaceInBlackPoint	= "inBlackPoint";
static const std::string kColorSpaceInWhitePoint	= "inWhitePoint";
static const std::string kColorSpaceInGammaSensito	= "inGammaSensito";

static const std::string kColorSpaceOutGammaValue	= "outGammaValue";
static const std::string kColorSpaceOutBlackPoint	= "outBlackPoint";
static const std::string kColorSpaceOutWhitePoint	= "outWhitePoint";
static const std::string kColorSpaceOutGammaSensito	= "outGammaSensito";

const std::string kParamProcessAlpha        = "processAlpha";
const std::string kParamInvert              = "invert";
*/
}
}
}

#endif

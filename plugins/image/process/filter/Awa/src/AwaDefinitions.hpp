#ifndef _TUTTLE_PLUGIN_AWA_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AWA_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace awa {

static const std::string kParamAlpha		= "alpha";  
static const std::string kParamEpsilonR	= "epsilonR";
static const std::string kParamEpsilonG	= "epsilonG"; 
static const std::string kParamEpsilonB	= "epsilonB"; 
 
const std::string kParamNoiseEstimation	= "noiseEstimation";

static const std::string kParamSize		= "size";
static const std::string kParamBorder		= "border";
static const std::string kParamBorderMirror	= "Mirror";
static const std::string kParamBorderConstant	= "Constant";
static const std::string kParamBorderBlack	= "Black";
static const std::string kParamBorderPadded	= "Padded";


enum EParamBorder
{
	eParamBorderMirror = 0,
	eParamBorderConstant,
	eParamBorderBlack,
	eParamBorderPadded
};

static const std::string kParamGroupAdvanced = "advanced";
static const std::string kParamNormalizedKernel = "normalizedKernel";
static const std::string kParamKernelEpsilon = "kernelEpsilon";

}
}
}

#endif

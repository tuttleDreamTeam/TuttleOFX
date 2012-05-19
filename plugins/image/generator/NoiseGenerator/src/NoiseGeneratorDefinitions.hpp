#ifndef _TUTTLE_PLUGIN_NOISEGENERATOR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_NOISEGENERATOR_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/GeneratorDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace noisegenerator {

static const std::string kNoiseGeneratorSigma 	= "sigma";
static const std::string kNoiseGeneratorNu 	= "nu";

const std::string kNoiseGeneratorColor 		= "Noise Color";
const std::string kNoiseGeneratorColorGrey 	= "Grey";
const std::string kNoiseGeneratorColorRGB 	= "RGB";

const std::string kParamDefault              	= "reset parameters with default values";

static const std::string kNoiseGeneratorRweight = "r_weight";
static const std::string kNoiseGeneratorGweight = "g_weight";
static const std::string kNoiseGeneratorBweight = "b_weight";
static const std::string kNoiseGeneratorBW 	= "bw_weight";


enum ENoiseGeneratorColor
{
	eNoiseGeneratorColorGrey = 0,
	eNoiseGeneratorColorRGB,
	
};

}
}
}

#endif

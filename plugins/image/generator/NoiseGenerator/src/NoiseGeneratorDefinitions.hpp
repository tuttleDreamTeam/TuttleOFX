#ifndef _TUTTLE_PLUGIN_NOISEGENERATOR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_NOISEGENERATOR_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/GeneratorDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace noisegenerator {



static const std::string kNoiseGeneratorSigma = "sigma";
static const std::string kNoiseGeneratorNu = "nu";

static const std::string kNoiseGeneratorColor = "Noise Color";
static const std::string kNoiseGeneratorColorGrey = "Grey";
static const std::string kNoiseGeneratorColorRGB = "RGB";

static const std::string kNoiseGeneratorRweight = "r_weight";
static const std::string kNoiseGeneratorGweight = "g_weight";
static const std::string kNoiseGeneratorBweight = "b_weight";
static const std::string kNoiseGeneratorBW = "bw_weight";
static const std::string kNoiseGeneratorNB = "nb";

enum ENoiseGeneratorColor
{
	eNoiseGeneratorColorGrey = 0,
	eNoiseGeneratorColorRGB,
	
};


}
}
}

#endif

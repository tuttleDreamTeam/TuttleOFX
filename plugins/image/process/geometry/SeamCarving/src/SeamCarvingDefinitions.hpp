#ifndef _TUTTLE_PLUGIN_SEAMCARVING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_SEAMCARVING_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ResolutionDefinition.hpp>
#include <tuttle/plugin/context/SamplerDefinition.hpp>


namespace tuttle {
namespace plugin {
namespace seamcarving {

static const std::string kClipMap        = "Map";
static const std::string kClipKeepMask   = "KeepMask";
static const std::string kClipDeleteMask = "DeleteMask";

static const std::string kParamSize = "size";
static const std::string kParamSizeLabel = "Size";

static const std::string kParamMap = "map";
static const std::string kParamMapLabel = "Show map";

static const std::string kParamSource = "src";
static const std::string kParamSourceLabel = "Show src";

}
}
}

#endif

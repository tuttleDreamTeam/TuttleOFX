#ifndef _TUTTLE_PLUGIN_NOISEGENERATOR_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_NOISEGENERATOR_PLUGIN_HPP_

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace noisegenerator {

/**
 * @brief
 */
class NoiseGeneratorPlugin : public GeneratorPlugin
{
public:
	NoiseGeneratorPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

public:

	OFX::RGBAParam* _color1;
	OFX::DoubleParam* _sigma;
	
};

}
}
}

#endif

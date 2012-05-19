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
	typedef float Scalar;
public:
	NoiseGeneratorPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

public:

	OFX::DoubleParam* _sigma;
	OFX::DoubleParam* _nu;

	OFX::ChoiceParam* _color;

	OFX::GroupParam* _color_grey;
	OFX::GroupParam* _color_rgb;

	OFX::DoubleParam* _r_weight;
	OFX::DoubleParam* _g_weight;
	OFX::DoubleParam* _b_weight;
	OFX::DoubleParam* _bw_weight;

};

}
}
}

#endif

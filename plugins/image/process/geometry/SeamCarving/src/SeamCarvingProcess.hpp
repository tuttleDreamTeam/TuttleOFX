#ifndef _TUTTLE_PLUGIN_SEAMCARVING_PROCESS_HPP_
#define _TUTTLE_PLUGIN_SEAMCARVING_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace seamcarving {

/**
 * @brief SeamCarving process
 *
 */
template<class View>
class SeamCarvingProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;

protected:
	SeamCarvingPlugin&			_plugin;	///< Rendering plugin
	SeamCarvingProcessParams<Scalar>	_params;	///< parameters

public:
	SeamCarvingProcess( SeamCarvingPlugin& effect );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "SeamCarvingProcess.tcc"

#endif

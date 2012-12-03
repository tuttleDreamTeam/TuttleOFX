#ifndef _TUTTLE_PLUGIN_AWA_PROCESS_HPP_
#define _TUTTLE_PLUGIN_AWA_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace awa {

/**
 * @brief Awa process
 *
 */
template<class View>
class AwaProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;
	typedef typename View::value_type Pixel;
	typedef typename View::point_t Point;
	typedef typename View::coord_t Coord;
	typedef typename terry::image_from_view<View>::type Image;

protected:
	AwaPlugin& _plugin; ///< Rendering plugin

	AwaProcessParams<Scalar> _params; ///< user parameters

public:
	AwaProcess( AwaPlugin& effect );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "AwaProcess.tcc"

#endif

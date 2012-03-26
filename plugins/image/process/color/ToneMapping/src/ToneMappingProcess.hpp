#ifndef _TUTTLE_PLUGIN_TONEMAPPING_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TONEMAPPING_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace toneMapping {

/**
 * @brief ToneMapping process
 *
 */
template<class View>
class ToneMappingProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;

protected:
	ToneMappingPlugin&               _plugin;        ///< Rendering plugin
	ToneMappingProcessParams<Scalar> _params;

public:
	ToneMappingProcess( ToneMappingPlugin& effect );
	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );	

	void test( const OfxRectI& procWindowRoW );
private:

};

}
}
}

#include "ToneMappingProcess.tcc"

#endif

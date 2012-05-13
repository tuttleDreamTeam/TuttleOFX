#ifndef _TUTTLE_PLUGIN_NOISEGENERATOR_PROCESS_HPP_
#define _TUTTLE_PLUGIN_NOISEGENERATOR_PROCESS_HPP_

#include <terry/globals.hpp>
#include <terry/generator/noisegenerator.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace noisegenerator {

template<class View>
struct NoiseGeneratorParams
{
	typedef typename View::value_type Pixel;
	

	float _sigma;
	float _nu;
	float _color;
	float _r_weight;
	float _g_weight;
	float _b_weight;
	float _bw_weight;
	float _nb;


typedef float Scalar;
	
};

template<class View>
class NoiseGeneratorProcess : public ImageGilProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef terry::generator::NoiseGeneratorFunctor<Pixel> NoiseGeneratorFunctorT;
	typedef typename NoiseGeneratorFunctorT::point_t Point;
	typedef boost::gil::virtual_2d_locator<NoiseGeneratorFunctorT, false> Locator;
	typedef boost::gil::image_view<Locator> NoiseGeneratorVirtualView;

protected:
	NoiseGeneratorPlugin&    _plugin;        ///< Rendering plugin
	NoiseGeneratorVirtualView _srcView;       ///< Source view

public:
	NoiseGeneratorProcess( NoiseGeneratorPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	NoiseGeneratorParams<View> getParams();

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "NoiseGeneratorProcess.tcc"

#endif

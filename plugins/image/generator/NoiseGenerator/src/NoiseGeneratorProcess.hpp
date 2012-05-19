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
	NoiseGeneratorPlugin&    	_plugin;        ///< Rendering plugin
	NoiseGeneratorVirtualView 	_srcView;       ///< Source view

public:
	NoiseGeneratorProcess( NoiseGeneratorPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	NoiseGeneratorParams<View> getParams();
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
public:
	OFX::ChoiceParam*       _color;

	OFX::GroupParam* 	_color_grey;
	OFX::GroupParam* 	_color_rgb;

	OFX::DoubleParam*       _nu;
	OFX::DoubleParam*       _sigma;
	OFX::DoubleParam*       _r_weight;
	OFX::DoubleParam*       _g_weight;
	OFX::DoubleParam*       _b_weight;
	OFX::DoubleParam*       _bw_weight;

private:
	void updateParameters();	
};

}
}
}

#include "NoiseGeneratorProcess.tcc"

#endif

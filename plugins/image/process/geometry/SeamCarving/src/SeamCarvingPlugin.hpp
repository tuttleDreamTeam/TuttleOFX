#ifndef _TUTTLE_PLUGIN_SEAMCARVING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_SEAMCARVING_PLUGIN_HPP_

#include "SeamCarvingDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/plugin/context/SamplerPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace seamcarving {

template<typename Scalar>
struct SeamCarvingProcessParams
{;
	boost::gil::point2<Scalar>                 _outputSize;
	OFX::Clip* _clipMap;
};

/**
 * @brief SeamCarving plugin
 */
class SeamCarvingPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
	typedef boost::gil::point2<double> Point2;
	OFX::Clip* _mapClip;
	

public:
	SeamCarvingPlugin( OfxImageEffectHandle handle );

public:
	SeamCarvingProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;


	void changedParam          ( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getRegionOfDefinition ( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest  ( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity            ( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render                ( const OFX::RenderArguments &args );

public:
	OFX::Int2DParam*        _paramSize;
};

}
}
}

#endif

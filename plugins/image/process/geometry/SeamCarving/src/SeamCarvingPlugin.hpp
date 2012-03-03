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
{
	bool                                       _changeCenter;
	boost::gil::point2<Scalar>                 _centerPoint;

	SamplerProcessParams                       _samplerProcessParams;
};

/**
 * @brief SeamCarving plugin
 */
class SeamCarvingPlugin : public SamplerPlugin
{
public:
	typedef float Scalar;
	typedef boost::gil::point2<double> Point2;

public:
	SeamCarvingPlugin( OfxImageEffectHandle handle );

public:
	SeamCarvingProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void updateVisibleTools();

	void changedParam          ( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getRegionOfDefinition ( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest  ( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity            ( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render                ( const OFX::RenderArguments &args );

public:
	OFX::ChoiceParam*       _paramMode;

	OFX::ChoiceParam*       _paramFormat;

	OFX::Int2DParam*        _paramSize;
	OFX::IntParam*          _paramSizeWidth;
	OFX::IntParam*          _paramSizeHeight;
	OFX::BooleanParam*      _paramSizeKeepRatio;
	OFX::ChoiceParam*       _paramSizeOrientation;
	
	OFX::Double2DParam*     _paramScale;

	OFX::BooleanParam*      _paramCenter;
	OFX::Double2DParam*     _paramCenterPoint;
};

}
}
}

#endif

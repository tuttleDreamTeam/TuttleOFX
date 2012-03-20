#include "SeamCarvingPlugin.hpp"
#include "SeamCarvingProcess.hpp"
#include "SeamCarvingDefinitions.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <terry/sampler/sampler.hpp>
#include <terry/point/operations.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace seamcarving {

using namespace ::terry::sampler;
using boost::numeric_cast;

SeamCarvingPlugin::SeamCarvingPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramSize            = fetchInt2DParam    ( kParamSize );

}


SeamCarvingProcessParams<SeamCarvingPlugin::Scalar> SeamCarvingPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	SeamCarvingProcessParams<Scalar> params;

	return params;
}

void SeamCarvingPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	
}

bool SeamCarvingPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace boost::gil;

	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	const Point2   srcRodSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );
//	const OfxRectD srcRodInDstFrame = { 0, 0, srcRodSize.x, srcRodSize.y };

	//OfxPointD centerPoint = _paramCenterPoint->getValue();

	//TUTTLE_COUT( centerPoint.x << " x " << centerPoint.y );

	SeamCarvingProcessParams<Scalar> params = getProcessParams();
	
//	TUTTLE_COUT( rod.x1 << ", " << rod.y1 << " || " << rod.x2 << ", " << rod.y2 );

  
			const OfxPointI s = _paramSize->getValue();
			
			rod.x1 = 0;
			rod.y1 = 0;
			rod.x2 = s.x;
			rod.y2 = s.y;

			return true;

}

void SeamCarvingPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	/*OfxRectD srcRoi;
	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
	srcRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
*/
	/*SeamCarvingProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
        srcwRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );*/
	/*
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	OfxRectD dstRod = _clipDst->getCanonicalRod( args.time );

	SeamCarvingProcessParams<Scalar> params = getProcessParams();

	OfxRectD outputRoi = args.regionOfInterest;
	outputRoi.x1 -= dstRod.x1; // to dest rod coordinates
	outputRoi.y1 -= dstRod.y1;
	outputRoi.x2 -= dstRod.x1;
	outputRoi.y2 -= dstRod.y1;
	OfxRectD srcRoi = ;
	srcRoi.x1    += srcRod.x1; // to RoW coordinates
	srcRoi.y1    += srcRod.y1;
	srcRoi.x2    += srcRod.x1;
	srcRoi.y2    += srcRod.y1;
	outputRoi.x1 += dstRod.x1; // to RoW coordinates
	outputRoi.y1 += dstRod.y1;
	outputRoi.x2 += dstRod.x1;
	outputRoi.y2 += dstRod.y1;
	//    srcRoi.x1 += 2; // if we remove 2 pixels to the needed RoI the plugin crash, because it tries to access to this pixels
	//    srcRoi.y1 += 2; // so the calcul of the RoI has a precision of one pixel
	//    srcRoi.x2 -= 2;
	//    srcRoi.y2 -= 2;
	OfxRectD srcRealRoi = rectanglesIntersection( srcRoi, srcRod );
	srcRealRoi = srcRod;*/

	//rois.setRegionOfInterest( *_clipSrc, outputRoi );
/*
	if( _debugDisplayRoi->getValue() )
	{
		_srcRealRoi = srcRealRoi;
		_srcRoi     = srcRoi;
		_dstRoi     = outputRoi;
	}*/
}

bool SeamCarvingPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	SeamCarvingProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void SeamCarvingPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<SeamCarvingProcess>( *this, args );
}


}
}
}

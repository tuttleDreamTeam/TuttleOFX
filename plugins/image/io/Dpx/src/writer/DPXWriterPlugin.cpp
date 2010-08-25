#include "DPXWriterPlugin.hpp"
#include "DPXWriterProcess.hpp"
#include "DPXWriterDefinitions.hpp"

#include <dpxEngine/dpxImage.hpp>

#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

DPXWriterPlugin::DPXWriterPlugin( OfxImageEffectHandle handle )
: WriterPlugin( handle )
{
	_componentsType = fetchChoiceParam( kParamComponentsType );
	_compressed     = fetchBooleanParam( kParamCompressed );
}

DPXWriterProcessParams DPXWriterPlugin::getProcessParams(const OfxTime time)
{
	DPXWriterProcessParams params;
	params._componentsType = _componentsType->getValue();
	params._compressed = _compressed->getValue();
	params._filepath = getAbsoluteFilenameAt(time);
	switch(static_cast<EParamBitDepth>(this->_paramBitDepth->getValue()))
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth10:
			params._bitDepth = 10;
			break;
		case eParamBitDepth12:
			params._bitDepth = 12;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		default:
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatErrValue, "Incorrect bit depth.") );
			break;
	}
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXWriterPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				DPXWriterProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				DPXWriterProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				DPXWriterProcess<rgba32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
}

void DPXWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kDPXWriterHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kDPXWriterHelpString );
	}
	else
	{
		WriterPlugin::changedParam(args, paramName);
	}
}

}
}
}
}

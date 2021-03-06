#include "PngReaderPlugin.hpp"
#include "PngReaderProcess.hpp"
#include "PngReaderDefinitions.hpp"

#include "PngEngine/png_adds.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

namespace bfs = boost::filesystem;

PngReaderPlugin::PngReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
{}

PngReaderProcessParams PngReaderPlugin::getProcessParams( const OfxTime time )
{
	PngReaderProcessParams params;

	params._filepath = getAbsoluteFilenameAt( time );
	return params;
}

void PngReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	ReaderPlugin::changedParam( args, paramName );
}

bool PngReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	const std::string filename( getAbsoluteFilenameAt( args.time ) );
	if( ! boost::filesystem::exists( filename ) )
	{
		BOOST_THROW_EXCEPTION( exception::FileInSequenceNotExist()
			<< exception::user( "PNG: Unable to open file" )
			<< exception::filename( filename ) );
	}

	try
	{
		point2<ptrdiff_t> pngDims = png_read_dimensions( filename );
		rod.x1 = 0;
		rod.x2 = pngDims.x * this->_clipDst->getPixelAspectRatio();
		rod.y1 = 0;
		rod.y2 = pngDims.y;
		TUTTLE_TCOUT_VAR( rod );
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
			<< exception::user( "PNG: Unable to open file" )
			<< exception::dev( e.what() )
			<< exception::filename( filename ) );
	}
	return true;
}

void PngReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename( getAbsoluteFirstFilename() );

	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
	{
		OFX::EBitDepth bd = OFX::eBitDepthNone;
		if( ! boost::filesystem::exists( filename ) )
		{
			BOOST_THROW_EXCEPTION( exception::FileNotExist()
				<< exception::user( "PNG: Unable to open file" )
				<< exception::filename( filename ) );
		}
		int bitDepth;
		bitDepth      = png_read_precision( filename );

		switch( bitDepth )
		{
			case 8:
				bd = OFX::eBitDepthUByte;
				break;
			case 16:
				bd = OFX::eBitDepthUShort;
				break;
			default:
				BOOST_THROW_EXCEPTION( exception::ImageFormat() );
		}
		clipPreferences.setClipBitDepth( *this->_clipDst, bd );
	}

	if( getExplicitChannelConversion() == eParamReaderChannelAuto )
	{
		switch( png_read_color_type( filename ) )
		{
			case 0 :
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentAlpha );
				break;
			case 2 :
				if( OFX::getImageEffectHostDescription()->supportsPixelComponent( OFX::ePixelComponentRGB ) )
					clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGB );
				else
					clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
			case 6 :
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
			default:
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
		}
	}

	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );

}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PngReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );
	doGilRender<PngReaderProcess>( *this, args );
}

}
}
}
}

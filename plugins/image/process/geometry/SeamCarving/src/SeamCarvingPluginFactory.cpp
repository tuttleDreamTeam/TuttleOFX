#include "SeamCarvingPluginFactory.hpp"
#include "SeamCarvingPlugin.hpp"
#include "SeamCarvingDefinitions.hpp"

#include <tuttle/plugin/context/SamplerPluginFactory.hpp>

#include <limits>


namespace tuttle {
namespace plugin {
namespace seamcarving {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void SeamCarvingPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
        desc.setLabels( "TuttleSeamCarving", "SeamCarving", "Image Retargeting" );
        desc.setPluginGrouping( "tuttle/image/process/geometry" );

        // add the supported contexts, only filter at the moment
        desc.addSupportedContext( OFX::eContextFilter );
        desc.addSupportedContext( OFX::eContextGeneral );

        // add supported pixel depths
        desc.addSupportedBitDepth( OFX::eBitDepthUByte );
        desc.addSupportedBitDepth( OFX::eBitDepthUShort );
        desc.addSupportedBitDepth( OFX::eBitDepthFloat );

        // plugin flags
        desc.setSupportsTiles( kSupportTiles );
        desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void SeamCarvingPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
        OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
        srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
        srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
        srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
        srcClip->setSupportsTiles( kSupportTiles );

        // Create the mandated output clip
        OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
        dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
        dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
        dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
        dstClip->setSupportsTiles( kSupportTiles );

        OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam( kParamMode );
        method->setLabel( "Mode" );
        method->appendOption( kParamModeFormat );
        method->appendOption( kParamModeSize );
        method->appendOption( kParamModeScale );
        method->setDefault( eParamModeFormat );

        OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
        format->setLabel( "Format" );
        format->appendOption( kParamFormatPCVideo );
        format->appendOption( kParamFormatNTSC );
        format->appendOption( kParamFormatPAL );
        format->appendOption( kParamFormatHD );
        format->appendOption( kParamFormatNTSC169 );
        format->appendOption( kParamFormatPAL169 );
        format->appendOption( kParamFormat1kSuper35 );
        format->appendOption( kParamFormat1kCinemascope );
        format->appendOption( kParamFormat2kSuper35 );
        format->appendOption( kParamFormat2kCinemascope );
        format->appendOption( kParamFormat4kSuper35 );
        format->appendOption( kParamFormat4kCinemascope );
        format->appendOption( kParamFormatSquare256 );
        format->appendOption( kParamFormatSquare512 );
        format->appendOption( kParamFormatSquare1k );
        format->appendOption( kParamFormatSquare2k );
        format->setDefault( eParamFormat2kCinemascope );

        OFX::Double2DParamDescriptor* scale = desc.defineDouble2DParam( kParamScale );
        scale->setLabel( "Scale" );
        scale->setDefault( 1.0, 1.0 );
        scale->setRange( 0.01, 0.01, std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
        scale->setDisplayRange( 0.1, 0.1, 2.5, 2.5 );
        scale->setHint( "Scale the input image [0, 0, width*scale, height*scale]." );

        OFX::BooleanParamDescriptor* keepRatio = desc.defineBooleanParam( kParamSizeKeepRatio );
        keepRatio->setLabel( "Keep ratio" );
        keepRatio->setDefault( false );
        keepRatio->setHint( "Keep input image ratio." );

        OFX::Int2DParamDescriptor* size = desc.defineInt2DParam( kParamSize );
        size->setLabel( "Size" );
        size->setDefault( 200, 200 );
        size->setRange( 1, 1, std::numeric_limits<int>::max(), std::numeric_limits<int>::max() );
        size->setHint( "Set the output size (width, height)." );

        OFX::ChoiceParamDescriptor* direction = desc.defineChoiceParam( kParamSizeOrientation );
        direction->setLabel( "Orientation" );
        direction->appendOption( kParamSizeOrientationX );
        direction->appendOption( kParamSizeOrientationY );
        direction->setDefault( eParamSizeOrientationX );

        OFX::IntParamDescriptor* width = desc.defineIntParam( kParamSizeWidth );
        width->setLabel( "Width" );
        width->setDefault( 200 );
        width->setRange( 1, std::numeric_limits<int>::max() );
        width->setDisplayRange( 0, 3000 );
        width->setHint( "Set the width in pixels and keep the input image ratio." );

        OFX::IntParamDescriptor* height = desc.defineIntParam( kParamSizeHeight );
        height->setLabel( "Height" );
        height->setDefault( 200 );
        height->setRange( 1, std::numeric_limits<int>::max() );
        height->setDisplayRange( 0, 3000 );
        height->setHint( "Set the height in pixels and keep the input image ratio." );

#ifndef TUTTLE_PRODUCTION
        OFX::BooleanParamDescriptor* center = desc.defineBooleanParam( kParamCenter );
        center->setLabel( "Center resizing" );
        center->setDefault( false );
        center->setHint( "SeamCarving around the center point." );

        OFX::Double2DParamDescriptor* centerPoint = desc.defineDouble2DParam( kParamCenterPoint );
        centerPoint->setDefault( 100, 100 );
        centerPoint->setLabel( "Center point at" );
        centerPoint->setHint( "Position of the center point." );
#endif

        // sampler parameters //
        describeSamplerParamsInContext( desc, context );

}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* SeamCarvingPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
        return new SeamCarvingPlugin( handle );
}

}
}
}


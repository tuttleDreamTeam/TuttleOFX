/**
 * @brief Display Adaptive TMO
 *
 * From:
 * Rafal Mantiuk, Scott Daly, Louis Kerofsky.
 * Display Adaptive Tone Mapping.
 * To appear in: ACM Transactions on Graphics (Proc. of SIGGRAPH'08) 27 (3)
 * http://www.mpi-inf.mpg.de/resources/hdr/datmo/
 *
 * This file is a part of PFSTMO package.
 * ---------------------------------------------------------------------- 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * ---------------------------------------------------------------------- 
 * 
 * @author Rafal Mantiuk, <mantiuk@gmail.com>
 *
 * $Id: pfstmo_mantiuk08.cpp,v 1.12 2009/02/23 18:46:36 rafm Exp $
 */

#include <config.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <fcntl.h>
#include <iostream>

#include <pfs.h>

#include "display_adaptive_tmo.h"


#define PROG_NAME "pfstmo_mantiuk08"

class QuietException 
{
};

using namespace std;

const char *temp_file_2pass = "datmo_tone_curves.tmp";
bool read_tone_curve( FILE *fh, datmoToneCurve *tc, double **x_scale );

void printHelp()
{
  fprintf( stderr, PROG_NAME " (" PACKAGE_STRING ") : \n"
    "\t[--display-function <df-spec>] [--display-size=<size-spec>]\n"
    "\t[--color-saturation <float>] [--contrast-enhancement <float>]\n"
    "\t[--white-y=<float>] [--temporal=<pass>]\n"
    "\t[--verbose] [--quiet] [--help]\n"
    "See man page for more information.\n" );
}


bool verbose = false;
bool quiet = false;

#define FRAME_NAME_MAX 30
char frame_name[FRAME_NAME_MAX+1];

int progress_report( int progress )
{
  if( !quiet ) {  
    fprintf( stderr, "\r'%s' completed %d%%", frame_name, progress );
    if( progress == 100 )
      fprintf( stderr, "\n" );    
  }
  return PFSTMO_CB_CONTINUE;
}



void tmo_mantiuk08(int argc, char * argv[])
{

  //--- default tone mapping parameters;
  float contrast_enhance_factor = 1.f;
  float saturation_factor = 1.f;
  float white_y = -2.f;
  int temporal_filter = 0;
  int itmax = 200;
  float tol = 1e-3;
  DisplayFunction *df = NULL;
  DisplaySize *ds = NULL;
  const char *output_tc = NULL;

  //--- process command line args

  df = createDisplayFunctionFromArgs( argc, argv );
  if( df == NULL )
    df = new DisplayFunctionGGBA( "lcd" );

  ds = createDisplaySizeFromArgs( argc, argv );
  if( ds == NULL )
    ds = new DisplaySize( 30.f, 0.5f );
  
  static struct option cmdLineOptions[] = {
    { "help", no_argument, NULL, 'h' },
    { "verbose", no_argument, NULL, 'v' },
    { "contrast-enhancement", required_argument, NULL, 'e' },
    { "color-saturation", required_argument, NULL, 'c' },
    { "white-y", required_argument, NULL, 'y' },
    { "temporal", required_argument, NULL, 't' },
    { "output-tone-curve", required_argument, NULL, 'o' },
    { "quiet", no_argument, NULL, 'q' },    
    { NULL, 0, NULL, 0 }
  };

  int optionIndex = 0;
  while( 1 ) {
    int c = getopt_long (argc, argv, "vhe:c:y:t:o:q", cmdLineOptions, &optionIndex);
    if( c == -1 ) break;
    switch( c ) {
    case 'h':
      printHelp();
      throw QuietException();
    case 'v':
      verbose = true;
      break;
    case 'q':
      quiet = true;
      break;
    case 'e':
      contrast_enhance_factor = (float)strtod( optarg, NULL );
      if( contrast_enhance_factor <= 0.0f )
        throw pfs::Exception("incorrect contrast enhancement factor, accepted value is any positive number");
      break;
    case 'c':
      saturation_factor = (float)strtod( optarg, NULL );
      if( saturation_factor < 0.0f || saturation_factor > 2.0f )
        throw pfs::Exception("incorrect saturation factor, accepted range is (0..2)");
      break;
    case 'y':
      if( !strcmp( optarg, "none" ) )
        white_y = -1;
      else
        white_y = (float)strtod( optarg, NULL );
      if( white_y < 0.0f )
        throw pfs::Exception("incorrect white-y value. The value must be greater than 0");
      break;
    case 't':
      temporal_filter = strtol( optarg, NULL, 10 );
      if( temporal_filter != 1 && temporal_filter != 2 )
        throw pfs::Exception("temporal filter pass (-t parameter) must be set to 1 or 2.");
      break;
    case 'o':
      output_tc = optarg;
      break;
    case '?':
      throw QuietException();
    case ':':
      throw QuietException();
    }
  }

  if( verbose ) {
    df->print( stderr );
    ds->print( stderr );
  }

  FILE *tc_FH = NULL;
  if( output_tc != NULL ) {
    tc_FH = fopen( output_tc, "w" );
    if( tc_FH == NULL )
      throw pfs::Exception("cannot open file for writing tone-curve.");
  }

  FILE *tp_FH = NULL;
  datmoToneCurve *tone_curves_2p[DATMO_TF_TAPSIZE] = { 0 };
  size_t tone_curves_2p_count = 0;
  double *x_scale = NULL;
  if( temporal_filter == 1 ) {
    tp_FH = fopen( temp_file_2pass, "w" );
    if( tp_FH == NULL )
      throw pfs::Exception( "cannot open 2-pass file for writing" );
  } else if( temporal_filter == 2 ) {
    tp_FH = fopen( temp_file_2pass, "r" );
    if( tp_FH == NULL )
      throw pfs::Exception( "cannot open 2-pass file for reading" );

    int i;
    for( i=0; i < DATMO_TF_TAPSIZE; i++ ) {
      tone_curves_2p[i] = new datmoToneCurve();
      if( !read_tone_curve( tp_FH, tone_curves_2p[i], &x_scale ) )
        break;
    }
    tone_curves_2p_count = i;
    if( i == 0 )
      throw pfs::Exception( "failed to read data from 2-pass file" );
  }
  
  
  pfs::DOMIO pfsio;

  size_t frame_no = 0;
  while( true ) {
    pfs::Frame *frame = pfsio.readFrame( stdin );
    if( frame == NULL )
      break;

    pfs::Channel *inX, *inY, *inZ;
	
    frame->getXYZChannels(inX, inY, inZ);
    int cols = frame->getWidth();
    int rows = frame->getHeight();

    const char *file_name = frame->getTags()->getString( "FILE_NAME" );
    if( file_name == NULL )
      sprintf( frame_name, "frame #%d", frame_no );
    else {
      int len = strlen( file_name );
      if( len > FRAME_NAME_MAX ) // In case file name is too long
        len = FRAME_NAME_MAX-3;
      strcpy( frame_name, "..." );
      strncpy( frame_name+3, file_name + strlen( file_name ) - len, len+1 );
    }
    

    {
      pfs::Array2DImpl R( cols, rows );
  
      pfs::transformColorSpace( pfs::CS_XYZ, inX, inY, inZ, pfs::CS_RGB, inX, &R, inZ );

      if( white_y == -2.f ) {       // If not overriden by command line options
        const char *white_y_str = frame->getTags()->getString( "WHITE_Y" );
        if( white_y_str != NULL ) {
          white_y = strtof( white_y_str, NULL );
          if( white_y == 0 ) {
            white_y = -1;
            fprintf( stderr, PROG_NAME ": warning - wrong WHITE_Y in the input image" );        
          }
        }
      }      
      if( verbose && frame_no == 0 ) {
        fprintf( stderr, "Luminance factor of the reference white: " );
        if( white_y < 0 )
          fprintf( stderr, "not specified\n" );
        else
          fprintf( stderr, "%g\n", white_y );      
      }

      const char *lum_data = frame->getTags()->getString("LUMINANCE");
      if( lum_data != NULL && !strcmp( lum_data, "DISPLAY" ) && frame_no == 0 )
        fprintf( stderr, PROG_NAME " warning: input image should be in linear (not gamma corrected) luminance factor units. Use '--linear' option with pfsin* commands.\n" );

      datmoToneCurve tc;

      if( temporal_filter == 2 ) {
        // Second pass of the temporal filter - filter tone curves
        tc.init( tone_curves_2p[0]->size, x_scale );
        datmo_filter_tone_curves( tone_curves_2p, tone_curves_2p_count, &tc );
        
      } else {
      
        std::auto_ptr<datmoConditionalDensity> C = datmo_compute_conditional_density( cols, rows, inY->getRawData(), progress_report );
        if( C.get() == NULL )
          throw pfs::Exception("failed to analyse the image");
  
        int res;
        res = datmo_compute_tone_curve( &tc, C.get(), df, ds, contrast_enhance_factor, white_y, progress_report );
        if( res != PFSTMO_OK )
          throw pfs::Exception( "failed to compute a tone-curve" );    
      }

      if( temporal_filter == 1 ) {
        // First pass of the temporal filter - store tone-curves, do not tone-map
        
        fprintf( tp_FH, "%d,%d\n", frame_no, tc.size );          
        for( int i=0; i < tc.size; i++ )
          fprintf( tp_FH, "%g,%g\n", tc.x_i[i], tc.y_i[i] );

        progress_report( 100 );        
      } else {

        int res;
        res = datmo_apply_tone_curve_cc( inX->getRawData(), R.getRawData(), inZ->getRawData(), cols, rows,
          inX->getRawData(), R.getRawData(), inZ->getRawData(), inY->getRawData(), &tc,
          df, saturation_factor );
        if( res != PFSTMO_OK )
          throw pfs::Exception( "failed to tone-map an image" );

        if( tc_FH != NULL ) {
          for( int i=0; i < tc.size; i++ )
            fprintf( tc_FH, "%d,%g,%g,%g\n", frame_no, tc.x_i[i], tc.y_i[i], df->inv_display( (float)pow( 10, tc.y_i[i] ) ) );
        }      
    
//    int res;
//    res = datmo_tonemap( inX->getRawData(), R.getRawData(), inZ->getRawData(), cols, rows,
//      inX->getRawData(), R.getRawData(), inZ->getRawData(), inY->getRawData(),
//    df, ds, contrast_enhance_factor, saturation_factor, white_y, progress_report );
        progress_report( 100 );        

        pfs::transformColorSpace( pfs::CS_RGB, inX, &R, inZ, pfs::CS_XYZ, inX, inY, inZ );
        frame->getTags()->setString("LUMINANCE", "DISPLAY");
  
        pfsio.writeFrame( frame, stdout );
      }

      if( temporal_filter == 2 ) {
        // Second pass of the temporal filter - shift array and read new tone-curve
        datmoToneCurve *first = tone_curves_2p[0];
        for( int i = 1; i < tone_curves_2p_count; i++ )
          tone_curves_2p[i-1] = tone_curves_2p[i];
        tone_curves_2p[tone_curves_2p_count-1] = first;
        if( !read_tone_curve( tp_FH, tone_curves_2p[tone_curves_2p_count-1], &x_scale ) )
          tone_curves_2p_count--;

        if( tone_curves_2p_count < 0 )
          throw pfs::Exception( "insufficient number of tone curves in the 2-pass file. The file may be corrupted." );          
      }
      
      pfsio.freeFrame(frame);
    }
    

    frame_no++;

  }

  if( tp_FH != NULL )
    fclose( tp_FH );
  
  if( tc_FH != NULL )
    fclose( tc_FH );

  delete []x_scale;
  for( int i=0; i < DATMO_TF_TAPSIZE; i++ ) 
    delete tone_curves_2p[i];

  delete df;
  delete ds;
}

bool read_tone_curve( FILE *fh, datmoToneCurve *tc, double **x_scale )
{
  int size, frame_no, read;
  read = fscanf( fh, "%d,%d\n", &frame_no, &size );
  if( read != 2 )
    return false;
  
  if( *x_scale == NULL )
    *x_scale = new double[size];

  tc->init( size, *x_scale );
  
  for( int i=0; i < size; i++ ) {
    float x, y;
    read = fscanf( fh, "%f,%f\n", &x, &y );
    if( read != 2 )
      throw pfs::Exception( "missing data in the 2-pass file"  );
    (*x_scale)[i] = x;
    tc->y_i[i] = y;
  }
  return true;
}



int main( int argc, char* argv[] )
{
  try {
    tmo_mantiuk08( argc, argv );
  }
  catch( pfs::Exception ex ) {
    fprintf( stderr, PROG_NAME " error: %s\n", ex.getMessage() );
    return EXIT_FAILURE;
  }        
  catch( QuietException  ex ) {
    return EXIT_FAILURE;
  }        
  return EXIT_SUCCESS;
}


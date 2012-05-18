/**
 * @file pfstmo_fattal02.cpp
 * @brief Tone map XYZ channels using Fattal02 model
 *
 * Gradient Domain High Dynamic Range Compression
 * R. Fattal, D. Lischinski, and M. Werman
 * In ACM Transactions on Graphics, 2002.
 *
 * 
 * This file is a part of PFSTMO package.
 * ---------------------------------------------------------------------- 
 * Copyright (C) 2003,2004 Grzegorz Krawczyk
 * 
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
 * @author Grzegorz Krawczyk, <krawczyk@mpi-sb.mpg.de>
 *
 * $Id: pfstmo_fattal02.cpp,v 1.3 2008/09/04 12:46:49 julians37 Exp $
 */

#include <config.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

#include <pfs.h>

#include "tmo_fattal02.h"

using namespace std;

#define PROG_NAME "pfstmo_fattal02"

/// verbose mode
bool verbose = false;

class QuietException 
{
};

void printHelp()
{
  fprintf( stderr, PROG_NAME " (" PACKAGE_STRING ") : \n"
    "\t[--alpha <val>] [--beta <val>] \n"
    "\t[--saturation <val>] \n"
    "\t[--noise <val>] \n"
    "\t[--verbose] [--help] \n"
    "See man page for more information.\n" );
}

void pfstmo_fattal02( int argc, char* argv[] )
{
  pfs::DOMIO pfsio;

  //--- default tone mapping parameters;
  float opt_alpha = 1.0f;
  float opt_beta = 0.9f;
  float opt_saturation=0.8f;
  float opt_noise = -1.0f; // not set!
  
  static struct option cmdLineOptions[] = {
    { "help", no_argument, NULL, 'h' },
    { "verbose", no_argument, NULL, 'v' },
    { "alpha", required_argument, NULL, 'a' },
    { "beta", required_argument, NULL, 'b' },
    { "saturation", required_argument, NULL, 's' },
    { "noise", required_argument, NULL, 'n' },
    { NULL, 0, NULL, 0 }
  };

  int optionIndex = 0;
  while( 1 ) {
    int c = getopt_long (argc, argv, "hva:b:s:n:", cmdLineOptions, &optionIndex);
    if( c == -1 ) break;
    switch( c ) {
    case 'h':
      printHelp();
      throw QuietException();
    case 'v':
      verbose = true;
      break;
    case 'a':
      opt_alpha = (float)strtod( optarg, NULL );
      if( opt_alpha<=0.0f )
        throw pfs::Exception("alpha value out of range, should be >0");
      break;
    case 'b':
      opt_beta = (float)strtod( optarg, NULL );
      if( opt_beta<=0.0f )
        throw pfs::Exception("beta value out of range, should be >0");
      break;
    case 's':
      opt_saturation = (float)strtod( optarg, NULL );
      if( opt_saturation<=0.0f || opt_saturation>1.0f )
        throw pfs::Exception("saturation value out of range, should be 0..1");
      break;
    case 'n':
      opt_noise = (float)strtod( optarg, NULL );
      if( opt_noise<0.0f )
        throw pfs::Exception("noise level value out of range, should be >=0");
      break;
    case '?':
      throw QuietException();
    case ':':
      throw QuietException();
    }
  }
  
  // adjust noise floor if not set by user
  if( opt_noise<0.0f )
    opt_noise = opt_alpha*0.01;

  VERBOSE_STR << "threshold gradient (alpha): " << opt_alpha << endl;
  VERBOSE_STR << "strengh of modification (beta): " << opt_beta << endl;
  VERBOSE_STR << "noise floor: " << opt_noise << endl;  
  VERBOSE_STR << "saturation: " << opt_saturation << endl;

  while( true ) 
  {
    pfs::Frame *frame = pfsio.readFrame( stdin );
    if( frame == NULL ) break; // No more frames

    pfs::Channel *X, *Y, *Z;
    frame->getXYZChannels( X, Y, Z );
    frame->getTags()->setString("LUMINANCE", "RELATIVE");
    //---

    if( Y==NULL || X==NULL || Z==NULL)
      throw pfs::Exception( "Missing X, Y, Z channels in the PFS stream" );
        
    // tone mapping
    int w = Y->getCols();
    int h = Y->getRows();

    pfs::Array2DImpl* L = new pfs::Array2DImpl(w,h);
    tmo_fattal02(w, h, Y->getRawData(), L->getRawData(), opt_alpha, opt_beta, opt_noise);
		
    for( int x=0 ; x<w ; x++ )
      for( int y=0 ; y<h ; y++ )
      {
        (*X)(x,y) = powf( (*X)(x,y)/(*Y)(x,y), opt_saturation ) * (*L)(x,y);
        (*Z)(x,y) = powf( (*Z)(x,y)/(*Y)(x,y), opt_saturation ) * (*L)(x,y);
        (*Y)(x,y) = (*L)(x,y);
      }

    delete L;
    
    //---
    pfsio.writeFrame( frame, stdout );
    pfsio.freeFrame( frame );        
  }
}

int main( int argc, char* argv[] )
{
  try {
    pfstmo_fattal02( argc, argv );
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

/**
 * @file tmo_fattal02.cpp
 * @brief TMO: Gradient Domain High Dynamic Range Compression
 *
 * Implementation of Gradient Domain High Dynamic Range Compression
 * by Raanan Fattal, Dani Lischinski, Michael Werman.
 *
 * @author Grzegorz Krawczyk, <krawczyk@mpi-sb.mpg.de>
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
 * $Id: tmo_fattal02.cpp,v 1.3 2008/11/04 23:43:08 rafm Exp $
 */

#include <config.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include <math.h>

#include <assert.h>

#include "pfstmo.h"
#include "pde.h"

using namespace std;


//!! TODO: for debugging purposes
// #define PFSEOL "\x0a"
// static void dumpPFS( const char *fileName, const pfstmo::Array2D *data, const char *channelName )
// {
//   FILE *fh = fopen( fileName, "wb" );
//   assert( fh != NULL );

//   int width = data->getCols();
//   int height = data->getRows();

//   fprintf( fh, "PFS1" PFSEOL "%d %d" PFSEOL "1" PFSEOL "0" PFSEOL
//     "%s" PFSEOL "0" PFSEOL "ENDH", width, height, channelName );

//   for( int y = 0; y < height; y++ )
//     for( int x = 0; x < width; x++ ) {
//       fwrite( &((*data)(x,y)), sizeof( float ), 1, fh );
//     }
  
//   fclose( fh );
// }

//--------------------------------------------------------------------

void downSample(pfstmo::Array2D* A, pfstmo::Array2D* B)
{
  int width = B->getCols();
  int height = B->getRows();
  int x,y;

  for( y=0 ; y<height ; y++ )
    for( x=0 ; x<width ; x++ )
    {
      float p = 0.0f;
      p += (*A)(2*x,2*y);
      p += (*A)(2*x+1,2*y);
      p += (*A)(2*x,2*y+1);
      p += (*A)(2*x+1,2*y+1);
      (*B)(x,y) = p / 4.0f;
    }	
}
	
void gaussianBlur( pfstmo::Array2D* I, pfstmo::Array2D* L )
{
  int width = I->getCols();
  int height = I->getRows();
  int size = width*height;
  int x,y;

  pfstmo::Array2D* T = new pfstmo::Array2D(width,height);

  //--- X blur
  for( y=0 ; y<height ; y++ )
  {
    for( x=1 ; x<width-1 ; x++ )
    {
      float t = 2*(*I)(x,y);
      t += (*I)(x-1,y);
      t += (*I)(x+1,y);
      (*T)(x,y) = t/4.0f;
    }
    (*T)(0,y) = ( 3*(*I)(0,y)+(*I)(1,y) ) / 4.0f;
    (*T)(width-1,y) = ( 3*(*I)(width-1,y)+(*I)(width-2,y) ) / 4.0f;
  }

  //--- Y blur
  for( x=0 ; x<width ; x++ )
  {
    for( y=1 ; y<height-1 ; y++ )
    {
      float t = 2*(*T)(x,y);
      t += (*T)(x,y-1);
      t += (*T)(x,y+1);
      (*L)(x,y) = t/4.0f;
    }
    (*L)(x,0) = ( 3*(*T)(x,0)+(*T)(x,1) ) / 4.0f;
    (*L)(x,height-1) = ( 3*(*T)(x,height-1)+(*T)(x,height-2) ) / 4.0f;
  }

  delete T;
}

int createGaussianPyramids( pfstmo::Array2D* H, pfstmo::Array2D** pyramids, int nlevels )
{
  int width = H->getCols();
  int height = H->getRows();
  int size = width*height;

  pyramids[0] = new pfstmo::Array2D(width,height);
  for( int i=0 ; i<size ; i++ )
    (*pyramids[0])(i) = (*H)(i);

  pfstmo::Array2D* L = new pfstmo::Array2D(width,height);
  gaussianBlur( pyramids[0], L );
	
  for( int k=1 ; k<nlevels ; k++ )
  {
    width /= 2;
    height /= 2;		
    pyramids[k] = new pfstmo::Array2D(width,height);
    downSample(L, pyramids[k]);
    
    delete L;
    L = new pfstmo::Array2D(width,height);
    gaussianBlur( pyramids[k], L );
  }

  delete L;
}

//--------------------------------------------------------------------

float calculateGradients(pfstmo::Array2D* H, pfstmo::Array2D* G, int k)
{
  int width = H->getCols();
  int height = H->getRows();
  float divider = pow( 2.0f, k+1 );
  float avgGrad = 0.0f;

  for( int y=0 ; y<height ; y++ )
  {
    for( int x=0 ; x<width ; x++ )
    {
      float gx, gy;
      int w, n, e, s;
      w = (x == 0 ? 0 : x-1);
      n = (y == 0 ? 0 : y-1);
      s = (y+1 == height ? y : y+1);
      e = (x+1 == width ? x : x+1);

      gx = ((*H)(w,y)-(*H)(e,y)) / divider;
        
      gy = ((*H)(x,s)-(*H)(x,n)) / divider;
      
      (*G)(x,y) = sqrt(gx*gx+gy*gy);
      avgGrad += (*G)(x,y);
    }
  }

  return avgGrad / (width*height);
}

//--------------------------------------------------------------------

void upSample(pfstmo::Array2D* A, pfstmo::Array2D* B)
{
  int width = B->getCols();
  int height = B->getRows();
  int awidth = A->getCols();
  int aheight = A->getRows();
  int x,y;

  for( y=0 ; y<height ; y++ )
    for( x=0 ; x<width ; x++ )
    {
      int ax = x/2;
      int ay = y/2;
      ax = (ax<awidth) ? ax : awidth-1;
      ay = (ay<aheight) ? ay : aheight-1;
      
      (*B)(x,y) = (*A)(ax,ay);
    }
//--- this code below produces 'use of uninitialized value error'
//   int width = A->getCols();
//   int height = A->getRows();
//   int x,y;

//   for( y=0 ; y<height ; y++ )
//     for( x=0 ; x<width ; x++ )
//     {
//       (*B)(2*x,2*y) = (*A)(x,y);
//       (*B)(2*x+1,2*y) = (*A)(x,y);
//       (*B)(2*x,2*y+1) = (*A)(x,y);
//       (*B)(2*x+1,2*y+1) = (*A)(x,y);
//     }	
}

void calculateFiMatrix(pfstmo::Array2D* FI, pfstmo::Array2D* gradients[], 
  float avgGrad[], int nlevels,
  float alfa, float beta, float noise)
{
  int width = gradients[nlevels-1]->getCols();
  int height = gradients[nlevels-1]->getRows();
  int k;
  pfstmo::Array2D** fi = new pfstmo::Array2D*[nlevels];

  fi[nlevels-1] = new pfstmo::Array2D(width,height);
  for( k=0 ; k<width*height ; k++ )
    (*fi[nlevels-1])(k) = 1.0f;
  
  for( k=nlevels-1 ; k>=0 ; k-- )
  {
    width = gradients[k]->getCols();
    height = gradients[k]->getRows();
		
    for( int y=0 ; y<height ; y++ )
      for( int x=0 ; x<width ; x++ )
      {
        float grad = (*gradients[k])(x,y);
        float a = alfa * avgGrad[k];

        float value=1.0;
        if( grad>1e-4 )
          value = a/(grad+noise) * pow((grad+noise)/a, beta);
        (*fi[k])(x,y) *= value;
      }
		
    // create next level
    if( k>1 )
    {
      width = gradients[k-1]->getCols();
      height = gradients[k-1]->getRows();
      fi[k-1] = new pfstmo::Array2D(width,height);
    }
    else
      fi[0] = FI;               // highest level -> result

    if( k>0 )
    {
      upSample(fi[k], fi[k-1]);		// upsample to next level
      gaussianBlur(fi[k-1],fi[k-1]);
    }
  }
	
  for( k=1 ; k<nlevels ; k++ )
    delete fi[k];
  delete[] fi;
}

//--------------------------------------------------------------------


static void findMaxMinPercentile(pfstmo::Array2D* I, float minPrct, float& minLum, 
  float maxPrct, float& maxLum)
{
  int size = I->getRows() * I->getCols();
  std::vector<float> vI;

  for( int i=0 ; i<size ; i++ )
    if( (*I)(i)!=0.0f )
      vI.push_back((*I)(i));
      
  std::sort(vI.begin(), vI.end());

  minLum = vI.at( int(minPrct*vI.size()) );
  maxLum = vI.at( int(maxPrct*vI.size()) );
}

//--------------------------------------------------------------------

void tmo_fattal02(unsigned int width, unsigned int height,
                  const float* nY, float* nL, float alfa, float beta, float noise)
{
  const pfstmo::Array2D* Y = new pfstmo::Array2D(width, height, const_cast<float*>(nY));
  pfstmo::Array2D* L = new pfstmo::Array2D(width, height, nL);

  const int MSIZE = 32;         // minimum size of gaussian pyramid
	
  int size = width*height;
  int x,y,i,k;

  // find max & min values, normalize to range 0..100 and take logarithm
  float minLum = (*Y)(0,0);
  float maxLum = (*Y)(0,0);
  for( i=0 ; i<size ; i++ )
  {
    minLum = ( (*Y)(i)<minLum ) ? (*Y)(i) : minLum;
    maxLum = ( (*Y)(i)>maxLum ) ? (*Y)(i) : maxLum;
  }
  pfstmo::Array2D* H = new pfstmo::Array2D(width, height);
  for( i=0 ; i<size ; i++ )
    (*H)(i) = log( 100.0f*(*Y)(i)/maxLum + 1e-4 );

  DEBUG_STR << "tmo_fattal02: calculating attenuation matrix" << endl;
  
  // create gaussian pyramids
  int mins = (width<height) ? width : height;	// smaller dimension
  int nlevels = 0;
  while( mins>=MSIZE )
  {
    nlevels++;
    mins /= 2;
  }
  pfstmo::Array2D** pyramids = new pfstmo::Array2D*[nlevels];
  createGaussianPyramids(H, pyramids, nlevels);

  // calculate gradients and its average values on pyramid levels
  pfstmo::Array2D** gradients = new pfstmo::Array2D*[nlevels];
  float* avgGrad = new float[nlevels];
  for( k=0 ; k<nlevels ; k++ )
  {
    gradients[k] = new pfstmo::Array2D(pyramids[k]->getCols(), pyramids[k]->getRows());
    avgGrad[k] = calculateGradients(pyramids[k],gradients[k], k);
  }

  // calculate fi matrix
  pfstmo::Array2D* FI = new pfstmo::Array2D(width, height);
  calculateFiMatrix(FI, gradients, avgGrad, nlevels, alfa, beta, noise);

//  dumpPFS( "FI.pfs", FI, "Y" );

  // attenuate gradients
  pfstmo::Array2D* Gx = new pfstmo::Array2D(width, height);
  pfstmo::Array2D* Gy = new pfstmo::Array2D(width, height);
  for( y=0 ; y<height ; y++ )
    for( x=0 ; x<width ; x++ )
    {
      int s, e;
      s = (y+1 == height ? y : y+1);
      e = (x+1 == width ? x : x+1);

      (*Gx)(x,y) = ((*H)(e,y)-(*H)(x,y)) * (*FI)(x,y);        
      (*Gy)(x,y) = ((*H)(x,s)-(*H)(x,y)) * (*FI)(x,y);      
    }

//   dumpPFS( "Gx.pfs", Gx, "Y" );
//   dumpPFS( "Gy.pfs", Gy, "Y" );
  
  DEBUG_STR << "tmo_fattal02: compressing gradients" << endl;
  
  // calculate divergence
  pfstmo::Array2D* DivG = new pfstmo::Array2D(width, height);
  for( y=0 ; y<height ; y++ )
    for( x=0 ; x<width ; x++ )
    {
      (*DivG)(x,y) = (*Gx)(x,y) + (*Gy)(x,y);
      if( x > 0 ) (*DivG)(x,y) -= (*Gx)(x-1,y);
      if( y > 0 ) (*DivG)(x,y) -= (*Gy)(x,y-1);
    }

//  dumpPFS( "DivG.pfs", DivG, "Y" );
  
  DEBUG_STR << "tmo_fattal02: recovering image" << endl;
  
  // solve pde and exponentiate (ie recover compressed image)
  pfstmo::Array2D* U = new pfstmo::Array2D(width, height);
  solve_pde_multigrid( DivG, U );
//  solve_pde_sor( DivG, U );

  for( y=0 ; y<height ; y++ )
    for( x=0 ; x<width ; x++ )
      (*L)(x,y) = exp( (*U)(x,y) ) - 1e-4;
	
  // remove percentile of min and max values and renormalize
  findMaxMinPercentile(L, 0.001f, minLum, 0.995f, maxLum);
  maxLum -= minLum;
  for( y=0 ; y<height ; y++ )
    for( x=0 ; x<width ; x++ )
    {
      (*L)(x,y) = ((*L)(x,y)-minLum) / maxLum;
      if( (*L)(x,y)<=0.0f )
        (*L)(x,y) = 1e-4;
    }

  // clean up
  DEBUG_STR << "tmo_fattal02: clean up" << endl;
  delete H;
  for( i=0 ; i<nlevels ; i++ )
  {
    delete pyramids[i];
    delete gradients[i];
  }
  delete[] pyramids;
  delete[] gradients;
  delete[] avgGrad;
  delete FI;
  delete Gx;
  delete Gy;
  delete DivG;
  delete U;

  delete L;
  delete Y;
}

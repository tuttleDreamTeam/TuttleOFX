#ifndef _TERRY_GENERATOR_NOISEGENERATOR_HPP_
#define _TERRY_GENERATOR_NOISEGENERATOR_HPP_

#include <boost/gil/utilities.hpp>

#include <cmath>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/random_device.hpp>
//#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/normal_distribution.hpp>

namespace terry {
namespace generator {

// Models a Unary Function
template <typename P>
// Models PixelValueConcept
struct NoiseGeneratorFunctor
{
	//typedef point2<ptrdiff_t>    point_t;
	typedef boost::gil::point2<double>    point_t;

	typedef NoiseGeneratorFunctor const_t;
	typedef P value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	float      _sigma;
	float	   _nu;
	float	   _color;
	float	   _r_weight;
	float	   _g_weight;
	float	   _b_weight;
	float	   _bw_weight;
	float	   _nb;
	
	NoiseGeneratorFunctor() {}
	NoiseGeneratorFunctor(  float sigma, float nu, int color, float r_weight, float	   g_weight, float	b_weight, float bw_weight, float nb) //set maybe an enum for
		:  _sigma(sigma), _nu(nu), _color(color), _r_weight(r_weight), _g_weight(g_weight), _b_weight(b_weight), _bw_weight(bw_weight), _nb(nb){}

	result_type operator()( const point_t& p ) const
	{
                boost::random::random_device gen;
                boost::random::normal_distribution<> dist(_nu, _sigma);
		//boost::random::uniform_int_distribution<> dist(0, 255); //other distribution there is a lot in boost random
		
value_type color;
		double nb,r,g,b,BW;
		 if (_color == 0)
		{
		BW = dist(gen) * _bw_weight; 
		  color_convert( rgba32f_pixel_t( BW, BW, BW, 1 ), color);	
		}
		else
		{
		r = dist(gen) * _r_weight;
		g = dist(gen) * _g_weight;
		b = dist(gen) * _b_weight;
		color_convert( rgba32f_pixel_t(r,g, b, 1 ), color);
		}
                return color;
	}
};

}
}

#endif

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

	value_type _in_color;
	float      _sigma;
	float	   _nu;
	
	


	NoiseGeneratorFunctor() {}
	NoiseGeneratorFunctor(  float sigma, float nu ) //set maybe an enum for
		:  _sigma(sigma), _nu(nu){}

	result_type operator()( const point_t& p ) const
	{
                boost::random::random_device gen;
		
		
                boost::random::normal_distribution<> dist(_nu, _sigma);
		//boost::random::uniform_int_distribution<> dist(0, 255);
		// color gray
                value_type color;
		double a;
		a = 0.004*dist(gen);
                color_convert( rgba32f_pixel_t( a, a, a, 1 ), color);

		//color rgba
		//color_convert( rgba32f_pixel_t( a, a, a, 1 ), color);
                return color;
	}


};

}
}

#endif

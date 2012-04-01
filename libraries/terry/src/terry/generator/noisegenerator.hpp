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


	NoiseGeneratorFunctor() {}
	NoiseGeneratorFunctor( const value_type& in_color, float sigma ) //set maybe an enum for
		: _in_color( in_color ), _sigma(sigma){}

	result_type operator()( const point_t& p ) const
	{
                boost::random::random_device gen;
		

                boost::random::normal_distribution<> dist(0, _sigma);
		//boost::random::uniform_int_distribution<> dist(0, 255);

                value_type color;
                color_convert( rgba32f_pixel_t( 0.004*dist(gen), 0.004*dist(gen), 0.004*dist(gen), 1 ), color );

                return color;
	}


};

}
}

#endif

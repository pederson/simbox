/** @file SignalGenerator.hpp
 *  @brief file with SignalGenerator class
 *
 *  This contains the SignalGenerator classes
 *  and associated functions, defs, and enums
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _SIGNALGENERATOR_H
#define _SIGNALGENERATOR_H

#include <math.h>
#include <iostream>
#include <limits>

const double SIGNALGENERATOR_PI=3.14159265358979323846264338327950288;

using namespace std;

/** @class SignalGenerator
 *  @brief base signal generator class
 *
 *  Defines functions expected of a signal
 *	generator class. It's pretty lightweight
 *	and (hopefully) flexible
 *
 */
class SignalGenerator{
public:

	// inspectors
	virtual double value(double t) const = 0;
	double magnitude() const {return m_magnitude;};
	double offtime() const {return m_toff;};
	virtual double freq_min() const {return 0.0;};
	virtual double freq_max() const {return 0.0;};
	virtual void print_summary() const = 0;

	// mutators
	void set_offtime(double toff) {m_toff = toff;};
	void set_magnitude(double magnitude);
	

protected:
	// general
	double m_magnitude;
	double m_toff;


};


/** @class SignalConstant
 *  @brief a constant signal
 *
 *	A signal of constant value
 *  
 */
class SignalConstant : public SignalGenerator{
public:
	SignalConstant(double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {return (t>m_toff? 0.0 : m_magnitude);};
	void print_summary() const{
		cout << "Constant: Mag=" << m_magnitude << " Toff=" << m_toff ;
	}

};

/** @class SignalGaussian
 *  @brief signal that is gaussian in time
 *
 *	this signal is temporally gaussian
 *  
 */
class SignalGaussian : public SignalGenerator{
public:
	SignalGaussian(double sigma_t, double t0=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_sigma_t = sigma_t;
		m_t0 = t0;
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {return (t>m_toff? 0.0 : m_magnitude*exp(-0.5*(t-m_t0)*(t-m_t0)/m_sigma_t/m_sigma_t));};
	void print_summary() const{
		cout << "Gaussian: Mag=" << m_magnitude << " Toff=" << m_toff ;
		cout << " T0=" << m_t0 << " SigmaT=" << m_sigma_t;
	}

	double freq_min() const {return 0.0;};
	double freq_max() const {return 1.0/m_sigma_t;};

protected:
	double m_sigma_t, m_t0;
};

/** @class SignalGaussianBurst
 *  @brief signal that is gaussian in frequency
 *
 *	this signal is spectrally gaussian
 *  
 */
class SignalGaussianBurst : public SignalGenerator{
public:
	SignalGaussianBurst(double sigma_f, double f0=0.0, double t0=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_sigma_f = sigma_f;
		m_f0 = f0;
		m_t0 = t0;
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {
		if (t>m_toff) return 0.0;
		double sigma_t = 1.0/(2*SIGNALGENERATOR_PI*m_sigma_f);
		return m_magnitude*sin(2*SIGNALGENERATOR_PI*m_f0*(t-m_t0))*exp(-0.5*4*SIGNALGENERATOR_PI*SIGNALGENERATOR_PI*m_sigma_f*m_sigma_f*(t-3*sigma_t-m_t0)*(t-3*sigma_t-m_t0));
	}
	void print_summary() const{
		cout << "GaussianBurst: Mag=" << m_magnitude << " Toff=" << m_toff ;
		cout << " T0=" << m_t0 << " F0=" << m_f0 << " SigmaF=" << m_sigma_f;
	}

	double freq_min() const {return std::max(m_f0-3.0*m_sigma_f, 0.0);};
	double freq_max() const {return m_f0+3.0*m_sigma_f;};

protected:
	double m_sigma_f, m_f0, m_t0;
};

/** @class SignalSinusoid
 *  @brief sinusoidal signal
 *
 *	signal that contains just 1 frequency
 *  
 */
class SignalSinusoid : public SignalGenerator{
public:
	SignalSinusoid(double freq_Hz, double phase=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_freq_Hz = freq_Hz;
		m_phase = phase;
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {return (t>m_toff? 0.0 : m_magnitude*sin(2*SIGNALGENERATOR_PI*m_freq_Hz*t + m_phase));};
	void print_summary() const{
		cout << "Sinusoid: Mag=" << m_magnitude << " Toff=" << m_toff ;
		cout << " Freq=" << m_freq_Hz << " Phase=" << m_phase;
	}

	double freq_min() const {return m_freq_Hz;};
	double freq_max() const {return m_freq_Hz;};

protected:
	double m_freq_Hz, m_phase;	// phase in radians
};

/** @class SignalTwoSinusoids
 *  @brief signal that contains 2 frequencies
 *
 *	two sinusoids added together
 *  
 */
class SignalTwoSinusoids : public SignalGenerator{
public:
	SignalTwoSinusoids(double freq_1, double magn_1, double phase_1, double freq_2, double magn_2, double phase_2, double toff = numeric_limits<double>::max())
	{
		m_freq_1 = freq_1;
		m_magn_1 = magn_1;
		m_phase_1 = phase_1;
		m_freq_2 = freq_2;
		m_magn_2 = magn_2;
		m_phase_2 = phase_2;
		m_magnitude = m_magn_1+m_magn_2;
		m_toff = toff;
	}

	double value(double t) const {return (t>m_toff? 0.0 : m_magn_1*sin(2*SIGNALGENERATOR_PI*m_freq_1*t + m_phase_1) + m_magn_2*sin(2*SIGNALGENERATOR_PI*m_freq_2*t + m_phase_2));};
	void print_summary() const{
		cout << "TwoSinusoids: Freq1=" << m_freq_1 << " Mag1=" << m_magn_1 << " Phase1=" << m_phase_1 ;
		cout << " Freq2=" << m_freq_2 << " Mag2=" << m_magn_2 << " Phase2=" << m_phase_2 << " Toff=" << m_toff ;
	}

	double freq_min() const {return min(m_freq_1, m_freq_2);};
	double freq_max() const {return max(m_freq_1, m_freq_2);};

protected:
	double m_freq_1, m_magn_1, m_phase_1;	// phase in radians
	double m_freq_2, m_magn_2, m_phase_2;	// phase in radians
};

/** @class SignalRamp
 *  @brief linear ramp signal
 *
 *  
 */
class SignalRamp : public SignalGenerator{
public:
	SignalRamp(double t1, double t0=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_t0 = t0;
		m_t1 = t1;
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {
		if (t>m_toff) return 0.0;

		if (t>m_t1) return m_magnitude;
		else if (t<m_t0) return 0.0;
		else return m_magnitude/(m_t1-m_t0)*t;
	}
	void print_summary() const{
		cout << "Ramp: Mag=" << m_magnitude << " Toff=" << m_toff ;
		cout << " T0=" << m_t0 << " T1=" << m_t1;
	}

protected:
	double m_t0, m_t1; // t0 = ramp start; t1 = ramp stop
};

/** @class SignalTanh
 *  @brief signal shaped like a hyperbolic tangent in time
 *
 *	use this for a slow, smooth ramp up to
 *	some constant value
 *  
 */
class SignalTanh : public SignalGenerator{
public:
	SignalTanh(double tup, double t0=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_t0 = t0;
		m_magnitude = magnitude;
		m_toff = toff;
		m_tup = tup;
	}

	double value(double t) const {return (t>m_toff ? 0.0 : m_magnitude*tanh((t-m_t0)*M_PI/m_tup));};
	void print_summary() const{
		cout << "Tanh: Mag=" << m_magnitude << " Tup= " << m_tup << " Toff=" << m_toff ;
		cout << " T0=" << m_t0 ;
	}

protected:
	double m_t0; // t0 = tanh start;
	double m_tup;
};

/** @class SignalRicker
 *  @brief a ricker signal
 *
 *	A Ricker signal has a zero DC
 *	component, with a gaussian-like 
 *	frequency spectrum.
 *
 */
class SignalRicker : public SignalGenerator{
public:
	SignalRicker(double f0=0.0, double t0=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_f0 = f0;
		m_t0 = t0;
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {
		double param = SIGNALGENERATOR_PI*m_f0*(t-m_t0);
		return (t>m_toff ? 0.0 : m_magnitude*(1.0-2.0*param*param)*exp(-param*param));
	};
	void print_summary() const{
		cout << "Ricker: Mag=" << m_magnitude << " Toff=" << m_toff ;
		cout << " T0=" << m_t0 << " F0=" << m_f0;
	}

	double freq_min() const {return 0.0;};
	double freq_max() const {return 3.0*m_f0;};

protected:
	double m_f0, m_t0; // t0 = ricker start;
};

/** @class SignalHeaviside
 *  @brief Heaviside step signal
 *
 *  
 */
class SignalHeaviside : public SignalGenerator{
public:
	SignalHeaviside(double t0=0.0, double magnitude = 1.0, double toff = numeric_limits<double>::max())
	{
		m_t0 = t0;
		m_magnitude = magnitude;
		m_toff = toff;
	}

	double value(double t) const {return ((t>m_toff || t<m_t0) ? 0.0 : m_magnitude);};
	void print_summary() const{
		cout << "Heaviside: Mag=" << m_magnitude << " Toff=" << m_toff ;
		cout << " T0=" << m_t0 ;
	}

protected:
	double m_t0; // t0 = heaviside start;
};


#endif
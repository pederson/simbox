/** @file SignalProcessing.hpp
 *  @brief file with SignalProcessing classes
 *
 *  This contains the SignalProcessing classes
 *  and associated functions, defs, and enums
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _SIGNALPROCESSING_H
#define _SIGNALPROCESSING_H

#include <math.h>
#include <cmath>
#include <iostream>
#include <complex>

/** @class StreamProcessing
 *  @brief sequential processing
 *
 *  StreamProcessing functions are meant
 *	to do signal processing in a sequential
 *	fashion (instead of in a batch)
 *
 */
namespace StreamProcessing 
{

	class dft{
	public:
		//dft();

		dft(double freq, double dt){
			m_freq = freq;
			m_dt = dt;
			m_transform = std::complex<double> (0.0, 0.0);
			m_kernel = exp(-2.0*M_PI*m_imag*m_freq*m_dt);
			m_n = 0;

			m_addval = pow(m_kernel, m_n);
		}

		// dft(const dft & d)
		// : m_freq(d.m_freq)
		// , m_dt(d.m_dt)
		// , m_transform(d.m_transform)
		// , m_kernel(d.m_kernel)
		// , m_n(d.m_n)
		// , m_addval(d.m_addval)
		// {
		// }

		dft& operator=(const dft & d)
		{
			m_freq = d.m_freq;
			m_dt = d.m_dt;
			m_transform = d.m_transform;
			m_kernel = d.m_kernel;
			m_n = d.m_n;
			m_addval = d.m_addval;
			return *this;
		}

		// inspectors
		double freq() const {return m_freq;};
		double dt() const {return m_dt;};
		double n() const {return m_n;};
		std::complex<double> value() const {return m_dt*m_transform;};

		// mutators
		template <class T>
		void accumulate_advance(T val){
			// std::cout << "i: " << m_imag << std::endl;
			// std::cout << "addval: " << m_addval << " value: " << val << " transform: " << m_transform << std::endl;
			// std::cout << "kernel: " << m_kernel << " abs: " << abs(m_kernel) << std::endl;
			m_transform += val*m_addval;
			m_addval = std::pow(m_kernel, ++m_n);
			// std::cout << "new addval: " << m_addval << " abs: " << abs(m_addval) << std::endl;
			// throw -1;
		}
		template <class T>
		void accumulate_hold(T val){
			m_transform += val*m_addval;
		}
		void advance_time() {m_addval = std::pow(m_kernel, ++m_n);};

	private:
		double m_freq, m_dt;
		const std::complex<double> m_imag = std::complex<double> (0.0, 1.0);
		std::complex<double> m_kernel;
		std::complex<double> m_transform;

		unsigned int m_n;
		std::complex<double> m_addval;

	};

}

/** @class SignalProcessing
 *  @brief batch processing
 *
 *  SignalProcessing functions are meant
 *	to do signal processing in a batch
 *	fashion (when the full signal is 
 *	available from the beginning)
 *
 */
namespace SignalProcessing
{
	template <class T>
	void dft(double dt, double freq, unsigned int npoints, T * input_signal, std::complex<double> & transform){
		// create the kernel
		std::complex<double> pure_imag(0.0, 1.0);
		std::complex<double> kernel = exp(-2.0*M_PI*pure_imag*freq*dt);
		std::complex<double> addval;

		// accumuluate the dft
		transform = std::complex<double> (0.0, 0.0);
		for (auto n=0; n<npoints; n++){
			addval = pow(kernel, n);

			transform += input_signal[n]*addval;
		}
	}

	template <class T>
	void convolve(unsigned int npoints, T * input1, T * input2, T * output){
		if (output == nullptr){
			std::cout << "ERROR: SignalProccessing::convolve: Must initialize the output pointer!" << std::endl;
			throw -1;
		}

		for (auto j=0; j<npoints; j++){
			output[j] = 0.0;
			for (auto k=0; k<=j; k++){
				output[j] += input1[k]*input2[j-k];
			}
		}
	}


}

#endif
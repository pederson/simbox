/** @file Timer.hpp
 *  @brief Class for Timer
 *
 *  This contains the Timer class
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>

/** @class Timer
 *  @brief stopwatch timer
 *
 *  class for timing functions designed to
 *	imitate a stopwatch
 *
 */
class Timer{
public:
	Timer(){
		gettimeofday(&m_begin, nullptr);
		m_end = m_begin;
		m_running = false;
	}

	~Timer(){};

	// resume the timer
	void start(){
		m_running = true;
		gettimeofday(&m_begin, nullptr);
	}

	// stop the timer if running
	void stop(){
		m_running = false;
		gettimeofday(&m_end, nullptr);
	}

	// reset the timer to 0.0
	// If running, the timer will continue to run after reset
	void reset(){
		gettimeofday(&m_begin, nullptr);
		m_end = m_begin;
	}

	// read off the current time on the timer
	double read(){
		if (m_running) gettimeofday(&m_end, nullptr);
		return elapsed(m_begin, m_end);
	}

private:
	double elapsed(const timeval & start, const timeval & stop){
		double elap;
		elap = (stop.tv_sec - start.tv_sec);
		elap += (stop.tv_usec - start.tv_usec)/1000000.0;
		return elap;
	}

	timeval 		m_begin;
	timeval			m_end;
	bool 			m_running;		// if true, the timer is running
};

#endif
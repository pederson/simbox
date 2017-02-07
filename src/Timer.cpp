#include "Timer.hpp"

using namespace std;

Timer::Timer(){
	gettimeofday(&m_begin, nullptr);
	m_end = m_begin;
	m_tot=0;
}

Timer::~Timer(){

}

void Timer::start(){
	gettimeofday(&m_begin, nullptr);
}

void Timer::reset(){
	gettimeofday(&m_begin, nullptr);
	m_end = m_begin;
	m_tot=0;
}

void Timer::stop(){
	gettimeofday(&m_end, nullptr);
	m_tot += elapsed(m_begin, m_end);
}

double Timer::totaltime(){
	return m_tot;
}

double Timer::elapsed(const timeval & start, const timeval & stop){
	double elap;
	elap = (stop.tv_sec - start.tv_sec);
	elap += (stop.tv_usec - start.tv_usec)/1000000.0;
	return elap;
}
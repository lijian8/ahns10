/**
 * @file
 * @author Alex Wainwright
 * 
 * Last Modified by: $Author$
 * 
 * $LastChangedDate: 2009-10-17 16:10:27 +1000 (Sat, 17 Oct 2009) $
 * $Rev: 755 $
 * 
 * Queensland University of Technology
 * 
 * @section DESCRIPTION
 * The source file for a moving average class for the AHNS project. 
 */ 

#include "ahns_moving_average.h"

#include <iostream>
#include <iomanip>

//#define DEBUG

using namespace std;

/**
 * Constructor for the moving average.  Sets the length, initialises the head and tail indexes and sets all the buffer values to the initial estimate.
 * @param len The length of the filter.
 * @guess The The initial guess for the average. 
 */
moving_average::moving_average(int len, double guess)
{
	int ii;
	if(len>AVERAGE_MAX_LEN)
	{
		m_length = AVERAGE_MAX_LEN;
	}
	else
	{
		m_length = len;
	}

	m_value = guess;

	for (ii=0; ii<m_length; ii++)
	{
		m_buffer[ii] = guess;
	}
	m_head = 0;
	m_tail = m_length -1;
}

/**
 * Changes the length of the moving average.  
 * @note This implementation is not mathematically exact, as the moving average only stores the number of values needed, and not as many as the full buffer, so if the filter size is reduced it can't remember all the old values to average.  Instead, when reducing the size, the new buffer is filled with the current estimate and the moving average works from there.  When increasing the size, the elements are rearranged in the buffer so that the current elements are the oldest ones, and the remaining elements are filled with the current estimate.
 */
void moving_average::setLength(int input)
{
	int ii, el;
	if (input>AVERAGE_MAX_LEN)
	{
		input = AVERAGE_MAX_LEN;
	}
	if (input>m_length) //increasing the buffer length
	{
		//copy the original elements into the backup buffer
		for(ii=0; ii<m_length; ii++)
		{
			m_copy_buffer[ii] = m_buffer[ii];			
		}
		//copy them back into the average buffer, but in the order that god intended
		for(ii=(m_head+1);ii<(m_head+m_length+1);ii++)
		{
			el = ii%m_length;
			m_buffer[el] = m_copy_buffer[el];
		}
		//now fill the rest of the buffer with the current estimate. not mathematically perfect, but do you have a better idea?
		for(ii=m_length;ii<input;ii++)
		{
			m_buffer[ii] = m_value;
		}
	}
	else
	{
		for(ii=0; ii<input; ii++)
		{
			m_buffer[ii] = m_value;
		}
	}
	m_length = input;
}

/**
 * This function updates the moving average with a new value.
 * @param input The next value of the signal being averaged.
 * @return Returns the updated estimate, the filtered value. 
 */
double moving_average::update(double input)
{
	int ii;
	double tail = m_buffer[m_head];
	m_buffer[m_head] = input;

	m_value = m_value + m_buffer[m_head]/m_length - tail/m_length;

#ifdef DEBUG
	for (ii=0;ii<m_length;ii++)
	{
		std::cerr << "Element " << ii;
		std::cerr << ": " << std::setw(4) << m_buffer[ii] << std::endl;
	}
	std::cerr << std::endl;
#endif

	m_head++;
	m_head = m_head%m_length;
	return m_value;
}


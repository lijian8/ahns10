/**
 * @file
 * @author Alex Wainwright
 * 
 * Last modified by: $Author&
 * 
 * $LastChangedDate: 2009-10-17 16:10:27 +1000 (Sat, 17 Oct 2009) $
 * $Rev: 757 $
 * 
 * Queensland University of Technology
 * 
 * @section DESCRIPTION 
 * The header file for a moving average class, %moving_average implemented in %ahns_moving_average.cpp
 */

#ifndef AHNS_MOVING_AVERAGE
#define AHNS_MOVING_AVERAGE

#define AVERAGE_MAX_LEN 500  /// The maximum length of the filter.  This macro sets the buffer size. 

/**
 * This is a moving average class used to low-pass-filter streaming data. The one object per data stream is set up using moving_average() with the filter length and the initial estimate of the value.  Every time a new value is received, the average is updated with update() which returns the latest etsimate, or filtered value. setLength() can be used to change the filter length, and getLength() will return the current filter length. The current value of the filtered signal can be found without updating the value by calling value().
 */
class moving_average
{
	public:

		moving_average(int len, double guess = 0);
		double update(double input);
		void setLength(int input);

		/// @return The current length of the filter.
		int getLength(){return m_length;};

		/// @return The current filtered value.
		double value(){return m_value;};

	private:

		/// The current filter length.
		int m_length;				

		/// The buffer in which the signal is stored.
		double m_buffer[AVERAGE_MAX_LEN];	

		/// A buffer used to temporarily store data while the size of the filter is being changed. 
		double m_copy_buffer[AVERAGE_MAX_LEN];	

		/// The current filtered value, the current average.
		double m_value;				

		/// The index of the head (ring buffer).
		int m_head;
		
		/// The index of the tail. 
		int m_tail;				
};

#endif


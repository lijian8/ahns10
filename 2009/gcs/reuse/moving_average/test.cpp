// the test driver for moving average

using namespace std;

#include "ahns_moving_average.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int ii, len = 30;

	double val;
	double sequence [len];
	
	for (ii=0;ii<len;ii++)
	{
		sequence[ii] = ii%10;
	}		

	cout << "Welcome." << endl;

	moving_average myAverage(5, 0);

	for (ii=0; ii<len/3; ii++)
	{
		cout << "Enter value: ";
		cin >> val;
		cout << endl;
		cout << "The new value: " <<  sequence[ii] << ",   Estimate: " <<myAverage.update(val) << endl;
	}

	myAverage.setLength(10);

	for (ii=len/3; ii<2*len/3; ii++)
	{
		cout << "Enter value: ";
		cin >> val;
		cout << endl;
		cout << "The new value: " <<  sequence[ii] << ",   Estimate: " <<myAverage.update(val) << endl;
	}

	myAverage.setLength(15);

	
	for (ii=2*len/3; ii<len; ii++)
	{
		cout << "Enter value: ";
		cin >> val;
		cout << endl;
		cout << "The new value: " <<  sequence[ii] << ",   Estimate: " <<myAverage.update(val) << endl;
	}
}
	
	

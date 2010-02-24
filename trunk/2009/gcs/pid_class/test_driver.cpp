// test program for pid class
// A Wainwright
// AHNS 2009

#include "pid_class.h"
#include <iostream>

using namespace std;

int main(void)
{
	double p,i,d;
	cout << "Gains? p,i,d: ";
	cin >> p >> i >> d;
	ahnsPID loop1(1.01, 2.02, 3.03);
	double cmd, pos, t;	
	t = 0;

	while(1)
	{
		cout << "Enter cmd and position: ";
		cin >> cmd >> pos;

		cout << "\noutput: " << loop1.run(cmd,pos,t) << endl;
		
		t++;
	}
}

#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[])
{
  int i = 0;
  double Si = 0.0;
  double temp = 0.0;

  for(;;)
  {
    printf("Hello World\n");
    temp = (double) ++i*M_PI;   
    printf("%f\n",temp);
    Si = sin(temp); 
  }

  return 0;
}

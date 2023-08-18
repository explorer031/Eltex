#include <stdio.h>
#include <stdlib.h>

extern double mult(double x, double y);
extern double divis(double x, double y);

int main()
{
   double x = 0, y = 0;

   printf("Enter x: ");
   scanf("%lf", &x);
   printf("Enter y: ");
   scanf("%lf", &y);

   printf("\nMultiplication = %lf\n", mult(x, y));
   printf("Division       = %lf\n\n", divis(x, y));

   return EXIT_SUCCESS;
}
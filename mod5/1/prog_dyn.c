#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main()
{
   double x = 0, y = 0;

   printf("Enter x: ");
   scanf("%lf", &x);
   printf("Enter y: ");
   scanf("%lf", &y);

   void* lib_handler = dlopen ("libfuncsd.so", RTLD_LAZY);
   if (!lib_handler)
   {
      perror("Unable to open libfuncs.so");
      exit(EXIT_FAILURE);
   }

   double (*mult)(double x, double y) = dlsym(lib_handler, "mult");
   printf("\nMultiplication = %lf\n", (*mult)(x, y));

   double (*divis)(double x, double y) = dlsym(lib_handler, "divis");
   printf("Division       = %lf\n\n", (*divis)(x, y));

   dlclose(lib_handler);

   return EXIT_SUCCESS;
}
#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    real_t * d = (real_t *)malloc(n * sizeof(real_t));
    real_t * e = (real_t *)malloc(n * sizeof(real_t));
    real_t * f = (real_t *)malloc(n * sizeof(real_t));
    real_t * g = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = rand() / (real_t)(RAND_MAX / 10);
        f[x] = 0.0;
        g[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n], d[0:n], e[0:n]) copy(c[0:n], f[0:n], g[0:n])
    {
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                f[x] = d[x] + e[x];
            }
        }
        #pragma acc parallel wait(1, 2) async(3)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                g[x] = c[x] + f[x];
            }
        }
        #pragma acc wait(3)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(g[x] - (a[x] + b[x] + d[x] + e[x])) > PRECISION){
            err = 1;
        }
    }

    free(a);
    free(b);
    free(c);
    free(d);
    free(e);
    free(f);
    free(g);

    return err;
}


int main()
{
  int i;                        /* Loop index */
  int result;           /* return value of the program */
  int failed=0;                 /* Number of failed tests */
  int success=0;                /* number of succeeded tests */
  static FILE * logFile;        /* pointer onto the logfile */
  static const char * logFileName = "OpenACC_testsuite.log";        /* name of the logfile */


  /* Open a new Logfile or overwrite the existing one. */
  logFile = fopen(logFileName,"w+");

  printf("######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  printf("## Repetitions: %3d                       ####\n",REPETITIONS);
  printf("## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  printf("##############################################\n");
  printf("Testing parallel_async\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing parallel_async\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of parallel_async out of %d\n\n",i+1,REPETITIONS);
    if (test() == 0) {
      fprintf(logFile,"Test successful.\n");
      success++;
    } else {
      fprintf(logFile,"Error: Test failed.\n");
      printf("Error: Test failed.\n");
      failed++;
    }
  }

  if(failed==0) {
    fprintf(logFile,"\nDirective worked without errors.\n");
    printf("Directive worked without errors.\n");
    result=0;
  } else {
    fprintf(logFile,"\nDirective failed the test %i times out of %i. %i were successful\n",failed,REPETITIONS,success);
    printf("Directive failed the test %i times out of %i.\n%i test(s) were successful\n",failed,REPETITIONS,success);
    result = (int) (((double) failed / (double) REPETITIONS ) * 100 );
  }
  printf ("Result: %i\n", result);
  return result;
}

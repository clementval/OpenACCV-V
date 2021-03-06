#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    int * high = (int *)malloc(n * sizeof(int));
    int high_current_index = 0;
    size_t * a_points = (size_t *)malloc(n * sizeof(void *));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        if (a[x] > 5) {
            high[high_current_index] = x;
            high_current_index += 1;
        }
    }

    #pragma acc enter data copyin(a[0:n])

    #pragma acc host_data use_device(a)
    {
        int x = 0;
        for (x = 0; x < high_current_index; ++x){
            a_points[x] = (size_t) a + (high[x]*sizeof(real_t*));
        }
        for (; x < n; ++x){
            a_points[x] = 0;
        }
    }
    #pragma acc enter data copyin(a_points[0:n])
    #pragma acc parallel present(a[0:n], a_points[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            if (a_points[x] != 0){
                *((real_t *) a_points[x]) -= 5;
            }
        }
    }
    #pragma acc exit data delete(a_points[0:n]) copyout(a[0:n])
    for (int x = 0; x < n; ++x){
        if (a[x] < 0 || a[x] > 5) {
            err += 1;
            break;
        }
    }

    free(a);
    free(high);
    free(a_points);
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
  printf("Testing host_data\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing host_data\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of host_data out of %d\n\n",i+1,REPETITIONS);
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


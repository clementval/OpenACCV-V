#include "acc_testsuite.h"

int test(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int * dev_test = (int *)malloc(n * sizeof(int));

    dev_test[0] = 1;
    #pragma acc enter data copyin(dev_test[0:1])
    #pragma acc parallel present(dev_test[0:1])
    {
        dev_test[0] = 0;
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    acc_copyin(a, n * sizeof(real_t));
    acc_copyin(b, n * sizeof(real_t));

    #pragma acc data copy(c[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    acc_copyout(a, n * sizeof(real_t));
    acc_copyout(b, n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    acc_copyin(a, n * sizeof(real_t));
    acc_copyin(b, n * sizeof(real_t));

    #pragma acc data copy(c[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data copyout(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    if (dev_test[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            a_copy[x] = a[x];
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            b_copy[x] = b[x];
            c[x] = 0.0;
        }

        acc_copyin(a, n * sizeof(real_t));
        acc_copyin(b, n * sizeof(real_t));

        for (int x = 0; x < n; ++x){
            a[x] = 0;
            b[x] = 0;
        }

        #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
        {
            #pragma acc parallel
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
        }

        acc_copyout(a, n * sizeof(real_t));
        acc_copyout(b, n * sizeof(real_t));

        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - a_copy[x]) > PRECISION){
                err += 1;
            }
            if (fabs(b[x] - b_copy[x]) > PRECISION){
                err += 1;
            }
            if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
                err += 1;
            }
        }
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    acc_pcopyin(a, n * sizeof(real_t));
    acc_pcopyin(b, n * sizeof(real_t));

    #pragma acc data copyout(c[0:n]) present(a[0:n], b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    acc_present_or_copyin(a, n * sizeof(real_t));
    acc_present_or_copyin(b, n * sizeof(real_t));

    #pragma acc data copyout(c[0:n]) present(a[0:n], b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    free(a);
    free(a_copy);
    free(b);
    free(b_copy);
    free(c);
    return err;
}


int main()
{
  int i;			/* Loop index */
  int result;		/* return value of the program */
  int failed=0; 		/* Number of failed tests */
  int success=0;		/* number of succeeded tests */
  static FILE * logFile;	/* pointer onto the logfile */
  static const char * logFileName = "test_acc_lib_acc_wait.log";	/* name of the logfile */


  /* Open a new Logfile or overwrite the existing one. */
  logFile = fopen(logFileName,"w+");

  printf("######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  printf("## Repetitions: %3d                       ####\n",REPETITIONS);
  printf("## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  printf("##############################################\n");
  printf("Testing test_acc_lib_acc_wait\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing test_acc_lib_acc_wait\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of test_acc_lib_acc_wait out of %d\n\n",i+1,REPETITIONS);
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

#include "acc_testsuite.h"

int test(){
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_host = (real_t *)malloc(n * sizeof(real_t));
    real_t *b_host = (real_t *)malloc(n * sizeof(real_t));

    int *devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel
    {
        devtest[0] = 0;
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        a_host[x] = a[x];
        b_host[x] = b[x];
    }



    #pragma acc data copyout(c[0:n]) create(a[0:n], b[0:n])
    {
        acc_update_device_async(a, n * sizeof(real_t), 1);
        #pragma acc parallel async(1)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] = a[x] * a[x];
            }
        }
        acc_update_device_async(b, n * sizeof(real_t), 2);
        #pragma acc parallel async(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = b[x] * b[x];
            }
        }
        #pragma acc parallel async(1) wait(2)
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc wait(1)
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a_host[x] * a_host[x] + b_host[x] * b_host[x])) > PRECISION){
            err += 1;
        }
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            c[x] = 0;
        }

        #pragma acc data copyout(c[0:n]) copyin(a[0:n], b[0:n])
        {
            #pragma acc parallel async(1)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    a[x] = a[x] * a[x];
                }
            }
            acc_update_device_async(a, n * sizeof(real_t), 1);
            #pragma acc parallel async(2)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    b[x] = b[x] * b[x];
                }
            }
            acc_update_device_async(b, n * sizeof(real_t), 2);
            #pragma acc parallel async(1) wait(2)
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
            #pragma acc wait(1)
        }


        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
                err += 1;
            }
        }
    }

    free(a);
    free(b);
    free(c);
    free(a_host);
    free(b_host);
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int count = 0; //variable for execution #'s'

// handler function for attempting to divide by 0. 
void handle_SIGFPE()
{
  printf("Attempted to divide by zero! Program will now exit\n");
 
  exit(1);
}

//handler function for ctrl + c that also prints total #
// of executions. 
void handle_SIGINT()
{
    printf("\ntotal number of executions: %d\n", count);
    exit(1);
}

int main () {
    int i, j, solution;
    char k;
    char buffer [128]; //buffers for input
    char buffer2 [128];
    char buffer3 [128];
    
    struct sigaction new_handler, ctrlc_handler;//创建sigaction对象
    new_handler.sa_handler = handle_SIGFPE;
    if (sigemptyset(&new_handler.sa_mask) != 0) {
        printf("Sigemptyset error. Will now exit.");
        exit(1);
    }

    if ( (sigaction(SIGFPE, &new_handler, NULL) != 0) ) {
        printf("Sigaction error. Will now exit.");
        exit(1);
    }

    ctrlc_handler.sa_handler = handle_SIGINT;//绑定回调函数
    if (sigemptyset(&ctrlc_handler.sa_mask) !=0) {
        printf("Sigemptyset error. Will now exit.");
        exit(1);
    }

    if ((sigaction(SIGINT, &ctrlc_handler, NULL)) != 0) {
        printf("SigINT error. Will now exit.");
        exit(1);
    }

    while(1) {
        printf ("Enter a number: ");
        fgets (buffer, 128, stdin);

        printf ("Enter a 2nd number: ");
        fgets (buffer2, 128, stdin);

        printf ("Enter an operand: ");
        fgets (buffer3, 128, stdin);

        i =  atoi(buffer); // {
         // printf("atoi conversion error, will now exit.");
          //exit(1);
        //}

        j =  atoi(buffer2); //== 0) {
            if (j == 0) {
            printf("atoi conversion error, will now exit.");
          exit(1);
           }
        
        k = buffer3[0]; //first character in third input used as operand.

        switch (k) {
          case '+':
              solution = i + j;
              printf("%d\n", solution);
              count = count + 1;
              break;

          case '-':
              solution = i - j;
              printf("%d\n", solution);
              count = count + 1;
              break;

          case '*':
              solution = i * j;
              printf("%d\n", solution);
              count = count + 1;
              break;

          case '/':
              solution = i / j;
              printf("%d\n", solution);
              count = count + 1;
              break;    

          default:
              printf("Program read bad operand, defaulting to addition:\n");
              solution = i + j;
              printf("%d\n", solution);
              break;
        }      
    }
    return 0;
  }
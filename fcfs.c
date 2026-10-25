#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <stdlib.h>  
#include <sys/wait.h> 
#include <string.h> 
#include <time.h> 
#include <signal.h>
#include <sys/time.h>
#include <stdarg.h>

// include my fcfs case for logging
#define CASE "fcfs"

/************************************************************************************************ 
		These DEFINE statements represent the workload size of each task and 
		the time quantum values for Round Robin scheduling for each task.
*************************************************************************************************/

#define WORKLOAD1 100000
#define WORKLOAD2 50000
#define WORKLOAD3 25000
#define WORKLOAD4 10000

#define QUANTUM1 1000
#define QUANTUM2 1000
#define QUANTUM3 1000
#define QUANTUM4 1000

/************************************************************************************************ 
					DO NOT CHANGE THE FUNCTION IMPLEMENTATION
*************************************************************************************************/
void myfunction(int param){

	int i = 2;
	int j, k;

	while(i < param){
		k = i; 
		for (j = 2; j <= k; j++)
		{
			if (k % j == 0){
				k = k/j;
				j--;
				if (k == 1){
					break;
				}
			}
		}
		i++;
	}
}
/************************************************************************************************/

// Log outputs to terminal and add to log file (reused from PA1)
void log_printf(FILE *log, const char *format, ...) {
    va_list args;

    // Log to terminal
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // Log to file
    va_start(args, format);
    vfprintf(log, format, args);
    va_end(args);

    fflush(log);
}

// Main function
int main(int argc, char const *argv[])
{
	pid_t pid1, pid2, pid3, pid4;
	int running1, running2, running3, running4;

	pid1 = fork();

	if (pid1 == 0){

		myfunction(WORKLOAD1);

		exit(0);
	}
	kill(pid1, SIGSTOP);

	pid2 = fork();

	if (pid2 == 0){

		myfunction(WORKLOAD2);

		exit(0);
	}
	kill(pid2, SIGSTOP);

	pid3 = fork();

	if (pid3 == 0){

		myfunction(WORKLOAD3);

		exit(0);
	}
	kill(pid3, SIGSTOP);

	pid4 = fork();

	if (pid4 == 0){

		myfunction(WORKLOAD4);

		exit(0);
	}
	kill(pid4, SIGSTOP);

	/************************************************************************************************ 
		At this point, all  newly-created child processes are stopped, and ready for scheduling.
	*************************************************************************************************/



	/************************************************************************************************
		- Scheduling code starts here
		- Below is a sample schedule. (which scheduling algorithm is this?)
		- For the assignment purposes, you have to replace this part with the other scheduling methods 
		to be implemented.
	************************************************************************************************/

    // Logging code setup (reused from PA1)
	FILE *log = fopen(CASE ".log", "w");

    if (log == NULL)
    {
        perror("fopen");
        return 1;
    }
    log_printf(log, "STARTING %s\n", CASE);

    // Initialize process IDs and workloads
    pid_t pids[4] = {pid1, pid2, pid3, pid4};

    int workloads[4] = {WORKLOAD1, WORKLOAD2, WORKLOAD3, WORKLOAD4};
    double response_time[4];
    int completed = 0;
    double total_response_time = 0.0;

    struct timespec ready_time;
    struct timespec dispatch_time;

    clock_gettime(CLOCK_MONOTONIC, &ready_time);

    // schedule all 4 procs w/ fcfs
    while (completed < 4) {
        int status;

        
        // Calculate response time in us
        clock_gettime(CLOCK_MONOTONIC, &dispatch_time);
        double seconds = (double)(dispatch_time.tv_sec - ready_time.tv_sec);
        double nanoseconds = (double)(dispatch_time.tv_nsec - ready_time.tv_nsec);
        response_time[completed] = seconds * 1000000.0 + nanoseconds / 1000.0;

        log_printf(log, "Dispatching process %d, workload = %d, response time = %.3f us\n", completed + 1, workloads[completed], response_time[completed]);

        kill(pids[completed], SIGCONT); // resume proc
        waitpid(pids[completed], &status, 0); // wait for proc

        total_response_time += response_time[completed];
        completed++;
    }

    double average_response_time = total_response_time / 4.0;
    log_printf(log, "\nFCFS response times:\n");

    for (int i = 0; i < 4; i++) {
        log_printf(log, "Process %d, workload = %d: %.3f microseconds\n", i + 1, workloads[i], response_time[i]);
    }

    log_printf(log, "Average response time: %.3f microseconds\n", average_response_time);
    log_printf(log, "FINISHED %s\n", CASE);
    fclose(log);

	/************************************************************************************************
		- Scheduling code ends here
	************************************************************************************************/

	return 0;
}
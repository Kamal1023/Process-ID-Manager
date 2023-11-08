#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#define MAX_PIDS 100
#define MAX_QUEUE_SIZE 10
#define TIME_SLICE 3
#define CHANCE_OF_IO_REQUEST 10
#define CHANCE_OF_IO_COMPLETE 4
typedef struct
{
  
int pid;
   
int arrival_time;
   
int service_time;
   
int priority;
   
int remaining_time;
   
int state;			// 0: ready to run, 1: sleeping on I/O, 2: swapped out
} Process;

typedef struct
{
  
Process processes[MAX_PIDS];
  
int count;
 
} ProcessList;

typedef struct
{
  
Process * queue[MAX_QUEUE_SIZE];
  

int front;
   
int rear;
 
} ProcessQueue;

void
initializeQueue (ProcessQueue * queue)
{
  
queue->front = 0;
  
queue->rear = -1;

} 
int

isQueueEmpty (ProcessQueue * queue)
{
  
return queue->rear < queue->front;

}


int
isQueueFull (ProcessQueue * queue)
{
  
return queue->rear == MAX_QUEUE_SIZE - 1;

}


void
enqueue (ProcessQueue * queue, Process * process)
{
  
if (isQueueFull (queue))
    {
      
printf ("Queue is full. Process cannot be enqueued.\n");
      
return;
    
}
  
queue->rear++;
  
queue->queue[queue->rear] = process;

}


Process * dequeue (ProcessQueue * queue)
{
  
if (isQueueEmpty (queue))
    {
      
printf ("Queue is empty. No process to dequeue.\n");
      
return NULL;
    
}
  
Process * process = queue->queue[queue->front];
  
 
queue->front++;
  
return process;

}


void
addProcess (ProcessList * processList, int pid, int arrival_time,
	    int service_time, int 
priority)
{
  
if (processList->count >= MAX_PIDS)
    {
      
printf
	("Maximum number of processes reached. Process cannot be added.\n");
      
return;
    
}
  
Process * process = &(processList->processes[processList->count]);
  
process->pid = pid;
  
process->arrival_time = arrival_time;
  
process->service_time = service_time;
  
process->priority = priority;
  
process->remaining_time = service_time;
  
process->state = 0;		// Ready to run
  processList->count++;

}


int
IO_request ()
{
  
if (rand () % CHANCE_OF_IO_REQUEST == 0)
    
return 1;
  
  else
    
return 0;

}


int
IO_complete ()
{
  
if (rand () % CHANCE_OF_IO_COMPLETE == 0)
    
return 1;
  
  else
    
return 0;

 
}


void
printProcessStats (ProcessList * processList)
{
  
printf (" | Total time | Total time | Total time \n");
  
printf ("Process# | in ready to run | in sleeping on | in system\n");
  
printf (" | state | I/O state |\n");
  
 
printf
    ("=========+=================+=================+===============\n");
  
for (int i = 0; i < processList->count; i++)
    {
      
Process * process = &(processList->processes[i]);
      
printf (" pid%d | %d | %d | %d \n", process->pid,
	       process->arrival_time, process->arrival_time,
	       process->arrival_time);
    
}
    
 
printf
    ("=============================================================\n");

} 
void

simulate (ProcessList * processList, int schedulingAlgorithm, int verbose)
{
  
ProcessQueue queues[5];
  
for (int i = 0; i < 5; i++)
    {
      
initializeQueue (&(queues[i]));
    
} 
int clock = 0;
  
int total_elapsed_time = 0;
  
int total_processes = processList->count;
  
int shortest_completion_time = processList->processes[0].service_time;
  
int longest_completion_time = processList->processes[0].service_time;
  
int total_completion_time = 0;
  
srand (1);			// Seed random number generator
  
while (processList->count > 0 && clock < 1000)
    {
      
	// Step 1: Add new incoming processes to the ready to run state
	for (int i = 0; i < processList->count; i++)
	{
	  
Process * process = &(processList->processes[i]);
	  
if (process->arrival_time == clock)
	    {
	      
enqueue (&(queues[process->priority]), process);
	    
}
	
}
      
	// Step 2: Choose process to execute based on scheduling algorithm
	Process * current_process = NULL;
      
if (schedulingAlgorithm == 0)
	{			// Prioritized round robin
	  for (int i = 0; i < 5; i++)
	    {
	      
if (!isQueueEmpty (&(queues[i])))
		{
		  
current_process = dequeue (&(queues[i]));
		  
break;
		
}
	    
}
	
}
      else if (schedulingAlgorithm == 1)
	{			// Shortest remaining time
	  int min_remaining_time = INT_MAX;
	  
int min_remaining_time_priority = -1;
	  
for (int i = 0; i < 5; i++)
	    {
	      
if (!isQueueEmpty (&(queues[i])))
		{
		  
Process * process = queues[i].queue[queues[i].front];
		  
if (process->remaining_time < min_remaining_time)
		    {
		      
min_remaining_time = process->remaining_time;
		      
min_remaining_time_priority = i;
		    
}
		
}
	    
}
	  
if (min_remaining_time_priority != -1)
	    {
	      
current_process =
		dequeue (&(queues[min_remaining_time_priority]));
	    
 
}
	
}
      
	// Step 3: Process current process
	if (current_process != NULL)
	{
	  
	    // Check if process has finished its work
	    if (current_process->remaining_time == 1)
	    {
	      
current_process->state = 2;	// Swapped out (process complete)
	      total_completion_time += current_process->service_time;
	      
if (current_process->service_time < shortest_completion_time)
		{
		  
shortest_completion_time = current_process->service_time;
		
}
	      
if (current_process->service_time > longest_completion_time)
		{
		  
longest_completion_time = current_process->service_time;
		
}
	      
processList->count--;
	      
current_process = NULL;
	    
}
	  else
	    {
	      
		// Check for higher priority processes
	      int higher_priority_exists = 0;
	      
for (int i = 0; i < current_process->priority; i++)
		{
		  
if (!isQueueEmpty (&(queues[i])))
		    {
		      
higher_priority_exists = 1;
		      
break;
		    
}
		
}
	      
if (higher_priority_exists)
		{
		  
current_process->state = 2;	// Swapped out (preempted)
		}
	      else
		{
		  
		    // Check for I/O request
		    if (current_process->remaining_time != 0)
		    {
		      
int io_request = IO_request ();
		      
 
if (io_request)
			{
			  
current_process->state = 1;	// Sleeping on I/O
			}
		    
}
		  
		    // Check if timeslice has expired
		    if (current_process->remaining_time != 0
			&& clock % TIME_SLICE == 0)
		    {
		      
current_process->state = 2;	// Swapped out (end of timeslice)
		      enqueue (&(queues[current_process->priority]),
			       current_process);
		      
current_process = NULL;
		    
}
		
}
	    
}
	
}
      
	// Step 4: Check for I/O completion
	for (int i = 0; i < 5; i++)
	{
	  
ProcessQueue * io_queue = &(queues[i]);
	  
while (!isQueueEmpty (io_queue))
	    {
	      
Process * process = io_queue->queue[io_queue->front];
	      
int io_complete = IO_complete ();
	      
if (io_complete)
		{
		  
process->state = 0;	// Ready to run
		  enqueue (&(queues[process->priority]), dequeue (io_queue));
		
}
	      else
		{
		  
break;
		
}
	    
}
	
}
      
	// Step 5: Update statistics
	clock++;
      
 
if (current_process != NULL)
	{
	  
current_process->remaining_time--;
	
}
      
	// Step 6: Print verbose output
	if (verbose)
	{
	  
printf ("%d:%d:%d:%s:", clock,
		   current_process != NULL ? current_process->pid : 
-1,
		   
current_process !=
		   NULL ? current_process->remaining_time : -1,
		   
current_process != NULL
		   && current_process->remaining_time !=
		   0 ? "true" 
 : "false");
	  
int io_completed_processes[5];
	  
int io_completed_processes_count = 0;
	  
for (int i = 0; i < 5; i++)
	    {
	      
if (!isQueueEmpty (&(queues[i])))
		{
		  
Process * process = queues[i].queue[queues[i].front];
		  
if (process->state == 1)
		    {
		      
io_completed_processes[io_completed_processes_count] =
			process->pid;
		      
io_completed_processes_count++;
		    
}
		
}
	    
}
	  
if (io_completed_processes_count > 0)
	    {
	      
for (int i = 0; i < io_completed_processes_count; i++)
		{
		  
printf ("%d,", io_completed_processes[i]);
	    
} 
}
	  else
	    {
	      
printf ("none");
	    
}
	  
 
if (current_process != NULL)
	    {
	      
if (current_process->state == 2)
		{
		  
printf (":preempted\n");
		
}
	      else if (current_process->state == 1)
		{
		  
printf (":sleeping\n");
		
}
	      else
		{
		  
printf (":still running\n");
		
}
	    
}
	  else
	    {
	      
printf (":none\n");
	    
}
	
}
    
}
  
    // Step 7: Print process statistics
    printProcessStats (processList);
  
    // Print overall simulation statistics
    printf ("Total simulation run time: %d\n", clock - 1);
  
printf ("Total number of processes: %d\n", total_processes);
  
printf ("Shortest process completion time: %d\n",
	   shortest_completion_time);
  
printf ("Longest process completion time: %d\n", longest_completion_time);
  
printf ("Average process completion time: %.2f\n",
	   (float) total_completion_time / 
total_processes);

} 
int

main (int argc, char *argv[])
{
  
int schedulingAlgorithm = 0;	// Default: Prioritized round robin
  int verbose = 0;
  
int numProcessIDs;
  
 
    // Parse command-line arguments
    if (argc > 1)
    {
      
if (strcmp (argv[1], "-L") == 0)
	{
	  
schedulingAlgorithm = 1;	// Shortest remaining time
	}
      else if (strcmp (argv[1], "-v") == 0)
	{
	  
verbose = 1;
	
}
    
}
  
    // Ask the user for the number of process IDs to manage
    printf ("Enter the number of process IDs to manage: ");
  
scanf ("%d", &numProcessIDs);
  
    // Read process data from stdin
    ProcessList processList;
  
processList.count = 0;
  
int pid, arrival_time, service_time, priority;
  
int i = 0;
  
while (i < numProcessIDs
	  &&
	  printf
	  ("Enter details for Process %d (pid arrival_time service_time priority): ",
	   i + 1)
	  && 
scanf ("%d %d %d %d", &pid, &arrival_time, &service_time,
		     &priority) == 4)
    {
      
addProcess (&processList, pid, arrival_time, service_time, priority);
      
i++;
    
}
  
    // Simulate process ID management
    simulate (&processList, schedulingAlgorithm, verbose);
  
return 0;

}



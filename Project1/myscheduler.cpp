//myschedule.cpp
/*Define all the functions in 'myschedule.h' here.*/
#include "myscheduler.h"


/*basic strategy: have 2 vector structures, stack and queue
*	The stack vector holds all threads initially and orders them by lowest arrival time.
*
*	Since the threads are ordered by arrival in stack and each arrival time is unique
*	the top value of stack will be the next thrread to become available.
*
*	The queue holds the threads that have already arrived, in the order specific to its policy.
*
*	Because we know when threads become available we can easily insert them into the queue for 
*	easy access for the assignCPUs() function.
*
*	CreateThread()-> if remaining_time > 0 then create and fill a node and then place 
*						the node in the stack in order of ascending arrival_time. (completed)
*
*	    Dispatch()-> Check if thread on top of stack is arriving, if so place thread in queue
*						based on the present policy, and finally call assign CPU's
*					Policies:	First Come First Serve-(Completed)
*								Shortest Time Remaining First, without preemption
*								Shortest Time Remaining First, with preemption
*								Priority Based Scheduling, with preemption
*
*	  assignCPUs()-> iterates through the CPUs and assigns the data from the mirror node in queue
*						checks if a thread is dead and if so disposes of the thread
*						and replaces with the next in line(completed)
*/



// create and fill a node
// place in the stack by order of arival time
void MyScheduler::CreateThread(int arriving_time, int remaining_time, int priority, int tid) //Thread ID not Process ID
{
	if (remaining_time <= 0);
		return;

	//for loop iterater
	int i;

	//create a new node
	ThreadDescriptorBlock* node = new ThreadDescriptorBlock();

	//fill the node
	node->priority = priority;
	node->tid = tid;
	node->remaining_time = remaining_time;
	node->arriving_time = arriving_time;
	
	if (stack.size() == 0)
		stack.insert(stack.begin(), node);
	else
	{

		// checks if created node is less than each value 
		for (i = 0; i < stack.size(); i++)
		{
			if (node->arriving_time < stack.at(i)->arriving_time)
			{
				stack.insert(stack.begin() + i, node);
				i = stack.size() + 1;
			}
		}// stack is now sorted by incoming


	}
}

bool MyScheduler::Dispatch()
{
	int i;
	ThreadDescriptorBlock *node;
	ThreadDescriptorBlock *temp= NULL;
	bool newNode = false;
	bool prempt = false;

	if (stack.at(0)->arriving_time == timer)
	{
		node = stack.at(0);
		stack.erase(stack.begin());
		newNode = true;
	}




	//Todo: Check and remove finished threads - all finished threads are removed after 
	//Todo: Check if all the threads are finished; if so, return false
	if (newNode)
	{
		switch (policy)
		{
//First Come First Serve: 
//		Since stack already is sorted in order of time,if newNode==true then node has just arrived 
//		and all elements in queue have faster arrival time so just add to end of queue
		case FCFS:

			if (num_cpu < queue.size())		queue.push_back(node);
			else
			{
				for (i = 0; i < num_cpu; i++)
				{
					if (queue.at(i) == NULL)
					{
						newNode = false;
						queue.at(i) = node;
						i = num_cpu + 1;
					}
				} if (newNode) queue.push_back(node);
			}

			break;


//Shortest Time Remaining First, without preemption
//		Place node into vector in order of shortest remaining time
//		If num_cpu< queue_size, start loop after num_cpu to take care of non premption
//		else put into the first null value.
//
		case STRFwoP:

			if (num_cpu < queue.size())//no open cpus 
			{
				for (i = num_cpu; i < queue.size(); i++)
				{
					if (node->remaining_time != queue.at(i)->remaining_time)// multiple with same time will move current to back

						if (node->remaining_time < queue.at(i)->remaining_time)
						{
							queue.insert(queue.begin() + i, node);
							i = queue.size() + 1; // exits for loop
						}


				}

			} else 
			{
				for (i = 0; i < num_cpu; i++)
				{
					if (queue.at(i) == NULL )
					{
						newNode = false;
						queue.at(i) = node;
						i = num_cpu + 1;

					}
				}if (newNode) queue.push_back(node);
			}

			break;




		case STRFwP:	//Shortest Time Remaining First, with preemption
			if (num_cpu < queue.size())//no open cpus 
			{
				for (i = 0; i < queue.size(); i++)
				{
					if (node->remaining_time != queue.at(i)->remaining_time ||queue.at(i)==NULL)// multiple with same time will move current to back
						if (node->remaining_time < queue.at(i)->remaining_time || queue.at(i) == NULL)
						{
						if (i < num_cpu) //prempt: replace current cpu thread with node and keep going through the loop with the displaced thread
						{
							temp = queue.at(i);
							queue.at(i) = node;
							node = temp;
							temp = NULL;
						} else // no prempt
						{
							queue.insert(queue.begin() + i, node);
							i = queue.size() + 1; // exits for loop					
						}
						
						}


				}

			}
			else
			{
				for (i = 0; i < num_cpu; i++)
				{
					if (queue.at(i) == NULL)
					{
						newNode = false;
						queue.at(i)= node;
						i = num_cpu + 1;

					}if (newNode) queue.push_back(node);
				}
			}
			
			
			
			
			
			
			
			
			break;

		case PBS:		//Priority Based Scheduling, with preemption
			if (num_cpu < queue.size())//no open cpus 
			{
				for (i = 0; i < queue.size(); i++)
				{
					if (node->priority != queue.at(i)->priority || queue.at(i) == NULL)// multiple with same time will move current to back
					{
						if (node->priority < queue.at(i)->priority || queue.at(i) == NULL)
						{
							if (i < num_cpu) //prempt: replace current cpu thread with node and keep going through the loop with the displaced thread
							{
								temp = queue.at(i);
								queue.at(i) = node;
								node = temp;
								temp = NULL;
							}
							else // no prempt
							{
								queue.insert(queue.begin() + i, node);
								i = queue.size() + 1; // exits for loop					
							}

						}
					}

				}

			}
			else
			{
				for (i = 0; i < num_cpu; i++)
				{
					if (queue.at(i) == NULL)
					{
						newNode = false;
						queue.at(i) = node;
						i = num_cpu + 1;

					}if (newNode) queue.push_back(node);
				}
			}

			break;
		default:
			cout << "Invalid policy!";
			throw 0;
		}
	}


	if (queue.size() > 0)
	{
		return assignCPUs();
		
	}
	//else
	if (stack.size() > 0)
	{
		return true; //nothing in queue but threads are waiting to arrive so wait but dont 
	}
	//else
	return true;
}
bool MyScheduler:: assignCPUs()
{
	vector<int> pop;
	int i;
	int null_count = 0;
	for (i = 0; i < num_cpu; i++)
	{
		if (i > queue.size() - 1)
		{
			CPUs[i] = NULL;
			null_count++;
		}
		else
		{

			CPUs[i] = queue.at(i);
			if (CPUs[i]->remaining_time <= 1)//this is the last iteration for this thread
				pop.push_back(i); //save the position of the dead thread for removal after for loop so nothing screws up
		}
	}
	if (null_count >= num_cpu)
		return false;

	for (i = 0; i < pop.size(); i++)
	{
		if (pop.at(i) < num_cpu)
		{
			if (queue.size() >= num_cpu)//at least 1 more thread on deck
			{
				queue.at(pop.at(i)) = queue.at(num_cpu);// replace with on deck at position num_cpu
				queue.erase(queue.begin() + num_cpu); //removal of original on deck after its been copied

			}
			else queue.at(pop.at(i)) = NULL; //no on decks

		} else return false; // if reaches something screwed up but IDK what
	}

}


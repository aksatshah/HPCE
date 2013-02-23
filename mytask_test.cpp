#include "tbb/tbb.h"
#include <iostream>
#include <math.h>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

using namespace tbb;
using namespace std;

class MyTask 
{
public: 
   int start, end;
   
   MyTask(int _start, int _end)
   { start=_start;  end=_end; }
   
   tbb::task * execute()
   {
       set_ref_count(3);
       MyTask &t1=*new(allocate_child()) MyTask(start,(start+end)/2); 
       spawn(t1);
       MyTask &t2=*new(allocate_child()) MyTask((start+end)/2, end);
       spawn(t2);
       wait_for_all();
   }
}; 
void CreateTasks(int start, int end)
{
   MyTask &root=*new(tbb::task::allocate_root()) MyTask(start,end);
   tbb::task::spawn_root_and_wait();
}
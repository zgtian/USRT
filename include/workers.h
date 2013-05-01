#ifndef USRT_WORKERS_H
#define USRT_WORKERS_H
#include <map>
#include <string>
#include <capability.h>
#include <CPBuffer.h>
#include <usrttype.h>
#include <MapMem.h>

namespace std {
  class workers {
    #define HEAPSIZE 256
    typedef int(*FuncCompare)(task_t*,task_t*);
    struct structHeap {
      int size;
      raw_spinlock_t lock;
      task_t* heap[HEAPSIZE];
      FuncCompare func;
    };
    struct structWorkHead {
      struct structHeap wait;
      struct structHeap ready;
      task_t card;
    };
       
    private:
      map <int64,CCapability *> mCapabilities;
      CPBuffer *buf;

    public:
      workers( int i );
      void start();
      ~workers();
      struct structWorkHead *head;
      task_t* pop( struct structHeap& h );
      int insert( struct structHeap& h, task_t* a );
      void down(struct structHeap& h, int index );
      void up(struct structHeap& h, int index );
      int heapCheck(struct structHeap& h, int debug );
      void dumpTaskTime( task_t * a );  
#if 0
      int init( int argc, char *argv[] );
      runAsThread();
      bindCpu();
#endif
  };
}
#endif

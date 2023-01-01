
#include "tcp.h"
#include <string.h>
#include "queue.h"
#include "config.h"
#include<stdio.h>



/*
 * A bounded, drop-tail queue
 */
class lhqueue : public Queue {
  public:
	lhqueue() { 
		q_ = new PacketQueue; 
		pq_ = q_;
        mean_pktsize_=1500;
        thresh=256*1024;//lh thresh in bytes,256KB
        lfThresh=256*1024;//longflow thresh in bytes
        buffer=256*1024;//lh total buffer
        rebuffer=buffer;//lh remain buffer
        usedBuffer=0;
        lfUsedBuffer=0;
		sfUsedBuffer=0;
		last_time=0;
		now_time=0;
        islf=false;
		print_interval=0.00001;
	    filename ="/home/dell3/lh/ns-allinone-2.35/ns-2.35/buffer.txt";
		fptr=fopen(filename,"w");
		bind_bool("drop_front_", &drop_front_);
		bind_bool("summarystats_", &summarystats);
		bind_bool("queue_in_bytes_", &qib_);  // boolean: q in bytes?
		bind("mean_pktsize_", &mean_pktsize_);
		//		_RENAMED("drop-front_", "drop_front_");

	}
	~lhqueue() {
		delete q_;
	}
  protected:
	void reset();
	int command(int argc, const char*const* argv); 
	void enque(Packet*);
	Packet* deque();
	void shrink_queue();	// To shrink queue and drop excessive packets.
    int flowid;//lh flowid
    int thresh;//lh thresh in bytes
    int lfThresh;//lh longflow thresh in bytes
    int buffer;
    int rebuffer;
    int usedBuffer;
    int lfUsedBuffer;
	int sfUsedBuffer;
    bool islf;//lh if this packet belong longflow
	PacketQueue *q_;	/* underlying FIFO queue */
	int drop_front_;	/* drop-from-front (rather than from tail) */
	int summarystats;
	void print_summarystats();
	int qib_;       	/* bool: queue measured in bytes? */
	int mean_pktsize_;	/* configured mean packet size in bytes */
	double last_time;
	double now_time;
	double print_interval;
	char* filename ;
	FILE* fptr;
};

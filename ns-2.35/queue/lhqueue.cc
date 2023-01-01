#include "lhqueue.h"

static class lhqueueClass : public TclClass {
 public:
	lhqueueClass() : TclClass("Queue/lhqueue") {}
	TclObject* create(int, const char*const*) {
		return (new lhqueue);
	}
} class_drop_tail;

void lhqueue::reset()
{
	Queue::reset();
}

int 
lhqueue::command(int argc, const char*const* argv) 
{
	if (argc==2) {
		if (strcmp(argv[1], "printstats") == 0) {
			print_summarystats();
			return (TCL_OK);
		}
 		if (strcmp(argv[1], "shrink-queue") == 0) {
 			shrink_queue();
 			return (TCL_OK);
 		}
	}
	if (argc == 3) {
		if (!strcmp(argv[1], "packetqueue-attach")) {
			delete q_;
			if (!(q_ = (PacketQueue*) TclObject::lookup(argv[2])))
				return (TCL_ERROR);
			else {
				pq_ = q_;
				return (TCL_OK);
			}
		}
	}
	return Queue::command(argc, argv);
}

/*
 * drop-tail
 */

void lhqueue::enque(Packet* p)
{
    
    if(hdr_cmn::access(p)->size()<500){
         q_->enque(p);
         return;
    }

    now_time=Scheduler::instance().clock();
    if(now_time-last_time>print_interval){
        fprintf(fptr,"%lf %d %d %d %d\n",now_time,lfUsedBuffer/1024,sfUsedBuffer/1024,(lfUsedBuffer+sfUsedBuffer)/1024,lfThresh/1024);
        last_time=now_time;
    } 
    //qlim_=100000;
    hdr_tcp *tcph = hdr_tcp::access(p);
	flowid = tcph->flowid();
   // printf("flowid=%d,packetsize=%d\n",flowid,hdr_cmn::access(p)->size());
    if(flowid<15){
        islf=true;
    }else{
        islf=false;
    }
    //printf("%d\n",hdr_cmn::access(p)->size());
    if(buffer>hdr_cmn::access(p)->size()+usedBuffer){
        if(islf){
            if(lfUsedBuffer+hdr_cmn::access(p)->size()>lfThresh){//protect longflow
            //if(usedBuffer+hdr_cmn::access(p)->size()>lfThresh){//don't protect longflow
                drop(p);
                return;
            }else{
                q_->enque(p);
                usedBuffer=usedBuffer+hdr_cmn::access(p)->size();
                lfUsedBuffer=lfUsedBuffer+hdr_cmn::access(p)->size();
                lfThresh=buffer-usedBuffer;//remember to magnify two lines
             
                if(lfThresh<=30*1024){
                    lfThresh=30*1024;
                }
                return;
            }


        }else{
            usedBuffer=usedBuffer+hdr_cmn::access(p)->size();
            sfUsedBuffer=sfUsedBuffer+hdr_cmn::access(p)->size();

            q_->enque(p);
            lfThresh=buffer-usedBuffer;
          
            if(lfThresh<=30*1024){
                lfThresh=30*1024;
            }
            return;
        }
 
    }else{
        drop(p);
        return;
    }
}

//AG if queue size changes, we drop excessive packets...
void lhqueue::shrink_queue() 
{
        int qlimBytes = qlim_ * mean_pktsize_;
	if (debug_)
		printf("shrink-queue: time %5.2f qlen %d, qlim %d\n",
 			Scheduler::instance().clock(),
			q_->length(), qlim_);
        while ((!qib_ && q_->length() > qlim_) || 
            (qib_ && q_->byteLength() > qlimBytes)) {
                if (drop_front_) { /* remove from head of queue */
                        Packet *pp = q_->deque();
                        drop(pp);
                } else {
                        Packet *pp = q_->tail();
                        q_->remove(pp);
                        drop(pp);
                }
        }
}

Packet* lhqueue::deque()
{
    // if (summarystats && &Scheduler::instance() != NULL) {
    //     Queue::updateStats(qib_?q_->byteLength():q_->length());
    // }
    if(q_->length()>0){
        Packet* p=q_->deque();
        int qlength=hdr_cmn::access(p)->size();
        // printf("%d\n",length);
        if(hdr_cmn::access(p)->size()<500){
            return p;
        }else{
            hdr_tcp *tcph = hdr_tcp::access(p);
	        flowid = tcph->flowid();
            //printf("flowid=%d,packetsize=%d\n",flowid,hdr_cmn::access(p)->size());
            if(flowid<15){
               islf=true;
            }else{
               islf=false;
            }
            if(islf){
                lfUsedBuffer=lfUsedBuffer-qlength;
                usedBuffer=usedBuffer-qlength;
            }else{
                sfUsedBuffer=sfUsedBuffer-qlength;
                usedBuffer=usedBuffer-qlength;
            }
        }
        return p;
    }
    else{
        return q_->deque();
    }
}

void lhqueue::print_summarystats()
{
	//double now = Scheduler::instance().clock();
        printf("True average queue: %5.3f", true_ave_);
        if (qib_)
                printf(" (in bytes)");
        printf(" time: %5.3f\n", total_time_);
}
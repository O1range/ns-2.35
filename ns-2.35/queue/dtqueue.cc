#include "dtqueue.h"

static class DTQueueClass : public TclClass {
 public:
	DTQueueClass() : TclClass("Queue/DTQueue") {}
	TclObject* create(int, const char*const*) {
		return (new DTQueue);
	}
} class_drop_tail;

void DTQueue::reset()
{
	Queue::reset();
}

int 
DTQueue::command(int argc, const char*const* argv) 
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

void DTQueue::enque(Packet* p)
{
    
    if(hdr_cmn::access(p)->size()<500){
         q_->enque(p);
         return;
    }


    // printf("thresh=%d\n",thresh/1024);
    // printf("used_buffer[0]=%d\n",used_buffer[0]/1024);
    // printf("used_buffer[1]=%d\n",used_buffer[1]/1024);
    now_time=Scheduler::instance().clock();
    if(DT){
        if(now_time-last_time>print_interval){
        fprintf(fptr,"%lf %d %d %d %d\n",now_time,thresh/1024,
                        (used_buffer[0]+used_buffer[1])/1024,
                        (lf_used_buffer[0]+lf_used_buffer[1])/1024,
                        (used_buffer[0]+used_buffer[1]-lf_used_buffer[0]-lf_used_buffer[1])/1024
                );
        last_time=now_time;
        }
    }else{
        if(now_time-last_time>print_interval){
        fprintf(fptr,"%lf %d %d %d %d %d %d\n",
                        now_time,thresh/1024,lfThresh[0]/1024,lfThresh[1]/1024,
                        (used_buffer[0]+used_buffer[1])/1024,
                        (lf_used_buffer[0]+lf_used_buffer[1])/1024,
                        (used_buffer[0]+used_buffer[1]-lf_used_buffer[0]-lf_used_buffer[1])/1024
                );
        
        last_time=now_time;
        } 
        // printf("used_buffer[0]=%d used_buffer[1]=%d\n",used_buffer[0]/1024,used_buffer[1]/1024);
        // fprintf(fptr,"used_buffer[0]=%d used_buffer[1]=%d\n",used_buffer[0]/1024,used_buffer[1]/1024);
        
    }
    hdr_tcp *tcph = hdr_tcp::access(p);
	flowid = tcph->flowid();
    if(DT){
        if(flowid<70){
            if(thresh>hdr_cmn::access(p)->size()+used_buffer[0]){
                q_->enque(p);
                used_buffer[0]+=hdr_cmn::access(p)->size();
                usedBuffer+=hdr_cmn::access(p)->size();
                thresh=(shared_buffer-usedBuffer)*2;
                if(flowid<10){
                    lf_used_buffer[0]+=hdr_cmn::access(p)->size();
                }
                return;
            }else{
            
                drop(p);
                return;
            }
        }else{
            if(thresh>hdr_cmn::access(p)->size()+used_buffer[1]){
                q_->enque(p);
                used_buffer[1]+=hdr_cmn::access(p)->size();
                usedBuffer+=hdr_cmn::access(p)->size();
                thresh=(shared_buffer-usedBuffer)*2;
                if(flowid<80){
                    lf_used_buffer[1]+=hdr_cmn::access(p)->size();
                }
                return;
            }else{
                drop(p);
                return;
            }
        }
    }
    if(DTplus){
        if(flowid<70){//flow go to the first port
            if(thresh>hdr_cmn::access(p)->size()+used_buffer[0]){//insure port fairness
                if(flowid<10){//is longflow
                    if(lfThresh[0]>hdr_cmn::access(p)->size()+lf_used_buffer[0]){
                        q_->enque(p);
                        lf_used_buffer[0]+=hdr_cmn::access(p)->size();
                        used_buffer[0]+=hdr_cmn::access(p)->size();
                        // printf("%d\n",used_buffer[0]/1024);
                        usedBuffer+=hdr_cmn::access(p)->size();
                        thresh=(shared_buffer-usedBuffer)*2;//thresh is port thresh
                        // lfThresh=(shared_buffer-usedBuffer)*0.25;
                        lfThresh[0]=(thresh-used_buffer[0]);
                    }else{
                        drop(p);
                        return;
                    }
                }else{
                    q_->enque(p);
                    used_buffer[0]+=hdr_cmn::access(p)->size();
                    usedBuffer+=hdr_cmn::access(p)->size();
                    thresh=(shared_buffer-usedBuffer)*2;//thresh is port thresh
                    lfThresh[0]=(thresh-used_buffer[0]);
                    return;
                }
            }else{
            
                drop(p);
                return;
            }

        }else{//flow go to the second port
            if(thresh>hdr_cmn::access(p)->size()+used_buffer[1]){//insure port fairness
                if(flowid<80){//is longflow
                    if(lfThresh[1]>hdr_cmn::access(p)->size()+lf_used_buffer[1]){
                        q_->enque(p);
                        lf_used_buffer[1]+=hdr_cmn::access(p)->size();
                        used_buffer[1]+=hdr_cmn::access(p)->size();
                        usedBuffer+=hdr_cmn::access(p)->size();
                        thresh=(shared_buffer-usedBuffer)*2;//thresh is port thresh
                        //lfThresh[1]=(shared_buffer-usedBuffer)*0.25;
                        lfThresh[1]=(thresh-used_buffer[1]);
                    }else{
                        drop(p);
                        return;
                    }
                }else{
                    q_->enque(p);
                    used_buffer[1]+=hdr_cmn::access(p)->size();
                    usedBuffer+=hdr_cmn::access(p)->size();
                    thresh=(shared_buffer-usedBuffer)*2;
                    lfThresh[1]=(thresh-used_buffer[1]);
                    return;
                }
            }else{
            
                drop(p);
                return;
            }
        }
    }
}
//AG if queue size changes, we drop excessive packets...
void DTQueue::shrink_queue() 
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
Packet* DTQueue::deque()
{

    if(q_->length()>0){
        Packet* p=q_->deque();
        int qlength=hdr_cmn::access(p)->size();
        if(hdr_cmn::access(p)->size()<500){
            return p;
        }else{
            hdr_tcp *tcph = hdr_tcp::access(p);
	        flowid = tcph->flowid();
            if(flowid<70){
                used_buffer[0]-=qlength;
                if(flowid<10){
                    lf_used_buffer[0]-=qlength;
                }
            }else{
                used_buffer[1]-=qlength;
                if(flowid<80){
                    lf_used_buffer[1]-=qlength;
                }
            }
            usedBuffer-=qlength;
            thresh=(shared_buffer-usedBuffer)*2;
            if(DTplus){
                if(flowid<70){
                    lfThresh[0]=(thresh-used_buffer[0]);
                }else{
                    lfThresh[1]=(thresh-used_buffer[1]);
                }
            }
        }

        return p;
    }
    else{
        return q_->deque();
    }
}

void DTQueue::print_summarystats()
{
	//double now = Scheduler::instance().clock();
        printf("True average queue: %5.3f", true_ave_);
        if (qib_)
                printf(" (in bytes)");
        printf(" time: %5.3f\n", total_time_);
}
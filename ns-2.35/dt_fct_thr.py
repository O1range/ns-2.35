import pandas as pd
from warnings import simplefilter
simplefilter(action='ignore' ,category=FutureWarning)
df=pd.read_table("dt_fct.txt",header=None,delim_whitespace=True,names=['time','id'])
list1=list(df['id'])
set1=set(list1)
src_port=list(set1)#the list of flow's sent port
# print(src_port)
flownum=len(src_port)# the total number of flow
# print("flownum=",flownum)
all_fct=[]
all_flowsize=[]
lf_thr=[]#the throughput of long flow
sf_thr=[]#the throughput of short flow

for i in range(flownum):
    flow_start_time=df[(df['id']==src_port[i])].iloc[0,0] 
    flow_end_time=df[(df['id']==src_port[i])].iloc[-1,0] 
    fct_eachflow=flow_end_time-flow_start_time#each flow's fct
    all_fct.append(fct_eachflow)
    
    fsize=len(list(df[(df['id']==src_port[i])]['id']))#the packet number of each flow
    all_flowsize.append(fsize)
    
    if fct_eachflow:
        thr_eachflow=fsize*1500.0*8/fct_eachflow/1000000000
        if fsize<500:
            sf_thr.append(thr_eachflow)
        else:
            lf_thr.append(thr_eachflow)
# print(all_flowsize)
print(lf_thr)
sfct=[]
lfct=[]
for i in range(flownum):
    if all_flowsize[i] <500:
        sfct.append(all_fct[i])
    else:
        lfct.append(all_fct[i])

port1_thr=0
port2_thr=0
for i in range(10):
    port1_thr=port1_thr+lf_thr[i]

for i in range(10):
    port2_thr=port2_thr+lf_thr[i+10]

lf_thr=(port1_thr+port2_thr)*1.0/2
print("lfthroughput=",lf_thr)
print("sfthroughput=",sum(sf_thr)*1.0/len(sf_thr))
print("sfct=",sum(sfct)*1.0/len(sfct))
print("lfct=",sum(lfct)*1.0/len(lfct))
print("total_fct=",sum(all_fct)*1.0/len(all_fct)) 
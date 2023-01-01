import pandas as pd
from warnings import simplefilter
simplefilter(action='ignore' ,category=FutureWarning)
df=pd.read_table("fct2.txt",header=None,delim_whitespace=True,names=['time','id'])
list1=list(df['id'])
set1=set(list1)
src_port=list(set1)#the list of flow's sent port

flownum=len(src_port)# the total number of flow
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

sfct=[]
lfct=[]
for i in range(flownum):
    if all_flowsize[i] <500:
        sfct.append(all_fct[i])
    else:
        lfct.append(all_fct[i])
print("lfthroughput=",sum(lf_thr)*1.0/len(lf_thr))
print("sfthroughput=",sum(sf_thr)*1.0/len(sf_thr))
print("sfct=",sum(sfct)*1.0/len(sfct))
print("lfct=",sum(lfct)*1.0/len(lfct))
print("total_fct=",sum(all_fct)*1.0/len(all_fct)) 
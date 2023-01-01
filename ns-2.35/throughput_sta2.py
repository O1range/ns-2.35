import pandas as pd
from warnings import simplefilter
simplefilter(action='ignore' ,category=FutureWarning)
df=pd.read_table("throughput_sta2.txt",header=None,delim_whitespace=True,names=['time','id'])
datanum=len(list(df['time']))#the number of data in row
list1=list(df['id'])
set1=set(list1)
src_port=list(set1)#the list of flow's sent port

flownum=len(src_port)# the total number of flow
all_fct=[]
all_flowsize=[]
islf=[]
for i in range(flownum):
    fsize=len(list(df[(df['id']==src_port[i])]['id']))#the packet number of each flow
    #print(fsize)
    if fsize<500:
        islf.append(0)
    else:
        islf.append(1)

last_time=df.iloc[0,0]
interval=0.0001
lf_num=0
sf_num=0
throughput=[]
row=[]
# src_port.index()
for i in range(datanum):
    now_time=df.iloc[i,0]
    now_port=df.iloc[i,1]
    site=src_port.index(now_port)
    if islf[site]==0:
        sf_num=sf_num+1
    else:
        lf_num=lf_num+1
    if now_time-last_time>interval:
        lf_thr=lf_num*1500*8.0/interval/1000000000
        sf_thr=sf_num*1500*8.0/interval/1000000000
        row.append(now_time)
        row.append(lf_thr)
        row.append(sf_thr)
        row.append(sf_thr+lf_thr)
        throughput.append(row)
        row=[]
        last_time=now_time
        sf_num=0
        lf_num=0
f = open("realtime_throughput.txt", "w")
for i in throughput:
    for j in i:
        f.write(str(j))
        f.write(" ")
    f.write("\n")
    # f.write(str(i[0]) + " " + str(i[1])+ " " + str(i[2])+"\n")
    # j = list(str(i))
    # k = j[1: len(j) - 2]
    # f.writelines(str(k)+"\n")


f.close()
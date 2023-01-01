import pandas as pd
from warnings import simplefilter
simplefilter(action='ignore' ,category=FutureWarning)

total_lf_pck_num=[]
total_sf_pck_num=[]
total_lf_pck_drop_num=[]
total_sf_pck_drop_num=[]
for i in range(6):
    a=i+1
    filename="drop"+str(a)+".txt"
    df1=pd.read_table(filename,header=None,delim_whitespace=True,names=['type','time','id'])
    
    list1=list(df1['id'])
    datanum=len(list1)#the number of data
    set1=set(list1)
    src_port=list(set1)#the list of flow's sent port
    # print(src_port)
    flownum=len(src_port)# the total number of flow
    islf=[]
    for i in range(flownum):
        fsize=len(list(df1[(df1['id']==src_port[i])]['id']))#the packet number of each flow
        
        if fsize<800:
            islf.append(0)
        else:
            islf.append(1)
    # print(islf)
    
    upper_time=0
    rownum=0
    
    for i in range(datanum):
        current_id=df1.iloc[datanum-1-i,2]
        site=src_port.index(current_id)
        if islf[site]==0:
            upper_time=df1.iloc[datanum-1-i,1]
            rownum=datanum-1-i
            break 
    df2=pd.read_table(filename,header=None,delim_whitespace=True,names=['type','time','id'],nrows=rownum+1)
    df2_length=len(list(df2['id']))

    lf_pck_num=0
    sf_pck_num=0
    lf_pck_drop_num=0
    sf_pck_drop_num=0
    for i in range(df2_length):
        current_type=df2.iloc[i,0]
        current_id=df2.iloc[i,2]
        site=src_port.index(current_id)
        if current_type=='+':
            if islf[site]==0:
               sf_pck_num=sf_pck_num+1
            else:
                lf_pck_num=lf_pck_num+1
        if current_type=='d':
            if islf[site]==0:
                sf_pck_drop_num=sf_pck_drop_num+1
            else:
                lf_pck_drop_num=lf_pck_drop_num+1
    total_lf_pck_drop_num.append(lf_pck_drop_num)
    total_lf_pck_num.append(lf_pck_num)
    total_sf_pck_drop_num.append(sf_pck_drop_num)
    total_sf_pck_num.append(sf_pck_num)
# print(total_lf_pck_num)
# print(total_sf_pck_num)
# print(total_lf_pck_drop_num)
# print(total_sf_pck_drop_num)
lf_drop_rate=sum(total_lf_pck_drop_num)*1.0/sum(total_lf_pck_num)
sf_drop_rate=sum(total_sf_pck_drop_num)*1.0/sum(total_sf_pck_num)
# print(sum(total_lf_pck_drop_num))
# print(sum(total_sf_pck_drop_num))
# print(sum(total_lf_pck_num))
# print(sum(total_sf_pck_num))
print("lf_drop_rate=",lf_drop_rate)
print("sf_drop_rate=",sf_drop_rate)
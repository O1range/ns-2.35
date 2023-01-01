#!/bin/sh

awk '{if($5=="tcp" && $3==0 && ($9<=10.0||$9>10.0&&$9<=30.0))print $1,$2,$9}' dt.tr > drop1.txt
awk '{if($5=="tcp" && $3==0 && ($9<=10.0||$9>30.0&&$9<=50.0))print $1,$2,$9}' dt.tr > drop2.txt
awk '{if($5=="tcp" && $3==0 && ($9<=10.0||$9>50.0&&$9<=70.0))print $1,$2,$9}' dt.tr > drop3.txt
awk '{if($5=="tcp" && $3==0 && ($9<=80.0&&$9>70.0||$9>80.0&&$9<=100.0))print $1,$2,$9}' dt.tr > drop4.txt
awk '{if($5=="tcp" && $3==0 && ($9<=80.0&&$9>70.0||$9>100.0&&$9<=120.0))print $1,$2,$9}' dt.tr > drop5.txt
awk '{if($5=="tcp" && $3==0 && ($9<=80.0&&$9>70.0||$9>120.0&&$9<=140.0))print $1,$2,$9}' dt.tr > drop6.txt

python3 dt_drop_sta.py

rm drop*.txt

awk '{if($5=="tcp" && $1=="+" && $3==0 && ($4==141||$4==142))print $2,$9 }' dt.tr > dt_fct.txt
python3 dt_fct_thr.py

rm dt_fct.txt

awk '{if($5=="tcp" && $1=="-" && $3==0 && ($4==141||$4==142))print $2,$9 }' dt.tr > dt_real_thr_sta.txt

python3 dt_real_thr.py

rm dt_real_thr_sta.txt
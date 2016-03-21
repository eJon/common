#!/usr/bin/env python
import sys,os,json,re,time,subprocess
from collections import defaultdict
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")

DATA_CENTRE="/home/w/share/monitor-server/data"
DATA_SETTLE=os.path.join(DATA_CENTRE, "settle")
pt_ipfile=re.compile(r'^\d+\.\d+\.\d+.\d+')

def main():
    cmd = subprocess.Popen("mkdir -p " + DATA_SETTLE + "; rsync -av 10.13.40.91::public/kpi.merge.v2 " + DATA_SETTLE , shell=True)
    cmd.wait()
    cmd = subprocess.Popen("mkdir -p " + DATA_SETTLE + "; rsync -av 10.13.40.91::public/merge.title.v2 " + DATA_SETTLE , shell=True)
    cmd.wait()
    
    title = json.loads(open(os.path.join(DATA_SETTLE,"merge.title.v2")).readline())
    title = title["settle"]
    title["data_path"] = os.path.join(DATA_SETTLE, "kpi.merge.v2")
    title_merge = open(os.path.join(DATA_CENTRE, "merge.title.v2")).readline()
    if (title_merge.strip() != ""):
        title_merge = json.loads(title_merge)
    else:
        title_merge = {}
    title_merge["settle"] = title
    fout = open(os.path.join(DATA_CENTRE, "merge.title.v2"), "w")
    fout2 = open(os.path.join(DATA_CENTRE, "merge.title.v2.pretty"), "w")

    print >> fout, json.dumps(title_merge)
    print >> fout2, json.dumps(title_merge, indent=4)
if __name__ == '__main__':
    main()

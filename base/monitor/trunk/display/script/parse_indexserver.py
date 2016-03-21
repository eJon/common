#!/usr/bin/env python
import sys,os,json,re,time
from collections import defaultdict
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")

DATA_CENTRE="/home/w/share/monitor-server/data"
DATA_INDEXSERVER=os.path.join(DATA_CENTRE, "indexserver")
pt_ipfile=re.compile(r'^\d+\.\d+\.\d+.\d+')

def tree():
    return defaultdict(tree)
def extract_kpi(f, values):
    for line in open(f):
        js = json.loads(line)
        content = js["content"]
        if len(content) == 0:
            continue
        ip = js["ip"]
        time_str = js["time"]
        timestamp = time.mktime(time.strptime(time_str, "%Y%m%d%H%M%S"))
        timestamp = int(timestamp - timestamp % 600)
        for t in content.split("|"):
            if len(t)  == 0 :
                continue
            key = t.split(":")[0]; value = t.split(":")[1]
            if type(values[timestamp][key]["total"]) == type(values):
                values[timestamp][key]["total"] = 0
            if type(values[timestamp][key]["ip"][ip]) == type(values):
                values[timestamp][key]["ip"][ip] = 0

            values[timestamp][key]["total"] += int(value)
            values[timestamp][key]["ip"][ip] += int(value)
            
def main():
    values = tree()
    for f in os.listdir(DATA_INDEXSERVER):
        if pt_ipfile.match(f):
            extract_kpi(os.path.join(DATA_INDEXSERVER,f), values)

    fout = open(os.path.join(DATA_INDEXSERVER, "index.kpi"), "w")
    fout2 = open(os.path.join(DATA_INDEXSERVER, "index.kpi.pretty"), "w")
    print >> fout, json.dumps(values)
    print >> fout2, json.dumps(values, indent=4)

    title = tree()
    for t in values:
        for kpi_name in values[t]:
            for dim_name in values[t][kpi_name]:
                if (dim_name != "total"):
                    title[kpi_name][dim_name] = values[t][kpi_name][dim_name].keys()
                else:
                    title[kpi_name]["NONE"] = [];
    title["data_path"] = os.path.join(DATA_INDEXSERVER, "index.kpi");

    title_merge = open(os.path.join(DATA_CENTRE, "merge.title.v2")).readline()
    if (title_merge.strip() != ""):
        title_merge = json.loads(title_merge)
    else:
        title_merge = {}
    title_merge["indexserver"] = title
    fout = open(os.path.join(DATA_CENTRE, "merge.title.v2"), "w")
    fout2 = open(os.path.join(DATA_CENTRE, "merge.title.v2.pretty"), "w")

    print >> fout, json.dumps(title_merge)
    print >> fout2, json.dumps(title_merge, indent=4)
if __name__ == '__main__':
    main()

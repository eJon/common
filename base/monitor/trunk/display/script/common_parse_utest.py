#!/usr/bin/env python
# auther hongbin2@staff.sina.com.cn

'''
the unit test of common_parse.py
'''

import sys,os,re,json,time
from collections import defaultdict
import MySQLdb as mdb
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")
from common_parse import *
from common_utils import *

class CommonParserTestCase(unittest.TestCase):
    def test_extract_1(self):
        kpi_col_timeline = KpiColTimeline()
        extract_kpi(os.path.join(local_path, "common_parse.in.1"), kpi_col_timeline);
        for timestamp in kpi_col_timeline.cols:
            print json.dumps(kpi_col_timeline.cols[timestamp].to_dict())

if __name__ == '__main__':
    unittest.main()


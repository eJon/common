#!/usr/bin/env python
# author hongbin2@staff.sina.com.cn

'''
some utils of monitor display server
'''
import sys,os,unittest,json
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")

class DimEntity:
    name = ""
    kv = {}
    def __init__(self, name):
        self.name = name
        self.kv = {}
    def acc(self, key, value):
        if not key in self.kv:
            self.kv[key] = 0
        self.kv[key] += value
    def assign(self, key, value):
        self.kv[key] = value
    def get_total(self):
        total = 0
        for key in self.kv:
            total += self.kv[key]
        return total
    def merge(self, another_dim):
        assert self.name == another_dim.name
        for key in another_dim.kv:
            self.acc(key, another_dim.kv[key])
class KpiEntity:
    name = ""
    total = ""
    dims = {}
    def __init__(self, name):
        self.name = name
        self.total = 0
        self.dims = {}
    def set_total(self,total):
        self.total = total
    def add_dim(self, dim):
        self.dims[dim.name] = dim
    def check_consistency(self):
        for dim in self.dims:
            if self.dims[dim].get_total() != self.total:
                return False
        return True
    def calc_total(self):
        for dim in self.dims:
            self.total = self.dims[dim].get_total()
    def to_dict(self):
        kpi_dict = {}
        kpi_dict["total"] = self.total
        for dim in self.dims:
            kpi_dict[self.dims[dim].name] = self.dims[dim].kv
        return kpi_dict
    def from_dict(self, kpi_dict):
        assert "total" in kpi_dict
        self.total = kpi_dict["total"]
        for dim_name in kpi_dict:
            if dim_name == "total":
                continue
            dim = DimEntity(dim_name)
            dim.kv = kpi_dict[dim_name]
            self.dims[dim_name] = dim
            #if self.total != dim.get_total():
            #    raise Exception("self.total != dim.get_total() : %s" % json.dumps(kpi_dict))

    def merge(self, another_kpi):
        assert self.name == another_kpi.name
        assert len(self.dims) == len(another_kpi.dims)
        for dim_name in another_kpi.dims:
            if dim_name in self.dims:
                self.dims[dim_name].merge(another_kpi.dims[dim_name])
            else:
                print dim_name
                assert False
        self.total += another_kpi.total


class KpiCollector:
    kpis = {}
    def __init__(self):
        self.kpis = {}
    def add_kpi(self, kpi):
        self.kpis[kpi.name] = kpi
    def calc_total(self):
        for name in self.kpis:
            self.kpis[name].calc_total()
    def to_dict(self):
        kpi_col_dict = {}
        for kpi in self.kpis:
            kpi_col_dict[self.kpis[kpi].name] = self.kpis[kpi].to_dict()
        return kpi_col_dict
    def from_dict(self, kpi_col_dict):
        for kpi_name in kpi_col_dict:
            kpi = KpiEntity(kpi_name)
            kpi.from_dict(kpi_col_dict[kpi_name])
            self.kpis[kpi_name] = kpi
    def merge(self,another_kpi_col):
        for kpi_name in another_kpi_col.kpis:
            if kpi_name in self.kpis:
                self.kpis[kpi_name].merge(another_kpi_col.kpis[kpi_name])
            else:
                self.kpis[kpi_name] = another_kpi_col.kpis[kpi_name]
    def gen_title(self):
        title = {}
        for kpi_name in self.kpis:
            title[kpi_name] = {"NONE":[]}
            title["data_path"] = 'mysql'
            if len(self.kpis[kpi_name].dims) == 0:
                pass
            else:
                for dim_name in self.kpis[kpi_name].dims:
                    title[kpi_name][dim_name] = self.kpis[kpi_name].dims[dim_name].kv.keys()
        return title
    
class KpiColTimeline:
    def __init__(self):
        self.cols = {}
    #def acc_col(self,timestamp, col):
    #    if not timestamp in self.cols:
    #        self.cols[timestamp] = col
    #    else:
    #        self.cols[timestamp].merge(col)
    def calc_rate(self, denominotor="__ALL__", molecules=[], prefix="rate_", isRawDelete=True):
        """figure up the rate format of kpis
        Because of some resons, the raw kpi collected was integral. But some kpis would be transformed 
        by some raw kpis. Thus as interaction rate is figured up as interation counts / exposure counts.
        @param denominotor 
        @molecules the kpis need to figure up. if len(molecules) == 0, the would kpis except denominotor would be 
        figure up.
        @prefix the new kpi figureed up is somethings like "rate_kpi"
        @isRawDelete once the new kpi was figured up, the raw kpis are deleted

        @note voker must make sure the denominotor is exists and the  dimensions in molecules exists in denominotor, otherwise Excepitons 
        would occur.

        @note must be voke after calc_total to make sure the total is correct.
        """
        molecules = set(molecules)
        for timestamp in self.cols:
            acol = self.cols[timestamp]
            for kpi in acol.kpis.keys():
                if kpi != denominotor and (kpi in molecules or len(molecules) == 0):
                    akpi = KpiEntity(prefix + kpi)
                    for dim in acol.kpis[kpi].dims:
                        adim = DimEntity(dim)
                        for key in acol.kpis[kpi].dims[dim].kv:
                            adim.kv[key] = acol.kpis[kpi].dims[dim].kv[key] * 1.0 / acol.kpis[denominotor].dims[dim].kv[key]
                        akpi.total = acol.kpis[kpi].total * 1.0 / acol.kpis[denominotor].total
                        akpi.dims[dim] = adim
                    if isRawDelete:
                        del acol.kpis[kpi]
                    acol.kpis[akpi.name] = akpi
            if isRawDelete:
                del acol.kpis[denominotor]
    def calc_total(self):
        """ calc the summary of the all dimensions. 
        @note never noke this method after the calc_rate. 
        """
        for timestamp in self.cols:
            self.cols[timestamp].calc_total()
    def acc(self, timestamp, kpi, dim, dimkey, value):
        """add a dimension data of a kpi
        to simplify the progress. The method define that a kpi must has at least one demension.
        If you kpi has no demension, a demension named "__SINGLE__" and dimemsion key name "__SINGLE" is suggested.
        """
        if not timestamp in self.cols:
            akpi = KpiEntity(kpi)
            dim = DimEntity(dim)
            dim.acc(dimkey, int(value))
            akpi.add_dim(dim)
            akpicol = KpiCollector()
            akpicol.add_kpi(akpi)
            self.cols[timestamp] = akpicol
        else:
            akpicol = self.cols[timestamp]
            if kpi in akpicol.kpis:
                akpi = akpicol.kpis[kpi]
            else:
                akpi = KpiEntity(kpi)
                akpicol.kpis[kpi] =akpi
            if dim in akpi.dims:
                adim = akpi.dims[dim]
            else: 
                adim = DimEntity(dim)
                akpi.dims[dim] = adim
            if dimkey in adim.kv:
                adim.kv[dimkey] += value
            else:
                adim.kv[dimkey] = value
            
    def to_dict(self):
        result = {}
        for timestamp in self.cols.keys():
            result[timestamp] = self.cols[timestamp].to_dict()
        return result
    def gen_title(self):
        if len(self.cols) == 0:
            return {}
        else:
            return self.cols[self.cols.keys()[-1]].gen_title()

# unit test
class CommonUtilsTestCase(unittest.TestCase):
    def test_1(self):
        kpiCol = KpiCollector()
        kpi = KpiEntity("qps")
        dim1 = DimEntity("location")
        dim1.acc("up", 40)
        dim1.acc("down", 43)
        dim1.acc("left", 13)
        dim1.acc("right", 143)
        dim1.acc("up", 20)
        kpi.add_dim(dim1)
        kpi.set_total(dim1.get_total())
        assert kpi.check_consistency()
        kpiCol.add_kpi(kpi)
        assert '{"qps": {"total": 259, "location": {"down": 43, "right": 143, "up": 60, "left": 13}}}' ==  json.dumps(kpiCol.to_dict())

        
        kpiCol2 = KpiCollector()
        kpi = KpiEntity("qps")
        dim1 = DimEntity("location")
        dim1.acc("up", 40)
        dim1.acc("down", 43)
        dim1.acc("left", 13)
        dim1.acc("right", 143)
        kpi.add_dim(dim1)
        kpi.set_total(dim1.get_total())
        assert kpi.check_consistency()
        kpiCol2.add_kpi(kpi)

        kpiCol.merge(kpiCol2)
        assert '{"qps": {"total": 498, "location": {"down": 86, "right": 286, "up": 100, "left": 26}}}' ==  json.dumps(kpiCol.to_dict())
    def test_2(self):
       kpis = KpiColTimeline()
       kpis.acc(1390918220, "qps", "location", "up", 60)
       kpis.acc(1390918220, "qps", "location", "up", 60)
       kpis.acc(1390918220, "qps", "location", "down", 60)
       kpis.acc(1390918220, "qps", "location", "left", 60)
       kpis.acc(1390918220, "qps", "location", "right", 60)
       kpis.acc(1390918220, "range", "location", "up", 100)
       kpis.acc(1390918220, "range", "location", "down", 100)
       kpis.acc(1390918220, "range", "location", "left", 100)
       kpis.acc(1390918220, "range", "location", "right", 100)
       kpis.calc_total()
       kpis.calc_rate("range") 
       assert '{"1390918220": {"rate_qps": {"total": 0.75, "location": {"down": 0.59999999999999998, "right": 0.59999999999999998, "up": 1.2, "left": 0.59999999999999998}}}}' == json.dumps(kpis.to_dict())

if __name__ == '__main__':
    unittest.main()

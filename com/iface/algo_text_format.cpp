#include "algo_text_format.h"
#include <sstream>

using namespace ms_dsad;
using namespace std;
namespace algo_interface {
#define COMMA_STR ","
#define LINE_STR "\n"
void AlgoTextFormat::PrintAdlistToString(const ms_dsad::RespAd& adlist, string &out) {
    int index = 0;
    const AdInfo* adinfo;
    ostringstream str_io;
    str_io << "========= Print Dataserver Return Adlist Start=========\n";
    for(index = 0; index < adlist.adinfolist_size(); ++index) {
        adinfo = &adlist.adinfolist(index);
        str_io << "adid=" << adinfo->adid() << COMMA_STR << "psid=" << adinfo->postid() << COMMA_STR
               << "price=" << adinfo->price() << COMMA_STR << "impression=" << adinfo->impression() << COMMA_STR
               << "version=" << adinfo->version() << COMMA_STR << "custid=" << adinfo->custid() << COMMA_STR
               << "biztype=" << adinfo->biztype() << COMMA_STR << "priority=" << adinfo->priority() << COMMA_STR
               << "allocid=" << adinfo->allocid() << COMMA_STR << "id=" << adinfo->id() << LINE_STR;
    }
    const DistributeItem *dist_item = NULL;
    const AllocItem* alloc_item = NULL;
    // Log_r::Notice(" distribute items ");
    for(index = 0; index < adlist.postitem_size(); ++index) {
        dist_item = &adlist.postitem(index);
        str_io << " this adlist freq level is =" << dist_item->freqlevel() << LINE_STR;
        for(int j = 0; j < dist_item->ads_size(); ++j) {
            alloc_item = &dist_item->ads(j);
            str_io << "adid=" << alloc_item->adid() << COMMA_STR << "reserved=" << alloc_item->reserved() << COMMA_STR
                   << COMMA_STR << "alloc=" << alloc_item->alloc() << COMMA_STR
                   << "type=" << alloc_item->type() << COMMA_STR << "biztype=" << alloc_item->biz() << COMMA_STR
                   << "allocid=" << alloc_item->allocid() << COMMA_STR << "id=" << alloc_item->id() << LINE_STR;
        }
    }
    out = str_io.str();
}
}

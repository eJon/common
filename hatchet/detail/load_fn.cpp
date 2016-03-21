// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Be used to load CowTable.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012

#include "load_fn.h"

namespace st {

int LoadFn::close() {
    _cur_item = NULL;
    _cur_field = NULL;
    
    return _reader.close();
}

} // namespace st

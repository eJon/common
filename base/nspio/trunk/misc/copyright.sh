#!/bin/bash

find nspio/ -name '*.h' | xargs -i{} bash -c "sed -i '1,9d;' {} && sed -i '1 i\// copyright:\n//            (C) SINA Inc.\n//\n//           file: {}\n//           desc: \n//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>\n//           date: 2014-02-14\n\n' {}";
find nspio/ -name '*.cc' | xargs -i{} bash -c "sed -i '1,9d;' {} && sed -i '1 i\// copyright:\n//            (C) SINA Inc.\n//\n//           file: {}\n//           desc: \n//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>\n//           date: 2014-02-14\n\n' {}";


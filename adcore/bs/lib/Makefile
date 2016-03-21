#COMAKE2 edit-mode: -*- Makefile -*-
####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
CC=gcc
CXX=g++
CXXFLAGS=-O1 \
  -ggdb \
  -pipe \
  -W \
  -Wall \
  -fPIC \
  -Werror \
  -Wno-invalid-offsetof
CFLAGS=
CPPFLAGS=-D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS
INCPATH=
DEP_INCPATH=-I../../../../../app/ecom/adx/interface/output/include \
  -I../../../../../app/ecom/cm/global \
  -I../../../../../app/ecom/cm/global/include \
  -I../../../../../app/ecom/cm/global/output \
  -I../../../../../app/ecom/cm/global/output/include \
  -I../../../../../app/ecom/cm/interface \
  -I../../../../../app/ecom/cm/interface/include \
  -I../../../../../app/ecom/cm/interface/output \
  -I../../../../../app/ecom/cm/interface/output/include \
  -I../../../../../app/ecom/cm/mrf/output/include \
  -I../../../../../app/ecom/cm/pb-rpc/output/include \
  -I../../../../../app/ecom/cm/utility/output/include \
  -I../../../../../app/ecom/nova/afs/config-io \
  -I../../../../../app/ecom/nova/afs/config-io/include \
  -I../../../../../app/ecom/nova/afs/config-io/output \
  -I../../../../../app/ecom/nova/afs/config-io/output/include \
  -I../../../../../app/ecom/nova/afs/dynamic-protobuf \
  -I../../../../../app/ecom/nova/afs/dynamic-protobuf/include \
  -I../../../../../app/ecom/nova/afs/dynamic-protobuf/output \
  -I../../../../../app/ecom/nova/afs/dynamic-protobuf/output/include \
  -I../../../../../app/ecom/nova/afs/smalltable \
  -I../../../../../app/ecom/nova/afs/smalltable/include \
  -I../../../../../app/ecom/nova/afs/smalltable/output \
  -I../../../../../app/ecom/nova/afs/smalltable/output/include \
  -I../../../../../app/ecom/nova/thirdparty/gflags \
  -I../../../../../app/ecom/nova/thirdparty/gflags/include \
  -I../../../../../app/ecom/nova/thirdparty/gflags/output \
  -I../../../../../app/ecom/nova/thirdparty/gflags/output/include \
  -I../../../../../app/ecom/uap/proto/dmp \
  -I../../../../../app/ecom/uap/proto/dmp/include \
  -I../../../../../app/ecom/uap/proto/dmp/output \
  -I../../../../../app/ecom/uap/proto/dmp/output/include \
  -I../../../../../com-test/itest/tools/fault \
  -I../../../../../com-test/itest/tools/fault/include \
  -I../../../../../com-test/itest/tools/fault/output \
  -I../../../../../com-test/itest/tools/fault/output/include \
  -I../../../../../com/btest/gtest \
  -I../../../../../com/btest/gtest/include \
  -I../../../../../com/btest/gtest/output \
  -I../../../../../com/btest/gtest/output/include \
  -I../../../../../com/idlcompiler \
  -I../../../../../com/idlcompiler/include \
  -I../../../../../com/idlcompiler/output \
  -I../../../../../com/idlcompiler/output/include \
  -I../../../../../lib2-64/bsl \
  -I../../../../../lib2-64/bsl/include \
  -I../../../../../lib2-64/bsl/output \
  -I../../../../../lib2-64/bsl/output/include \
  -I../../../../../lib2-64/ccode \
  -I../../../../../lib2-64/ccode/include \
  -I../../../../../lib2-64/ccode/output \
  -I../../../../../lib2-64/ccode/output/include \
  -I../../../../../lib2-64/dict \
  -I../../../../../lib2-64/dict/include \
  -I../../../../../lib2-64/dict/output \
  -I../../../../../lib2-64/dict/output/include \
  -I../../../../../lib2-64/ullib \
  -I../../../../../lib2-64/ullib/include \
  -I../../../../../lib2-64/ullib/output \
  -I../../../../../lib2-64/ullib/output/include \
  -I../../../../../op/oped/noah/webfoot/naming-lib \
  -I../../../../../op/oped/noah/webfoot/naming-lib/include \
  -I../../../../../op/oped/noah/webfoot/naming-lib/output \
  -I../../../../../op/oped/noah/webfoot/naming-lib/output/include \
  -I../../../../../public/bslext \
  -I../../../../../public/bslext/include \
  -I../../../../../public/bslext/output \
  -I../../../../../public/bslext/output/include \
  -I../../../../../public/comlog-plugin \
  -I../../../../../public/comlog-plugin/include \
  -I../../../../../public/comlog-plugin/output \
  -I../../../../../public/comlog-plugin/output/include \
  -I../../../../../public/comlogext/cronoapd \
  -I../../../../../public/comlogext/cronoapd/include \
  -I../../../../../public/comlogext/cronoapd/output \
  -I../../../../../public/comlogext/cronoapd/output/include \
  -I../../../../../public/configure \
  -I../../../../../public/configure/include \
  -I../../../../../public/configure/output \
  -I../../../../../public/configure/output/include \
  -I../../../../../public/connectpool \
  -I../../../../../public/connectpool/include \
  -I../../../../../public/connectpool/output \
  -I../../../../../public/connectpool/output/include \
  -I../../../../../public/idlcompiler \
  -I../../../../../public/idlcompiler/include \
  -I../../../../../public/idlcompiler/output \
  -I../../../../../public/idlcompiler/output/include \
  -I../../../../../public/mcpack \
  -I../../../../../public/mcpack/include \
  -I../../../../../public/mcpack/output \
  -I../../../../../public/mcpack/output/include \
  -I../../../../../public/nshead \
  -I../../../../../public/nshead/include \
  -I../../../../../public/nshead/output \
  -I../../../../../public/nshead/output/include \
  -I../../../../../public/odict \
  -I../../../../../public/odict/include \
  -I../../../../../public/odict/output \
  -I../../../../../public/odict/output/include \
  -I../../../../../public/sign \
  -I../../../../../public/sign/include \
  -I../../../../../public/sign/output \
  -I../../../../../public/sign/output/include \
  -I../../../../../public/spreg \
  -I../../../../../public/spreg/include \
  -I../../../../../public/spreg/output \
  -I../../../../../public/spreg/output/include \
  -I../../../../../public/ub \
  -I../../../../../public/ub/include \
  -I../../../../../public/ub/output \
  -I../../../../../public/ub/output/include \
  -I../../../../../public/uconv \
  -I../../../../../public/uconv/include \
  -I../../../../../public/uconv/output \
  -I../../../../../public/uconv/output/include \
  -I../../../../../public/urlparser \
  -I../../../../../public/urlparser/include \
  -I../../../../../public/urlparser/output \
  -I../../../../../public/urlparser/output/include \
  -I../../../../../quality/autotest/bmock \
  -I../../../../../quality/autotest/bmock/include \
  -I../../../../../quality/autotest/bmock/output \
  -I../../../../../quality/autotest/bmock/output/include \
  -I../../../../../quality/autotest/reportlib/cpp \
  -I../../../../../quality/autotest/reportlib/cpp/include \
  -I../../../../../quality/autotest/reportlib/cpp/output \
  -I../../../../../quality/autotest/reportlib/cpp/output/include \
  -I../../../../../third-64/boost \
  -I../../../../../third-64/boost/include \
  -I../../../../../third-64/boost/output \
  -I../../../../../third-64/boost/output/include \
  -I../../../../../third-64/libxml2 \
  -I../../../../../third-64/libxml2/include \
  -I../../../../../third-64/libxml2/output \
  -I../../../../../third-64/libxml2/output/include \
  -I../../../../../third-64/pcre \
  -I../../../../../third-64/pcre/include \
  -I../../../../../third-64/pcre/output \
  -I../../../../../third-64/pcre/output/include \
  -I../../../../../third-64/protobuf \
  -I../../../../../third-64/protobuf/include \
  -I../../../../../third-64/protobuf/output \
  -I../../../../../third-64/protobuf/output/include \
  -I../../../../../third-64/snappy \
  -I../../../../../third-64/snappy/include \
  -I../../../../../third-64/snappy/output \
  -I../../../../../third-64/snappy/output/include \
  -I../../../../../thirdsrc/protobuf \
  -I../../../../../thirdsrc/protobuf/include \
  -I../../../../../thirdsrc/protobuf/output \
  -I../../../../../thirdsrc/protobuf/output/include

#============ CCP vars ============
CCHECK=@ccheck.py
CCHECK_FLAGS=-c ../../cm/global/cc.conf
PCLINT=@pclint
PCLINT_FLAGS=
CCP=@ccp.py
CCP_FLAGS=


#COMAKE UUID
COMAKE_MD5=b244195931f6ebfcb5ca534d45a63ea6  COMAKE


.PHONY:all
all:comake2_makefile_check lib-main 
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mall[0m']"
	@echo "make all done"

.PHONY:comake2_makefile_check
comake2_makefile_check:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mcomake2_makefile_check[0m']"
	#in case of error, update 'Makefile' by 'comake2'
	@echo "$(COMAKE_MD5)">comake2.md5
	@md5sum -c --status comake2.md5
	@rm -f comake2.md5

.PHONY:ccpclean
ccpclean:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mccpclean[0m']"
	rm -rf .ccheck.tag
	rm -rf .ccheck_das_handler.h.tag
	rm -rf .ccheck_das_inc_reader.h.tag
	rm -rf .ccheck_das_lib_log.h.tag
	rm -rf .ccheck_lib_manager.h.tag
	rm -rf .ccheck_lib_util.h.tag
	rm -rf .ccheck_reloadable_table_handler.h.tag
	rm -rf .ccheck_table_defs.h.tag
	rm -rf .ccheck_table_group.h.tag
	rm -rf .ccheck_target_type.h.tag
	rm -rf .ccheck_das_handler.cpp.tag
	rm -rf .ccheck_das_inc_reader.cpp.tag
	rm -rf .ccheck_lib_manager.cpp.tag
	rm -rf .ccheck_lib_util.cpp.tag
	rm -rf .ccheck_main.cpp.tag
	rm -rf .ccheck_reloadable_table_handler.cpp.tag
	rm -rf .ccheck_table_defs.cpp.tag
	rm -rf .ccheck_table_group.cpp.tag
	@echo "make ccpclean done"

.PHONY:clean
clean:ccpclean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf lib-main
	rm -rf ./output/bin/lib-main
	rm -rf lib-main_das_handler.o
	rm -rf lib-main_das_inc_reader.o
	rm -rf lib-main_lib_manager.o
	rm -rf lib-main_lib_util.o
	rm -rf lib-main_main.o
	rm -rf lib-main_reloadable_table_handler.o
	rm -rf lib-main_table_defs.o
	rm -rf lib-main_table_group.o

.PHONY:dist
dist:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdist[0m']"
	tar czvf output.tar.gz output
	@echo "make dist done"

.PHONY:distclean
distclean:clean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdistclean[0m']"
	rm -f output.tar.gz
	@echo "make distclean done"

.PHONY:love
love:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlove[0m']"
	@echo "make love done"

lib-main:lib-main_das_handler.o \
  lib-main_das_inc_reader.o \
  lib-main_lib_manager.o \
  lib-main_lib_util.o \
  lib-main_main.o \
  lib-main_reloadable_table_handler.o \
  lib-main_table_defs.o \
  lib-main_table_group.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main[0m']"
	$(CXX) lib-main_das_handler.o \
  lib-main_das_inc_reader.o \
  lib-main_lib_manager.o \
  lib-main_lib_util.o \
  lib-main_main.o \
  lib-main_reloadable_table_handler.o \
  lib-main_table_defs.o \
  lib-main_table_group.o -Xlinker "-("  ../../../../../app/ecom/adx/interface/libadx_if.a \
  ../../../../../app/ecom/cm/interface/libcm_interface.a \
  ../../../../../app/ecom/cm/mrf/libmrf.a \
  ../../../../../app/ecom/cm/pb-rpc/libpb-rpc.a \
  ../../../../../app/ecom/cm/utility/libcm_utility.a \
  ../../../../../app/ecom/nova/afs/config-io/libconfigio.a \
  ../../../../../app/ecom/nova/afs/dynamic-protobuf/libdynamicprotobuf.a \
  ../../../../../app/ecom/nova/afs/smalltable/libsmalltable.a \
  ../../../../../app/ecom/nova/thirdparty/gflags/output/lib/libgflags.a \
  ../../../../../app/ecom/uap/proto/dmp/libdmp.a \
  ../../../../../com-test/itest/tools/fault/output/lib/libfault.a \
  ../../../../../com-test/itest/tools/fault/output/lib/libfault1.a \
  ../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../lib2-64/ccode/lib/libulccode.a \
  ../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../op/oped/noah/webfoot/naming-lib/output/lib/libwebfoot_naming.a \
  ../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../public/comlog-plugin/libcomlog.a \
  ../../../../../public/comlog-plugin/output/lib/libdfsappender.a \
  ../../../../../public/comlogext/cronoapd/libcomlog_cronoapd.a \
  ../../../../../public/configure/libconfig.a \
  ../../../../../public/connectpool/libconnectpool.a \
  ../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../public/mcpack/libmcpack.a \
  ../../../../../public/nshead/libnshead.a \
  ../../../../../public/odict/libodict.a \
  ../../../../../public/sign/libuln_sign.a \
  ../../../../../public/spreg/libspreg.a \
  ../../../../../public/ub/output/lib/libub.a \
  ../../../../../public/ub/output/lib/libub_aserver.a \
  ../../../../../public/ub/output/lib/libub_client.a \
  ../../../../../public/ub/output/lib/libub_conf.a \
  ../../../../../public/ub/output/lib/libub_galileo.a \
  ../../../../../public/ub/output/lib/libub_log.a \
  ../../../../../public/ub/output/lib/libub_misc.a \
  ../../../../../public/ub/output/lib/libub_monitor.a \
  ../../../../../public/ub/output/lib/libub_naming.a \
  ../../../../../public/ub/output/lib/libub_server.a \
  ../../../../../public/ub/output/lib/libubex.a \
  ../../../../../public/ub/output/lib/libubfw.a \
  ../../../../../public/uconv/libuconv.a \
  ../../../../../public/urlparser/libnewurl.a \
  ../../../../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../../../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../third-64/libxml2/lib/libxml2.a \
  ../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../third-64/snappy/lib/libsnappy.a \
  ../../../../../thirdsrc/protobuf/output/lib/libprotobuf-lite.a \
  ../../../../../thirdsrc/protobuf/output/lib/libprotobuf.a \
  ../../../../../thirdsrc/protobuf/output/lib/libprotoc.a -lpthread \
  -lcrypto \
  -lrt -Xlinker "-)" -o lib-main
	mkdir -p ./output/bin
	cp -f --link lib-main ./output/bin

.PHONY:ccheck
ccheck:.ccheck_das_handler.h.tag \
  .ccheck_das_inc_reader.h.tag \
  .ccheck_das_lib_log.h.tag \
  .ccheck_lib_manager.h.tag \
  .ccheck_lib_util.h.tag \
  .ccheck_reloadable_table_handler.h.tag \
  .ccheck_table_defs.h.tag \
  .ccheck_table_group.h.tag \
  .ccheck_target_type.h.tag \
  .ccheck_das_handler.cpp.tag \
  .ccheck_das_inc_reader.cpp.tag \
  .ccheck_lib_manager.cpp.tag \
  .ccheck_lib_util.cpp.tag \
  .ccheck_main.cpp.tag \
  .ccheck_reloadable_table_handler.cpp.tag \
  .ccheck_table_defs.cpp.tag \
  .ccheck_table_group.cpp.tag
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mccheck[0m']"
	touch .ccheck.tag
	@echo "make ccheck done"

.PHONY:ccp
ccp:ccheck
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mccp[0m']"
	@echo "make ccp done"

lib-main_das_handler.o:das_handler.cpp \
  das_handler.h \
  table_defs.h \
  target_type.h \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_das_handler.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_das_handler.o das_handler.cpp

lib-main_das_inc_reader.o:das_inc_reader.cpp \
  das_inc_reader.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_das_inc_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_das_inc_reader.o das_inc_reader.cpp

lib-main_lib_manager.o:lib_manager.cpp \
  lib_manager.h \
  table_defs.h \
  target_type.h \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp \
  das_inc_reader.h \
  das_handler.h \
  reloadable_table_handler.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_lib_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_lib_manager.o lib_manager.cpp

lib-main_lib_util.o:lib_util.cpp \
  lib_util.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_lib_util.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_lib_util.o lib_util.cpp

lib-main_main.o:main.cpp \
  lib_manager.h \
  table_defs.h \
  target_type.h \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp \
  das_inc_reader.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_main.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_main.o main.cpp

lib-main_reloadable_table_handler.o:reloadable_table_handler.cpp \
  reloadable_table_handler.h \
  table_defs.h \
  target_type.h \
  lib_util.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_reloadable_table_handler.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_reloadable_table_handler.o reloadable_table_handler.cpp

lib-main_table_defs.o:table_defs.cpp \
  table_defs.h \
  target_type.h \
  lib_util.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_table_defs.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_table_defs.o table_defs.cpp

lib-main_table_group.o:table_group.cpp \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp \
  table_defs.h \
  target_type.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlib-main_table_group.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o lib-main_table_group.o table_group.cpp

.ccheck_das_handler.h.tag:das_handler.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_das_handler.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  das_handler.h
	touch .ccheck_das_handler.h.tag

.ccheck_das_inc_reader.h.tag:das_inc_reader.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_das_inc_reader.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  das_inc_reader.h
	touch .ccheck_das_inc_reader.h.tag

.ccheck_das_lib_log.h.tag:das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_das_lib_log.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  das_lib_log.h
	touch .ccheck_das_lib_log.h.tag

.ccheck_lib_manager.h.tag:lib_manager.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_lib_manager.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  lib_manager.h
	touch .ccheck_lib_manager.h.tag

.ccheck_lib_util.h.tag:lib_util.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_lib_util.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  lib_util.h
	touch .ccheck_lib_util.h.tag

.ccheck_reloadable_table_handler.h.tag:reloadable_table_handler.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_reloadable_table_handler.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  reloadable_table_handler.h
	touch .ccheck_reloadable_table_handler.h.tag

.ccheck_table_defs.h.tag:table_defs.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_table_defs.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  table_defs.h
	touch .ccheck_table_defs.h.tag

.ccheck_table_group.h.tag:table_group.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_table_group.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  table_group.h
	touch .ccheck_table_group.h.tag

.ccheck_target_type.h.tag:target_type.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_target_type.h.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  target_type.h
	touch .ccheck_target_type.h.tag

.ccheck_das_handler.cpp.tag:das_handler.cpp \
  das_handler.h \
  table_defs.h \
  target_type.h \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_das_handler.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  das_handler.cpp
	touch .ccheck_das_handler.cpp.tag

.ccheck_das_inc_reader.cpp.tag:das_inc_reader.cpp \
  das_inc_reader.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_das_inc_reader.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  das_inc_reader.cpp
	touch .ccheck_das_inc_reader.cpp.tag

.ccheck_lib_manager.cpp.tag:lib_manager.cpp \
  lib_manager.h \
  table_defs.h \
  target_type.h \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp \
  das_inc_reader.h \
  das_handler.h \
  reloadable_table_handler.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_lib_manager.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  lib_manager.cpp
	touch .ccheck_lib_manager.cpp.tag

.ccheck_lib_util.cpp.tag:lib_util.cpp \
  lib_util.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_lib_util.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  lib_util.cpp
	touch .ccheck_lib_util.cpp.tag

.ccheck_main.cpp.tag:main.cpp \
  lib_manager.h \
  table_defs.h \
  target_type.h \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp \
  das_inc_reader.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_main.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  main.cpp
	touch .ccheck_main.cpp.tag

.ccheck_reloadable_table_handler.cpp.tag:reloadable_table_handler.cpp \
  reloadable_table_handler.h \
  table_defs.h \
  target_type.h \
  lib_util.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_reloadable_table_handler.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  reloadable_table_handler.cpp
	touch .ccheck_reloadable_table_handler.cpp.tag

.ccheck_table_defs.cpp.tag:table_defs.cpp \
  table_defs.h \
  target_type.h \
  lib_util.h \
  das_lib_log.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_table_defs.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  table_defs.cpp
	touch .ccheck_table_defs.cpp.tag

.ccheck_table_group.cpp.tag:table_group.cpp \
  table_group.h \
  table_manager.hpp \
  table_strategy.hpp \
  lib_util.h \
  table_strategy_inl.hpp \
  das_lib_log.h \
  table_manager_inl.hpp \
  lib_dump.hpp \
  table_defs.h \
  target_type.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m.ccheck_table_group.cpp.tag[0m']"
	$(CCHECK) $(CCHECK_FLAGS) $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS)  table_group.cpp
	touch .ccheck_table_group.cpp.tag

endif #ifeq ($(shell uname -m),x86_64)




####################64Bit Mode####################                                                                                                                                                                 #ifeq ($(shell uname -m),x86_64)

CC=gcc
CXX=g++
CXXFLAGS=-pipe \
  -Wall \
  -W \
  -g \
  -O2 \
  -DNDEBUG \
  -fPIC \
  -Wno-invalid-offsetof \
  -fstrict-aliasing \
  -fpermissive
CFLAGS=
CPPFLAGS=
INCPATH=-I./ 
DEP_INCPATH=-Idetail/\
        -I../3rd/include


.PHONY:all
all:libsmalltable.a


.PHONY:ccpclean
ccpclean:

.PHONY:clean
clean:ccpclean
	rm -rf libsmalltable.a
	rm -rf ./output/lib/libsmalltable.a
	rm -rf ./output/include/array_op.hpp
	rm -rf ./output/include/base_function_object.hpp
	rm -rf ./output/include/basic_tuple.hpp
	rm -rf ./output/include/bitmap_index.hpp
	rm -rf ./output/include/bitwise_map.hpp
	rm -rf ./output/include/c_common.hpp
	rm -rf ./output/include/c_connector.hpp
	rm -rf ./output/include/c_list.hpp
	rm -rf ./output/include/c_map.hpp
	rm -rf ./output/include/c_math.hpp
	rm -rf ./output/include/c_predicate.hpp
	rm -rf ./output/include/c_selector.hpp
	rm -rf ./output/include/c_show.hpp
	rm -rf ./output/include/coalesced_hash_map.hpp
	rm -rf ./output/include/combined_tuple.hpp
	rm -rf ./output/include/common_function_objects.hpp
	rm -rf ./output/include/compare.hpp
	rm -rf ./output/include/cow_hash_cluster_map.hpp
	rm -rf ./output/include/cow_hash_cluster_set.hpp
	rm -rf ./output/include/cow_hash_map.hpp
	rm -rf ./output/include/cow_hash_set.hpp
	rm -rf ./output/include/cow_table.hpp
	rm -rf ./output/include/cowbass4.hpp
	rm -rf ./output/include/dodge_hash_map.hpp
	rm -rf ./output/include/fixed_array.hpp
	rm -rf ./output/include/fixed_deque.hpp
	rm -rf ./output/include/functional.hpp
	rm -rf ./output/include/graph.hpp
	rm -rf ./output/include/group_view.hpp
	rm -rf ./output/include/hd_map.hpp
	rm -rf ./output/include/index_traits.hpp
	rm -rf ./output/include/is_base_of.hpp
	rm -rf ./output/include/join_iterator.hpp
	rm -rf ./output/include/linear_hash_map.hpp
	rm -rf ./output/include/named_tuple.hpp
	rm -rf ./output/include/object_pool.hpp
	rm -rf ./output/include/observer.hpp
	rm -rf ./output/include/partial_iterator.hpp
	rm -rf ./output/include/pointer_iterator.hpp
	rm -rf ./output/include/smalltable2.hpp
	rm -rf ./output/include/st_shared_array.hpp
	rm -rf ./output/include/st_shared_ptr.hpp
	rm -rf ./output/include/string_reader.hpp
	rm -rf ./output/include/string_writer.hpp
	rm -rf ./output/include/table_stack.hpp
	rm -rf ./output/include/unique_cluster_index.hpp
	rm -rf ./output/include/unique_index.hpp
	rm -rf ./output/include/version_manager.hpp
	rm -rf ./output/include/array_op.h
	rm -rf ./output/include/attribute.h
	rm -rf ./output/include/base_cow_hash_map.h
	rm -rf ./output/include/base_cowbass4.h
	rm -rf ./output/include/bit_tree.h
	rm -rf ./output/include/bitmap.h
	rm -rf ./output/include/choose_variadic.h
	rm -rf ./output/include/common.h
	rm -rf ./output/include/compile_time_assert.h
	rm -rf ./output/include/debug.h
	rm -rf ./output/include/default_value.h
	rm -rf ./output/include/dyn_index.h
	rm -rf ./output/include/dyn_table.h
	rm -rf ./output/include/dyn_tuple.h
	rm -rf ./output/include/dyn_unique_index.h
	rm -rf ./output/include/get_proc_stat.h
	rm -rf ./output/include/object_hanger.h
	rm -rf ./output/include/rc_memory_pool.h
	rm -rf ./output/include/st_errno.h
	rm -rf ./output/include/st_hash.h
	rm -rf ./output/include/st_timer.h
	rm -rf ./output/include/st_utility.h
	rm -rf detail/smalltable_array_op.o
	rm -rf detail/smalltable_base_cow_hash_map.o
	rm -rf detail/smalltable_base_cowbass4.o
	rm -rf detail/smalltable_binary_file_reader.o
	rm -rf detail/smalltable_dump_fn.o
	rm -rf detail/smalltable_dyn_table.o
	rm -rf detail/smalltable_dyn_tuple.o
	rm -rf detail/smalltable_dyn_unique_index.o
	rm -rf detail/smalltable_get_proc_stat.o
	rm -rf detail/smalltable_load_fn.o
	rm -rf detail/smalltable_string_reader.o
	rm -rf detail/smalltable_version_manager.o


libsmalltable.a:detail/smalltable_array_op.o \
  detail/smalltable_base_cow_hash_map.o \
  detail/smalltable_base_cowbass4.o \
  detail/smalltable_binary_file_reader.o \
  detail/smalltable_dump_fn.o \
  detail/smalltable_dyn_table.o \
  detail/smalltable_dyn_tuple.o \
  detail/smalltable_dyn_unique_index.o \
  detail/smalltable_get_proc_stat.o \
  detail/smalltable_load_fn.o \
  detail/smalltable_string_reader.o \
  detail/smalltable_version_manager.o \
  array_op.hpp \
  base_function_object.hpp \
  basic_tuple.hpp \
  bitmap_index.hpp \
  bitwise_map.hpp \
  c_common.hpp \
  c_connector.hpp \
  c_list.hpp \
  c_map.hpp \
  c_math.hpp \
  c_predicate.hpp \
  c_selector.hpp \
  c_show.hpp \
  coalesced_hash_map.hpp \
  combined_tuple.hpp \
  common_function_objects.hpp \
  compare.hpp \
  cow_hash_cluster_map.hpp \
  cow_hash_cluster_set.hpp \
  cow_hash_map.hpp \
  cow_hash_set.hpp \
  cow_table.hpp \
  cowbass4.hpp \
  dodge_hash_map.hpp \
  fixed_array.hpp \
  fixed_deque.hpp \
  functional.hpp \
  graph.hpp \
  group_view.hpp \
  hd_map.hpp \
  index_traits.hpp \
  is_base_of.hpp \
  join_iterator.hpp \
  linear_hash_map.hpp \
  named_tuple.hpp \
  object_pool.hpp \
  observer.hpp \
  partial_iterator.hpp \
  pointer_iterator.hpp \
  smalltable2.hpp \
  st_shared_array.hpp \
  st_shared_ptr.hpp \
  string_reader.hpp \
  string_writer.hpp \
  table_stack.hpp \
  unique_cluster_index.hpp \
  unique_index.hpp \
  version_manager.hpp \
  array_op.h \
  attribute.h \
  base_cow_hash_map.h \
  base_cowbass4.h \
  bit_tree.h \
  bitmap.h \
  choose_variadic.h \
  common.h \
  compile_time_assert.h \
  debug.h \
  default_value.h \
  dyn_index.h \
  dyn_table.h \
  dyn_tuple.h \
  dyn_unique_index.h \
  get_proc_stat.h \
  object_hanger.h \
  rc_memory_pool.h \
  st_errno.h \
  st_hash.h \
  st_timer.h \
  st_utility.h
	ar crs libsmalltable.a detail/smalltable_array_op.o \
  detail/smalltable_base_cow_hash_map.o \
  detail/smalltable_base_cowbass4.o \
  detail/smalltable_binary_file_reader.o \
  detail/smalltable_dump_fn.o \
  detail/smalltable_dyn_table.o \
  detail/smalltable_dyn_tuple.o \
  detail/smalltable_dyn_unique_index.o \
  detail/smalltable_get_proc_stat.o \
  detail/smalltable_load_fn.o \
  detail/smalltable_string_reader.o \
  detail/smalltable_version_manager.o
	mkdir -p ./output/lib
	cp -f libsmalltable.a ./output/lib
	mkdir -p ./output/include
	cp -f array_op.hpp base_function_object.hpp basic_tuple.hpp bitmap_index.hpp bitwise_map.hpp c_common.hpp c_connector.hpp c_list.hpp c_map.hpp c_math.hpp c_predicate.hpp c_selector.hpp c_show.hpp coalesced_hash_map.hpp combined_tuple.hpp common_function_objects.hpp compare.hpp cow_hash_cluster_map.hpp cow_hash_cluster_set.hpp cow_hash_map.hpp cow_hash_set.hpp cow_table.hpp cowbass4.hpp dodge_hash_map.hpp fixed_array.hpp fixed_deque.hpp functional.hpp graph.hpp group_view.hpp hd_map.hpp index_traits.hpp is_base_of.hpp join_iterator.hpp linear_hash_map.hpp named_tuple.hpp object_pool.hpp observer.hpp partial_iterator.hpp pointer_iterator.hpp smalltable2.hpp st_shared_array.hpp st_shared_ptr.hpp string_reader.hpp string_writer.hpp table_stack.hpp unique_cluster_index.hpp unique_index.hpp version_manager.hpp array_op.h attribute.h base_cow_hash_map.h base_cowbass4.h bit_tree.h bitmap.h choose_variadic.h common.h compile_time_assert.h debug.h default_value.h dyn_index.h dyn_table.h dyn_tuple.h dyn_unique_index.h get_proc_stat.h object_hanger.h rc_memory_pool.h st_errno.h st_hash.h st_timer.h st_utility.h ./output/include

detail/smalltable_array_op.o:detail/array_op.cpp \
  array_op.h \
  debug.h \
  st_errno.h \
  st_timer.h
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_array_op.o detail/array_op.cpp

detail/smalltable_base_cow_hash_map.o:detail/base_cow_hash_map.cpp \
  base_cow_hash_map.h \
  rc_memory_pool.h \
  common.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h \
  st_shared_ptr.hpp \
  detail/st_shared_count.hpp \
  detail/st_sp_counted_base.hpp \
  detail/st_sp_counted_base_gcc_x86.hpp \
  detail/st_sp_counted_impl.hpp \
  st_utility.h
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_base_cow_hash_map.o detail/base_cow_hash_map.cpp

detail/smalltable_base_cowbass4.o:detail/base_cowbass4.cpp \
  base_cowbass4.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  array_op.h \
  rc_memory_pool.h \
  common.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h \
  st_shared_ptr.hpp \
  detail/st_shared_count.hpp \
  detail/st_sp_counted_base.hpp \
  detail/st_sp_counted_base_gcc_x86.hpp \
  detail/st_sp_counted_impl.hpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_base_cowbass4.o detail/base_cowbass4.cpp

detail/smalltable_binary_file_reader.o:detail/binary_file_reader.cpp \
  detail/binary_file_reader.h \
  detail/meta_reader.hpp \
  debug.h \
  st_errno.h \
  st_timer.h
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_binary_file_reader.o detail/binary_file_reader.cpp

detail/smalltable_dump_fn.o:detail/dump_fn.cpp \
  detail/dump_fn.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  detail/meta_reader.hpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_dump_fn.o detail/dump_fn.cpp

detail/smalltable_dyn_table.o:detail/dyn_table.cpp \
  dyn_table.h \
  dyn_index.h \
  dyn_tuple.h \
  common.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h \
  st_hash.h \
  detail/dyn_tuple_inl.h \
  compare.hpp \
  base_function_object.hpp \
  detail/base_function_object_inl.hpp \
  index_traits.hpp \
  partial_iterator.hpp \
  pointer_iterator.hpp \
  default_value.h \
  cow_hash_cluster_set.hpp \
  cow_hash_set.hpp \
  base_cow_hash_map.h \
  rc_memory_pool.h \
  st_shared_ptr.hpp \
  detail/st_shared_count.hpp \
  detail/st_sp_counted_base.hpp \
  detail/st_sp_counted_base_gcc_x86.hpp \
  detail/st_sp_counted_impl.hpp \
  cowbass4.hpp \
  c_math.hpp \
  array_op.hpp \
  common_function_objects.hpp \
  st_utility.h \
  combined_tuple.hpp \
  named_tuple.hpp \
  basic_tuple.hpp \
  c_list.hpp \
  detail/c_list_inl.hpp \
  attribute.h \
  dyn_unique_index.h \
  string_reader.hpp \
  detail/string_reader_inl.hpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_dyn_table.o detail/dyn_table.cpp

detail/smalltable_dyn_tuple.o:detail/dyn_tuple.cpp \
  dyn_tuple.h \
  common.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h \
  st_hash.h \
  detail/dyn_tuple_inl.h \
  compare.hpp \
  base_function_object.hpp \
  detail/base_function_object_inl.hpp \
  string_reader.hpp \
  detail/string_reader_inl.hpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_dyn_tuple.o detail/dyn_tuple.cpp

detail/smalltable_dyn_unique_index.o:detail/dyn_unique_index.cpp \
  dyn_unique_index.h \
  dyn_index.h \
  dyn_tuple.h \
  common.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h \
  st_hash.h \
  detail/dyn_tuple_inl.h \
  compare.hpp \
  base_function_object.hpp \
  detail/base_function_object_inl.hpp \
  index_traits.hpp \
  partial_iterator.hpp \
  pointer_iterator.hpp \
  default_value.h \
  cow_hash_cluster_set.hpp \
  cow_hash_set.hpp \
  base_cow_hash_map.h \
  rc_memory_pool.h \
  st_shared_ptr.hpp \
  detail/st_shared_count.hpp \
  detail/st_sp_counted_base.hpp \
  detail/st_sp_counted_base_gcc_x86.hpp \
  detail/st_sp_counted_impl.hpp \
  cowbass4.hpp \
  c_math.hpp \
  array_op.hpp \
  common_function_objects.hpp \
  st_utility.h \
  combined_tuple.hpp \
  named_tuple.hpp \
  basic_tuple.hpp \
  c_list.hpp \
  detail/c_list_inl.hpp \
  attribute.h
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_dyn_unique_index.o detail/dyn_unique_index.cpp

detail/smalltable_get_proc_stat.o:detail/get_proc_stat.cpp \
  get_proc_stat.h
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_get_proc_stat.o detail/get_proc_stat.cpp

detail/smalltable_load_fn.o:detail/load_fn.cpp \
  detail/load_fn.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  detail/binary_file_reader.h \
  detail/meta_reader.hpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_load_fn.o detail/load_fn.cpp

detail/smalltable_string_reader.o:detail/string_reader.cpp \
  string_reader.hpp \
  common.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h \
  detail/string_reader_inl.hpp
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_string_reader.o detail/string_reader.cpp

detail/smalltable_version_manager.o:detail/version_manager.cpp \
  version_manager.hpp \
  common.h \
  debug.h \
  st_errno.h \
  st_timer.h \
  string_writer.hpp \
  object_hanger.h \
  c_common.hpp \
  compile_time_assert.h \
  is_base_of.hpp \
  c_show.hpp \
  choose_variadic.h
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o detail/smalltable_version_manager.o detail/version_manager.cpp

#endif #ifeq ($(shell uname -m),x86_64)

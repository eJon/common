#COMAKE2 edit-mode: -*- Makefile -*-
####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
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
  -fstrict-aliasing
CFLAGS=
CPPFLAGS=
INCPATH=-I../
DEP_INCPATH=-I../../../../../../com/btest/gtest \
  -I../../../../../../com/btest/gtest/include \
  -I../../../../../../com/btest/gtest/output \
  -I../../../../../../com/btest/gtest/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/dict \
  -I../../../../../../lib2-64/dict/include \
  -I../../../../../../lib2-64/dict/output \
  -I../../../../../../lib2-64/dict/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/odict \
  -I../../../../../../public/odict/include \
  -I../../../../../../public/odict/output \
  -I../../../../../../public/odict/output/include \
  -I../../../../../../quality/autotest/reportlib/cpp \
  -I../../../../../../quality/autotest/reportlib/cpp/include \
  -I../../../../../../quality/autotest/reportlib/cpp/output \
  -I../../../../../../quality/autotest/reportlib/cpp/output/include

#============ CCP vars ============
CCHECK=@ccheck.py
CCHECK_FLAGS=
PCLINT=@pclint
PCLINT_FLAGS=
CCP=@ccp.py
CCP_FLAGS=


#COMAKE UUID
COMAKE_MD5=a7c6bccce95c2b518baa80c456495a7a  COMAKE


.PHONY:all
all:comake2_makefile_check test_cowbass4 test_cow_hash_map test_bitwise_map test_bitmap_index test_hd_map test_dodge_hash_map test_bit_tree test_bit_tree test_version_manager test_array_op test_void_array_op test_shared_ptr test_hash_map test_hash_cluster test_cow_table test_group_view test_fun test_selector test_named_tuple test_perf test_pool test_string_writer test_string_reader test_graph test_fixed_deque test_template_folding test_stack test_dyn_tuple test_memory_ref test_offset_list test_adt test_override_and_specialize test_dyn_unique_index test_dyn_table test_load_dump_fn 
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
	@echo "make ccpclean done"

.PHONY:clean
clean:ccpclean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf test_cowbass4
	rm -rf ./output/bin/test_cowbass4
	rm -rf test_cow_hash_map
	rm -rf ./output/bin/test_cow_hash_map
	rm -rf test_bitwise_map
	rm -rf ./output/bin/test_bitwise_map
	rm -rf test_bitmap_index
	rm -rf ./output/bin/test_bitmap_index
	rm -rf test_hd_map
	rm -rf ./output/bin/test_hd_map
	rm -rf test_dodge_hash_map
	rm -rf ./output/bin/test_dodge_hash_map
	rm -rf test_bit_tree
	rm -rf ./output/bin/test_bit_tree
	rm -rf test_bit_tree
	rm -rf ./output/bin/test_bit_tree
	rm -rf test_version_manager
	rm -rf ./output/bin/test_version_manager
	rm -rf test_array_op
	rm -rf ./output/bin/test_array_op
	rm -rf test_void_array_op
	rm -rf ./output/bin/test_void_array_op
	rm -rf test_shared_ptr
	rm -rf ./output/bin/test_shared_ptr
	rm -rf test_hash_map
	rm -rf ./output/bin/test_hash_map
	rm -rf test_hash_cluster
	rm -rf ./output/bin/test_hash_cluster
	rm -rf test_cow_table
	rm -rf ./output/bin/test_cow_table
	rm -rf test_group_view
	rm -rf ./output/bin/test_group_view
	rm -rf test_fun
	rm -rf ./output/bin/test_fun
	rm -rf test_selector
	rm -rf ./output/bin/test_selector
	rm -rf test_named_tuple
	rm -rf ./output/bin/test_named_tuple
	rm -rf test_perf
	rm -rf ./output/bin/test_perf
	rm -rf test_pool
	rm -rf ./output/bin/test_pool
	rm -rf test_string_writer
	rm -rf ./output/bin/test_string_writer
	rm -rf test_string_reader
	rm -rf ./output/bin/test_string_reader
	rm -rf test_graph
	rm -rf ./output/bin/test_graph
	rm -rf test_fixed_deque
	rm -rf ./output/bin/test_fixed_deque
	rm -rf test_template_folding
	rm -rf ./output/bin/test_template_folding
	rm -rf test_stack
	rm -rf ./output/bin/test_stack
	rm -rf test_dyn_tuple
	rm -rf ./output/bin/test_dyn_tuple
	rm -rf test_memory_ref
	rm -rf ./output/bin/test_memory_ref
	rm -rf test_offset_list
	rm -rf ./output/bin/test_offset_list
	rm -rf test_adt
	rm -rf ./output/bin/test_adt
	rm -rf test_override_and_specialize
	rm -rf ./output/bin/test_override_and_specialize
	rm -rf test_dyn_unique_index
	rm -rf ./output/bin/test_dyn_unique_index
	rm -rf test_dyn_table
	rm -rf ./output/bin/test_dyn_table
	rm -rf test_load_dump_fn
	rm -rf ./output/bin/test_load_dump_fn
	rm -rf ../detail/test_cowbass4_array_op.o
	rm -rf ../detail/test_cowbass4_base_cow_hash_map.o
	rm -rf ../detail/test_cowbass4_base_cowbass4.o
	rm -rf ../detail/test_cowbass4_binary_file_reader.o
	rm -rf ../detail/test_cowbass4_dump_fn.o
	rm -rf ../detail/test_cowbass4_dyn_table.o
	rm -rf ../detail/test_cowbass4_dyn_tuple.o
	rm -rf ../detail/test_cowbass4_dyn_unique_index.o
	rm -rf ../detail/test_cowbass4_get_proc_stat.o
	rm -rf ../detail/test_cowbass4_load_fn.o
	rm -rf ../detail/test_cowbass4_string_reader.o
	rm -rf ../detail/test_cowbass4_version_manager.o
	rm -rf test_cowbass4_test_cowbass4.o
	rm -rf ../detail/test_cow_hash_map_array_op.o
	rm -rf ../detail/test_cow_hash_map_base_cow_hash_map.o
	rm -rf ../detail/test_cow_hash_map_base_cowbass4.o
	rm -rf ../detail/test_cow_hash_map_binary_file_reader.o
	rm -rf ../detail/test_cow_hash_map_dump_fn.o
	rm -rf ../detail/test_cow_hash_map_dyn_table.o
	rm -rf ../detail/test_cow_hash_map_dyn_tuple.o
	rm -rf ../detail/test_cow_hash_map_dyn_unique_index.o
	rm -rf ../detail/test_cow_hash_map_get_proc_stat.o
	rm -rf ../detail/test_cow_hash_map_load_fn.o
	rm -rf ../detail/test_cow_hash_map_string_reader.o
	rm -rf ../detail/test_cow_hash_map_version_manager.o
	rm -rf test_cow_hash_map_test_cow_hash_map.o
	rm -rf ../detail/test_bitwise_map_array_op.o
	rm -rf ../detail/test_bitwise_map_base_cow_hash_map.o
	rm -rf ../detail/test_bitwise_map_base_cowbass4.o
	rm -rf ../detail/test_bitwise_map_binary_file_reader.o
	rm -rf ../detail/test_bitwise_map_dump_fn.o
	rm -rf ../detail/test_bitwise_map_dyn_table.o
	rm -rf ../detail/test_bitwise_map_dyn_tuple.o
	rm -rf ../detail/test_bitwise_map_dyn_unique_index.o
	rm -rf ../detail/test_bitwise_map_get_proc_stat.o
	rm -rf ../detail/test_bitwise_map_load_fn.o
	rm -rf ../detail/test_bitwise_map_string_reader.o
	rm -rf ../detail/test_bitwise_map_version_manager.o
	rm -rf test_bitwise_map_test_bitwise_map.o
	rm -rf ../detail/test_bitwise_map_bitmap.o
	rm -rf ../detail/test_bitmap_index_array_op.o
	rm -rf ../detail/test_bitmap_index_base_cow_hash_map.o
	rm -rf ../detail/test_bitmap_index_base_cowbass4.o
	rm -rf ../detail/test_bitmap_index_binary_file_reader.o
	rm -rf ../detail/test_bitmap_index_dump_fn.o
	rm -rf ../detail/test_bitmap_index_dyn_table.o
	rm -rf ../detail/test_bitmap_index_dyn_tuple.o
	rm -rf ../detail/test_bitmap_index_dyn_unique_index.o
	rm -rf ../detail/test_bitmap_index_get_proc_stat.o
	rm -rf ../detail/test_bitmap_index_load_fn.o
	rm -rf ../detail/test_bitmap_index_string_reader.o
	rm -rf ../detail/test_bitmap_index_version_manager.o
	rm -rf test_bitmap_index_test_bitmap_index.o
	rm -rf ../detail/test_bitmap_index_bitmap.o
	rm -rf ../detail/test_hd_map_array_op.o
	rm -rf ../detail/test_hd_map_base_cow_hash_map.o
	rm -rf ../detail/test_hd_map_base_cowbass4.o
	rm -rf ../detail/test_hd_map_binary_file_reader.o
	rm -rf ../detail/test_hd_map_dump_fn.o
	rm -rf ../detail/test_hd_map_dyn_table.o
	rm -rf ../detail/test_hd_map_dyn_tuple.o
	rm -rf ../detail/test_hd_map_dyn_unique_index.o
	rm -rf ../detail/test_hd_map_get_proc_stat.o
	rm -rf ../detail/test_hd_map_load_fn.o
	rm -rf ../detail/test_hd_map_string_reader.o
	rm -rf ../detail/test_hd_map_version_manager.o
	rm -rf test_hd_map_test_hd_map.o
	rm -rf ../detail/test_dodge_hash_map_array_op.o
	rm -rf ../detail/test_dodge_hash_map_base_cow_hash_map.o
	rm -rf ../detail/test_dodge_hash_map_base_cowbass4.o
	rm -rf ../detail/test_dodge_hash_map_binary_file_reader.o
	rm -rf ../detail/test_dodge_hash_map_dump_fn.o
	rm -rf ../detail/test_dodge_hash_map_dyn_table.o
	rm -rf ../detail/test_dodge_hash_map_dyn_tuple.o
	rm -rf ../detail/test_dodge_hash_map_dyn_unique_index.o
	rm -rf ../detail/test_dodge_hash_map_get_proc_stat.o
	rm -rf ../detail/test_dodge_hash_map_load_fn.o
	rm -rf ../detail/test_dodge_hash_map_string_reader.o
	rm -rf ../detail/test_dodge_hash_map_version_manager.o
	rm -rf test_dodge_hash_map_test_dodge_hash_map.o
	rm -rf test_bit_tree_test_bit_tree.o
	rm -rf ../detail/test_version_manager_array_op.o
	rm -rf ../detail/test_version_manager_base_cow_hash_map.o
	rm -rf ../detail/test_version_manager_base_cowbass4.o
	rm -rf ../detail/test_version_manager_binary_file_reader.o
	rm -rf ../detail/test_version_manager_dump_fn.o
	rm -rf ../detail/test_version_manager_dyn_table.o
	rm -rf ../detail/test_version_manager_dyn_tuple.o
	rm -rf ../detail/test_version_manager_dyn_unique_index.o
	rm -rf ../detail/test_version_manager_get_proc_stat.o
	rm -rf ../detail/test_version_manager_load_fn.o
	rm -rf ../detail/test_version_manager_string_reader.o
	rm -rf ../detail/test_version_manager_version_manager.o
	rm -rf test_version_manager_test_version_manager.o
	rm -rf test_array_op_test_array_op.o
	rm -rf test_void_array_op_test_void_array_op.o
	rm -rf ../detail/test_void_array_op_array_op.o
	rm -rf test_shared_ptr_test_shared_ptr.o
	rm -rf ../detail/test_hash_map_array_op.o
	rm -rf ../detail/test_hash_map_base_cow_hash_map.o
	rm -rf ../detail/test_hash_map_base_cowbass4.o
	rm -rf ../detail/test_hash_map_binary_file_reader.o
	rm -rf ../detail/test_hash_map_dump_fn.o
	rm -rf ../detail/test_hash_map_dyn_table.o
	rm -rf ../detail/test_hash_map_dyn_tuple.o
	rm -rf ../detail/test_hash_map_dyn_unique_index.o
	rm -rf ../detail/test_hash_map_get_proc_stat.o
	rm -rf ../detail/test_hash_map_load_fn.o
	rm -rf ../detail/test_hash_map_string_reader.o
	rm -rf ../detail/test_hash_map_version_manager.o
	rm -rf test_hash_map_test_hash_map.o
	rm -rf ../detail/test_hash_cluster_array_op.o
	rm -rf ../detail/test_hash_cluster_base_cow_hash_map.o
	rm -rf ../detail/test_hash_cluster_base_cowbass4.o
	rm -rf ../detail/test_hash_cluster_binary_file_reader.o
	rm -rf ../detail/test_hash_cluster_dump_fn.o
	rm -rf ../detail/test_hash_cluster_dyn_table.o
	rm -rf ../detail/test_hash_cluster_dyn_tuple.o
	rm -rf ../detail/test_hash_cluster_dyn_unique_index.o
	rm -rf ../detail/test_hash_cluster_get_proc_stat.o
	rm -rf ../detail/test_hash_cluster_load_fn.o
	rm -rf ../detail/test_hash_cluster_string_reader.o
	rm -rf ../detail/test_hash_cluster_version_manager.o
	rm -rf test_hash_cluster_test_hash_cluster.o
	rm -rf ../detail/test_cow_table_array_op.o
	rm -rf ../detail/test_cow_table_base_cow_hash_map.o
	rm -rf ../detail/test_cow_table_base_cowbass4.o
	rm -rf ../detail/test_cow_table_binary_file_reader.o
	rm -rf ../detail/test_cow_table_dump_fn.o
	rm -rf ../detail/test_cow_table_dyn_table.o
	rm -rf ../detail/test_cow_table_dyn_tuple.o
	rm -rf ../detail/test_cow_table_dyn_unique_index.o
	rm -rf ../detail/test_cow_table_get_proc_stat.o
	rm -rf ../detail/test_cow_table_load_fn.o
	rm -rf ../detail/test_cow_table_string_reader.o
	rm -rf ../detail/test_cow_table_version_manager.o
	rm -rf test_cow_table_test_cow_table.o
	rm -rf ../detail/test_group_view_array_op.o
	rm -rf ../detail/test_group_view_base_cow_hash_map.o
	rm -rf ../detail/test_group_view_base_cowbass4.o
	rm -rf ../detail/test_group_view_binary_file_reader.o
	rm -rf ../detail/test_group_view_dump_fn.o
	rm -rf ../detail/test_group_view_dyn_table.o
	rm -rf ../detail/test_group_view_dyn_tuple.o
	rm -rf ../detail/test_group_view_dyn_unique_index.o
	rm -rf ../detail/test_group_view_get_proc_stat.o
	rm -rf ../detail/test_group_view_load_fn.o
	rm -rf ../detail/test_group_view_string_reader.o
	rm -rf ../detail/test_group_view_version_manager.o
	rm -rf test_group_view_test_group_view.o
	rm -rf test_fun_test_fun.o
	rm -rf ../detail/test_selector_array_op.o
	rm -rf ../detail/test_selector_base_cow_hash_map.o
	rm -rf ../detail/test_selector_base_cowbass4.o
	rm -rf ../detail/test_selector_binary_file_reader.o
	rm -rf ../detail/test_selector_dump_fn.o
	rm -rf ../detail/test_selector_dyn_table.o
	rm -rf ../detail/test_selector_dyn_tuple.o
	rm -rf ../detail/test_selector_dyn_unique_index.o
	rm -rf ../detail/test_selector_get_proc_stat.o
	rm -rf ../detail/test_selector_load_fn.o
	rm -rf ../detail/test_selector_string_reader.o
	rm -rf ../detail/test_selector_version_manager.o
	rm -rf test_selector_test_selector.o
	rm -rf ../detail/test_named_tuple_array_op.o
	rm -rf ../detail/test_named_tuple_base_cow_hash_map.o
	rm -rf ../detail/test_named_tuple_base_cowbass4.o
	rm -rf ../detail/test_named_tuple_binary_file_reader.o
	rm -rf ../detail/test_named_tuple_dump_fn.o
	rm -rf ../detail/test_named_tuple_dyn_table.o
	rm -rf ../detail/test_named_tuple_dyn_tuple.o
	rm -rf ../detail/test_named_tuple_dyn_unique_index.o
	rm -rf ../detail/test_named_tuple_get_proc_stat.o
	rm -rf ../detail/test_named_tuple_load_fn.o
	rm -rf ../detail/test_named_tuple_string_reader.o
	rm -rf ../detail/test_named_tuple_version_manager.o
	rm -rf test_named_tuple_test_named_tuple.o
	rm -rf test_perf_test_perf.o
	rm -rf ../detail/test_perf_get_proc_stat.o
	rm -rf ../detail/test_pool_array_op.o
	rm -rf ../detail/test_pool_base_cow_hash_map.o
	rm -rf ../detail/test_pool_base_cowbass4.o
	rm -rf ../detail/test_pool_binary_file_reader.o
	rm -rf ../detail/test_pool_dump_fn.o
	rm -rf ../detail/test_pool_dyn_table.o
	rm -rf ../detail/test_pool_dyn_tuple.o
	rm -rf ../detail/test_pool_dyn_unique_index.o
	rm -rf ../detail/test_pool_get_proc_stat.o
	rm -rf ../detail/test_pool_load_fn.o
	rm -rf ../detail/test_pool_string_reader.o
	rm -rf ../detail/test_pool_version_manager.o
	rm -rf test_pool_test_pool.o
	rm -rf ../detail/test_string_writer_array_op.o
	rm -rf ../detail/test_string_writer_base_cow_hash_map.o
	rm -rf ../detail/test_string_writer_base_cowbass4.o
	rm -rf ../detail/test_string_writer_binary_file_reader.o
	rm -rf ../detail/test_string_writer_dump_fn.o
	rm -rf ../detail/test_string_writer_dyn_table.o
	rm -rf ../detail/test_string_writer_dyn_tuple.o
	rm -rf ../detail/test_string_writer_dyn_unique_index.o
	rm -rf ../detail/test_string_writer_get_proc_stat.o
	rm -rf ../detail/test_string_writer_load_fn.o
	rm -rf ../detail/test_string_writer_string_reader.o
	rm -rf ../detail/test_string_writer_version_manager.o
	rm -rf test_string_writer_test_string_writer.o
	rm -rf test_string_reader_test_string_reader.o
	rm -rf ../detail/test_string_reader_string_reader.o
	rm -rf ../detail/test_graph_array_op.o
	rm -rf ../detail/test_graph_base_cow_hash_map.o
	rm -rf ../detail/test_graph_base_cowbass4.o
	rm -rf ../detail/test_graph_binary_file_reader.o
	rm -rf ../detail/test_graph_dump_fn.o
	rm -rf ../detail/test_graph_dyn_table.o
	rm -rf ../detail/test_graph_dyn_tuple.o
	rm -rf ../detail/test_graph_dyn_unique_index.o
	rm -rf ../detail/test_graph_get_proc_stat.o
	rm -rf ../detail/test_graph_load_fn.o
	rm -rf ../detail/test_graph_string_reader.o
	rm -rf ../detail/test_graph_version_manager.o
	rm -rf test_graph_test_graph.o
	rm -rf ../detail/test_fixed_deque_array_op.o
	rm -rf ../detail/test_fixed_deque_base_cow_hash_map.o
	rm -rf ../detail/test_fixed_deque_base_cowbass4.o
	rm -rf ../detail/test_fixed_deque_binary_file_reader.o
	rm -rf ../detail/test_fixed_deque_dump_fn.o
	rm -rf ../detail/test_fixed_deque_dyn_table.o
	rm -rf ../detail/test_fixed_deque_dyn_tuple.o
	rm -rf ../detail/test_fixed_deque_dyn_unique_index.o
	rm -rf ../detail/test_fixed_deque_get_proc_stat.o
	rm -rf ../detail/test_fixed_deque_load_fn.o
	rm -rf ../detail/test_fixed_deque_string_reader.o
	rm -rf ../detail/test_fixed_deque_version_manager.o
	rm -rf test_fixed_deque_test_fixed_deque.o
	rm -rf test_template_folding_test_template_folding.o
	rm -rf ../detail/test_stack_array_op.o
	rm -rf ../detail/test_stack_base_cow_hash_map.o
	rm -rf ../detail/test_stack_base_cowbass4.o
	rm -rf ../detail/test_stack_binary_file_reader.o
	rm -rf ../detail/test_stack_dump_fn.o
	rm -rf ../detail/test_stack_dyn_table.o
	rm -rf ../detail/test_stack_dyn_tuple.o
	rm -rf ../detail/test_stack_dyn_unique_index.o
	rm -rf ../detail/test_stack_get_proc_stat.o
	rm -rf ../detail/test_stack_load_fn.o
	rm -rf ../detail/test_stack_string_reader.o
	rm -rf ../detail/test_stack_version_manager.o
	rm -rf test_stack_test_stack.o
	rm -rf test_dyn_tuple_test_dyn_tuple.o
	rm -rf ../detail/test_dyn_tuple_dyn_tuple.o
	rm -rf ../detail/test_dyn_tuple_string_reader.o
	rm -rf test_memory_ref_test_memory_ref.o
	rm -rf test_offset_list_test_offset_list.o
	rm -rf test_adt_test_adt.o
	rm -rf test_override_and_specialize_test_override_and_specialize.o
	rm -rf test_dyn_unique_index_test_dyn_unique_index.o
	rm -rf ../detail/test_dyn_unique_index_dyn_tuple.o
	rm -rf ../detail/test_dyn_unique_index_dyn_unique_index.o
	rm -rf ../detail/test_dyn_unique_index_base_cow_hash_map.o
	rm -rf ../detail/test_dyn_unique_index_string_reader.o
	rm -rf test_dyn_table_test_dyn_table.o
	rm -rf ../detail/test_dyn_table_dyn_table.o
	rm -rf ../detail/test_dyn_table_dyn_tuple.o
	rm -rf ../detail/test_dyn_table_dyn_unique_index.o
	rm -rf ../detail/test_dyn_table_base_cow_hash_map.o
	rm -rf ../detail/test_dyn_table_string_reader.o
	rm -rf ../detail/test_load_dump_fn_array_op.o
	rm -rf ../detail/test_load_dump_fn_base_cow_hash_map.o
	rm -rf ../detail/test_load_dump_fn_base_cowbass4.o
	rm -rf ../detail/test_load_dump_fn_binary_file_reader.o
	rm -rf ../detail/test_load_dump_fn_dump_fn.o
	rm -rf ../detail/test_load_dump_fn_dyn_table.o
	rm -rf ../detail/test_load_dump_fn_dyn_tuple.o
	rm -rf ../detail/test_load_dump_fn_dyn_unique_index.o
	rm -rf ../detail/test_load_dump_fn_get_proc_stat.o
	rm -rf ../detail/test_load_dump_fn_load_fn.o
	rm -rf ../detail/test_load_dump_fn_string_reader.o
	rm -rf ../detail/test_load_dump_fn_version_manager.o
	rm -rf test_load_dump_fn_test_load_dump_fn.o

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

test_cowbass4:../detail/test_cowbass4_array_op.o \
  ../detail/test_cowbass4_base_cow_hash_map.o \
  ../detail/test_cowbass4_base_cowbass4.o \
  ../detail/test_cowbass4_binary_file_reader.o \
  ../detail/test_cowbass4_dump_fn.o \
  ../detail/test_cowbass4_dyn_table.o \
  ../detail/test_cowbass4_dyn_tuple.o \
  ../detail/test_cowbass4_dyn_unique_index.o \
  ../detail/test_cowbass4_get_proc_stat.o \
  ../detail/test_cowbass4_load_fn.o \
  ../detail/test_cowbass4_string_reader.o \
  ../detail/test_cowbass4_version_manager.o \
  test_cowbass4_test_cowbass4.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_cowbass4[0m']"
	$(CXX) ../detail/test_cowbass4_array_op.o \
  ../detail/test_cowbass4_base_cow_hash_map.o \
  ../detail/test_cowbass4_base_cowbass4.o \
  ../detail/test_cowbass4_binary_file_reader.o \
  ../detail/test_cowbass4_dump_fn.o \
  ../detail/test_cowbass4_dyn_table.o \
  ../detail/test_cowbass4_dyn_tuple.o \
  ../detail/test_cowbass4_dyn_unique_index.o \
  ../detail/test_cowbass4_get_proc_stat.o \
  ../detail/test_cowbass4_load_fn.o \
  ../detail/test_cowbass4_string_reader.o \
  ../detail/test_cowbass4_version_manager.o \
  test_cowbass4_test_cowbass4.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_cowbass4
	mkdir -p ./output/bin
	cp -f --link test_cowbass4 ./output/bin

test_cow_hash_map:../detail/test_cow_hash_map_array_op.o \
  ../detail/test_cow_hash_map_base_cow_hash_map.o \
  ../detail/test_cow_hash_map_base_cowbass4.o \
  ../detail/test_cow_hash_map_binary_file_reader.o \
  ../detail/test_cow_hash_map_dump_fn.o \
  ../detail/test_cow_hash_map_dyn_table.o \
  ../detail/test_cow_hash_map_dyn_tuple.o \
  ../detail/test_cow_hash_map_dyn_unique_index.o \
  ../detail/test_cow_hash_map_get_proc_stat.o \
  ../detail/test_cow_hash_map_load_fn.o \
  ../detail/test_cow_hash_map_string_reader.o \
  ../detail/test_cow_hash_map_version_manager.o \
  test_cow_hash_map_test_cow_hash_map.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_cow_hash_map[0m']"
	$(CXX) ../detail/test_cow_hash_map_array_op.o \
  ../detail/test_cow_hash_map_base_cow_hash_map.o \
  ../detail/test_cow_hash_map_base_cowbass4.o \
  ../detail/test_cow_hash_map_binary_file_reader.o \
  ../detail/test_cow_hash_map_dump_fn.o \
  ../detail/test_cow_hash_map_dyn_table.o \
  ../detail/test_cow_hash_map_dyn_tuple.o \
  ../detail/test_cow_hash_map_dyn_unique_index.o \
  ../detail/test_cow_hash_map_get_proc_stat.o \
  ../detail/test_cow_hash_map_load_fn.o \
  ../detail/test_cow_hash_map_string_reader.o \
  ../detail/test_cow_hash_map_version_manager.o \
  test_cow_hash_map_test_cow_hash_map.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_cow_hash_map
	mkdir -p ./output/bin
	cp -f --link test_cow_hash_map ./output/bin

test_bitwise_map:../detail/test_bitwise_map_array_op.o \
  ../detail/test_bitwise_map_base_cow_hash_map.o \
  ../detail/test_bitwise_map_base_cowbass4.o \
  ../detail/test_bitwise_map_binary_file_reader.o \
  ../detail/test_bitwise_map_dump_fn.o \
  ../detail/test_bitwise_map_dyn_table.o \
  ../detail/test_bitwise_map_dyn_tuple.o \
  ../detail/test_bitwise_map_dyn_unique_index.o \
  ../detail/test_bitwise_map_get_proc_stat.o \
  ../detail/test_bitwise_map_load_fn.o \
  ../detail/test_bitwise_map_string_reader.o \
  ../detail/test_bitwise_map_version_manager.o \
  test_bitwise_map_test_bitwise_map.o \
  ../detail/test_bitwise_map_bitmap.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_bitwise_map[0m']"
	$(CXX) ../detail/test_bitwise_map_array_op.o \
  ../detail/test_bitwise_map_base_cow_hash_map.o \
  ../detail/test_bitwise_map_base_cowbass4.o \
  ../detail/test_bitwise_map_binary_file_reader.o \
  ../detail/test_bitwise_map_dump_fn.o \
  ../detail/test_bitwise_map_dyn_table.o \
  ../detail/test_bitwise_map_dyn_tuple.o \
  ../detail/test_bitwise_map_dyn_unique_index.o \
  ../detail/test_bitwise_map_get_proc_stat.o \
  ../detail/test_bitwise_map_load_fn.o \
  ../detail/test_bitwise_map_string_reader.o \
  ../detail/test_bitwise_map_version_manager.o \
  test_bitwise_map_test_bitwise_map.o \
  ../detail/test_bitwise_map_bitmap.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_bitwise_map
	mkdir -p ./output/bin
	cp -f --link test_bitwise_map ./output/bin

test_bitmap_index:../detail/test_bitmap_index_array_op.o \
  ../detail/test_bitmap_index_base_cow_hash_map.o \
  ../detail/test_bitmap_index_base_cowbass4.o \
  ../detail/test_bitmap_index_binary_file_reader.o \
  ../detail/test_bitmap_index_dump_fn.o \
  ../detail/test_bitmap_index_dyn_table.o \
  ../detail/test_bitmap_index_dyn_tuple.o \
  ../detail/test_bitmap_index_dyn_unique_index.o \
  ../detail/test_bitmap_index_get_proc_stat.o \
  ../detail/test_bitmap_index_load_fn.o \
  ../detail/test_bitmap_index_string_reader.o \
  ../detail/test_bitmap_index_version_manager.o \
  test_bitmap_index_test_bitmap_index.o \
  ../detail/test_bitmap_index_bitmap.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_bitmap_index[0m']"
	$(CXX) ../detail/test_bitmap_index_array_op.o \
  ../detail/test_bitmap_index_base_cow_hash_map.o \
  ../detail/test_bitmap_index_base_cowbass4.o \
  ../detail/test_bitmap_index_binary_file_reader.o \
  ../detail/test_bitmap_index_dump_fn.o \
  ../detail/test_bitmap_index_dyn_table.o \
  ../detail/test_bitmap_index_dyn_tuple.o \
  ../detail/test_bitmap_index_dyn_unique_index.o \
  ../detail/test_bitmap_index_get_proc_stat.o \
  ../detail/test_bitmap_index_load_fn.o \
  ../detail/test_bitmap_index_string_reader.o \
  ../detail/test_bitmap_index_version_manager.o \
  test_bitmap_index_test_bitmap_index.o \
  ../detail/test_bitmap_index_bitmap.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_bitmap_index
	mkdir -p ./output/bin
	cp -f --link test_bitmap_index ./output/bin

test_hd_map:../detail/test_hd_map_array_op.o \
  ../detail/test_hd_map_base_cow_hash_map.o \
  ../detail/test_hd_map_base_cowbass4.o \
  ../detail/test_hd_map_binary_file_reader.o \
  ../detail/test_hd_map_dump_fn.o \
  ../detail/test_hd_map_dyn_table.o \
  ../detail/test_hd_map_dyn_tuple.o \
  ../detail/test_hd_map_dyn_unique_index.o \
  ../detail/test_hd_map_get_proc_stat.o \
  ../detail/test_hd_map_load_fn.o \
  ../detail/test_hd_map_string_reader.o \
  ../detail/test_hd_map_version_manager.o \
  test_hd_map_test_hd_map.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_hd_map[0m']"
	$(CXX) ../detail/test_hd_map_array_op.o \
  ../detail/test_hd_map_base_cow_hash_map.o \
  ../detail/test_hd_map_base_cowbass4.o \
  ../detail/test_hd_map_binary_file_reader.o \
  ../detail/test_hd_map_dump_fn.o \
  ../detail/test_hd_map_dyn_table.o \
  ../detail/test_hd_map_dyn_tuple.o \
  ../detail/test_hd_map_dyn_unique_index.o \
  ../detail/test_hd_map_get_proc_stat.o \
  ../detail/test_hd_map_load_fn.o \
  ../detail/test_hd_map_string_reader.o \
  ../detail/test_hd_map_version_manager.o \
  test_hd_map_test_hd_map.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_hd_map
	mkdir -p ./output/bin
	cp -f --link test_hd_map ./output/bin

test_dodge_hash_map:../detail/test_dodge_hash_map_array_op.o \
  ../detail/test_dodge_hash_map_base_cow_hash_map.o \
  ../detail/test_dodge_hash_map_base_cowbass4.o \
  ../detail/test_dodge_hash_map_binary_file_reader.o \
  ../detail/test_dodge_hash_map_dump_fn.o \
  ../detail/test_dodge_hash_map_dyn_table.o \
  ../detail/test_dodge_hash_map_dyn_tuple.o \
  ../detail/test_dodge_hash_map_dyn_unique_index.o \
  ../detail/test_dodge_hash_map_get_proc_stat.o \
  ../detail/test_dodge_hash_map_load_fn.o \
  ../detail/test_dodge_hash_map_string_reader.o \
  ../detail/test_dodge_hash_map_version_manager.o \
  test_dodge_hash_map_test_dodge_hash_map.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dodge_hash_map[0m']"
	$(CXX) ../detail/test_dodge_hash_map_array_op.o \
  ../detail/test_dodge_hash_map_base_cow_hash_map.o \
  ../detail/test_dodge_hash_map_base_cowbass4.o \
  ../detail/test_dodge_hash_map_binary_file_reader.o \
  ../detail/test_dodge_hash_map_dump_fn.o \
  ../detail/test_dodge_hash_map_dyn_table.o \
  ../detail/test_dodge_hash_map_dyn_tuple.o \
  ../detail/test_dodge_hash_map_dyn_unique_index.o \
  ../detail/test_dodge_hash_map_get_proc_stat.o \
  ../detail/test_dodge_hash_map_load_fn.o \
  ../detail/test_dodge_hash_map_string_reader.o \
  ../detail/test_dodge_hash_map_version_manager.o \
  test_dodge_hash_map_test_dodge_hash_map.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_dodge_hash_map
	mkdir -p ./output/bin
	cp -f --link test_dodge_hash_map ./output/bin

test_bit_tree:test_bit_tree_test_bit_tree.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_bit_tree[0m']"
	$(CXX) test_bit_tree_test_bit_tree.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_bit_tree
	mkdir -p ./output/bin
	cp -f --link test_bit_tree ./output/bin

test_version_manager:../detail/test_version_manager_array_op.o \
  ../detail/test_version_manager_base_cow_hash_map.o \
  ../detail/test_version_manager_base_cowbass4.o \
  ../detail/test_version_manager_binary_file_reader.o \
  ../detail/test_version_manager_dump_fn.o \
  ../detail/test_version_manager_dyn_table.o \
  ../detail/test_version_manager_dyn_tuple.o \
  ../detail/test_version_manager_dyn_unique_index.o \
  ../detail/test_version_manager_get_proc_stat.o \
  ../detail/test_version_manager_load_fn.o \
  ../detail/test_version_manager_string_reader.o \
  ../detail/test_version_manager_version_manager.o \
  test_version_manager_test_version_manager.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_version_manager[0m']"
	$(CXX) ../detail/test_version_manager_array_op.o \
  ../detail/test_version_manager_base_cow_hash_map.o \
  ../detail/test_version_manager_base_cowbass4.o \
  ../detail/test_version_manager_binary_file_reader.o \
  ../detail/test_version_manager_dump_fn.o \
  ../detail/test_version_manager_dyn_table.o \
  ../detail/test_version_manager_dyn_tuple.o \
  ../detail/test_version_manager_dyn_unique_index.o \
  ../detail/test_version_manager_get_proc_stat.o \
  ../detail/test_version_manager_load_fn.o \
  ../detail/test_version_manager_string_reader.o \
  ../detail/test_version_manager_version_manager.o \
  test_version_manager_test_version_manager.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_version_manager
	mkdir -p ./output/bin
	cp -f --link test_version_manager ./output/bin

test_array_op:test_array_op_test_array_op.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_array_op[0m']"
	$(CXX) test_array_op_test_array_op.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_array_op
	mkdir -p ./output/bin
	cp -f --link test_array_op ./output/bin

test_void_array_op:test_void_array_op_test_void_array_op.o \
  ../detail/test_void_array_op_array_op.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_void_array_op[0m']"
	$(CXX) test_void_array_op_test_void_array_op.o \
  ../detail/test_void_array_op_array_op.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_void_array_op
	mkdir -p ./output/bin
	cp -f --link test_void_array_op ./output/bin

test_shared_ptr:test_shared_ptr_test_shared_ptr.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_shared_ptr[0m']"
	$(CXX) test_shared_ptr_test_shared_ptr.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_shared_ptr
	mkdir -p ./output/bin
	cp -f --link test_shared_ptr ./output/bin

test_hash_map:../detail/test_hash_map_array_op.o \
  ../detail/test_hash_map_base_cow_hash_map.o \
  ../detail/test_hash_map_base_cowbass4.o \
  ../detail/test_hash_map_binary_file_reader.o \
  ../detail/test_hash_map_dump_fn.o \
  ../detail/test_hash_map_dyn_table.o \
  ../detail/test_hash_map_dyn_tuple.o \
  ../detail/test_hash_map_dyn_unique_index.o \
  ../detail/test_hash_map_get_proc_stat.o \
  ../detail/test_hash_map_load_fn.o \
  ../detail/test_hash_map_string_reader.o \
  ../detail/test_hash_map_version_manager.o \
  test_hash_map_test_hash_map.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_hash_map[0m']"
	$(CXX) ../detail/test_hash_map_array_op.o \
  ../detail/test_hash_map_base_cow_hash_map.o \
  ../detail/test_hash_map_base_cowbass4.o \
  ../detail/test_hash_map_binary_file_reader.o \
  ../detail/test_hash_map_dump_fn.o \
  ../detail/test_hash_map_dyn_table.o \
  ../detail/test_hash_map_dyn_tuple.o \
  ../detail/test_hash_map_dyn_unique_index.o \
  ../detail/test_hash_map_get_proc_stat.o \
  ../detail/test_hash_map_load_fn.o \
  ../detail/test_hash_map_string_reader.o \
  ../detail/test_hash_map_version_manager.o \
  test_hash_map_test_hash_map.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_hash_map
	mkdir -p ./output/bin
	cp -f --link test_hash_map ./output/bin

test_hash_cluster:../detail/test_hash_cluster_array_op.o \
  ../detail/test_hash_cluster_base_cow_hash_map.o \
  ../detail/test_hash_cluster_base_cowbass4.o \
  ../detail/test_hash_cluster_binary_file_reader.o \
  ../detail/test_hash_cluster_dump_fn.o \
  ../detail/test_hash_cluster_dyn_table.o \
  ../detail/test_hash_cluster_dyn_tuple.o \
  ../detail/test_hash_cluster_dyn_unique_index.o \
  ../detail/test_hash_cluster_get_proc_stat.o \
  ../detail/test_hash_cluster_load_fn.o \
  ../detail/test_hash_cluster_string_reader.o \
  ../detail/test_hash_cluster_version_manager.o \
  test_hash_cluster_test_hash_cluster.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_hash_cluster[0m']"
	$(CXX) ../detail/test_hash_cluster_array_op.o \
  ../detail/test_hash_cluster_base_cow_hash_map.o \
  ../detail/test_hash_cluster_base_cowbass4.o \
  ../detail/test_hash_cluster_binary_file_reader.o \
  ../detail/test_hash_cluster_dump_fn.o \
  ../detail/test_hash_cluster_dyn_table.o \
  ../detail/test_hash_cluster_dyn_tuple.o \
  ../detail/test_hash_cluster_dyn_unique_index.o \
  ../detail/test_hash_cluster_get_proc_stat.o \
  ../detail/test_hash_cluster_load_fn.o \
  ../detail/test_hash_cluster_string_reader.o \
  ../detail/test_hash_cluster_version_manager.o \
  test_hash_cluster_test_hash_cluster.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_hash_cluster
	mkdir -p ./output/bin
	cp -f --link test_hash_cluster ./output/bin

test_cow_table:../detail/test_cow_table_array_op.o \
  ../detail/test_cow_table_base_cow_hash_map.o \
  ../detail/test_cow_table_base_cowbass4.o \
  ../detail/test_cow_table_binary_file_reader.o \
  ../detail/test_cow_table_dump_fn.o \
  ../detail/test_cow_table_dyn_table.o \
  ../detail/test_cow_table_dyn_tuple.o \
  ../detail/test_cow_table_dyn_unique_index.o \
  ../detail/test_cow_table_get_proc_stat.o \
  ../detail/test_cow_table_load_fn.o \
  ../detail/test_cow_table_string_reader.o \
  ../detail/test_cow_table_version_manager.o \
  test_cow_table_test_cow_table.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_cow_table[0m']"
	$(CXX) ../detail/test_cow_table_array_op.o \
  ../detail/test_cow_table_base_cow_hash_map.o \
  ../detail/test_cow_table_base_cowbass4.o \
  ../detail/test_cow_table_binary_file_reader.o \
  ../detail/test_cow_table_dump_fn.o \
  ../detail/test_cow_table_dyn_table.o \
  ../detail/test_cow_table_dyn_tuple.o \
  ../detail/test_cow_table_dyn_unique_index.o \
  ../detail/test_cow_table_get_proc_stat.o \
  ../detail/test_cow_table_load_fn.o \
  ../detail/test_cow_table_string_reader.o \
  ../detail/test_cow_table_version_manager.o \
  test_cow_table_test_cow_table.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_cow_table
	mkdir -p ./output/bin
	cp -f --link test_cow_table ./output/bin

test_group_view:../detail/test_group_view_array_op.o \
  ../detail/test_group_view_base_cow_hash_map.o \
  ../detail/test_group_view_base_cowbass4.o \
  ../detail/test_group_view_binary_file_reader.o \
  ../detail/test_group_view_dump_fn.o \
  ../detail/test_group_view_dyn_table.o \
  ../detail/test_group_view_dyn_tuple.o \
  ../detail/test_group_view_dyn_unique_index.o \
  ../detail/test_group_view_get_proc_stat.o \
  ../detail/test_group_view_load_fn.o \
  ../detail/test_group_view_string_reader.o \
  ../detail/test_group_view_version_manager.o \
  test_group_view_test_group_view.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_group_view[0m']"
	$(CXX) ../detail/test_group_view_array_op.o \
  ../detail/test_group_view_base_cow_hash_map.o \
  ../detail/test_group_view_base_cowbass4.o \
  ../detail/test_group_view_binary_file_reader.o \
  ../detail/test_group_view_dump_fn.o \
  ../detail/test_group_view_dyn_table.o \
  ../detail/test_group_view_dyn_tuple.o \
  ../detail/test_group_view_dyn_unique_index.o \
  ../detail/test_group_view_get_proc_stat.o \
  ../detail/test_group_view_load_fn.o \
  ../detail/test_group_view_string_reader.o \
  ../detail/test_group_view_version_manager.o \
  test_group_view_test_group_view.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_group_view
	mkdir -p ./output/bin
	cp -f --link test_group_view ./output/bin

test_fun:test_fun_test_fun.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_fun[0m']"
	$(CXX) test_fun_test_fun.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_fun
	mkdir -p ./output/bin
	cp -f --link test_fun ./output/bin

test_selector:../detail/test_selector_array_op.o \
  ../detail/test_selector_base_cow_hash_map.o \
  ../detail/test_selector_base_cowbass4.o \
  ../detail/test_selector_binary_file_reader.o \
  ../detail/test_selector_dump_fn.o \
  ../detail/test_selector_dyn_table.o \
  ../detail/test_selector_dyn_tuple.o \
  ../detail/test_selector_dyn_unique_index.o \
  ../detail/test_selector_get_proc_stat.o \
  ../detail/test_selector_load_fn.o \
  ../detail/test_selector_string_reader.o \
  ../detail/test_selector_version_manager.o \
  test_selector_test_selector.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_selector[0m']"
	$(CXX) ../detail/test_selector_array_op.o \
  ../detail/test_selector_base_cow_hash_map.o \
  ../detail/test_selector_base_cowbass4.o \
  ../detail/test_selector_binary_file_reader.o \
  ../detail/test_selector_dump_fn.o \
  ../detail/test_selector_dyn_table.o \
  ../detail/test_selector_dyn_tuple.o \
  ../detail/test_selector_dyn_unique_index.o \
  ../detail/test_selector_get_proc_stat.o \
  ../detail/test_selector_load_fn.o \
  ../detail/test_selector_string_reader.o \
  ../detail/test_selector_version_manager.o \
  test_selector_test_selector.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_selector
	mkdir -p ./output/bin
	cp -f --link test_selector ./output/bin

test_named_tuple:../detail/test_named_tuple_array_op.o \
  ../detail/test_named_tuple_base_cow_hash_map.o \
  ../detail/test_named_tuple_base_cowbass4.o \
  ../detail/test_named_tuple_binary_file_reader.o \
  ../detail/test_named_tuple_dump_fn.o \
  ../detail/test_named_tuple_dyn_table.o \
  ../detail/test_named_tuple_dyn_tuple.o \
  ../detail/test_named_tuple_dyn_unique_index.o \
  ../detail/test_named_tuple_get_proc_stat.o \
  ../detail/test_named_tuple_load_fn.o \
  ../detail/test_named_tuple_string_reader.o \
  ../detail/test_named_tuple_version_manager.o \
  test_named_tuple_test_named_tuple.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_named_tuple[0m']"
	$(CXX) ../detail/test_named_tuple_array_op.o \
  ../detail/test_named_tuple_base_cow_hash_map.o \
  ../detail/test_named_tuple_base_cowbass4.o \
  ../detail/test_named_tuple_binary_file_reader.o \
  ../detail/test_named_tuple_dump_fn.o \
  ../detail/test_named_tuple_dyn_table.o \
  ../detail/test_named_tuple_dyn_tuple.o \
  ../detail/test_named_tuple_dyn_unique_index.o \
  ../detail/test_named_tuple_get_proc_stat.o \
  ../detail/test_named_tuple_load_fn.o \
  ../detail/test_named_tuple_string_reader.o \
  ../detail/test_named_tuple_version_manager.o \
  test_named_tuple_test_named_tuple.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_named_tuple
	mkdir -p ./output/bin
	cp -f --link test_named_tuple ./output/bin

test_perf:test_perf_test_perf.o \
  ../detail/test_perf_get_proc_stat.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_perf[0m']"
	$(CXX) test_perf_test_perf.o \
  ../detail/test_perf_get_proc_stat.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_perf
	mkdir -p ./output/bin
	cp -f --link test_perf ./output/bin

test_pool:../detail/test_pool_array_op.o \
  ../detail/test_pool_base_cow_hash_map.o \
  ../detail/test_pool_base_cowbass4.o \
  ../detail/test_pool_binary_file_reader.o \
  ../detail/test_pool_dump_fn.o \
  ../detail/test_pool_dyn_table.o \
  ../detail/test_pool_dyn_tuple.o \
  ../detail/test_pool_dyn_unique_index.o \
  ../detail/test_pool_get_proc_stat.o \
  ../detail/test_pool_load_fn.o \
  ../detail/test_pool_string_reader.o \
  ../detail/test_pool_version_manager.o \
  test_pool_test_pool.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_pool[0m']"
	$(CXX) ../detail/test_pool_array_op.o \
  ../detail/test_pool_base_cow_hash_map.o \
  ../detail/test_pool_base_cowbass4.o \
  ../detail/test_pool_binary_file_reader.o \
  ../detail/test_pool_dump_fn.o \
  ../detail/test_pool_dyn_table.o \
  ../detail/test_pool_dyn_tuple.o \
  ../detail/test_pool_dyn_unique_index.o \
  ../detail/test_pool_get_proc_stat.o \
  ../detail/test_pool_load_fn.o \
  ../detail/test_pool_string_reader.o \
  ../detail/test_pool_version_manager.o \
  test_pool_test_pool.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_pool
	mkdir -p ./output/bin
	cp -f --link test_pool ./output/bin

test_string_writer:../detail/test_string_writer_array_op.o \
  ../detail/test_string_writer_base_cow_hash_map.o \
  ../detail/test_string_writer_base_cowbass4.o \
  ../detail/test_string_writer_binary_file_reader.o \
  ../detail/test_string_writer_dump_fn.o \
  ../detail/test_string_writer_dyn_table.o \
  ../detail/test_string_writer_dyn_tuple.o \
  ../detail/test_string_writer_dyn_unique_index.o \
  ../detail/test_string_writer_get_proc_stat.o \
  ../detail/test_string_writer_load_fn.o \
  ../detail/test_string_writer_string_reader.o \
  ../detail/test_string_writer_version_manager.o \
  test_string_writer_test_string_writer.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_string_writer[0m']"
	$(CXX) ../detail/test_string_writer_array_op.o \
  ../detail/test_string_writer_base_cow_hash_map.o \
  ../detail/test_string_writer_base_cowbass4.o \
  ../detail/test_string_writer_binary_file_reader.o \
  ../detail/test_string_writer_dump_fn.o \
  ../detail/test_string_writer_dyn_table.o \
  ../detail/test_string_writer_dyn_tuple.o \
  ../detail/test_string_writer_dyn_unique_index.o \
  ../detail/test_string_writer_get_proc_stat.o \
  ../detail/test_string_writer_load_fn.o \
  ../detail/test_string_writer_string_reader.o \
  ../detail/test_string_writer_version_manager.o \
  test_string_writer_test_string_writer.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_string_writer
	mkdir -p ./output/bin
	cp -f --link test_string_writer ./output/bin

test_string_reader:test_string_reader_test_string_reader.o \
  ../detail/test_string_reader_string_reader.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_string_reader[0m']"
	$(CXX) test_string_reader_test_string_reader.o \
  ../detail/test_string_reader_string_reader.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_string_reader
	mkdir -p ./output/bin
	cp -f --link test_string_reader ./output/bin

test_graph:../detail/test_graph_array_op.o \
  ../detail/test_graph_base_cow_hash_map.o \
  ../detail/test_graph_base_cowbass4.o \
  ../detail/test_graph_binary_file_reader.o \
  ../detail/test_graph_dump_fn.o \
  ../detail/test_graph_dyn_table.o \
  ../detail/test_graph_dyn_tuple.o \
  ../detail/test_graph_dyn_unique_index.o \
  ../detail/test_graph_get_proc_stat.o \
  ../detail/test_graph_load_fn.o \
  ../detail/test_graph_string_reader.o \
  ../detail/test_graph_version_manager.o \
  test_graph_test_graph.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_graph[0m']"
	$(CXX) ../detail/test_graph_array_op.o \
  ../detail/test_graph_base_cow_hash_map.o \
  ../detail/test_graph_base_cowbass4.o \
  ../detail/test_graph_binary_file_reader.o \
  ../detail/test_graph_dump_fn.o \
  ../detail/test_graph_dyn_table.o \
  ../detail/test_graph_dyn_tuple.o \
  ../detail/test_graph_dyn_unique_index.o \
  ../detail/test_graph_get_proc_stat.o \
  ../detail/test_graph_load_fn.o \
  ../detail/test_graph_string_reader.o \
  ../detail/test_graph_version_manager.o \
  test_graph_test_graph.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_graph
	mkdir -p ./output/bin
	cp -f --link test_graph ./output/bin

test_fixed_deque:../detail/test_fixed_deque_array_op.o \
  ../detail/test_fixed_deque_base_cow_hash_map.o \
  ../detail/test_fixed_deque_base_cowbass4.o \
  ../detail/test_fixed_deque_binary_file_reader.o \
  ../detail/test_fixed_deque_dump_fn.o \
  ../detail/test_fixed_deque_dyn_table.o \
  ../detail/test_fixed_deque_dyn_tuple.o \
  ../detail/test_fixed_deque_dyn_unique_index.o \
  ../detail/test_fixed_deque_get_proc_stat.o \
  ../detail/test_fixed_deque_load_fn.o \
  ../detail/test_fixed_deque_string_reader.o \
  ../detail/test_fixed_deque_version_manager.o \
  test_fixed_deque_test_fixed_deque.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_fixed_deque[0m']"
	$(CXX) ../detail/test_fixed_deque_array_op.o \
  ../detail/test_fixed_deque_base_cow_hash_map.o \
  ../detail/test_fixed_deque_base_cowbass4.o \
  ../detail/test_fixed_deque_binary_file_reader.o \
  ../detail/test_fixed_deque_dump_fn.o \
  ../detail/test_fixed_deque_dyn_table.o \
  ../detail/test_fixed_deque_dyn_tuple.o \
  ../detail/test_fixed_deque_dyn_unique_index.o \
  ../detail/test_fixed_deque_get_proc_stat.o \
  ../detail/test_fixed_deque_load_fn.o \
  ../detail/test_fixed_deque_string_reader.o \
  ../detail/test_fixed_deque_version_manager.o \
  test_fixed_deque_test_fixed_deque.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_fixed_deque
	mkdir -p ./output/bin
	cp -f --link test_fixed_deque ./output/bin

test_template_folding:test_template_folding_test_template_folding.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_template_folding[0m']"
	$(CXX) test_template_folding_test_template_folding.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_template_folding
	mkdir -p ./output/bin
	cp -f --link test_template_folding ./output/bin

test_stack:../detail/test_stack_array_op.o \
  ../detail/test_stack_base_cow_hash_map.o \
  ../detail/test_stack_base_cowbass4.o \
  ../detail/test_stack_binary_file_reader.o \
  ../detail/test_stack_dump_fn.o \
  ../detail/test_stack_dyn_table.o \
  ../detail/test_stack_dyn_tuple.o \
  ../detail/test_stack_dyn_unique_index.o \
  ../detail/test_stack_get_proc_stat.o \
  ../detail/test_stack_load_fn.o \
  ../detail/test_stack_string_reader.o \
  ../detail/test_stack_version_manager.o \
  test_stack_test_stack.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_stack[0m']"
	$(CXX) ../detail/test_stack_array_op.o \
  ../detail/test_stack_base_cow_hash_map.o \
  ../detail/test_stack_base_cowbass4.o \
  ../detail/test_stack_binary_file_reader.o \
  ../detail/test_stack_dump_fn.o \
  ../detail/test_stack_dyn_table.o \
  ../detail/test_stack_dyn_tuple.o \
  ../detail/test_stack_dyn_unique_index.o \
  ../detail/test_stack_get_proc_stat.o \
  ../detail/test_stack_load_fn.o \
  ../detail/test_stack_string_reader.o \
  ../detail/test_stack_version_manager.o \
  test_stack_test_stack.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_stack
	mkdir -p ./output/bin
	cp -f --link test_stack ./output/bin

test_dyn_tuple:test_dyn_tuple_test_dyn_tuple.o \
  ../detail/test_dyn_tuple_dyn_tuple.o \
  ../detail/test_dyn_tuple_string_reader.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dyn_tuple[0m']"
	$(CXX) test_dyn_tuple_test_dyn_tuple.o \
  ../detail/test_dyn_tuple_dyn_tuple.o \
  ../detail/test_dyn_tuple_string_reader.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_dyn_tuple
	mkdir -p ./output/bin
	cp -f --link test_dyn_tuple ./output/bin

test_memory_ref:test_memory_ref_test_memory_ref.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_memory_ref[0m']"
	$(CXX) test_memory_ref_test_memory_ref.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_memory_ref
	mkdir -p ./output/bin
	cp -f --link test_memory_ref ./output/bin

test_offset_list:test_offset_list_test_offset_list.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_offset_list[0m']"
	$(CXX) test_offset_list_test_offset_list.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_offset_list
	mkdir -p ./output/bin
	cp -f --link test_offset_list ./output/bin

test_adt:test_adt_test_adt.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_adt[0m']"
	$(CXX) test_adt_test_adt.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_adt
	mkdir -p ./output/bin
	cp -f --link test_adt ./output/bin

test_override_and_specialize:test_override_and_specialize_test_override_and_specialize.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_override_and_specialize[0m']"
	$(CXX) test_override_and_specialize_test_override_and_specialize.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_override_and_specialize
	mkdir -p ./output/bin
	cp -f --link test_override_and_specialize ./output/bin

test_dyn_unique_index:test_dyn_unique_index_test_dyn_unique_index.o \
  ../detail/test_dyn_unique_index_dyn_tuple.o \
  ../detail/test_dyn_unique_index_dyn_unique_index.o \
  ../detail/test_dyn_unique_index_base_cow_hash_map.o \
  ../detail/test_dyn_unique_index_string_reader.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dyn_unique_index[0m']"
	$(CXX) test_dyn_unique_index_test_dyn_unique_index.o \
  ../detail/test_dyn_unique_index_dyn_tuple.o \
  ../detail/test_dyn_unique_index_dyn_unique_index.o \
  ../detail/test_dyn_unique_index_base_cow_hash_map.o \
  ../detail/test_dyn_unique_index_string_reader.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_dyn_unique_index
	mkdir -p ./output/bin
	cp -f --link test_dyn_unique_index ./output/bin

test_dyn_table:test_dyn_table_test_dyn_table.o \
  ../detail/test_dyn_table_dyn_table.o \
  ../detail/test_dyn_table_dyn_tuple.o \
  ../detail/test_dyn_table_dyn_unique_index.o \
  ../detail/test_dyn_table_base_cow_hash_map.o \
  ../detail/test_dyn_table_string_reader.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dyn_table[0m']"
	$(CXX) test_dyn_table_test_dyn_table.o \
  ../detail/test_dyn_table_dyn_table.o \
  ../detail/test_dyn_table_dyn_tuple.o \
  ../detail/test_dyn_table_dyn_unique_index.o \
  ../detail/test_dyn_table_base_cow_hash_map.o \
  ../detail/test_dyn_table_string_reader.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_dyn_table
	mkdir -p ./output/bin
	cp -f --link test_dyn_table ./output/bin

test_load_dump_fn:../detail/test_load_dump_fn_array_op.o \
  ../detail/test_load_dump_fn_base_cow_hash_map.o \
  ../detail/test_load_dump_fn_base_cowbass4.o \
  ../detail/test_load_dump_fn_binary_file_reader.o \
  ../detail/test_load_dump_fn_dump_fn.o \
  ../detail/test_load_dump_fn_dyn_table.o \
  ../detail/test_load_dump_fn_dyn_tuple.o \
  ../detail/test_load_dump_fn_dyn_unique_index.o \
  ../detail/test_load_dump_fn_get_proc_stat.o \
  ../detail/test_load_dump_fn_load_fn.o \
  ../detail/test_load_dump_fn_string_reader.o \
  ../detail/test_load_dump_fn_version_manager.o \
  test_load_dump_fn_test_load_dump_fn.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_load_dump_fn[0m']"
	$(CXX) ../detail/test_load_dump_fn_array_op.o \
  ../detail/test_load_dump_fn_base_cow_hash_map.o \
  ../detail/test_load_dump_fn_base_cowbass4.o \
  ../detail/test_load_dump_fn_binary_file_reader.o \
  ../detail/test_load_dump_fn_dump_fn.o \
  ../detail/test_load_dump_fn_dyn_table.o \
  ../detail/test_load_dump_fn_dyn_tuple.o \
  ../detail/test_load_dump_fn_dyn_unique_index.o \
  ../detail/test_load_dump_fn_get_proc_stat.o \
  ../detail/test_load_dump_fn_load_fn.o \
  ../detail/test_load_dump_fn_string_reader.o \
  ../detail/test_load_dump_fn_version_manager.o \
  test_load_dump_fn_test_load_dump_fn.o -Xlinker "-("  ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a -lpthread -Xlinker "-)" -o test_load_dump_fn
	mkdir -p ./output/bin
	cp -f --link test_load_dump_fn ./output/bin

../detail/test_cowbass4_array_op.o:../detail/array_op.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_array_op.o ../detail/array_op.cpp

../detail/test_cowbass4_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_cowbass4_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_cowbass4_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_cowbass4_dump_fn.o:../detail/dump_fn.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_dump_fn.o ../detail/dump_fn.cpp

../detail/test_cowbass4_dyn_table.o:../detail/dyn_table.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_dyn_table.o ../detail/dyn_table.cpp

../detail/test_cowbass4_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_cowbass4_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_cowbass4_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_cowbass4_load_fn.o:../detail/load_fn.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_load_fn.o ../detail/load_fn.cpp

../detail/test_cowbass4_string_reader.o:../detail/string_reader.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_string_reader.o ../detail/string_reader.cpp

../detail/test_cowbass4_version_manager.o:../detail/version_manager.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cowbass4_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cowbass4_version_manager.o ../detail/version_manager.cpp

test_cowbass4_test_cowbass4.o:test_cowbass4.cpp \
  ../cowbass4.hpp \
  ../array_op.hpp \
  ../array_op.h \
  ../base_cowbass4.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_cowbass4_test_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_cowbass4_test_cowbass4.o test_cowbass4.cpp

../detail/test_cow_hash_map_array_op.o:../detail/array_op.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_array_op.o ../detail/array_op.cpp

../detail/test_cow_hash_map_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_cow_hash_map_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_cow_hash_map_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_cow_hash_map_dump_fn.o:../detail/dump_fn.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_dump_fn.o ../detail/dump_fn.cpp

../detail/test_cow_hash_map_dyn_table.o:../detail/dyn_table.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_dyn_table.o ../detail/dyn_table.cpp

../detail/test_cow_hash_map_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_cow_hash_map_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_cow_hash_map_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_cow_hash_map_load_fn.o:../detail/load_fn.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_load_fn.o ../detail/load_fn.cpp

../detail/test_cow_hash_map_string_reader.o:../detail/string_reader.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_string_reader.o ../detail/string_reader.cpp

../detail/test_cow_hash_map_version_manager.o:../detail/version_manager.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_hash_map_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_hash_map_version_manager.o ../detail/version_manager.cpp

test_cow_hash_map_test_cow_hash_map.o:test_cow_hash_map.cpp \
  ../base_cow_hash_map.h \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_cow_hash_map_test_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_cow_hash_map_test_cow_hash_map.o test_cow_hash_map.cpp

../detail/test_bitwise_map_array_op.o:../detail/array_op.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_array_op.o ../detail/array_op.cpp

../detail/test_bitwise_map_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_bitwise_map_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_bitwise_map_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_bitwise_map_dump_fn.o:../detail/dump_fn.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_dump_fn.o ../detail/dump_fn.cpp

../detail/test_bitwise_map_dyn_table.o:../detail/dyn_table.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_dyn_table.o ../detail/dyn_table.cpp

../detail/test_bitwise_map_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_bitwise_map_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_bitwise_map_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_bitwise_map_load_fn.o:../detail/load_fn.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_load_fn.o ../detail/load_fn.cpp

../detail/test_bitwise_map_string_reader.o:../detail/string_reader.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_string_reader.o ../detail/string_reader.cpp

../detail/test_bitwise_map_version_manager.o:../detail/version_manager.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitwise_map_version_manager.o ../detail/version_manager.cpp

test_bitwise_map_test_bitwise_map.o:test_bitwise_map.cpp \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_bitwise_map_test_bitwise_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_bitwise_map_test_bitwise_map.o test_bitwise_map.cpp

../detail/test_bitwise_map_bitmap.o:../detail/bitmap.c \
  ../bitwise_map.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitwise_map_bitmap.o[0m']"
	$(CC) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CFLAGS)  -o ../detail/test_bitwise_map_bitmap.o ../detail/bitmap.c

../detail/test_bitmap_index_array_op.o:../detail/array_op.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_array_op.o ../detail/array_op.cpp

../detail/test_bitmap_index_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_bitmap_index_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_bitmap_index_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_bitmap_index_dump_fn.o:../detail/dump_fn.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_dump_fn.o ../detail/dump_fn.cpp

../detail/test_bitmap_index_dyn_table.o:../detail/dyn_table.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_dyn_table.o ../detail/dyn_table.cpp

../detail/test_bitmap_index_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_bitmap_index_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_bitmap_index_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_bitmap_index_load_fn.o:../detail/load_fn.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_load_fn.o ../detail/load_fn.cpp

../detail/test_bitmap_index_string_reader.o:../detail/string_reader.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_string_reader.o ../detail/string_reader.cpp

../detail/test_bitmap_index_version_manager.o:../detail/version_manager.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_bitmap_index_version_manager.o ../detail/version_manager.cpp

test_bitmap_index_test_bitmap_index.o:test_bitmap_index.cpp \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_bitmap_index_test_bitmap_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_bitmap_index_test_bitmap_index.o test_bitmap_index.cpp

../detail/test_bitmap_index_bitmap.o:../detail/bitmap.c \
  ../bitmap_index.hpp \
  ../bitmap.h \
  ../detail/bitop.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_bitmap_index_bitmap.o[0m']"
	$(CC) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CFLAGS)  -o ../detail/test_bitmap_index_bitmap.o ../detail/bitmap.c

../detail/test_hd_map_array_op.o:../detail/array_op.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_array_op.o ../detail/array_op.cpp

../detail/test_hd_map_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_hd_map_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_hd_map_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_hd_map_dump_fn.o:../detail/dump_fn.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_dump_fn.o ../detail/dump_fn.cpp

../detail/test_hd_map_dyn_table.o:../detail/dyn_table.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_dyn_table.o ../detail/dyn_table.cpp

../detail/test_hd_map_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_hd_map_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_hd_map_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_hd_map_load_fn.o:../detail/load_fn.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_load_fn.o ../detail/load_fn.cpp

../detail/test_hd_map_string_reader.o:../detail/string_reader.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_string_reader.o ../detail/string_reader.cpp

../detail/test_hd_map_version_manager.o:../detail/version_manager.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hd_map_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hd_map_version_manager.o ../detail/version_manager.cpp

test_hd_map_test_hd_map.o:test_hd_map.cpp \
  ../hd_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_hd_map_test_hd_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_hd_map_test_hd_map.o test_hd_map.cpp

../detail/test_dodge_hash_map_array_op.o:../detail/array_op.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_array_op.o ../detail/array_op.cpp

../detail/test_dodge_hash_map_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_dodge_hash_map_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_dodge_hash_map_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_dodge_hash_map_dump_fn.o:../detail/dump_fn.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_dump_fn.o ../detail/dump_fn.cpp

../detail/test_dodge_hash_map_dyn_table.o:../detail/dyn_table.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_dyn_table.o ../detail/dyn_table.cpp

../detail/test_dodge_hash_map_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_dodge_hash_map_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_dodge_hash_map_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_dodge_hash_map_load_fn.o:../detail/load_fn.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_load_fn.o ../detail/load_fn.cpp

../detail/test_dodge_hash_map_string_reader.o:../detail/string_reader.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_string_reader.o ../detail/string_reader.cpp

../detail/test_dodge_hash_map_version_manager.o:../detail/version_manager.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dodge_hash_map_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dodge_hash_map_version_manager.o ../detail/version_manager.cpp

test_dodge_hash_map_test_dodge_hash_map.o:test_dodge_hash_map.cpp \
  ../dodge_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dodge_hash_map_test_dodge_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_dodge_hash_map_test_dodge_hash_map.o test_dodge_hash_map.cpp

test_bit_tree_test_bit_tree.o:test_bit_tree.cpp \
  ../bit_tree.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_bit_tree_test_bit_tree.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_bit_tree_test_bit_tree.o test_bit_tree.cpp

../detail/test_version_manager_array_op.o:../detail/array_op.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_array_op.o ../detail/array_op.cpp

../detail/test_version_manager_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_version_manager_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_version_manager_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_version_manager_dump_fn.o:../detail/dump_fn.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_dump_fn.o ../detail/dump_fn.cpp

../detail/test_version_manager_dyn_table.o:../detail/dyn_table.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_dyn_table.o ../detail/dyn_table.cpp

../detail/test_version_manager_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_version_manager_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_version_manager_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_version_manager_load_fn.o:../detail/load_fn.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_load_fn.o ../detail/load_fn.cpp

../detail/test_version_manager_string_reader.o:../detail/string_reader.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_string_reader.o ../detail/string_reader.cpp

../detail/test_version_manager_version_manager.o:../detail/version_manager.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_version_manager_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_version_manager_version_manager.o ../detail/version_manager.cpp

test_version_manager_test_version_manager.o:test_version_manager.cpp \
  ../version_manager.hpp \
  ../cow_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_version_manager_test_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_version_manager_test_version_manager.o test_version_manager.cpp

test_array_op_test_array_op.o:test_array_op.cpp \
  ../array_op.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_array_op_test_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_array_op_test_array_op.o test_array_op.cpp

test_void_array_op_test_void_array_op.o:test_void_array_op.cpp \
  ../array_op.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_void_array_op_test_void_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_void_array_op_test_void_array_op.o test_void_array_op.cpp

../detail/test_void_array_op_array_op.o:../detail/array_op.cpp \
  ../array_op.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_void_array_op_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_void_array_op_array_op.o ../detail/array_op.cpp

test_shared_ptr_test_shared_ptr.o:test_shared_ptr.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_shared_ptr_test_shared_ptr.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_shared_ptr_test_shared_ptr.o test_shared_ptr.cpp

../detail/test_hash_map_array_op.o:../detail/array_op.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_array_op.o ../detail/array_op.cpp

../detail/test_hash_map_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_hash_map_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_hash_map_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_hash_map_dump_fn.o:../detail/dump_fn.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_dump_fn.o ../detail/dump_fn.cpp

../detail/test_hash_map_dyn_table.o:../detail/dyn_table.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_dyn_table.o ../detail/dyn_table.cpp

../detail/test_hash_map_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_hash_map_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_hash_map_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_hash_map_load_fn.o:../detail/load_fn.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_load_fn.o ../detail/load_fn.cpp

../detail/test_hash_map_string_reader.o:../detail/string_reader.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_string_reader.o ../detail/string_reader.cpp

../detail/test_hash_map_version_manager.o:../detail/version_manager.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_map_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_map_version_manager.o ../detail/version_manager.cpp

test_hash_map_test_hash_map.o:test_hash_map.cpp \
  ../coalesced_hash_map.hpp \
  ../cow_hash_map.hpp \
  ../dodge_hash_map.hpp \
  ../linear_hash_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_hash_map_test_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_hash_map_test_hash_map.o test_hash_map.cpp

../detail/test_hash_cluster_array_op.o:../detail/array_op.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_array_op.o ../detail/array_op.cpp

../detail/test_hash_cluster_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_hash_cluster_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_hash_cluster_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_hash_cluster_dump_fn.o:../detail/dump_fn.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_dump_fn.o ../detail/dump_fn.cpp

../detail/test_hash_cluster_dyn_table.o:../detail/dyn_table.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_dyn_table.o ../detail/dyn_table.cpp

../detail/test_hash_cluster_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_hash_cluster_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_hash_cluster_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_hash_cluster_load_fn.o:../detail/load_fn.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_load_fn.o ../detail/load_fn.cpp

../detail/test_hash_cluster_string_reader.o:../detail/string_reader.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_string_reader.o ../detail/string_reader.cpp

../detail/test_hash_cluster_version_manager.o:../detail/version_manager.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_hash_cluster_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_hash_cluster_version_manager.o ../detail/version_manager.cpp

test_hash_cluster_test_hash_cluster.o:test_hash_cluster.cpp \
  ../cow_hash_cluster_map.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_hash_cluster_test_hash_cluster.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_hash_cluster_test_hash_cluster.o test_hash_cluster.cpp

../detail/test_cow_table_array_op.o:../detail/array_op.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_array_op.o ../detail/array_op.cpp

../detail/test_cow_table_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_cow_table_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_cow_table_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_cow_table_dump_fn.o:../detail/dump_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_dump_fn.o ../detail/dump_fn.cpp

../detail/test_cow_table_dyn_table.o:../detail/dyn_table.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_dyn_table.o ../detail/dyn_table.cpp

../detail/test_cow_table_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_cow_table_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_cow_table_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_cow_table_load_fn.o:../detail/load_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_load_fn.o ../detail/load_fn.cpp

../detail/test_cow_table_string_reader.o:../detail/string_reader.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_string_reader.o ../detail/string_reader.cpp

../detail/test_cow_table_version_manager.o:../detail/version_manager.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_cow_table_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_cow_table_version_manager.o ../detail/version_manager.cpp

test_cow_table_test_cow_table.o:test_cow_table.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_cow_table_test_cow_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_cow_table_test_cow_table.o test_cow_table.cpp

../detail/test_group_view_array_op.o:../detail/array_op.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_array_op.o ../detail/array_op.cpp

../detail/test_group_view_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_group_view_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_group_view_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_group_view_dump_fn.o:../detail/dump_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_dump_fn.o ../detail/dump_fn.cpp

../detail/test_group_view_dyn_table.o:../detail/dyn_table.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_dyn_table.o ../detail/dyn_table.cpp

../detail/test_group_view_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_group_view_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_group_view_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_group_view_load_fn.o:../detail/load_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_load_fn.o ../detail/load_fn.cpp

../detail/test_group_view_string_reader.o:../detail/string_reader.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_string_reader.o ../detail/string_reader.cpp

../detail/test_group_view_version_manager.o:../detail/version_manager.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_group_view_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_group_view_version_manager.o ../detail/version_manager.cpp

test_group_view_test_group_view.o:test_group_view.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../group_view.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_group_view_test_group_view.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_group_view_test_group_view.o test_group_view.cpp

test_fun_test_fun.o:test_fun.cpp \
  ../c_common.hpp \
  ../c_math.hpp \
  ../c_list.hpp \
  ../detail/c_list_inl.hpp \
  ../c_map.hpp \
  ../common_function_objects.hpp \
  ../st_hash.h \
  ../compare.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_fun_test_fun.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_fun_test_fun.o test_fun.cpp

../detail/test_selector_array_op.o:../detail/array_op.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_array_op.o ../detail/array_op.cpp

../detail/test_selector_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_selector_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_selector_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_selector_dump_fn.o:../detail/dump_fn.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_dump_fn.o ../detail/dump_fn.cpp

../detail/test_selector_dyn_table.o:../detail/dyn_table.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_dyn_table.o ../detail/dyn_table.cpp

../detail/test_selector_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_selector_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_selector_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_selector_load_fn.o:../detail/load_fn.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_load_fn.o ../detail/load_fn.cpp

../detail/test_selector_string_reader.o:../detail/string_reader.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_string_reader.o ../detail/string_reader.cpp

../detail/test_selector_version_manager.o:../detail/version_manager.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_selector_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_selector_version_manager.o ../detail/version_manager.cpp

test_selector_test_selector.o:test_selector.cpp \
  ../c_selector.hpp \
  ../detail/c_selector_inl.hpp \
  ../c_connector.hpp \
  ../detail/c_connector_inl.hpp \
  ../c_predicate.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_selector_test_selector.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_selector_test_selector.o test_selector.cpp

../detail/test_named_tuple_array_op.o:../detail/array_op.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_array_op.o ../detail/array_op.cpp

../detail/test_named_tuple_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_named_tuple_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_named_tuple_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_named_tuple_dump_fn.o:../detail/dump_fn.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_dump_fn.o ../detail/dump_fn.cpp

../detail/test_named_tuple_dyn_table.o:../detail/dyn_table.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_dyn_table.o ../detail/dyn_table.cpp

../detail/test_named_tuple_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_named_tuple_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_named_tuple_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_named_tuple_load_fn.o:../detail/load_fn.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_load_fn.o ../detail/load_fn.cpp

../detail/test_named_tuple_string_reader.o:../detail/string_reader.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_string_reader.o ../detail/string_reader.cpp

../detail/test_named_tuple_version_manager.o:../detail/version_manager.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_named_tuple_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_named_tuple_version_manager.o ../detail/version_manager.cpp

test_named_tuple_test_named_tuple.o:test_named_tuple.cpp \
  ../named_tuple.hpp \
  ../basic_tuple.hpp \
  foo.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_named_tuple_test_named_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_named_tuple_test_named_tuple.o test_named_tuple.cpp

test_perf_test_perf.o:test_perf.cpp \
  ../get_proc_stat.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_perf_test_perf.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_perf_test_perf.o test_perf.cpp

../detail/test_perf_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../get_proc_stat.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_perf_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_perf_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_pool_array_op.o:../detail/array_op.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_array_op.o ../detail/array_op.cpp

../detail/test_pool_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_pool_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_pool_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_pool_dump_fn.o:../detail/dump_fn.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_dump_fn.o ../detail/dump_fn.cpp

../detail/test_pool_dyn_table.o:../detail/dyn_table.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_dyn_table.o ../detail/dyn_table.cpp

../detail/test_pool_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_pool_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_pool_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_pool_load_fn.o:../detail/load_fn.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_load_fn.o ../detail/load_fn.cpp

../detail/test_pool_string_reader.o:../detail/string_reader.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_string_reader.o ../detail/string_reader.cpp

../detail/test_pool_version_manager.o:../detail/version_manager.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_pool_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_pool_version_manager.o ../detail/version_manager.cpp

test_pool_test_pool.o:test_pool.cpp \
  ../object_pool.hpp \
  ../rc_memory_pool.h \
  ../unused/memory_pool.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_pool_test_pool.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_pool_test_pool.o test_pool.cpp

../detail/test_string_writer_array_op.o:../detail/array_op.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_array_op.o ../detail/array_op.cpp

../detail/test_string_writer_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_string_writer_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_string_writer_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_string_writer_dump_fn.o:../detail/dump_fn.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_dump_fn.o ../detail/dump_fn.cpp

../detail/test_string_writer_dyn_table.o:../detail/dyn_table.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_dyn_table.o ../detail/dyn_table.cpp

../detail/test_string_writer_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_string_writer_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_string_writer_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_string_writer_load_fn.o:../detail/load_fn.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_load_fn.o ../detail/load_fn.cpp

../detail/test_string_writer_string_reader.o:../detail/string_reader.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_string_reader.o ../detail/string_reader.cpp

../detail/test_string_writer_version_manager.o:../detail/version_manager.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_writer_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_writer_version_manager.o ../detail/version_manager.cpp

test_string_writer_test_string_writer.o:test_string_writer.cpp \
  ../string_writer.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_string_writer_test_string_writer.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_string_writer_test_string_writer.o test_string_writer.cpp

test_string_reader_test_string_reader.o:test_string_reader.cpp \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_string_reader_test_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_string_reader_test_string_reader.o test_string_reader.cpp

../detail/test_string_reader_string_reader.o:../detail/string_reader.cpp \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_string_reader_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_string_reader_string_reader.o ../detail/string_reader.cpp

../detail/test_graph_array_op.o:../detail/array_op.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_array_op.o ../detail/array_op.cpp

../detail/test_graph_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_graph_base_cowbass4.o:../detail/base_cowbass4.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_graph_binary_file_reader.o:../detail/binary_file_reader.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_graph_dump_fn.o:../detail/dump_fn.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_dump_fn.o ../detail/dump_fn.cpp

../detail/test_graph_dyn_table.o:../detail/dyn_table.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_dyn_table.o ../detail/dyn_table.cpp

../detail/test_graph_dyn_tuple.o:../detail/dyn_tuple.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_graph_dyn_unique_index.o:../detail/dyn_unique_index.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_graph_get_proc_stat.o:../detail/get_proc_stat.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_graph_load_fn.o:../detail/load_fn.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_load_fn.o ../detail/load_fn.cpp

../detail/test_graph_string_reader.o:../detail/string_reader.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_string_reader.o ../detail/string_reader.cpp

../detail/test_graph_version_manager.o:../detail/version_manager.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_graph_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_graph_version_manager.o ../detail/version_manager.cpp

test_graph_test_graph.o:test_graph.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_graph_test_graph.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_graph_test_graph.o test_graph.cpp

../detail/test_fixed_deque_array_op.o:../detail/array_op.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_array_op.o ../detail/array_op.cpp

../detail/test_fixed_deque_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_fixed_deque_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_fixed_deque_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_fixed_deque_dump_fn.o:../detail/dump_fn.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_dump_fn.o ../detail/dump_fn.cpp

../detail/test_fixed_deque_dyn_table.o:../detail/dyn_table.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_dyn_table.o ../detail/dyn_table.cpp

../detail/test_fixed_deque_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_fixed_deque_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_fixed_deque_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_fixed_deque_load_fn.o:../detail/load_fn.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_load_fn.o ../detail/load_fn.cpp

../detail/test_fixed_deque_string_reader.o:../detail/string_reader.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_string_reader.o ../detail/string_reader.cpp

../detail/test_fixed_deque_version_manager.o:../detail/version_manager.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_fixed_deque_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_fixed_deque_version_manager.o ../detail/version_manager.cpp

test_fixed_deque_test_fixed_deque.o:test_fixed_deque.cpp \
  ../fixed_deque.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_fixed_deque_test_fixed_deque.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_fixed_deque_test_fixed_deque.o test_fixed_deque.cpp

test_template_folding_test_template_folding.o:test_template_folding.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_template_folding_test_template_folding.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_template_folding_test_template_folding.o test_template_folding.cpp

../detail/test_stack_array_op.o:../detail/array_op.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_array_op.o ../detail/array_op.cpp

../detail/test_stack_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_stack_base_cowbass4.o:../detail/base_cowbass4.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_stack_binary_file_reader.o:../detail/binary_file_reader.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_stack_dump_fn.o:../detail/dump_fn.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_dump_fn.o ../detail/dump_fn.cpp

../detail/test_stack_dyn_table.o:../detail/dyn_table.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_dyn_table.o ../detail/dyn_table.cpp

../detail/test_stack_dyn_tuple.o:../detail/dyn_tuple.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_stack_dyn_unique_index.o:../detail/dyn_unique_index.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_stack_get_proc_stat.o:../detail/get_proc_stat.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_stack_load_fn.o:../detail/load_fn.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_load_fn.o ../detail/load_fn.cpp

../detail/test_stack_string_reader.o:../detail/string_reader.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_string_reader.o ../detail/string_reader.cpp

../detail/test_stack_version_manager.o:../detail/version_manager.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_stack_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_stack_version_manager.o ../detail/version_manager.cpp

test_stack_test_stack.o:test_stack.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_stack_test_stack.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_stack_test_stack.o test_stack.cpp

test_dyn_tuple_test_dyn_tuple.o:test_dyn_tuple.cpp \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dyn_tuple_test_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_dyn_tuple_test_dyn_tuple.o test_dyn_tuple.cpp

../detail/test_dyn_tuple_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_tuple_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_tuple_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_dyn_tuple_string_reader.o:../detail/string_reader.cpp \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_tuple_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_tuple_string_reader.o ../detail/string_reader.cpp

test_memory_ref_test_memory_ref.o:test_memory_ref.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_memory_ref_test_memory_ref.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_memory_ref_test_memory_ref.o test_memory_ref.cpp

test_offset_list_test_offset_list.o:test_offset_list.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_offset_list_test_offset_list.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_offset_list_test_offset_list.o test_offset_list.cpp

test_adt_test_adt.o:test_adt.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_adt_test_adt.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_adt_test_adt.o test_adt.cpp

test_override_and_specialize_test_override_and_specialize.o:test_override_and_specialize.cpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_override_and_specialize_test_override_and_specialize.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_override_and_specialize_test_override_and_specialize.o test_override_and_specialize.cpp

test_dyn_unique_index_test_dyn_unique_index.o:test_dyn_unique_index.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dyn_unique_index_test_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_dyn_unique_index_test_dyn_unique_index.o test_dyn_unique_index.cpp

../detail/test_dyn_unique_index_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_unique_index_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_unique_index_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_dyn_unique_index_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_unique_index_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_unique_index_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_dyn_unique_index_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_unique_index_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_unique_index_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_dyn_unique_index_string_reader.o:../detail/string_reader.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_unique_index_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_unique_index_string_reader.o ../detail/string_reader.cpp

test_dyn_table_test_dyn_table.o:test_dyn_table.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../dyn_table.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_dyn_table_test_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_dyn_table_test_dyn_table.o test_dyn_table.cpp

../detail/test_dyn_table_dyn_table.o:../detail/dyn_table.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../dyn_table.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_table_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_table_dyn_table.o ../detail/dyn_table.cpp

../detail/test_dyn_table_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../dyn_table.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_table_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_table_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_dyn_table_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../dyn_table.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_table_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_table_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_dyn_table_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../dyn_table.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_table_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_table_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_dyn_table_string_reader.o:../detail/string_reader.cpp \
  ../base_cow_hash_map.h \
  ../dyn_unique_index.h \
  ../dyn_index.h \
  ../dyn_tuple.h \
  ../detail/dyn_tuple_inl.h \
  ../dyn_table.h \
  ../string_reader.hpp \
  ../detail/string_reader_inl.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_dyn_table_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_dyn_table_string_reader.o ../detail/string_reader.cpp

../detail/test_load_dump_fn_array_op.o:../detail/array_op.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_array_op.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_array_op.o ../detail/array_op.cpp

../detail/test_load_dump_fn_base_cow_hash_map.o:../detail/base_cow_hash_map.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_base_cow_hash_map.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_base_cow_hash_map.o ../detail/base_cow_hash_map.cpp

../detail/test_load_dump_fn_base_cowbass4.o:../detail/base_cowbass4.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_base_cowbass4.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_base_cowbass4.o ../detail/base_cowbass4.cpp

../detail/test_load_dump_fn_binary_file_reader.o:../detail/binary_file_reader.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_binary_file_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_binary_file_reader.o ../detail/binary_file_reader.cpp

../detail/test_load_dump_fn_dump_fn.o:../detail/dump_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_dump_fn.o ../detail/dump_fn.cpp

../detail/test_load_dump_fn_dyn_table.o:../detail/dyn_table.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_dyn_table.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_dyn_table.o ../detail/dyn_table.cpp

../detail/test_load_dump_fn_dyn_tuple.o:../detail/dyn_tuple.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_dyn_tuple.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_dyn_tuple.o ../detail/dyn_tuple.cpp

../detail/test_load_dump_fn_dyn_unique_index.o:../detail/dyn_unique_index.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_dyn_unique_index.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_dyn_unique_index.o ../detail/dyn_unique_index.cpp

../detail/test_load_dump_fn_get_proc_stat.o:../detail/get_proc_stat.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_get_proc_stat.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_get_proc_stat.o ../detail/get_proc_stat.cpp

../detail/test_load_dump_fn_load_fn.o:../detail/load_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_load_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_load_fn.o ../detail/load_fn.cpp

../detail/test_load_dump_fn_string_reader.o:../detail/string_reader.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_string_reader.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_string_reader.o ../detail/string_reader.cpp

../detail/test_load_dump_fn_version_manager.o:../detail/version_manager.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m../detail/test_load_dump_fn_version_manager.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ../detail/test_load_dump_fn_version_manager.o ../detail/version_manager.cpp

test_load_dump_fn_test_load_dump_fn.o:test_load_dump_fn.cpp \
  ../c_selector.hpp \
  ../cow_table.hpp \
  ../detail/cow_table_inl.hpp \
  ../cow_hash_map.hpp \
  ../cow_hash_cluster_map.hpp \
  ../cowbass4.hpp \
  ../named_tuple.hpp \
  ../detail/binary_file_reader.h \
  ../detail/meta_reader.hpp
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtest_load_dump_fn_test_load_dump_fn.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o test_load_dump_fn_test_load_dump_fn.o test_load_dump_fn.cpp

endif #ifeq ($(shell uname -m),x86_64)



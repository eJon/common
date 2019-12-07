**What is smalltable?**  
smalltable is an in-memory single-machine solution for efficient storage and query of data. It stores data in tables and supports inter-table operations. smalltable may extend its storage capability in an unique way that uses SSD or Mola as its backend later.
> smalltable 是一种用于单机内存型数据高效存储和查询的解决方案。samlltable 将数据存储于表中并且支持表内操作。 smalltable 在将来也许会使用 SSD 或者 Mola 作为其唯一的底层存储组件来扩展存储能力。  

**What is smalltable1?**  
smalltable1 is an experimental build for relatively simple storage and query of data. It unifies the unit of data as tuple which is a set of attributes.  
> smalltable1 是实验性质的版本，提供了较为简单的数据存储和查询能力。它使用元组（tuple）的概念将一组属性（attribute）集合统一了数据单位。 

Data are treated as a list of tuples and stored in two kinds of indexes:  
> 数据被当成元组列表存储在两种索引形式中：  
  + PrimaryHashIndex: tuples stored in this index are different at some subset of attributes which is called key. This index is a combination of tuple and regular hash mapping for fast addressing of one tuple by one key.
  + > 主哈希索引：存储在该索引中的元组在某些属性子集（称为键）上有所不同。这种索引将元组和正常的哈希表组合，从而能够通过一个key快速找到元组数据。  
  + NonPrimaryHashIndex: tuples stored in this index may have same keys. Tuples with the same key are grouped together and ordered in a data structure called Cowbass(Copy-On-Write Block-Accessed Sorted Set). This index enables fast addressing of a group of ordered tuples by their key, and iterating these ordered tuples is as fast as native array.
  + > 非主哈希索引：存储在该索引中的元组有相同的key。具有相同key的元组被分组在一起并在名为Cowbass的数据结构中排序。这种索引能够通过key快速定位到一组有序元组，并且做到像数组一样快速的遍历这些有序元组。
  
Indexes are not directly used by users, they are part of table which is a container of tuples. Table uses indexes to speed up query, since an index is generally designed for one query criteria, a table may have multiple indexes to speed up queries originated from different attribute sets.
> 索引并非直接被用户使用，索引是作为元组容器的表的一部分。表使用索引去加速查询，因为一个索引通常被设计为一个查询条件，一个表可能会有多个索引去针对不同的属性集合实现查询加速。
Users use selectors to specify query criteria. smalltable1 implemented selectors on one table or three tables(experimental) with conjunctive predicates. The predicates can be: ==, !=, >=, <=, >, < and function objects accepting 1 or 2 parameters and returning booleans. smalltable1 specially treats "==" as bi-directional propagation of values and developed an algorithm to find out query sequence of tables and which indexes to choose.
Users use selectors to specify query criteria. smalltable1 implemented. 
> 用户使用选择器去指定查询条件。smalltable1 通过联合谓词在一个表或三个表中实现选择器。这些谓词包括 ==, !=, >=, <=, >, < 和函数，函数可以接收一个或者两个参数并返回布尔值。smalltable1 特别将“==”视为值的双向传播，并开发了一种算法来找出表的查询顺序以及要选择的索引。

+ What is smalltable2?
  smalltable2 is production-quality build to provide tables, multi-table 
  selectors, multi-table triggers and materialized views. It improves 
  smalltable1 in all aspects.
  + storage
    + A point change in Cowbass of smalltable1 enforces the whole Cowbass to 
      copy itself, in smalltable2 it's not.
    + Point search inside Cowbass of smalltable1 is slow, proportional to the
      length of the Cowbass. Point search inside Cowbass of smalltable2 is
      fast.
    + Changes to Cowbass of smalltable1 are not in-place, users call additional
      interfaces to merge changes in place. This design has the advantage of
      very fast insertions and erasures, but if the interfaces are not
      called, early versions of smalltable1 do not allow users to access the
      changes and later versions are slow. And this design made point search
      in a modifed Cowbass impossible (with a reasonable speed). Changes to
      Cowbass of smalltable2 are in-place and its speed is ok and quite
      improvable later.
    + Cowbass of smalltable1 is aware of background and foreground access (to
      share memory) and has two sets of interfaces prefixed with bg_ and fg_
      respectively. This is proved to be a really bad practice for users.
      In smalltable2, one cowbass just stores one collection of values and 
      users control different Cowbasses to share memory at bottom level which
      has nothing to do with interfaces.
(TO BE CONTINUED)

+ What is smalltable?
  smalltable is an in-memory single-machine solution for efficient storage 
  and query of data. It stores data in tables and supports inter-table
  operations. smalltable may extend its storage capability in an unique way
  that uses SSD or Mola as its backend later.

+ What is smalltable1?
  smalltable1 is an experimental build for relatively simple storage and query
  of data. It unifies the unit of data as tuple which is a set of attributes.
  Data are treated as a list of tuples and stored in two kinds of indexes:
  + PrimaryHashIndex: tuples stored in this index are different at some subset
    of attributes which is called key. This index is a combination of tuple and
    regular hash mapping for fast addressing of one tuple by one key.
  + NonPrimaryHashIndex: tuples stored in this index may have same keys. Tuples
    with the same key are grouped together and ordered in a data structure 
    called Cowbass(Copy-On-Write Block-Accessed Sorted Set). This index enables
    fast addressing of a group of ordered tuples by their key, and iterating
    these ordered tuples is as fast as native array.
  
  Indexes are not directly used by users, they are part of table which is a
  container of tuples. Table uses indexes to speed up query, since an index is
  generally designed for one query criteria, a table may have multiple indexes 
  to speed up queries originated from different attribute sets.
  
  Users use selectors to specify query criteria. smalltable1 implemented
  selectors on one table or three tables(experimental) with conjunctive 
  predicates. The predicates can be: ==, !=, >=, <=, >, < and function objects
  accepting 1 or 2 parameters and returning booleans. smalltable1 specially 
  treats "==" as bi-directional propagation of values and developed an
  algorithm to find out query sequence of tables and which indexes to choose.

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

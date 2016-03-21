// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Definition of graphics
// Author: yanlin@baidu.com
// Date: Thu Aug  5 10:27:32 2010
#pragma once
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <iterator>
#include <bsl/map.h>
#include <bsl/list.h>
#include <bsl/string.h>
#include "debug.h"
#include "string_writer.hpp"

namespace st{

#define DEFAULT_VERTEX_COUNT 64

/**
 * Definition of the node in adjacency list
 */
template <typename T>
struct adjacency_node {
    typedef adjacency_node<T> this_type;
    T vertex;
    this_type *next;
};

template < typename T, typename vertex_equal_func=std::equal_to<T> > class graph_t;

/**
 * Definition of adjacency list iterator
 */
template <typename T>
struct adjacency_list_iterator {
    typedef adjacency_node<T>* node_pointer;
    typedef T reference;
    typedef T * pointer;
    typedef adjacency_list_iterator<T> self;

    friend class graph_t<T>;
private:
    adjacency_list_iterator(node_pointer x)
    {
        _node = x;
    }

public:
    adjacency_list_iterator()
    {
        _node = NULL;
    }
    adjacency_list_iterator(const self& x)
    {
        _node = x._node;
    }
    reference operator *() const
    {
        return _node->vertex;
    }
        
    bool operator == (const self & __iter) const
    {
        return (_node == __iter._node);
    }
    bool operator != (const self & __iter) const
    {
        return (_node != __iter._node);
    }
    pointer operator ->() const
    {
        return &_node->vertex;
    }

    self& operator ++ ()
    {
        if (NULL != _node) {
            _node = _node->next;            
        }
        return *this;
    }

    self operator ++ (int)
    {
        self iter = *this;
        ++ *this;
        return iter;
    }
private:
    node_pointer _node;
};

/**
 * Definition of DFS visitor, the base class
 * 
 */
struct dfs_visitor_t {
    template <typename T>
    void start_vertex(const T&) {}

    template <typename T>
    void back_edge(const T&) {}

    template <typename T>
    void finish_vertex(const T&) {}

    template <typename T>
    void forward_or_cross_edge(const T&) {}
};

/**
 * Definition of topo sort visitor
 * 
 */
template <typename T, typename OutIterator>
struct topo_visitor_t : dfs_visitor_t {
    topo_visitor_t(OutIterator iter):_is_dag(true), _iter(iter) {}

    void back_edge(const T&)
    {
        _is_dag = false;
    }
    void finish_vertex(const T&t)
    {
        *_iter++ = t;
    }
    bool is_dag() {return _is_dag;};
private:
    bool _is_dag;
    OutIterator _iter;
};

/**
 * Definition of iterator of vertex table
 * By this iterator, caller could enumerate all vertexes in the graph
 * 
 */
template <typename T>
struct vertex_iterator {
    typedef typename bsl::hashmap<T, adjacency_node<T> *>::iterator table_iterator;
    typedef vertex_iterator <T> self;
    typedef T reference;
    typedef T * pointer;

    friend class graph_t<T>;
private:
    vertex_iterator(table_iterator x)
    {
        _iter = x;
    }

public:
    vertex_iterator()
    {
        _iter = NULL;
    }
    vertex_iterator(const self& x)
    {
        _iter = x._iter;
    }
    reference operator *() const
    {
        return _iter->first;
    }
        
    bool operator == (const self & __other) const
    {
        return (_iter == __other._iter);
    }
    bool operator != (const self & __other) const
    {
        return (_iter != __other._iter);
    }
    pointer operator ->() const
    {
        return &(_iter->first);
    }

    self& operator ++ ()
    {
        ++_iter;
        return *this;
    }

    self operator ++ (int)
    {
        self iter = *this;
        ++_iter;
        return iter;
    }
private:
    table_iterator _iter;
};
/**
 * Definition of const iterator of vertex table
 * By this iterator, caller could enumerate all vertexes in the graph
 * 
 */
template <typename T>
struct vertex_const_iterator {
    typedef typename bsl::hashmap<T, adjacency_node<T> *>::const_iterator table_const_iterator;
    typedef vertex_const_iterator <T> self;
    typedef T reference;
    typedef T * pointer;

    friend class graph_t<T>;
private:
    vertex_const_iterator(table_const_iterator x)
    {
        _iter = x;
    }

public:
    vertex_const_iterator()
    {
        _iter = NULL;
    }
    vertex_const_iterator(const self& x)
    {
        _iter = x._iter;
    }
    reference operator *() const
    {
        return _iter->first;
    }
        
    bool operator == (const self & __other) const
    {
        return (_iter == __other._iter);
    }
    bool operator != (const self & __other) const
    {
        return (_iter != __other._iter);
    }
    pointer operator ->() const
    {
        return &(_iter->first);
    }

    self& operator ++ ()
    {
        ++_iter;
        return *this;
    }

    self operator ++ (int)
    {
        self iter = *this;
        ++_iter;
        return iter;
    }
private:
    table_const_iterator _iter;
};

/**
 * The graph class, a simple implementation, based on adjacency list
 * 
 */
template < typename T, 
           typename vertex_equal_func >
class graph_t
{
public:
    typedef adjacency_list_iterator<T> adjacency_iterator;
    typedef adjacency_node<T> adjacency_node_t;
    typedef adjacency_node<T> *adjacency_node_pointer;
    typedef typename bsl::hashmap<T, adjacency_node_pointer> vertex_table_t;
    typedef typename bsl::hashmap<T, adjacency_node_pointer>::iterator table_iterator;
    typedef typename bsl::hashmap<T, adjacency_node_pointer>::const_iterator table_const_iterator;
    
    // typedef `vertex_iterator' raises conflicts since
    // `vertex_iterator<T>' has already defined before
    // Specify namespace shall fix this problem
    typedef ::st::vertex_iterator<T> vertex_iterator;
    typedef ::st::vertex_const_iterator<T> vertex_const_iterator;
    typedef std::pair<adjacency_iterator, adjacency_iterator> adjacency_iterator_pair;


    enum COLOR {WHITE, BLACK, GRAY};
    /**
     * Constructor
     * @param vertex_count the default count of vertexes
     * @param equal_func the function to compare vertex
     */
    graph_t(int vertex_count = DEFAULT_VERTEX_COUNT, vertex_equal_func equal_func=vertex_equal_func()):
        _vertex_equal(equal_func)
    {
        _vertex_table.create(vertex_count);
    }
    /**
     * Constructor
     * @param equal_func the function to compare vertex
     */
    graph_t(vertex_equal_func equal_func):
        _vertex_equal(equal_func)
    {
        _vertex_table.create(DEFAULT_VERTEX_COUNT);
    }
    /**
     * Destructor
     * @param 
     */
    ~graph_t()
    {
        clear();
    }

    bool add_vertex(const T& vertex)
    {
        if (_vertex_table.find(vertex) == NULL) {
            _vertex_table.set(vertex, NULL);
        }
        return true;
    }

    /**
     * Add an age to the graph
     * @param from the begin vertex
     * @param to the end vertex
     * @return True if the edge is created. False if the memory can not be allocated
     */
    bool add_edge(const T&from, const T&to)
    {
        adjacency_node_pointer node = NULL;
        if (_vertex_table.get(from, &node) == bsl::HASH_EXIST) {
            adjacency_node_pointer pre_node = node;
            while (NULL != node) {
                if (_vertex_equal(to, node->vertex)) {
                    break;
                }
                pre_node = node;
                node = node->next;
            }
            if (NULL == node) {
                node = new (std::nothrow) adjacency_node_t();
                if (NULL != node) {
                    node->next = NULL;
                    node->vertex = to;
                }
                else {
                    ST_FATAL("Fail to allocate adjacency_node_t");
                    return false;
                }
                if (NULL != pre_node) {
                    pre_node->next = node;
                }
                else {
                    _vertex_table.set(from, node, 1);
                }
                if (_vertex_table.find(to) == NULL) {
                    _vertex_table.set(to, NULL, 1);
                }
            }
        }
        else {
            node = new (std::nothrow) adjacency_node_t();
            if (NULL == node) {
                ST_FATAL("Fail to allocate adjacency_node_t");
                return false;
            }
            node->vertex = to;
            node->next = NULL;
            _vertex_table.set(from, node);
        }
        return true;
    }
    /**
     * Get iterator for enumerate adjacency list
     * @param t the vertex object
     * @see reference function
     * @return a std::pair object, the first is the beginning, the second is the end
     */
    adjacency_iterator_pair get_adjacency_list(const T& t) const
    {
        adjacency_iterator_pair result;
        result.second = adjacency_iterator(NULL);

        adjacency_node_pointer node = NULL;
        _vertex_table.get(t, &node);

        result.first = adjacency_iterator(node);
        return result;
    }
    /**
     * Return the beginning iterator of vertex table
     * @return vertex_iterator object.
     * 
     */
    vertex_iterator vertex_table_begin()
    {
        return vertex_iterator(_vertex_table.begin());
    }    
    /**
     * Return the ending iterator of vertex table
     * @return vertex_iterator object.
     * 
     */
    vertex_iterator vertex_table_end()
    {
        return vertex_iterator(_vertex_table.end());
    }
    /**
     * Return the beginning iterator of vertex table
     * @return vertex_const_iterator object.
     * 
     */
    vertex_const_iterator vertex_table_begin() const
    {
        return vertex_const_iterator(_vertex_table.begin());
    }    
    /**
     * Return the ending iterator of vertex table
     * @return vertex_const_iterator object.
     * 
     */
    vertex_const_iterator vertex_table_end() const
    {
        return vertex_const_iterator(_vertex_table.end());
    }
    /**
     * Return the size of vertex table
     * @return size
     * 
     */
    size_t vertex_table_size() const
    {
        return _vertex_table.size();
    }
    /**
     * Clear all vertexes and edges in the graph
     *
     */
    void clear()
    {
        table_iterator iter = _vertex_table.begin();
        table_iterator end = _vertex_table.end();

        for (; iter != end; ++iter) {
            adjacency_node_pointer node = iter->second;
            while (NULL != node) {
                adjacency_node_pointer n = node;
                node = node->next;
                delete n;
            };
            iter->second = NULL;
        }
    }
    /**
     * Dump content to string
     * @param sb the string build that dump to.
     */
    void to_string(StringWriter &sb) const
    {
        
        table_const_iterator it = _vertex_table.begin();
        for(; it != _vertex_table.end(); ++it ) {
            sb << it->first << ":";
            adjacency_node_pointer node = it->second;
            while (NULL != node) {
                sb << "->" << node->vertex;
                node = node->next;
            }
            sb << "\n";
        }
    }
private:
    vertex_table_t _vertex_table;
    vertex_equal_func _vertex_equal;
};

/**
 * A DFS method
 * @param graph The graph_t object
 * @param visitor The visitor object to be called when visiting each vertex
 *
 * @see graph_t
 * @see dfs_visitor_t
 */
template <typename T, typename Visitor>
void depth_first_search(const graph_t<T> &graph, Visitor &visitor)
{
    typedef graph_t<T> Graph;
    typename Graph::vertex_const_iterator iter = graph.vertex_table_begin();
    typename Graph::vertex_const_iterator the_end = graph.vertex_table_end();
    int size = graph.vertex_table_size();

    if (0 == size) {
        ST_WARN ("The graph is empty");
        return;
    }

    bsl::hashmap<T, int> in_edge_map;
    bsl::hashmap<T, typename Graph::COLOR> color_map;

    in_edge_map.create(size);
    color_map.create(size);
        
    //initial
    for (; iter != the_end; ++iter) {
        T t = *iter;
        typename Graph::adjacency_iterator_pair adj_iter_pair = graph.get_adjacency_list(t);
        typename Graph::adjacency_iterator adj_iter = adj_iter_pair.first;
        for (; adj_iter != adj_iter_pair.second; ++adj_iter) {
            int v;
            if (in_edge_map.get(*adj_iter, &v) == bsl::HASH_EXIST) {
                in_edge_map.set(*adj_iter, ++v, 1);
            }
            else {
                in_edge_map.set(*adj_iter, 1);
            }            
        }    
        color_map.set(t, Graph::WHITE);
    }

    //find top vertex
    iter = graph.vertex_table_begin();
    for (; iter != the_end; ++iter) {
        if (in_edge_map.find(*iter) == NULL) {
            //visit from this vertex
            depth_first_search_impl(graph, *iter, visitor, color_map);            
        }
    }
}

/**
 * A DFS method
 * @param graph The graph_t object
 * @param t the vertex object that start from
 * @param visitor The visitor object to be called when visiting each vertex
 * @see graph_t
 * @see dfs_visitor_t
 */
template <typename T, typename Visitor>
void depth_first_search(const graph_t<T> &graph, T t, Visitor &visitor)
{
    typedef graph_t<T> Graph;
    typename Graph::vertex_const_iterator iter = graph.vertex_table_begin();
    typename Graph::vertex_const_iterator the_end = graph.vertex_table_end();
    int size = graph.vertex_table_size();

    bsl::hashmap<T, typename Graph::COLOR> color_map;

    color_map.create(size);
        
    //initial
    for (; iter != the_end; ++iter) {
        color_map.set(*iter, Graph::WHITE);
    }

    //visit from this vertex
    depth_first_search_impl(graph, t, visitor, color_map);            
}

/**
 * A DFS method implementation, use recursive method
 *
 * @param graph The graph_t object
 * @param t the vertex object
 * @param visitor The visitor object to be called when visiting each vertex
 * @param color_map the color to mark each vertexes
 *
 * @see depth_first_search
 * @see graph_t
 * @see dfs_visitor_t
 */
template <typename T, typename Visitor>
void depth_first_search_impl(const graph_t<T> &graph, T t, Visitor &visitor, 
                            bsl::hashmap<T, typename graph_t<T>::COLOR> &color_map)
{
    typedef graph_t<T> Graph;

    visitor.start_vertex(t);
    color_map.set(t, Graph::GRAY, 1);

    typename Graph::adjacency_iterator_pair adj_iter_pair = graph.get_adjacency_list(t);
    typename Graph::adjacency_iterator adj_iter = adj_iter_pair.first;
    for (; adj_iter != adj_iter_pair.second; ++adj_iter) {
        typename Graph::COLOR c;
        T v = *adj_iter;
        if (color_map.get(v, &c) == bsl::HASH_NOEXIST) {
            ST_FATAL("Not existed in color map");
            continue;
        }
        if (Graph::WHITE == c) {
            depth_first_search_impl(graph, v, visitor, color_map);
        }
        else if (Graph::GRAY == c) {
            visitor.back_edge(v);
        }
        else {
            visitor.forward_or_cross_edge(v);
        }
    }
    visitor.finish_vertex(t);
    color_map.set(t, Graph::BLACK, 1);
}

/**
 * Topological sort algorithm
 * The implementation is mainly a call to depth-first search
 *
 * @param graph The graph object
 * @param iter an OutputIterator. The result will be inserted into this iterator.
 *             Note that if it is a back_insert_iterator, the result may be in reverse order. 
 *             So we recomment to input front_insert_iterator here. 
 *
 * @see graph_t
 * @see depth_first_search 
 *
 * @return True if the sorting succeed. False if the graph is not a DAG(Directed acyclic graph)
 * 
 */
template <typename T, typename OutIterator>
bool topological_sort(graph_t<T> &graph,  OutIterator iter)
{
    topo_visitor_t<T, OutIterator> visitor(iter);
    depth_first_search(graph, visitor);
    return visitor.is_dag();
}

}
#endif /* _GRAPHICS_H_ */

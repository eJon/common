#ifndef _ST_BIT_MAP_H_
#define _ST_BIT_MAP_H_

#pragma once
#include <sstream>                          // ostringstream
#include "functional.hpp"
#include "st_errno.h"
#include "bitmap.h"

namespace st {

template <class _Map> class BitwiseIterator {

public:    
    typedef class _Map::Item Value;
    typedef typename _Map::BitmapType BtType;

    BitwiseIterator():bitmap_(NULL),array_(NULL),cur_pos_(0),bm_len_(0)
    {}

    BitwiseIterator(BtType bt,Value* ar,int cp,int len )    : bitmap_ (bt)
                                                            ,array_  (ar)
                                                            ,cur_pos_(cp)
                                                            ,bm_len_(len)
    { find_valid_node(cur_pos_); }

    bool operator== (const BitwiseIterator& rhs) const
    {
        return is_same(rhs);    
    }
    
    bool operator!= (const BitwiseIterator& rhs) const
    {
        return ! is_same(rhs);
    }

    BitwiseIterator& operator++()
    {
        find_valid_node(cur_pos_+1); 

        return *this;
    }
    
    BitwiseIterator operator++ (int) 
    {
        BitwiseIterator tmp = *this;
        this->operator++();
        return tmp;
    }

    Value& operator* () const
    {
        return *reinterpret_cast<Value*>(&array_[cur_pos_]);
    }

    Value* operator-> ()const
    {
        return reinterpret_cast<Value*>(&array_[cur_pos_]);
    }

    operator bool() const  {  return cur_pos_ <  bm_len_;   }

    void set_end() { cur_pos_ = bm_len_ ; }

    void to_string (StringWriter& sw) const
    {
        sw << "BitmapIter{p_item=" << ((cur_pos_ > bm_len_) && (array_ != NULL))
            ? NULL: operator*()
           << "}";
    }
    
private:
    void find_valid_node(int pos)
    {
        cur_pos_ = find_next_bit(bitmap_,bm_len_,pos);
    }
    
    bool is_same(const BitwiseIterator& rhs) const
    {
        return (cur_pos_ == rhs.cur_pos_)
            && (bitmap_  == rhs.bitmap_ )
            && (array_   == rhs.array_  )
            && (bm_len_  == rhs.bm_len_ );
    }
    
    BtType  bitmap_;
    Value * array_;
    int cur_pos_;
    size_t bm_len_;
};


const size_t CH_MIN_BITMAP_LEN     = 256;       // minimum length of bitmap
const size_t CH_DEFAULT_BITMAP_LEN = 51200;     // default length of bitmap
const size_t CH_MAX_BITMAP_LEN     = 50000000;  //max len of bitmap
    

template <class _Key,
          class _Val,
          class _ComputeKey = Hash<_Key>,
          class _Equal      = Equal<_Key> >
class BitwiseMap {
template <class _Map> friend class BitwiseIterator;

public:
    typedef std::pair<_Key,_Val> Pair;
    typedef Pair Item;
    typedef _Key Key;
    typedef _Val Value;
    typedef _Val* Pointer;
    typedef _Val& Reference;
    typedef const _Val& ConstReference;
    typedef BitwiseIterator<BitwiseMap> Iterator;
    typedef unsigned long* BitmapType;

    BitwiseMap ()
        : n_item_(0)
        , bitmap_len_(CH_DEFAULT_BITMAP_LEN)
        , item_size_(0)
        , bitmap_(NULL)
        , ap_entry_(NULL)
        {}
    
    ~BitwiseMap()
    {
        if( NULL != bitmap_) {
            delete[] bitmap_;
        }
        if( NULL != ap_entry_) {
            delete[] ap_entry_;
        }
    }
        
    int init (size_t bitmap_len) {

        if(bitmap_len > CH_MAX_BITMAP_LEN) {
            ST_WARN("too large length to init bitmap,bitmap_len is %lu, max len is %lu",
                    bitmap_len,CH_MAX_BITMAP_LEN);
            return ECONFLICT;
        }
        
        std::ostringstream oss;
        oss << c_show(Key) << "->" << c_show(Value);

        desc_ = oss.str(); 

        if ( !not_init()  ) {
            ST_FATAL("%s is already initialized", desc_.c_str());
            return ECONFLICT;
        }

        item_size_ = sizeof(Pair);
        n_item_    = 0;
        
        bitmap_len_ = bitmap_len < CH_MIN_BITMAP_LEN ? CH_MIN_BITMAP_LEN : bitmap_len;

        bitmap_ = new (std::nothrow) unsigned long[BITS_TO_LONGS(bitmap_len_)];
        if( NULL == bitmap_) {
            ST_FATAL("Fail to new bit_map_");
            ap_entry_ = NULL;
            return ENOMEM;
        }
       
        bitmap_zero(bitmap_, bitmap_len_); 
        
        ap_entry_ = ST_NEW_ARRAY(Pair, bitmap_len_);
        if (NULL == ap_entry_) {
            ST_FATAL("Fail to new ap_entry_");

            delete[] bitmap_;
            bitmap_ = NULL;
            return ENOMEM;
        }
        
        return 0;
    }

    BitwiseMap(const BitwiseMap& rhs)
    {   
        bitmap_     = NULL;
        ap_entry_   = NULL;
        n_item_     = 0;
        bitmap_len_ = CH_DEFAULT_BITMAP_LEN;
        item_size_  = 0;
 
        if ( rhs.not_init()  ) {
            ST_FATAL("source is not initialized");
            // and we keep ourself uninitialized
            
            return;
        }
    
        bitmap_ = new (std::nothrow) unsigned long[BITS_TO_LONGS(rhs.bitmap_len_)];
        if( NULL == bitmap_) {
            ST_FATAL("Failed to new bitmap_");

            ap_entry_ = NULL;
            return ;
        }
    
        ap_entry_ = ST_NEW_ARRAY(Pair,rhs.bitmap_len_);
        if( NULL == ap_entry_) {
            ST_FATAL("Failed to new ap_entry_");

            delete[] bitmap_;
            bitmap_ = NULL;
            return;
        }

        bitmap_zero(bitmap_, rhs.bitmap_len_);
        bitmap_or(bitmap_,bitmap_,rhs.bitmap_,rhs.bitmap_len_);
    
 
        for(size_t i = 0; i < rhs.bitmap_len_; i++) {
            ap_entry_[i] = rhs.ap_entry_[i];
        }

        n_item_     = rhs.n_item_;
        bitmap_len_ = rhs.bitmap_len_;    
        item_size_  = rhs.item_size_;
        desc_       = rhs.desc_;
    }
 
    BitwiseMap& operator= (const BitwiseMap& rhs)
    {
        if(&rhs == this) {
            return *this;
        }

        if (not_init()) {
        //just call copy-constructor
            ST_NEW_ON(this, BitwiseMap, rhs);
            return *this;
        }
                                         
        if (rhs.not_init()) {  // source is not initialized 
            delete[] bitmap_;
            delete[] ap_entry_;
            this->bitmap_   = NULL;
            this->ap_entry_ = NULL;
            return *this;
        }
        
        if ( bitmap_len_ != rhs.bitmap_len_) {
          
            delete[] bitmap_;
            delete[] ap_entry_; 
    
            bitmap_ = new (std::nothrow) unsigned long[BITS_TO_LONGS(rhs.bitmap_len_)];
            if( NULL == bitmap_) {
                ST_FATAL("Failed to new bitmap_");

                ap_entry_ = NULL;
                return *this;
            }
        
            ap_entry_ = ST_NEW_ARRAY(Pair,rhs.bitmap_len_);
            if( NULL == ap_entry_) {
                ST_FATAL("Failed to new ap_entry_");

                delete[] bitmap_;
                bitmap_ = NULL;
                return *this;
            }
        }

        bitmap_zero(bitmap_, rhs.bitmap_len_);
        bitmap_or(bitmap_,bitmap_,rhs.bitmap_,rhs.bitmap_len_);
 
        for(size_t i = 0; i < rhs.bitmap_len_; i++) {
            if( 1 == test_bit(i,bitmap_) ) {
                ap_entry_[i] = rhs.ap_entry_[i];
            }
        }

        n_item_     = rhs.n_item_;
        bitmap_len_ = rhs.bitmap_len_;    
        item_size_  = rhs.item_size_;
        desc_ = rhs.desc_;

        return *this;
    }


    Pointer insert(const Key& key,const Value& value)
    {
        u_int bm_key  = _ComputeKey()(key); 


        //if bm_key > current bitmap len, resize is needed
        if(bm_key >= bitmap_len_) {
            
            int bitmap_len2 = (bm_key * 2) < CH_MAX_BITMAP_LEN ? bm_key * 2 : bm_key;
            
            if( false == resize(bitmap_len2+1) ) {
                return NULL;
            }
        }

        //no equal item
        if (0 == test_bit(bm_key, bitmap_) ) {
            n_item_ ++;;
        }

        Pair pair(key,value);
        set_bit(bm_key,bitmap_);
        ap_entry_[bm_key] = pair;

        return &(ap_entry_[bm_key].second);
    }

    bool erase(const Key& key) 
    {        
        u_int bm_key  = _ComputeKey()(key); 

        //earse out of boundary
        if(bm_key >= bitmap_len_ ) {
            return false;
        }

        //no equal item
        if (0 == test_bit(bm_key, bitmap_) ) {
            return false;
        }

        clear_bit(bm_key,bitmap_);

        n_item_ --;
        
        return true;
    }
    
    void clear()
    {
        bitmap_zero(bitmap_,bitmap_len_); 
        n_item_ = 0;
    } 

    Pointer seek(const Key& key) const
    {
        u_int bm_key  = _ComputeKey()(key); 

        //out of boundary check
        if(bm_key >= bitmap_len_) {
            return NULL;
        }

        //no equal item
        if (0 == test_bit(bm_key, bitmap_) ) {
            return NULL;
        }

        return &(ap_entry_[bm_key].second);
    }

    bool resize(size_t bitmap_len2)
    {
        if(bitmap_len_ == bitmap_len2) {
            return true;
        }

        if(bitmap_len2 > CH_MAX_BITMAP_LEN) {
            ST_WARN("resize bitmap, size is greater than MAX_LEN,\
                    bitmap_len2 is %lu,max len is %lu",bitmap_len2,
                    CH_MAX_BITMAP_LEN);
            return false;
        }
        
        //resize to small size, return false
        if(bitmap_len2 < bitmap_len_) {
            ST_WARN("resize bitmap, size is less than current bitmap len,\
                    bitmap_len2 is %lu,bitmap_len_ is %lu",
                    bitmap_len2,bitmap_len_);
            return false;
        }

        unsigned long *bitmap2;                  
        Pair* ap_entry2;                      
        bitmap2 = new (std::nothrow) unsigned long[BITS_TO_LONGS(bitmap_len2)];
        
        if( NULL == bitmap2) {
            ST_FATAL("resize bitmap,fail to new bit_map2");
            return false;
        }
       
        
        ap_entry2 = ST_NEW_ARRAY(Pair, bitmap_len2);
        if (NULL == ap_entry2) {
            ST_FATAL("resize bitmap,fail to new ap_entry2");
            delete[] bitmap2;
            return false;
        }
        
        //copy bitmap
        bitmap_zero(bitmap2, bitmap_len2); 
        bitmap_or(bitmap2,bitmap_,bitmap2,bitmap_len_);
   
        //copy item array
        for(size_t i = 0; i < bitmap_len_; i++) {
            //copy item when it is valid
            if( 1 == test_bit(i, bitmap2) ) {
                ap_entry2[i] = ap_entry_[i];
            }
        }
 
        delete[] bitmap_;
        delete[] ap_entry_;

        bitmap_   = bitmap2;
        ap_entry_ = ap_entry2;
        
        bitmap_len_ = bitmap_len2;

        return true;
    }
    
    Iterator begin() const 
    {
        return Iterator(bitmap_,ap_entry_,0,bitmap_len_);
    }

    Iterator end() const
    {
        return Iterator(bitmap_,ap_entry_,bitmap_len_,bitmap_len_);
    }
    
    bool not_init() const {return NULL == ap_entry_; }

    bool empty() const {return 0 == n_item_;}

    size_t size() const {return n_item_;}

    size_t bitmap_len() { return bitmap_len_; }

    size_t mem()
    {
        return sizeof(*this) + sizeof(Pair) * bitmap_len
            + sizeof(unsigned long) * BITS_TO_LONGS(bitmap_len_);
    }
    
    void to_string (StringWriter& sw) const
    {
        sw  << "BitwiseMap{"     << desc_
            << "bitmap_len  = " << bitmap_len_
            << "element num = " << n_item_;

        sw << "}";
    }

    friend std::ostream& operator<< (std::ostream& os, const BitwiseMap& cbs)
    {
        int len     = cbs.bitmap_len();
        int el_num  = cbs.size();
        
        return os << c_show(BitwiseMap)
                  << "bitmap len = "  << len
                  << "element num = " << el_num;
    }

    std::string desc() const { return desc;  }

    BitmapType get_bitmap() const {return bitmap_;}

    Item* get_array() const {return ap_entry_;}
    
private:    
    size_t n_item_;                             // number of items
    size_t bitmap_len_;                         // length of bitmap 
    u_int item_size_;                           // size of item
    BitmapType bitmap_;                         // bitmap to indicate whether the 
                                                // corresponding element in ap_entry_ is valid
    Pair* ap_entry_;                            // array of items
    std::string desc_;
    
};

template <class _Item,
          class _GetKey,
          class _ComputeKey  = Hash<TCAP(ReturnType, _GetKey, _Item)>,
          class _Equal = Equal<TCAP(ReturnType, _GetKey, _Item)> >
class BitwiseSet {
template <class _Map> friend class BitwiseIterator;

public:
    typedef TCAP(ReturnType, _GetKey, _Item) Key;
    typedef _Item Item;
    typedef _Item* Pointer;
    typedef _Item& Reference;
    typedef const _Item& ConstReference;
    typedef BitwiseIterator<BitwiseSet> Iterator;
    typedef unsigned long* BitmapType;

    BitwiseSet ()
        : n_item_(0)
        , bitmap_len_(CH_DEFAULT_BITMAP_LEN)
        , item_size_(0)
        , bitmap_(NULL)
        , ap_entry_(NULL)
        {}
    
    ~BitwiseSet()
    {
        if( NULL != bitmap_) {
            delete[] bitmap_;
        }
        if( NULL != ap_entry_) {
            delete[] ap_entry_;
        }
    }
        
    int init (size_t bitmap_len) {
        
        if(bitmap_len > CH_MAX_BITMAP_LEN) {
            ST_WARN("too large length to init bitmap,bitmap_len is %lu, max len is %lu",
                    bitmap_len,CH_MAX_BITMAP_LEN);
            return ECONFLICT;
        }
        
        std::ostringstream oss;
        oss << c_show(Key) << "->" << c_show(_Item);

        desc_ = oss.str(); 

        if ( !not_init()  ) {
            ST_FATAL("%s is already initialized", desc_.c_str());
            return ECONFLICT;
        }

        item_size_ = sizeof(Item);
        n_item_    = 0;
        
        bitmap_len_ = bitmap_len < CH_MIN_BITMAP_LEN ? CH_MIN_BITMAP_LEN : bitmap_len;

        bitmap_ = new (std::nothrow) unsigned long[BITS_TO_LONGS(bitmap_len_)];
        if( NULL == bitmap_) {
            ST_FATAL("Fail to new bit_map_");

            ap_entry_ = NULL;
            return ENOMEM;
        }
       
        bitmap_zero(bitmap_, bitmap_len_); 
        
        ap_entry_ = ST_NEW_ARRAY(Item, bitmap_len_);
        if (NULL == ap_entry_) {
            ST_FATAL("Fail to new ap_entry_");

            delete[] bitmap_;
            bitmap_ = NULL;
            return ENOMEM;
        }
        
        return 0;
    }

    BitwiseSet(const BitwiseSet& rhs)
    {
        bitmap_     = NULL;
        ap_entry_   = NULL;
        n_item_     = 0;
        bitmap_len_ = CH_DEFAULT_BITMAP_LEN;
        item_size_  = 0;
 
        if ( rhs.not_init()  ) {
            ST_FATAL("source is not initialized");
            // and we keep ourself uninitialized
            return;
        }
    
        bitmap_ = new (std::nothrow) unsigned long[BITS_TO_LONGS(rhs.bitmap_len_)];
        if( NULL == bitmap_) {
            ST_FATAL("Failed to new bitmap_");

            ap_entry_ = NULL;
            return ;
        }
    
        ap_entry_ = ST_NEW_ARRAY(Item,rhs.bitmap_len_);
        if( NULL == ap_entry_) {
            ST_FATAL("Failed to new ap_entry_");

            delete[] bitmap_;
            bitmap_ = NULL;
            return;
        }

        bitmap_zero(bitmap_, rhs.bitmap_len_);
        bitmap_or(bitmap_,bitmap_,rhs.bitmap_,rhs.bitmap_len_);
    
 
        for(size_t i = 0; i < rhs.bitmap_len_; i++) {
            ap_entry_[i] = rhs.ap_entry_[i];
        }

        n_item_     = rhs.n_item_;
        bitmap_len_ = rhs.bitmap_len_;    
        item_size_  = rhs.item_size_;
        desc_ = rhs.desc_;
    }
 
    BitwiseSet& operator= (const BitwiseSet& rhs)
    {
        if(&rhs == this) {
            return *this;
        }

        if (not_init()) {
        //just call copy-constructor
            ST_NEW_ON(this, BitwiseSet, rhs);
            return *this;
        }
                                         
        if (rhs.not_init()) {  // source is not initialized 
            delete[] bitmap_;
            delete[] ap_entry_;
            this->bitmap_   = NULL;
            this->ap_entry_ = NULL;
            return *this;
        }
        
   
        if ( bitmap_len_ != rhs.bitmap_len_) {
            
            delete[] bitmap_;
            delete[] ap_entry_; 
            
            bitmap_ = new (std::nothrow) unsigned long[BITS_TO_LONGS(rhs.bitmap_len_)];
            if( NULL == bitmap_) {
                ST_FATAL("Failed to new bitmap_");

                ap_entry_ = NULL;
                return *this;
            }
       
            ap_entry_ = ST_NEW_ARRAY(Item,rhs.bitmap_len_);
            if( NULL == ap_entry_) {
                ST_FATAL("Failed to new ap_entry_");

                delete[] bitmap_;
                bitmap_ = NULL;
                return *this;
            }
        }
        
        bitmap_zero(bitmap_, rhs.bitmap_len_);
        bitmap_or(bitmap_,bitmap_,rhs.bitmap_,rhs.bitmap_len_);
    
 
        for(size_t i = 0; i < rhs.bitmap_len_; i++) {
            if( 1 == test_bit(i,bitmap_) ) {
                ap_entry_[i] = rhs.ap_entry_[i];
            }
        }

        n_item_     = rhs.n_item_;
        bitmap_len_ = rhs.bitmap_len_;    
        item_size_  = rhs.item_size_;
        desc_       = rhs.desc_;

        return *this;
    }


    Pointer insert(const Item& item)
    {
        const Key key = _GetKey()(item);    

        u_int bm_key  = _ComputeKey()(key); 
       
        //if bm_key > current bitmap len, resize is needed
        if(bm_key >= bitmap_len_) {

            int bitmap_len2 = (bm_key * 2) < CH_MAX_BITMAP_LEN ? bm_key * 2 : bm_key;
            
            if(false == resize(bitmap_len2 + 1) ) {
                return NULL;
            }
        }

        //no equal item
        if (0 == test_bit(bm_key, bitmap_) ) {
            n_item_ ++;;
        }

        set_bit(bm_key,bitmap_);
        ap_entry_[bm_key] = item;

        return &ap_entry_[bm_key];
    }

    bool erase(const Key& key) 
    {        
        u_int bm_key  = _ComputeKey()(key); 

        //earse out of boundary
        if(bm_key >= bitmap_len_ ){
            return false;
        }

        //no equal item
        if (0 == test_bit(bm_key, bitmap_) ) {
            return false;
        }

        clear_bit(bm_key,bitmap_);

        n_item_ --;
        
        return true;
    }
    
    bool erase_by_item (const Item& item)
    {
        const Key key = _GetKey()(item);    
        return erase( (_GetKey()(item)) );

    }

    void clear()
    {
        bitmap_zero(bitmap_,bitmap_len_); 
        n_item_ = 0;
    } 

    Pointer seek(const Key& key) const
    {
        u_int bm_key  = _ComputeKey()(key); 
        //out of boundary check
        if(bm_key >= bitmap_len_) {
            return NULL;
        }

        //no equal item
        if (0 == test_bit(bm_key, bitmap_) ) {
            return NULL;
        }

        return &ap_entry_[bm_key];
 
    }

    //Iterator find(const Key& key) const ();
    
    Pointer seek_by_item(const Item& item) const
    {
        return seek( _GetKey()(item) );
    }
   
    bool resize(size_t bitmap_len2)
    {
        if(bitmap_len_ == bitmap_len2) {
            return true;
        }

        if(bitmap_len2 > CH_MAX_BITMAP_LEN) {
            ST_WARN("resize bitmap, size is greater than MAX_LEN,\
                    bitmap_len2 is %lu, max len is %lu",
                    bitmap_len2,CH_MAX_BITMAP_LEN);
            return false;
        }
         //resize to small size, return false
        if(bitmap_len2 < bitmap_len_) {
            ST_WARN("resize bitmap, size is less than current bitmap len,\
                    bitmap_len2 is %lu,bitmap_len_ is %lu",
                    bitmap_len2,bitmap_len_);
            return false;
        }
        
        unsigned long *bitmap2;                  
        Item* ap_entry2;                      
        bitmap2 = new (std::nothrow) unsigned long[BITS_TO_LONGS(bitmap_len2)];
        
        if( NULL == bitmap2) {
            ST_FATAL("resize bitmap,fail to new bit_map2");
            return false;
        }
       
        
        ap_entry2 = ST_NEW_ARRAY(Item, bitmap_len2);
        if (NULL == ap_entry2) {
            ST_FATAL("resize bitmap,fail to new ap_entry2");
            delete[] bitmap2;
            return false;
        }
        
        //copy bitmap
        bitmap_zero(bitmap2, bitmap_len2); 
        bitmap_or(bitmap2,bitmap_,bitmap2,bitmap_len_);
     
        //copy item array
        for(size_t i = 0; i < bitmap_len_; i++) {
            //copy item when it is valid
            if( 1 == test_bit(i, bitmap2) ) {
                ap_entry2[i] = ap_entry_[i];
            }
        }
 
        delete[] bitmap_;
        delete[] ap_entry_;

        bitmap_   = bitmap2;
        ap_entry_ = ap_entry2;
        
        bitmap_len_ = bitmap_len2;

        return true;
    }
    
    Iterator begin() const 
    {
        return Iterator(bitmap_,ap_entry_,0,bitmap_len_);
    }

    Iterator end() const
    {
        return Iterator(bitmap_,ap_entry_,bitmap_len_ , bitmap_len_);

    }
    
    bool not_init() const {return NULL == ap_entry_; }

    bool empty() const {return 0 == n_item_;}

    size_t size() const {return n_item_;}

    size_t bitmap_len() { return bitmap_len_; }

    size_t mem()
    {
        return sizeof(*this) + sizeof(Item) * bitmap_len
            + sizeof(unsigned long) * BITS_TO_LONGS(bitmap_len_);
    }
    
    void to_string (StringWriter& sw) const
    {
        sw  << "BitwiseSet{"  << desc_
            << "bitmap_len  = " << bitmap_len_
            << "element num = " << n_item_;

        sw << "}";
    }

    friend std::ostream& operator<< (std::ostream& os, const BitwiseSet& cbs)
    {
        int len     = cbs.bitmap_len();
        int el_num  = cbs.size();
        
        return os << c_show(BitwiseSet)
                  << "bitmap len = "  << len
                  << "element num = " << el_num;
    }

    std::string desc() const { return desc;  }

    BitmapType get_bitmap() const {return bitmap_;}

    Item* get_array() const {return ap_entry_;}
    
private:    
    size_t n_item_;                             // number of items
    size_t bitmap_len_;                         // len of bitmap 
    u_int item_size_;                           // size of item
    BitmapType bitmap_;                         // bitmap to indicate whether the 
                                                // corresponding element in ap_entry_ is valid
    Item* ap_entry_;                            // array of items
    std::string desc_;
    
};

}  // namespace st

#endif  //_ST_BITMAP_H_

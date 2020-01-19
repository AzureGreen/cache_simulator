/**
 * File              : lirs.h
 * Author            : orglanss <orglanss@gmail.com>
 * Date              : 27.08.2017
 * Last Modified Date: 30.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_LIRS_H
#define CACHE_LIRS_H

#include "cache.h"

#include <unordered_map>
#include <cstddef>
#include "include/defs.h"

template<class K>
class LIRSCache : public Cache<K> {
 public:
  enum ResidentInCache { IN = true, OUT = false }; // to judge weather the node is in cache
  enum LIROrHIR { LIR = true, HIR = false };
  struct LIRSnode
  {
    K  key;
    size_t size;
    ResidentInCache flag1;
    LIROrHIR flag2;
    LIRSnode* s_prev;
    LIRSnode* s_next;
    LIRSnode* q_prev;
    LIRSnode* q_next;
    LIRSnode() {}
    LIRSnode(K _key, size_t _size, ResidentInCache _flag1, LIROrHIR _flag2):
      key(_key), size(_size), flag1(_flag1), flag2(_flag2){}
  };


  typedef LIRSnode* LIRSptr;
 private:
  LIRSptr s_head;
  LIRSptr s_tail;
  LIRSptr q_head; // hir stack head
  LIRSptr q_tail; // hir stack tail
  std::unordered_map<K, LIRSptr > cache_stack;
  unsigned long long int max_q_size;
  unsigned long long int q_size;
  typename std::unordered_map<K, LIRSptr >::iterator find_res;
  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;
  using Cache<K>::chunksize;

 public:
  LIRSCache(size_t capacity)
    : Cache<K>(LIRS, capacity) {
    q_size = 0u;
    max_q_size = (unsigned long long int)(capacity * constant::Q_SIZE_RATIO);
    //max_s_size = size - max_q_size;
    s_head = new LIRSnode;
    s_tail = new LIRSnode;
    s_head->s_prev = s_tail->s_next = NULL;
    s_head->q_next = s_head->q_prev = NULL;
    s_tail->q_next = s_tail->q_prev = NULL;
    s_head->s_next = s_tail;
    s_tail->s_prev = s_head;
    q_head = new LIRSnode;
    q_tail = new LIRSnode;
    q_head->q_prev = q_tail->q_next = NULL;
    q_head->s_next = q_head->s_prev = NULL;
    q_tail->s_next = q_tail->s_prev = NULL;
    q_head->q_next = q_tail;
    q_tail->q_prev = q_head;
  }

  ~LIRSCache() {
    auto it=cache_stack.begin();
    while(it!=cache_stack.end())
    {
      delete it->second;
      cache_stack.erase(it);
      it=cache_stack.begin();
    }
    delete s_head;
    delete s_tail;
    delete q_head;
    delete q_tail;
  }

  bool get(K key,size_t &size) {
    find_res = cache_stack.find(key);
    if(find_res != cache_stack.end()){
      /* Access a LIR block */
      LIRSptr node = find_res->second;
      if (node->flag2){
        size = node->size;
        detach(node, true);
        attach(node, true);
        ensure_bottom_lir();
        return true;
      }
      else{/* node in HIR, but be uncertain in cache! could affect cache! */
        if (node->flag1){/* Access a HIR Resident block (a Hit), doesn't affect cache */
          detach(node, false);
          size = node->size;
          if (node->s_next){
            detach(node, true);
            node->flag2 = LIR;/* transform to LIR file */
            q_size -= chunksize(node->size);
            attach(node, true);  
            demote();					
          }
          else{
            attach(node,false);
            attach(node, true);  
          }                                    
          return true;
        }
        else 
          // Access a HIR Non-Resident block (a miss), process at insert
          // procedure
          return false;
      }
    }
    return false;
  }

  bool insert(K key, size_t size) {
    size_t chunk_size = chunksize(size);
    if (find_res != cache_stack.end()){
      /* Access a Non-Resident HIR block (a Miss), affect cache! */
      LIRSptr node = find_res->second;

      detach(node,true);
      node->flag1 = IN;
      node->flag2 = LIR;					
      node->size = size;
      /* insert and evict operate follow the global function */
      while (cache_size_ + chunk_size > cache_capacity_)
      {
        evict();
      }
      attach(node,true);
      cache_size_ += chunk_size;
    }
    else
    {
      while (cache_size_ + chunk_size > cache_capacity_)
      {
        evict();
      }
      LIRSptr temp_node = new LIRSnode(key, size, IN, HIR);
      cache_stack[key] = temp_node;
      attach(temp_node, false);
      attach(temp_node, true);
      q_size += chunk_size;
      cache_size_ += chunk_size;

    }		
    return true;
  }	

  void evict() {
    demote();
    LIRSptr temp_node = q_tail->q_prev;
    size_t chunk_size = chunksize(temp_node->size);
    q_size -= chunk_size;
    cache_size_ -= chunk_size;
    detach(temp_node, false);
    if (temp_node->s_next!=NULL){
      temp_node->flag1=OUT;
      temp_node->flag2=HIR;
    }
    else
    {
      auto it = cache_stack.find(temp_node->key);
      cache_stack.erase(it);
      delete temp_node;			
    }		
  }
  unsigned long long int get_size()
  {
    return cache_size_;
  }	
 private:
  /* stack_flag: true means execute stack s, or stack q */
  void detach(LIRSptr node, bool stack_flag) {
    if (stack_flag) {
      node->s_prev->s_next = node->s_next;
      node->s_next->s_prev = node->s_prev;
      node->s_next = node->s_prev = NULL;
    } else {
      node->q_prev->q_next = node->q_next;
      node->q_next->q_prev = node->q_prev;
      node->q_next = node->q_prev = NULL;
    }
  }

  void attach(LIRSptr node, bool stack_flag) {
    if (stack_flag) /* attach in stack s */
    {
      node->s_next = s_head->s_next;
      node->s_prev = s_head;
      s_head->s_next = node;
      node->s_next->s_prev = node;
    }
    else /* attach in stack q */
    {
      node->q_next = q_head->q_next;
      node->q_prev = q_head;
      q_head->q_next = node;
      node->q_next->q_prev = node;
    }
  }

  void demote() {
    LIRSptr node;
    while ((s_tail->s_prev!=s_head) && 
        ((cache_size_ - q_size > cache_capacity_ - max_q_size)||q_tail->q_prev==q_head))
    {
      node = s_tail->s_prev;
      node->flag2 = HIR;
      q_size += chunksize(node->size);
      attach(node, false);
      ensure_bottom_lir();
    }
  }

  void ensure_bottom_lir() {
    LIRSptr node;
    while(s_tail->s_prev!=s_head&&!s_tail->s_prev->flag2)
    {
      node = s_tail->s_prev;
      detach(node, true);
      if(node->q_next==NULL)
      {
        auto it = cache_stack.find(node->key);
        cache_stack.erase(it);				
        delete node;	
      }
    }
  }	
};

#endif

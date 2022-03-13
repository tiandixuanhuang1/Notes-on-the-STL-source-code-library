/*
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_SLIST_H
#define __SGI_STL_INTERNAL_SLIST_H


__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

struct __slist_node_base                                                   // slist_node_base 数据只有一个指针（指向下一个节点）
{
  __slist_node_base* next;
};

inline __slist_node_base* __slist_make_link(__slist_node_base* prev_node,
                                            __slist_node_base* new_node)
{											                               // 插入一个节点new_node，该节点在prev_node 之后。  
  new_node->next = prev_node->next;
  prev_node->next = new_node;
  return new_node;
}

inline __slist_node_base* __slist_previous(__slist_node_base* head,
                                           const __slist_node_base* node)
{													                                                   	// 找到 node 的前一个节点。
  while (head && head->next != node)
    head = head->next;
  return head;
}

inline const __slist_node_base* __slist_previous(const __slist_node_base* head,
                                                 const __slist_node_base* node)
{                                                                            // const 版本 （语法需要好好看看， 这里有点不懂）
  while (head && head->next != node)
    head = head->next;
  return head;
}

inline void __slist_splice_after(__slist_node_base* pos,					// 将[before_first+1, before_last+1) 部分弄到pos之后
                                 __slist_node_base* before_first,           // 节点类型为 __slist_node_base*
                                 __slist_node_base* before_last)
{
  if (pos != before_first && pos != before_last) {
    __slist_node_base* first = before_first->next;
    __slist_node_base* after = pos->next;
    before_first->next = before_last->next;
    pos->next = first;
    before_last->next = after;
  }
}

inline __slist_node_base* __slist_reverse(__slist_node_base* node)
{
  __slist_node_base* result = node;
  node = node->next;
  result->next = 0;
  while(node) {
    __slist_node_base* next = node->next;
    node->next = result;
    result = node;
    node = next;
  }
  return result;
}

template <class T>
struct __slist_node : public __slist_node_base        // 在 slist_node_base 的基础上加入 数据data。
{
  T data;
};

struct __slist_iterator_base                           // slist_iterator_base可以看成是指向 slist_node_base 指针的究极无敌加强版，增加了许多有用的方法或属性。
{
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef forward_iterator_tag iterator_category;

  __slist_node_base* node;

	//构造函数
  __slist_iterator_base(__slist_node_base* x) : node(x) {}
  
  void incr() { node = node->next; }        // 转到本节点的下一个节点。

  bool operator==(const __slist_iterator_base& x) const { // 两个迭代器相等的条件
    return node == x.node;
  }
  bool operator!=(const __slist_iterator_base& x) const { // 不等的条件
    return node != x.node;
  }
};

template <class T, class Ref, class Ptr>
struct __slist_iterator : public __slist_iterator_base
{
  typedef __slist_iterator<T, T&, T*>             iterator;
  typedef __slist_iterator<T, const T&, const T*> const_iterator;
  typedef __slist_iterator<T, Ref, Ptr>           self;

  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef __slist_node<T> list_node;										//

  __slist_iterator(list_node* x) : __slist_iterator_base(x) {}              // 三个构造函数
  __slist_iterator() : __slist_iterator_base(0) {}
  __slist_iterator(const iterator& x) : __slist_iterator_base(x.node) {}

  reference operator*() const { return ((list_node*) node)->data; }				// 模拟指针的 *操作
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }							// 当使用()->obj时，可以理解为，()->()->obj。 
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  self& operator++()                                                            // 前置++的重载
  {
    incr();
    return *this;
  }
  self operator++(int)															// 后置 ++ 的重载。
  {
    self tmp = *this;
    incr();
    return tmp;
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION
																			// 下面的这段代码为 萃取迭代器的特性。
inline ptrdiff_t*
distance_type(const __slist_iterator_base&)
{
  return 0;
}

inline forward_iterator_tag
iterator_category(const __slist_iterator_base&)
{
  return forward_iterator_tag();
}

template <class T, class Ref, class Ptr> 
inline T* 
value_type(const __slist_iterator<T, Ref, Ptr>&) {
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

inline size_t __slist_size(__slist_node_base* node)                                   // 得到节点node到最尾部的距离。
{
  size_t result = 0;
  for ( ; node != 0; node = node->next)
    ++result;
  return result;
}

template <class T, class Alloc = alloc>                                           // 进入 slist 主容器。
class slist
{
public:
  typedef T 									  value_type;
  typedef value_type* 							  pointer;
  typedef const value_type* 					  const_pointer;
  typedef value_type& 							  reference;
  typedef const value_type& 					  const_reference;
  typedef size_t 								  size_type;
  typedef ptrdiff_t 							  difference_type;

  typedef __slist_iterator<T, T&, T*>             iterator;
  typedef __slist_iterator<T, const T&, const T*> const_iterator;

private:
  typedef __slist_node<T> 						  list_node;
  typedef __slist_node_base 					  list_node_base;
  typedef __slist_iterator_base 				  iterator_base;
  typedef simple_alloc<list_node, Alloc> 		  list_node_allocator;   // 分配器

  static list_node* create_node(const value_type& x) {					//	创建一个节点。
    list_node* node = list_node_allocator::allocate();
    __STL_TRY {
      construct(&node->data, x);
      node->next = 0;
    }
    __STL_UNWIND(list_node_allocator::deallocate(node));
    return node;
  }
  
  static void destroy_node(list_node* node) {							// 销毁一个节点，首先对它的data尽心析构，然后再释放节点所占的内存。
    destroy(&node->data);
    list_node_allocator::deallocate(node);
  }

  void fill_initialize(size_type n, const value_type& x) {				// 这个函数应该是slist构造时用到的。
    head.next = 0;
    __STL_TRY {
      _insert_after_fill(&head, n, x); // 实际调用 _insert_after_fill 函数
    }
    __STL_UNWIND(clear());
  }    

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void range_initialize(InputIterator first, InputIterator last) {     // 将其他容器的 [first, last) 用以初始化本slist；
    head.next = 0; // 首先为确保万无一失，head.next = 0;
    __STL_TRY {
      _insert_after_range(&head, first, last);
    }
    __STL_UNWIND(clear());
  }
#else /* __STL_MEMBER_TEMPLATES */
  void range_initialize(const value_type* first, const value_type* last) { // 下面是关于range_initialize 的其他版本。
    head.next = 0;
    __STL_TRY {
      _insert_after_range(&head, first, last);
    }
    __STL_UNWIND(clear());
  }
  void range_initialize(const_iterator first, const_iterator last) {
    head.next = 0;
    __STL_TRY {
      _insert_after_range(&head, first, last);
    }
    __STL_UNWIND(clear());
  }
#endif /* __STL_MEMBER_TEMPLATES */

private:
  list_node_base head;                                                  // 本 slist 所维护的数据————头节点（空）

public:
  slist() { head.next = 0; }											// 最简单的构造函数

  slist(size_type n, const value_type& x) { fill_initialize(n, x); }    // 使用n个x来初始化一个slist，下面几个和这类似
  slist(int n, const value_type& x) { fill_initialize(n, x); }
  slist(long n, const value_type& x) { fill_initialize(n, x); }
  explicit slist(size_type n) { fill_initialize(n, value_type()); }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  slist(InputIterator first, InputIterator last) {						// 使用 range_initialize( , ) 用来构造slist。
    range_initialize(first, last);
  }

#else /* __STL_MEMBER_TEMPLATES */
  slist(const_iterator first, const_iterator last) {
    range_initialize(first, last);
  }
  slist(const value_type* first, const value_type* last) {
    range_initialize(first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */

  slist(const slist& L) { range_initialize(L.begin(), L.end()); }

  slist& operator= (const slist& L);									// 重载赋值操作符 =

  ~slist() { clear(); }													// 析构函数，清空链表

public:

  iterator begin() { return iterator((list_node*)head.next); }			// 返回首节点的迭代器（head的下一个节点）
  const_iterator begin() const { return const_iterator((list_node*)head.next);}

  iterator end() { return iterator(0); }								// 返回尾节点的迭代器（判断slist的节点是否为最后一个节点关键在于该节点的next指针指向是否为空。）
  const_iterator end() const { return const_iterator(0); }

  size_type size() const { return __slist_size(head.next); }			// 得到 slist 的长度
  size_type max_size() const { return size_type(-1); }					// 理论上的最大长度。
  bool empty() const { return head.next == 0; }							// 是否为空

  void swap(slist& L)													// 交换两个slist，其实就是交换head.next。
  {
    list_node_base* tmp = head.next;
    head.next = L.head.next;
    L.head.next = tmp;
  }

public:
  friend bool operator== __STL_NULL_TMPL_ARGS(const slist<T, Alloc>& L1, 	// 判断两个slist是否相同
                                              const slist<T, Alloc>& L2);
public:

  reference front() { return ((list_node*) head.next)->data; }			// 返回头节点的data
  const_reference front() const { return ((list_node*) head.next)->data; }
  void push_front(const value_type& x)   {								// 插入头节点，使用 __slist_make_link()函数，插入的节点位置在head之后。
    __slist_make_link(&head, create_node(x));
  }
  void pop_front() {													// 将头节点删除。
    list_node* node = (list_node*) head.next;
    head.next = node->next;
    destroy_node(node); // destroy it
  }

  iterator previous(const_iterator pos) {								// 找到 pos 的前一个节点。（iterator 版本）
    return iterator((list_node*) __slist_previous(&head, pos.node));
  }
  const_iterator previous(const_iterator pos) const {                    // 找到 pos 的前一个节点。（const_iterator 版本）
    return const_iterator((list_node*) __slist_previous(&head, pos.node));
  }

private:
  list_node* _insert_after(list_node_base* pos, const value_type& x) {    // 在 位置 pos 之后插入值为x的节点。（调用 __slist_make_link() 函数）
    return (list_node*) (__slist_make_link(pos, create_node(x)));
  }

  void _insert_after_fill(list_node_base* pos,							 // 在位置pos 之后插入n个值为x的节点。
                          size_type n, const value_type& x) {
    for (size_type i = 0; i < n; ++i)
      pos = __slist_make_link(pos, create_node(x));
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InIter>
  void _insert_after_range(list_node_base* pos, InIter first, InIter last) { // 在位置 pos 之后加入 [first, last) 类似的节点。（InIter 模板）
    while (first != last) {
      pos = __slist_make_link(pos, create_node(*first));
      ++first;
    }
  }
#else /* __STL_MEMBER_TEMPLATES */
  void _insert_after_range(list_node_base* pos,                              // 上一个模板的偏特化版本，const_iterator。
                           const_iterator first, const_iterator last) {
    while (first != last) {
      pos = __slist_make_link(pos, create_node(*first));
      ++first;
    }
  }
  
  void _insert_after_range(list_node_base* pos,                               // 上一个模板的偏特化版本（指针版本） const value_type*
                           const value_type* first, const value_type* last) {
    while (first != last) {
      pos = __slist_make_link(pos, create_node(*first));
      ++first;
    }
  }
#endif /* __STL_MEMBER_TEMPLATES */

                                    // 下面的几个函数，返回的是list_node_base*
  list_node_base* erase_after(list_node_base* pos) {                           // 删除pos位置后的节点。
    list_node* next = (list_node*) (pos->next); // 要删除的节点
    list_node_base* next_next = next->next; // 找到接 pos 盘的节点
    pos->next = next_next;
    destroy_node(next);
    return next_next;
  }
   
  list_node_base* erase_after(list_node_base* before_first,						// 删除 (before_first, last_node) 之间的节点。
                              list_node_base* last_node) {
    list_node* cur = (list_node*) (before_first->next);
    while (cur != last_node) {
      list_node* tmp = cur;
      cur = (list_node*) cur->next;
      destroy_node(tmp);
    }
    before_first->next = last_node;
    return last_node;
  }


public:

  iterator insert_after(iterator pos, const value_type& x) {                   // 在pos之后插入值为x的节点。
    return iterator(_insert_after(pos.node, x));
  }

  iterator insert_after(iterator pos) {										 // 在pos之后插入值为 value_type()的节点。
    return insert_after(pos, value_type());
  }

  void insert_after(iterator pos, size_type n, const value_type& x) {        // 在pos之后插入值为x 的n个节点，n(size_type)
    _insert_after_fill(pos.node, n, x);
  }
  void insert_after(iterator pos, int n, const value_type& x) {				// 在pos之后插入值为x 的n个节点，n(int)
    _insert_after_fill(pos.node, (size_type) n, x);
  }
  void insert_after(iterator pos, long n, const value_type& x) {             // 在pos之后插入值为x 的n个节点，n(long)
    _insert_after_fill(pos.node, (size_type) n, x);
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InIter>
  void insert_after(iterator pos, InIter first, InIter last) {              // 在pos之后插入 [first, last)，模板（InIter）
    _insert_after_range(pos.node, first, last);
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert_after(iterator pos, const_iterator first, const_iterator last) {  // 在pos之后插入 [first, last)，偏特化(const_iterator)
    _insert_after_range(pos.node, first, last);
  }
  void insert_after(iterator pos,                                            // 在pos之后插入 [first, last)，偏特化(const value_type*)
                    const value_type* first, const value_type* last) {
    _insert_after_range(pos.node, first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */

  iterator insert(iterator pos, const value_type& x) {                       // 在pos位置插入值为x的节点
    return iterator(_insert_after(__slist_previous(&head, pos.node), x));    // 由于在pos位置插入，所以需要先得到pos前一个节点的信息。
  }

  iterator insert(iterator pos) {											// 在pos位置插入值为 value_type() 的节点
    return iterator(_insert_after(__slist_previous(&head, pos.node),
                                  value_type()));
  }

  void insert(iterator pos, size_type n, const value_type& x) {             // 在 pos位置插入n个值为x的节点。  n(size_type)
    _insert_after_fill(__slist_previous(&head, pos.node), n, x);             // 调用 __slist_previous()函数以得到位置
  } 
  void insert(iterator pos, int n, const value_type& x) {                    // n(int)
    _insert_after_fill(__slist_previous(&head, pos.node), (size_type) n, x);
  } 
  void insert(iterator pos, long n, const value_type& x) {                    // n(long)
    _insert_after_fill(__slist_previous(&head, pos.node), (size_type) n, x);
  } 
    
#ifdef __STL_MEMBER_TEMPLATES
  template <class InIter>
  void insert(iterator pos, InIter first, InIter last) {                      // 在pos位置插入 [first, last)的节点，模板（InIter）
    _insert_after_range(__slist_previous(&head, pos.node), first, last);
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator pos, const_iterator first, const_iterator last) {       // 在pos位置插入 [first, last)的节点，偏特化(const_iterator)
    _insert_after_range(__slist_previous(&head, pos.node), first, last);
  }
  void insert(iterator pos, const value_type* first, const value_type* last) { // 在pos位置插入 [first, last)的节点，偏特化(const value_type*)
    _insert_after_range(__slist_previous(&head, pos.node), first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */


public:
  iterator erase_after(iterator pos) {											// 下面的erase函数和之前的erase函数类似，只是返回的类型不同
    return iterator((list_node*)erase_after(pos.node));                          // 这里返回的为 iterator
  }
  iterator erase_after(iterator before_first, iterator last) {
    return iterator((list_node*)erase_after(before_first.node, last.node));
  }

  iterator erase(iterator pos) {
    return (list_node*) erase_after(__slist_previous(&head, pos.node));
  }
  iterator erase(iterator first, iterator last) {
    return (list_node*) erase_after(__slist_previous(&head, first.node),
                                    last.node);
  }

  void resize(size_type new_size, const T& x);									// 对 slist 进行重新确定大小。
  void resize(size_type new_size) { resize(new_size, T()); }                     // 于上一个函数类似
  void clear() { erase_after(&head, 0); }									    // 清空slist

public:
  // Moves the range [before_first + 1, before_last + 1) to *this,
  //  inserting it immediately after pos.  This is constant time.
  void splice_after(iterator pos, 												// [before_first+1, before_last+1)节点复制到pos位置之后
                    iterator before_first, iterator before_last)                 // 封装了一下，上面的是__slist_node_base*
  {                                                                              // 这里是iterator
    if (before_first != before_last) 
      __slist_splice_after(pos.node, before_first.node, before_last.node);
  }

  // Moves the element that follows prev to *this, inserting it immediately
  //  after pos.  This is constant time.
  void splice_after(iterator pos, iterator prev)								// 将 prev.node->next节点弄到pos之后
  {
    __slist_splice_after(pos.node, prev.node, prev.node->next);
  }


  // 这里函数的名字是 splice，注意与上面的 splice_after 函数名以示区别。
  // Linear in distance(begin(), pos), and linear in L.size().
  void splice(iterator pos, slist& L) {											// [L.head + 1, L.end+1) 装在位置pos上。
    if (L.head.next)
      __slist_splice_after(__slist_previous(&head, pos.node),
                           &L.head,
                           __slist_previous(&L.head, 0));
  }

  // Linear in distance(begin(), pos), and in distance(L.begin(), i).
  void splice(iterator pos, slist& L, iterator i) { 							// 将 silis L中的节点 i，弄到pos位置。 
    __slist_splice_after(__slist_previous(&head, pos.node),
                         __slist_previous(&L.head, i.node),                     // 区间为：[i-1+1, i+1)。
                         i.node);
  }

  // Linear in distance(begin(), pos), in distance(L.begin(), first),
  // and in distance(first, last).
  void splice(iterator pos, slist& L, iterator first, iterator last)            // 将slist L中的 [first-1+1, last-1+1) 弄到pos位置上。
  {
    if (first != last)
      __slist_splice_after(__slist_previous(&head, pos.node),
                           __slist_previous(&L.head, first.node),
                           __slist_previous(first.node, last.node));
  }

public:
  void reverse() { if (head.next) head.next = __slist_reverse(head.next); } // 将slist调转。

  void remove(const T& val);                                                // 移除值为val的节点
  void unique(); 															// 移除连续相同的节点（只保留一个）
  void merge(slist& L);
  void sort();     															// slist 排序。

#ifdef __STL_MEMBER_TEMPLATES                                              // 下面是更高级的函数吧
  template <class Predicate> void remove_if(Predicate pred);
  template <class BinaryPredicate> void unique(BinaryPredicate pred); 
  template <class StrictWeakOrdering> void merge(slist&, StrictWeakOrdering); 
  template <class StrictWeakOrdering> void sort(StrictWeakOrdering comp); 
#endif /* __STL_MEMBER_TEMPLATES */
};
//#######################################################################################################################################
//############################################################    类的主题结束  #########################################################
//#######################################################################################################################################

template <class T, class Alloc>
slist<T, Alloc>& slist<T,Alloc>::operator=(const slist<T, Alloc>& L)   // 对slist 进行赋值。
{
  if (&L != this) { // 首先确保L不为本身
    list_node_base* p1 = &head; //p1 保存head的地址
    list_node* n1 = (list_node*) head.next;   // n1 指向第一个节点
    const list_node* n2 = (const list_node*) L.head.next; // n2指向L的头节点。
    while (n1 && n2) { // n1和n2同时不为空时。
      n1->data = n2->data;  // 对n1赋值（赋的值为n2的data）
      p1 = n1;              // p1 和 n1 指向同一个节点。
      n1 = (list_node*) n1->next; // n1指向p1的下一个节点
      n2 = (const list_node*) n2->next; // n2 指向 n2的下一个节点
    }
	
    if (n2 == 0) // 如果本slist 的长度比 L 长，删除之后的节点；
      erase_after(p1, 0);
    else					// 否则在p1之后处，插入 [n2, 0) 的值
      _insert_after_range(p1,
                          const_iterator((list_node*)n2), const_iterator(0));
  }
  return *this;
} 

template <class T, class Alloc>									// 判断两个slist是否相等
bool operator==(const slist<T, Alloc>& L1, const slist<T, Alloc>& L2)// 判断标准：长度，数值相同。
{
  typedef typename slist<T,Alloc>::list_node list_node;
  list_node* n1 = (list_node*) L1.head.next;
  list_node* n2 = (list_node*) L2.head.next;
  while (n1 && n2 && n1->data == n2->data) {
    n1 = (list_node*) n1->next;
    n2 = (list_node*) n2->next;
  }
  return n1 == 0 && n2 == 0;
}

template <class T, class Alloc>                                  // 两个slist比大小。
inline bool operator<(const slist<T, Alloc>& L1, const slist<T, Alloc>& L2)
{
  return lexicographical_compare(L1.begin(), L1.end(), L2.begin(), L2.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class T, class Alloc>                                  // 两个slist交换。
inline void swap(slist<T, Alloc>& x, slist<T, Alloc>& y) {
  x.swap(y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


template <class T, class Alloc>
void slist<T, Alloc>::resize(size_type len, const T& x)                      // 对slist的大小进行重新分配
{
  list_node_base* cur = &head;                                               
  while (cur->next != 0 && len > 0) {
    --len;
    cur = cur->next;
  }
  if (cur->next)               //若 L.size > len 在 len 之后的元素全部删除。
    erase_after(cur, 0); 
  else                         // 否则，用x填充
    _insert_after_fill(cur, len, x);
}

template <class T, class Alloc>
void slist<T,Alloc>::remove(const T& val)                                      // 删除值为 val 的节点。
{
  list_node_base* cur = &head;
  while (cur && cur->next) {
    if (((list_node*) cur->next)->data == val) // 这里是 cur->next 的值。
      erase_after(cur);                   // 删除的是cur之后的那一个节点。
    else
      cur = cur->next;
  }
}

template <class T, class Alloc> 
void slist<T,Alloc>::unique()                                                   // 删除连续值相同的节点（仅保留一个）
{
  list_node_base* cur = head.next;
  if (cur) {
    while (cur->next) {
      if (((list_node*)cur)->data == ((list_node*)(cur->next))->data)
        erase_after(cur);
      else
        cur = cur->next;
    }
  }
}

template <class T, class Alloc>
void slist<T,Alloc>::merge(slist<T,Alloc>& L)                             // 将slist L 和本slist合并。
{
  list_node_base* n1 = &head;
  while (n1->next && L.head.next) {
    if (((list_node*) L.head.next)->data < ((list_node*) n1->next)->data) 
      __slist_splice_after(n1, &L.head, L.head.next);
    n1 = n1->next;
  }
  if (L.head.next) {
    n1->next = L.head.next;
    L.head.next = 0;
  }
}

template <class T, class Alloc>
void slist<T,Alloc>::sort()                                               // 节点排序。
{
  if (head.next && head.next->next) {
    slist carry;
    slist counter[64];
    int fill = 0;
    while (!empty()) {
      __slist_splice_after(&carry.head, &head, head.next);
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry);
        carry.swap(counter[i]);
        ++i;
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }

    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i-1]);
    this->swap(counter[fill-1]);
  }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class T, class Alloc> 
template <class Predicate> void slist<T,Alloc>::remove_if(Predicate pred)
{
  list_node_base* cur = &head;
  while (cur->next) {
    if (pred(((list_node*) cur->next)->data))
      erase_after(cur);
    else
      cur = cur->next;
  }
}

template <class T, class Alloc> template <class BinaryPredicate> 
void slist<T,Alloc>::unique(BinaryPredicate pred)
{
  list_node* cur = (list_node*) head.next;
  if (cur) {
    while (cur->next) {
      if (pred(((list_node*)cur)->data, ((list_node*)(cur->next))->data))
        erase_after(cur);
      else
        cur = (list_node*) cur->next;
    }
  }
}

template <class T, class Alloc> template <class StrictWeakOrdering>
void slist<T,Alloc>::merge(slist<T,Alloc>& L, StrictWeakOrdering comp)
{
  list_node_base* n1 = &head;
  while (n1->next && L.head.next) {
    if (comp(((list_node*) L.head.next)->data,
             ((list_node*) n1->next)->data))
      __slist_splice_after(n1, &L.head, L.head.next);
    n1 = n1->next;
  }
  if (L.head.next) {
    n1->next = L.head.next;
    L.head.next = 0;
  }
}

template <class T, class Alloc> template <class StrictWeakOrdering> 
void slist<T,Alloc>::sort(StrictWeakOrdering comp)
{
  if (head.next && head.next->next) {
    slist carry;
    slist counter[64];
    int fill = 0;
    while (!empty()) {
      __slist_splice_after(&carry.head, &head, head.next);
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry, comp);
        carry.swap(counter[i]);
        ++i;
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }

    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i-1], comp);
    this->swap(counter[fill-1]);
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE 

#endif /* __SGI_STL_INTERNAL_SLIST_H */

// Local Variables:
// mode:C++
// End:

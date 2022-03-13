/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
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
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_DEQUE_H
#define __SGI_STL_INTERNAL_DEQUE_H

/* Class invariants:
 *  For any nonsingular iterator i:
 *    i.node is the address of an element in the map array.  The
 *      contents of i.node is a pointer to the beginning of a node.
 *    i.first == *(i.node) 
 *    i.last  == i.first + node_size
 *    i.cur is a pointer in the range [i.first, i.last).  NOTE:
 *      the implication of this is that i.cur is always a dereferenceable
 *      pointer, even if i is a past-the-end iterator.
 *  Start and Finish are always nonsingular iterators.  NOTE: this means
 *    that an empty deque must have one node, and that a deque
 *    with N elements, where N is the buffer size, must have two nodes.
 *  For every node other than start.node and finish.node, every element
 *    in the node is an initialized object.  If start.node == finish.node,
 *    then [start.cur, finish.cur) are initialized objects, and
 *    the elements outside that range are uninitialized storage.  Otherwise,
 *    [start.cur, start.last) and [finish.first, finish.cur) are initialized
 *    objects, and [start.first, start.cur) and [finish.cur, finish.last)
 *    are uninitialized storage.
 *  [map, map + map_size) is a valid, non-empty range.  
 *  [start.node, finish.node] is a valid range contained within 
 *    [map, map + map_size).  
 *  A pointer in the range [map, map + map_size) points to an allocated
 *    node if and only if the pointer is in the range [start.node, finish.node].
 */


/*
 * In previous versions of deque, node_size was fixed by the 
 * implementation.  In this version, however, users can select
 * the node size.  Deque has three template parameters; the third,
 * a number of type size_t, is the number of elements per node.
 * If the third template parameter is 0 (which is the default), 
 * then deque will use a default node size.
 *
 * The only reason for using an alternate node size is if your application
 * requires a different performance tradeoff than the default.  If,
 * for example, your program contains many deques each of which contains
 * only a few elements, then you might want to save memory (possibly
 * by sacrificing some speed) by using smaller nodes.
 *
 * Unfortunately, some compilers have trouble with non-type template 
 * parameters; stl_config.h defines __STL_NON_TYPE_TMPL_PARAM_BUG if
 * that is the case.  If your compiler is one of them, then you will
 * not be able to use alternate node sizes; you will have to use the
 * default value.
 */

/*
1，指针相减 = （地址1 - 地址2）/sizeof(类型) 
*/



__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

// Note: this function is simply a kludge to work around several compilers'
//  bugs in handling constant expressions.
//注意：这个函数只是为了解决几个编译器在处理常量表达式时出现的错误而做的一个小动作。
inline size_t __deque_buf_size(size_t n, size_t sz)
{
  return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
  // 若 n 为 0 且元素大小sz小于 512，则size_t = 512/sz
  // 若 n 为 0 且元素大小sz大于 512，则size_t 1
  // 若 n 不为 0，则就为 n 
}

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG                                                                                                           // 防止有bug ??
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {                                                     
  typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
  typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T)); } // 静态变量  得到缓冲区的大小。 
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */

template <class T, class Ref, class Ptr>
struct __deque_iterator {                                              // ***关于deque 的迭代器 
  typedef __deque_iterator<T, T&, T*>             iterator;
  typedef __deque_iterator<T, const T&, const T*> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(0, sizeof(T)); }// 缓冲区的大小 buffer_size() 是一个静态函数。 
#endif

  typedef random_access_iterator_tag iterator_category; // (1)迭代器类型 deque为 random_access_iterator_tag 
  typedef T value_type;                                 // (2) 数据类型 
  typedef Ptr pointer;                                  // (3) 指针 
  typedef Ref reference;                                // (4) 引用 
  typedef size_t size_type;                        
  typedef ptrdiff_t difference_type;                    // (5)  距离 
  typedef T** map_pointer;                              // 指针的指针 

  typedef __deque_iterator self;                                                                                                                  // __deque_iterator 为什么不带模板 

  T* cur;                                               // 迭代器中的数据  cur 为当前的位置，first 为当前buffer的头部，last 为当前buffer的尾部，map_pointer为本buffer控制中心的位置 
  T* first;
  T* last;
  map_pointer node;
  /* node指向控制中心中一个节点的执政，控制中心为一
  个元素为指针的 vector，node为一个指向指针的指针 */

  __deque_iterator(T* x, map_pointer y)                     // 有参构造函数，数据为 x（cur为指向x的指针），first为 *y（T*），last为 *y + buffer_size()，node为 y（T**） 
    : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    
  __deque_iterator() : cur(0), first(0), last(0), node(0) {}//默认构造 
  
  __deque_iterator(const iterator& x)                       // 拷贝构造函数 
    : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

  reference operator*() const { return *cur; }              // 解引用 reference = Ref（一般应该为 T&） 
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }     // 返回 *cur的地址 
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  difference_type operator-(const self& x) const {          // 重载函数 -（迭代器的减法），self = __deque_iterator ，认为 x 在 所指元素的前边。
    return difference_type(buffer_size()) * (node - x.node - 1) +
      (cur - first) + (x.last - x.cur);
  }

  self& operator++() {                                     //self = __deque_iterator，++ 为cur指向的位置变换 
     // 切换下一个元素
	 // 如果抵达缓冲区尾端
	 //就跳转至下一个 node(缓冲区)的起点 
    ++cur;
    if (cur == last) {
      set_node(node + 1);                                                                                                                        // set_node()  
	  cur = first;
    }
    return *this; 
  }
  
  self operator++(int)  {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }
  
  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }

  self& operator+=(difference_type n) {              // += 重载 
    difference_type offset = n + (cur - first);
    if (offset >= 0 && offset < difference_type(buffer_size()))
    	//目标位置在同一个缓冲区 
      cur += n;
    else { // 目标区域不在缓冲区 
      difference_type node_offset =
        offset > 0 ? offset / difference_type(buffer_size())
                   : -difference_type((-offset - 1) / buffer_size()) - 1;
      set_node(node + node_offset);
      // 切换到正确的缓冲区域 
      cur = first + (offset - node_offset * difference_type(buffer_size()));
      // 切换到正确的节点位置处 
    }
    return *this;
  }

  self operator+(difference_type n) const {        // +重载（用到了+=） 
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n)              // -=重载 
  { return *this += -n; }
 
  self operator-(difference_type n) const {        // - 重载（用到了-=） 
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const { return *(*this + n); } // []取值函数实现 

  bool operator==(const self& x) const { return cur == x.cur; }// 相等条件，cur相等（也即指向的节点相等） 
  bool operator!=(const self& x) const { return !(*this == x); }// 不相等 
  bool operator<(const self& x) const {                         // < 先node后cur 
    return (node == x.node) ? (cur < x.cur) : (node < x.node);
  }

  void set_node(map_pointer new_node) {                       // 调整 node 只是对现有的buffer做操作（没有凭空增加 buffer） 
    node = new_node;
    first = *new_node;
    last = first + difference_type(buffer_size());
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG

template <class T, class Ref, class Ptr, size_t BufSiz>
inline random_access_iterator_tag                             // 萃取迭代器的iterator category（__deque_iterator<T,Ref,Ptr,BufSiz>） 107
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return random_access_iterator_tag();
}

template <class T, class Ref, class Ptr, size_t BufSiz>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {// 萃取迭代器的value type 
  return 0;
}

template <class T, class Ref, class Ptr, size_t BufSiz>       // 萃取迭代器的distance type 
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}

#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */

template <class T, class Ref, class Ptr>
inline random_access_iterator_tag                          // 萃取迭代器的iterator category （__deque_iterator<T,Ref,Ptr>） 113
iterator_category(const __deque_iterator<T, Ref, Ptr>&) {
  return random_access_iterator_tag();
}

template <class T, class Ref, class Ptr>                   // 萃取迭代器的value type 
inline T* value_type(const __deque_iterator<T, Ref, Ptr>&) { return 0; }

template <class T, class Ref, class Ptr>                   // 萃取迭代器的distance type 
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr>&) {
  return 0;
}

#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// See __deque_buf_size().  The only reason that the default value is 0
//  is as a workaround for bugs in the way that some compilers handle
//  constant expressions.
// 默认值为 0 的唯一原因是为了解决某些编译器处理常量表达式时出现的错误。
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
public:                         // Basic types
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

public:                         // Iterators
#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG
  typedef __deque_iterator<T, T&, T*, BufSiz>              iterator;
  typedef __deque_iterator<T, const T&, const T&, BufSiz>  const_iterator;
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */
  typedef __deque_iterator<T, T&, T*>                      iterator;
  typedef __deque_iterator<T, const T&, const T*>          const_iterator;
#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
                           difference_type>  
          const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator; 
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:                      // Internal typedefs
  typedef pointer* map_pointer;                             // pointer = value_type* 
  typedef simple_alloc<value_type, Alloc> data_allocator;   // date_allocator 中分配 value_type 
  typedef simple_alloc<pointer, Alloc> map_allocator;       // map_allocator 分配 pointer = value_type* 

  static size_type buffer_size() {                          // 得到deque的一个缓冲区的大小 
    return __deque_buf_size(BufSiz, sizeof(value_type));
  }
  static size_type initial_map_size() { return 8; }         // 初始化 map（控制中心的大小）为8 

protected:                      // Data members
  iterator start;                                            // 一个deque所包含 iterator start，finish 
  iterator finish;

  map_pointer map;                                           // 以及 map (指向控制中心)，map_pointer = T** 
  size_type map_size;                                        // size_type = size_t 

public:                         // Basic accessors
  iterator begin() { return start; }                         // 返回 start iterator
  iterator end() { return finish; }                          // 返回finish iterator 下面的同理 
  
  const_iterator begin() const { return start; } 
  const_iterator end() const { return finish; }

  reverse_iterator rbegin() { return reverse_iterator(finish); }
  reverse_iterator rend() { return reverse_iterator(start); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(finish);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(start);
  }

  reference operator[](size_type n) { return start[difference_type(n)]; } // 得到第 n 个迭代器所指的值，deque[n] = start[n] = *(start+n)
  const_reference operator[](size_type n) const {
    return start[difference_type(n)];
  }

  reference front() { return *start; }                        // 返回deque最前面的值 
  reference back() {                                          // 返回deque最后的值 
    iterator tmp = finish;
    --tmp;
    return *tmp;
  }
  const_reference front() const { return *start; }            // const_reference = const value_type& 
  const_reference back() const {
    const_iterator tmp = finish;
    --tmp;
    return *tmp;
  }

  size_type size() const { return finish - start;; }          // 得到deque的大小。- 的重载见150 
  size_type max_size() const { return size_type(-1); }        // 得到deque的理论大小 
  bool empty() const { return finish == start; }              // deque是否为空 

public:                         // Constructor, destructor.
  deque()                                                     // ctor 
    : start(), finish(), map(0), map_size(0) 
  {
    create_map_and_nodes(0);
  }

  deque(const deque& x)                                       // 拷贝构造函数（用 x） 
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(x.size());// x的大小 359 
    __STL_TRY {
      uninitialized_copy(x.begin(), x.end(), start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

  deque(size_type n, const value_type& value)
    : start(), finish(), map(0), map_size(0)                  // 用n个value初始化deque（size_type） 
  {
    fill_initialize(n, value);
  }

  deque(int n, const value_type& value)                       // 用n个value初始化deque（int）
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }
 
  deque(long n, const value_type& value)                      // 用n个value初始化deque（long）
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }

  explicit deque(size_type n)                                 // 不可进行隐式替换 value = value_type() 
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value_type());
  }

#ifdef __STL_MEMBER_TEMPLATES

  template <class InputIterator>
  deque(InputIterator first, InputIterator last)              // [first, last)来初始化 deque （input iterator） 
    : start(), finish(), map(0), map_size(0)
  {
    range_initialize(first, last, iterator_category(first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  deque(const value_type* first, const value_type* last)      // [first, last)来初始化 deque （const value_type*） 
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

  deque(const_iterator first, const_iterator last)           // [first, last)来初始化 deque 
    : start(), finish(), map(0), map_size(0)                 // const_iterator = __deque_iterator<T, const T&, const T&, BufSiz> 
  {                                                          // const_iterator = __deque_iterator<T, const T&, const T&>
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

#endif /* __STL_MEMBER_TEMPLATES */                            // end ctor 

  ~deque() {                                                   // dctor 
    destroy(start, finish);
    destroy_map_and_nodes();
  }

  deque& operator= (const deque& x) {                          // deque的赋值函数
  //  
    const size_type len = size();
    if (&x != this) {// x 不等于本身 (deque)
      if (len >= x.size())
	  // deque.size() >= x.size()，将x.begin()到x.end()部分拷贝到 deque，再把deque之后的删去 
        erase(copy(x.begin(), x.end(), start), finish);
      else {// deque.size < x.size() 
        const_iterator mid = x.begin() + difference_type(len);
        copy(x.begin(), mid, start); //先将x.begin() 到 x.begin() + len的部分拷贝到deque
        insert(finish, mid, x.end()); //再将余下（mid到x.end()）的插入到x之后
      }
    }
    return *this;// 返回自身 deque 
  }        

  void swap(deque& x) {                                        // 两个deque交换（一个deque中仅包含 start，finish，map，map_size） 
    __STD::swap(start, x.start);
    __STD::swap(finish, x.finish);
    __STD::swap(map, x.map);
    __STD::swap(map_size, x.map_size);
  }

public:                         // push_* and pop_*
  
  void push_back(const value_type& t) {                        // 将值为t的节点后插到deque 
    if (finish.cur != finish.last - 1) {
    	// finish所在的buffer 还有剩余的节点可以插入。 
      construct(finish.cur, t);
      ++finish.cur;
    }
    else// finish所在的buffer已经满了 
      push_back_aux(t);                                                                                                                                // push_back_aux() 
  }

  void push_front(const value_type& t) {                       // 将值为t的节点 前插到deque
    if (start.cur != start.first) {
    	// start的当前位置不为start的 first 位置（start所在的buffer还可以装） 
      construct(start.cur - 1, t);
      --start.cur;
    }
    else// start所在的buffer装不下了 
      push_front_aux(t);                                                                                                                               // push_front_aux()
  }

  void pop_back() {                                            // 删除最后一个节点 
    if (finish.cur != finish.first) {
      --finish.cur;
      destroy(finish.cur);
    }
    else// 该情形为 finish 所指的buffer没有一个元素 
      pop_back_aux();                                                                                                                                  // pop_back_aux() 
  }

  void pop_front() {                                           // 删除最前的一个节点 
    if (start.cur != start.last - 1) {
      destroy(start.cur);
      ++start.cur;
    }
    else// 该情形为 start 所指的buffer仅有一个元素
      pop_front_aux();                                                                                                                                  // pop_front_aux() 
  }

public:                         // Insert

  iterator insert(iterator position, const value_type& x) {    // 在position位置插入值为x地节点。 
    if (position.cur == start.cur) {// 若position不是start指向的cur，则前插 
      push_front(x);
      return start;
    }
    else if (position.cur == finish.cur) { // 若position不是finish指向的cur，则后插 
      push_back(x);
      iterator tmp = finish;
      --tmp;
      return tmp;
    }
    else {           // 若要插得位置为deque中间的位置，调用 insert_aux()                                                                                // insert_aux() 
      return insert_aux(position, x);
    }
  }

  iterator insert(iterator position) { return insert(position, value_type()); }// insert()的重载版本 

  void insert(iterator pos, size_type n, const value_type& x); // 在pos位置插入 n(size_type) 个值为x的节点。 

  void insert(iterator pos, int n, const value_type& x) {       // 在pos位置插入 n(int，后面会强制转型为size_type) 个值为x的节点。 
    insert(pos, (size_type) n, x);
  }
  void insert(iterator pos, long n, const value_type& x) {     //在pos位置插入 n(long，后面会强制转型为size_type) 个值为x的节点。 
    insert(pos, (size_type) n, x);
  }

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last) {// 将[fisrt, last)插入到pos位置，迭代器为input iterator 
    insert(pos, first, last, iterator_category(first)); 
	// insert的一个重载函数 
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator pos, const value_type* first, const value_type* last);// 将[fisrt, last)插入到pos位置，迭代器为 从const value_type* 
  void insert(iterator pos, const_iterator first, const_iterator last);// 将[fisrt, last)插入到pos位置，迭代器为 从const_iterator = __deque_iterator<T, const T, const T*>

#endif /* __STL_MEMBER_TEMPLATES */   // insert end 

  void resize(size_type new_size, const value_type& x) {       // resize函数 
    const size_type len = size();
    if (new_size < len) // 若new_size 小于 len 则只取前new_size 个元素 
      erase(start + new_size, finish);
    else // 否则在之后插入new_size - len个值为x的节点 
      insert(finish, new_size - len, x);
  }

  void resize(size_type new_size) { resize(new_size, value_type()); } // resize的重载版本 

public:                         // Erase
  iterator erase(iterator pos) {                                     // 删除位置为pos的节点。 
    iterator next = pos;
    ++next;// next 后移一位(pos的后一位) 
    difference_type index = pos - start;
    // 目标位置pos和start的距离 ——index 
    if (index < (size() >> 1)) {
      // index 更靠近头节点 
      copy_backward(start, pos, next);
      // 移动清除点pos之前的元素。                                                                                                                           // （将[start,pos)移动到next位置 ） 
      pop_front();
      // 最前面的元素删除(pos所指的节点) 
    }
    else {// 更靠近尾节点 
      copy(next, finish, pos);
      // 移动清除点之后的节点（元素） 
      pop_back();
      // 最后面的元素删除。 
    }
    return start + index;// 返回的迭代器指向pos的后一位元素 
  }

  iterator erase(iterator first, iterator last);                   // 删除[first, last)之间的元素 
  void clear();                                                      // 清除deque(删除所有的节点) 

protected:                        // Internal construction/destruction（内置的构造函数和析构函数） 

  void create_map_and_nodes(size_type num_elements);  // 构造 map和创建node。 
  void destroy_map_and_nodes();                       // 析构 map 
  void fill_initialize(size_type n, const value_type& value);                                                                                                                  // 猜是用n个value初始化deque 

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void range_initialize(InputIterator first, InputIterator last,                                                                                                                 // 猜是用[first, last)初始化  input iterator版本 
                        input_iterator_tag);

  template <class ForwardIterator>
  void range_initialize(ForwardIterator first, ForwardIterator last, // forward iterator 版本 
                        forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

protected:                        // Internal push_* and pop_* 关于push_back 和 push_front 的辅助函数 

  void push_back_aux(const value_type& t);
  void push_front_aux(const value_type& t);
  void pop_back_aux();
  void pop_front_aux();

protected:                        // Internal insert functions

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last, // 将[first, last)插入到pos位置，针对input_iterator_tag
              input_iterator_tag);

  template <class ForwardIterator>
  void insert(iterator pos, ForwardIterator first, ForwardIterator last,// 将[first, last)插入到pos位置，针对forward_iterator_tag 
              forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */
                                                                        // insert_aux()函数 
  iterator insert_aux(iterator pos, const value_type& x);
  void insert_aux(iterator pos, size_type n, const value_type& x);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class ForwardIterator>// 针对forward iterator 
  void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last,
                  size_type n);

#else /* __STL_MEMBER_TEMPLATES */
  
  void insert_aux(iterator pos,// 针对const value_type* 
                  const value_type* first, const value_type* last,
                  size_type n);

  void insert_aux(iterator pos, const_iterator first, const_iterator last,
                  size_type n);// 针对const_iterator 
 
#endif /* __STL_MEMBER_TEMPLATES */

iterator reserve_elements_at_front(size_type n) {          // 在头部加入n个存储的节点 
    size_type vacancies = start.cur - start.first; 
	// buffer还剩下几个可以插入的节点。 
    if (n > vacancies) 
    // 如果 n 大于 vacancies 
      new_elements_at_front(n - vacancies);// 添加新的 node(增加buffers) 
    return start - difference_type(n); // 返回的迭代器（start向前走n步，start前已经有n个储存的节点elems） 
  }

  iterator reserve_elements_at_back(size_type n) {         // 在尾部加入n个存储的节点 
    size_type vacancies = (finish.last - finish.cur) - 1;
    // finish所在buffer剩下可以插入的节点  
    if (n > vacancies)// 若n大于 vacancies 
      new_elements_at_back(n - vacancies);// 在最后加入新的 node 
    return finish + difference_type(n);// 返回迭代器，finish往后走n步。 
  }
                                                           // 再头和尾加入新的元素（未赋值），分别被 reserve_elements_at_front 和 reserve_elements_at_back 调用。 
  void new_elements_at_front(size_type new_elements);   
  void new_elements_at_back(size_type new_elements);
                                                           //这两个函数感觉是为了debug (基本出现在__STL_UNWIND) 
  void destroy_nodes_at_front(iterator before_start);
  void destroy_nodes_at_back(iterator after_finish);

protected:                      // Allocation of map and nodes

  // Makes sure the map has space for new nodes.  Does not actually
  //  add the nodes.  Can invalidate map pointers.  (And consequently, 
  //  deque iterators.)

  void reserve_map_at_back (size_type nodes_to_add = 1) {    
    if (nodes_to_add + 1 > map_size - (finish.node - map))
      reallocate_map(nodes_to_add, false);
  }

  void reserve_map_at_front (size_type nodes_to_add = 1) {
    if (nodes_to_add > start.node - map)
      reallocate_map(nodes_to_add, true);
  }

  void reallocate_map(size_type nodes_to_add, bool add_at_front);

  pointer allocate_node() { return data_allocator::allocate(buffer_size()); } // 分配node的内存。 
  void deallocate_node(pointer n) {                                           // 析构node指针 n 
    data_allocator::deallocate(n, buffer_size());
  }

#ifdef __STL_NON_TYPE_TMPL_PARAM_BUG
public:
  bool operator==(const deque<T, Alloc, 0>& x) const {
    return size() == x.size() && equal(begin(), end(), x.begin());
  }
  bool operator!=(const deque<T, Alloc, 0>& x) const {
    return size() != x.size() || !equal(begin(), end(), x.begin());
  }
  bool operator<(const deque<T, Alloc, 0>& x) const {
    return lexicographical_compare(begin(), end(), x.begin(), x.end());
  }
#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */
};

// Non-inline member functions

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      size_type n, const value_type& x) {
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    uninitialized_fill(new_start, start, x);
    start = new_start;
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    uninitialized_fill(finish, new_finish, x);
    finish = new_finish;
  }
  else 
    insert_aux(pos, n, x);
}

#ifndef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      const value_type* first,
                                      const value_type* last) {
  size_type n = last - first;
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(first, last, new_start);
      start = new_start;
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(first, last, finish);
      finish = new_finish;
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
  else
    insert_aux(pos, first, last, n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      const_iterator first,
                                      const_iterator last)
{
  size_type n = last - first;
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(first, last, new_start);
      start = new_start;
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(first, last, finish);
      finish = new_finish;
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
  else
    insert_aux(pos, first, last, n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
deque<T, Alloc, BufSize>::iterator       // 返回值 deque<T,Alloc. BufSize>::iterator 
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {// 函数名 deque<T, Alloc, BufSize>::earse()  
  if (first == start && last == finish) { 
    // 恰好为全deque 
    clear();
    return finish;// 返回finish 
  }
  else { // 若不为全deque 
    difference_type n = last - first;
	// 确定清楚的元素个数 
    difference_type elems_before = first - start;
    // first距离start的元素个数 
    if (elems_before < (size() - n) / 2) {
    // first之前的元素个数更多 
      copy_backward(start, first, last);// 移动[first，last)到start位置 
      // 由于[first,last)已经为deque的前几个元素，接下来的事情只需要将前面的元素删除即可。
	  iterator new_start = start + n; 
      destroy(start, new_start);// 删除[start，new_start)之间的元素 
      for (map_pointer cur = start.node; cur < new_start.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());// 析构 map中的node 
      start = new_start;// 重新指定start 
    }
    else {// last之后的元素更多 
      copy(last, finish, first);// [last,finish)移动到first位置
	  //接下来只需删除deque后面的元素。 
      iterator new_finish = finish - n;
      destroy(new_finish, finish);// 删除完毕 
      for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());// 析构map中的node 
      finish = new_finish;// 重新指定finish 
    }
    return start + elems_before;// 返回的迭代器指向原来first位置的元素。 
  }
}

template <class T, class Alloc, size_t BufSize>            // 清空deque中的所有元素 
void deque<T, Alloc, BufSize>::clear() {                                                                                                                         // deallocate和distory函数不熟练（为内存管理的内容） 
  for (map_pointer node = start.node + 1; node < finish.node; ++node) {
    // 留start所在的node和finish所在的node 
	destroy(*node, *node + buffer_size()); // 析构node所在的buffer（也即这个buffer上的所有节点。）
    data_allocator::deallocate(*node, buffer_size()); // 再清空内存。
  }

  if (start.node != finish.node) {// 若两者不在同一个node 
    destroy(start.cur, start.last);
    destroy(finish.first, finish.cur);
	// 留一个 buffer
    data_allocator::deallocate(finish.first, buffer_size()); 
  }
  else
    destroy(start.cur, finish.cur);

  finish = start; // 留的这个buffer 回到最原始的时候，finish = start。
}

// 这个函数只有在deque的构造函数中才使用，调用它的fill_initialized
// 和range_initialized函数也只被deque的构造函数调用。 
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
  // num_elements 为新的元素个数
  size_type num_nodes = num_elements / buffer_size() + 1;
  map_size = max(initial_map_size(), num_nodes + 2);
  map = map_allocator::allocate(map_size);
  // 首先确定需要的node数；再获取map_size；最后用map_allocator::allocate函数分配内存 
  map_pointer nstart = map + (map_size - num_nodes) / 2;
  map_pointer nfinish = nstart + num_nodes - 1;// 减1应该是因为 nstart占了一个node，接下来只需要 num_nodes - 1个。 
  /*
   确定nstart和nfinish  这里的处理很有技巧
  1,对于nstart = map+(map_size - num_nodes)/2，保证了 nstart - map>= 1，这么处理保证了nstart前至少有一个缓冲区。
  2,对于nfinish = nstart + num_nodes - 1， 保证了 nfinish<= map_size - 1，同理这么处理也使得nfinish后至少有一个缓冲区 
  */ 
  map_pointer cur; 
  __STL_TRY {
    for (cur = nstart; cur <= nfinish; ++cur)
      *cur = allocate_node();// 分配内存（给nstart和nfinish之间的node） 
  }
#     ifdef  __STL_USE_EXCEPTIONS 
  catch(...) {
    for (map_pointer n = nstart; n < cur; ++n)
      deallocate_node(*n);
    map_allocator::deallocate(map, map_size);
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */
// 设置start，finish的first、last、cur                                    具体的可以查看set_node()函数 
  start.set_node(nstart);// start 移动到 nstart 
  finish.set_node(nfinish);// finish 移动到 nfinish 
  start.cur = start.first;// 确定start的cur 
  finish.cur = finish.first + num_elements % buffer_size(); // 确定finish的cur   %是求余！！！！！！
}

// This is only used as a cleanup function in catch clauses.
// 这仅用作catch子句中的清理函数。(try()...catch())
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes() {                  // map的析构函数 
  for (map_pointer cur = start.node; cur <= finish.node; ++cur)           // cur_node 是指向每一个buffer 头部的指针。
    deallocate_node(*cur);
  map_allocator::deallocate(map, map_size);                               // map 不一定为 start.node。
}
  

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
                                               const value_type& value) {
  create_map_and_nodes(n);// 得到所需的node数（不一定等于n） 
  map_pointer cur;
  __STL_TRY {
    for (cur = start.node; cur < finish.node; ++cur)
      uninitialized_fill(*cur, *cur + buffer_size(), value);
    uninitialized_fill(finish.first, finish.cur, value);
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (map_pointer n = start.node; n < cur; ++n)
      destroy(*n, *n + buffer_size());
    destroy_map_and_nodes();
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::range_initialize(InputIterator first,
                                                InputIterator last,
                                                input_iterator_tag) {
  create_map_and_nodes(0);
  for ( ; first != last; ++first)
    push_back(*first);      // 后插 first所指的值。 
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::range_initialize(ForwardIterator first,
                                                ForwardIterator last,
                                                forward_iterator_tag) {
  size_type n = 0;
  distance(first, last, n);
  create_map_and_nodes(n);
  __STL_TRY {
    uninitialized_copy(first, last, start);
  }
  __STL_UNWIND(destroy_map_and_nodes());
}

#endif /* __STL_MEMBER_TEMPLATES */

// Called only if finish.cur == finish.last - 1.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_back();
  *(finish.node + 1) = allocate_node();
  __STL_TRY {
    construct(finish.cur, t_copy);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
  }
  __STL_UNWIND(deallocate_node(*(finish.node + 1)));
}

// Called only if start.cur == start.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_front();
  *(start.node - 1) = allocate_node();
  __STL_TRY {
    start.set_node(start.node - 1);
    start.cur = start.last - 1;    // 确定位置
    construct(start.cur, t_copy);   // 赋值
  }
#     ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    start.set_node(start.node + 1);
    start.cur = start.first;
    deallocate_node(*(start.node - 1));
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */
} 

// Called only if finish.cur == finish.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>:: pop_back_aux() {
  deallocate_node(finish.first);
  finish.set_node(finish.node - 1);
  finish.cur = finish.last - 1;
  destroy(finish.cur);
}

// Called only if start.cur == start.last - 1.  Note that if the deque
//  has at least one element (a necessary precondition for this member
//  function), and if start.cur == start.last, then the deque must have
//  at least two nodes.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
  destroy(start.cur);
  deallocate_node(start.first);  // 析构这个node所在的buffer
  start.set_node(start.node + 1);
  start.cur = start.first;
}      

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,    // insert函数的实现，关于input iterator 
                                      InputIterator first, InputIterator last,
                                      input_iterator_tag) {
  copy(first, last, inserter(*this, pos));// 将[first，last)移动到
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,     // insert函数的实现，关于forward iterator 
                                      ForwardIterator first,
                                      ForwardIterator last,
                                      forward_iterator_tag) {
  size_type n = 0;
  distance(first, last, n);
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(first, last, new_start);
      start = new_start;
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(first, last, finish);
      finish = new_finish;
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
  else
    insert_aux(pos, first, last, n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {// 最普通的insert_aux(). 
  difference_type index = pos - start; //距离start的值。 
  value_type x_copy = x;
  if (index < size() / 2) {// 若插入点在前半部分 
    push_front(front());// 在头部插入节点 
    iterator front1 = start;
    ++front1;
    iterator front2 = front1;
    ++front2;
    pos = start + index;
    iterator pos1 = pos;
    ++pos1;
    copy(front2, pos1, front1);
  }
  else {
    push_back(back());
    iterator back1 = finish;
    --back1;
    iterator back2 = back1;
    --back2;
    pos = start + index;
    copy_backward(pos, back2, back1);
  }
  *pos = x_copy;
  return pos;
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          size_type n, const value_type& x) {
  const difference_type elems_before = pos - start;
  // 得到插入的位置距离start的距离——elems_before。 
  size_type length = size();
  value_type x_copy = x; // x为const类型，无法修改。 
  
  if (elems_before < length / 2) {
  // 插入的位置更倾于头节点 
    iterator new_start = reserve_elements_at_front(n);
    // 
    iterator old_start = start;
    // old_start 为老的start 
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start; // start更新。 
        copy(start_n, pos, old_start);// 
        fill(pos - difference_type(n), pos, x_copy);
      }
      else {
        __uninitialized_copy_fill(start, pos, new_start, start, x_copy);
        start = new_start;
        fill(old_start, pos, x_copy);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        fill(pos, pos + difference_type(n), x_copy);
      }
      else {
        __uninitialized_fill_copy(finish, pos + difference_type(n),
                                  x_copy,
                                  pos, finish);
        finish = new_finish;
        fill(pos, old_finish, x_copy);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          ForwardIterator first,
                                          ForwardIterator last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n); 
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        ForwardIterator mid = first;
        advance(mid, difference_type(n) - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        ForwardIterator mid = first;
        advance(mid, elems_after);
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const value_type* first,
                                          const value_type* last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        const value_type* mid = first + (difference_type(n) - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        const value_type* mid = first + elems_after;
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const_iterator first,
                                          const_iterator last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= n) {
        iterator start_n = start + n;
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        const_iterator mid = first + (n - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = length - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > n) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        const_iterator mid = first + elems_after;
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) { //  在 front加入新的元素（elems） 
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();// 需要增加的node数 (new_nodes)
  reserve_map_at_front(new_nodes);// 在map(控制中心)加入new_nodes个node。 
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(start.node - i) = allocate_node(); // 分配内存给新的 node 
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {  // debug，防止出错 
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(start.node - j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {//在back加入新的元素（elems），步骤和new_elements_at_front()类似。 
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
  reserve_map_at_back(new_nodes);
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(finish.node + i) = allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(finish.node + j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

// 个人感觉这个函数的出现就是为了debug 
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start) {// deallocate before_start 与 start之间的 node 
  for (map_pointer n = before_start.node; n < start.node; ++n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish) { // deallocate finish 与 after_finish 之间的node 
  for (map_pointer n = after_finish.node; n > finish.node; --n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add,
                                              bool add_at_front) {
  size_type old_num_nodes = finish.node - start.node + 1;
  size_type new_num_nodes = old_num_nodes + nodes_to_add;

  map_pointer new_nstart;
  if (map_size > 2 * new_num_nodes) {   // 如果map_size > 2倍new_num_nodes，那么对原有的map进行调整（尽量放到map的正中间）
    new_nstart = map + (map_size - new_num_nodes) / 2 
                     + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < start.node)
      copy(start.node, finish.node + 1, new_nstart);
    else
      copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
  }
  else {                               // 否则重新找一块内存创建新的map，
    size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;

    map_pointer new_map = map_allocator::allocate(new_map_size);
    new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
    copy(start.node, finish.node + 1, new_nstart);
    map_allocator::deallocate(map, map_size);

    map = new_map;
    map_size = new_map_size;
  }

  start.set_node(new_nstart);// 确定start迭代器的位置 
  finish.set_node(new_nstart + old_num_nodes - 1);// 确定finish迭代器的位置
}


// Nonmember functions.

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG

template <class T, class Alloc, size_t BufSiz>
bool operator==(const deque<T, Alloc, BufSiz>& x,
                const deque<T, Alloc, BufSiz>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc, size_t BufSiz>
bool operator<(const deque<T, Alloc, BufSiz>& x,
               const deque<T, Alloc, BufSiz>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

#if defined(__STL_FUNCTION_TMPL_PARTIAL_ORDER) && \
    !defined(__STL_NON_TYPE_TMPL_PARAM_BUG)

template <class T, class Alloc, size_t BufSiz>
inline void swap(deque<T, Alloc, BufSiz>& x, deque<T, Alloc, BufSiz>& y) {
  x.swap(y);
}

#endif

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif
          
__STL_END_NAMESPACE 
  
#endif /* __SGI_STL_INTERNAL_DEQUE_H */

// Local Variables:
// mode:C++
// End:

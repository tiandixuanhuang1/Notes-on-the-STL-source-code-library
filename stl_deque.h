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
1��ָ����� = ����ַ1 - ��ַ2��/sizeof(����) 
*/



__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

// Note: this function is simply a kludge to work around several compilers'
//  bugs in handling constant expressions.
//ע�⣺�������ֻ��Ϊ�˽�������������ڴ��������ʽʱ���ֵĴ��������һ��С������
inline size_t __deque_buf_size(size_t n, size_t sz)
{
  return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
  // �� n Ϊ 0 ��Ԫ�ش�СszС�� 512����size_t = 512/sz
  // �� n Ϊ 0 ��Ԫ�ش�Сsz���� 512����size_t 1
  // �� n ��Ϊ 0�����Ϊ n 
}

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG                                                                                                           // ��ֹ��bug ??
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {                                                     
  typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
  typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T)); } // ��̬����  �õ��������Ĵ�С�� 
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */

template <class T, class Ref, class Ptr>
struct __deque_iterator {                                              // ***����deque �ĵ����� 
  typedef __deque_iterator<T, T&, T*>             iterator;
  typedef __deque_iterator<T, const T&, const T*> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(0, sizeof(T)); }// �������Ĵ�С buffer_size() ��һ����̬������ 
#endif

  typedef random_access_iterator_tag iterator_category; // (1)���������� dequeΪ random_access_iterator_tag 
  typedef T value_type;                                 // (2) �������� 
  typedef Ptr pointer;                                  // (3) ָ�� 
  typedef Ref reference;                                // (4) ���� 
  typedef size_t size_type;                        
  typedef ptrdiff_t difference_type;                    // (5)  ���� 
  typedef T** map_pointer;                              // ָ���ָ�� 

  typedef __deque_iterator self;                                                                                                                  // __deque_iterator Ϊʲô����ģ�� 

  T* cur;                                               // �������е�����  cur Ϊ��ǰ��λ�ã�first Ϊ��ǰbuffer��ͷ����last Ϊ��ǰbuffer��β����map_pointerΪ��buffer�������ĵ�λ�� 
  T* first;
  T* last;
  map_pointer node;
  /* nodeָ�����������һ���ڵ��ִ������������Ϊһ
  ��Ԫ��Ϊָ��� vector��nodeΪһ��ָ��ָ���ָ�� */

  __deque_iterator(T* x, map_pointer y)                     // �вι��캯��������Ϊ x��curΪָ��x��ָ�룩��firstΪ *y��T*����lastΪ *y + buffer_size()��nodeΪ y��T**�� 
    : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    
  __deque_iterator() : cur(0), first(0), last(0), node(0) {}//Ĭ�Ϲ��� 
  
  __deque_iterator(const iterator& x)                       // �������캯�� 
    : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

  reference operator*() const { return *cur; }              // ������ reference = Ref��һ��Ӧ��Ϊ T&�� 
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }     // ���� *cur�ĵ�ַ 
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  difference_type operator-(const self& x) const {          // ���غ��� -���������ļ�������self = __deque_iterator ����Ϊ x �� ��ָԪ�ص�ǰ�ߡ�
    return difference_type(buffer_size()) * (node - x.node - 1) +
      (cur - first) + (x.last - x.cur);
  }

  self& operator++() {                                     //self = __deque_iterator��++ Ϊcurָ���λ�ñ任 
     // �л���һ��Ԫ��
	 // ����ִﻺ����β��
	 //����ת����һ�� node(������)����� 
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

  self& operator+=(difference_type n) {              // += ���� 
    difference_type offset = n + (cur - first);
    if (offset >= 0 && offset < difference_type(buffer_size()))
    	//Ŀ��λ����ͬһ�������� 
      cur += n;
    else { // Ŀ�������ڻ����� 
      difference_type node_offset =
        offset > 0 ? offset / difference_type(buffer_size())
                   : -difference_type((-offset - 1) / buffer_size()) - 1;
      set_node(node + node_offset);
      // �л�����ȷ�Ļ������� 
      cur = first + (offset - node_offset * difference_type(buffer_size()));
      // �л�����ȷ�Ľڵ�λ�ô� 
    }
    return *this;
  }

  self operator+(difference_type n) const {        // +���أ��õ���+=�� 
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n)              // -=���� 
  { return *this += -n; }
 
  self operator-(difference_type n) const {        // - ���أ��õ���-=�� 
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const { return *(*this + n); } // []ȡֵ����ʵ�� 

  bool operator==(const self& x) const { return cur == x.cur; }// ���������cur��ȣ�Ҳ��ָ��Ľڵ���ȣ� 
  bool operator!=(const self& x) const { return !(*this == x); }// ����� 
  bool operator<(const self& x) const {                         // < ��node��cur 
    return (node == x.node) ? (cur < x.cur) : (node < x.node);
  }

  void set_node(map_pointer new_node) {                       // ���� node ֻ�Ƕ����е�buffer��������û��ƾ������ buffer�� 
    node = new_node;
    first = *new_node;
    last = first + difference_type(buffer_size());
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG

template <class T, class Ref, class Ptr, size_t BufSiz>
inline random_access_iterator_tag                             // ��ȡ��������iterator category��__deque_iterator<T,Ref,Ptr,BufSiz>�� 107
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return random_access_iterator_tag();
}

template <class T, class Ref, class Ptr, size_t BufSiz>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {// ��ȡ��������value type 
  return 0;
}

template <class T, class Ref, class Ptr, size_t BufSiz>       // ��ȡ��������distance type 
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}

#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */

template <class T, class Ref, class Ptr>
inline random_access_iterator_tag                          // ��ȡ��������iterator category ��__deque_iterator<T,Ref,Ptr>�� 113
iterator_category(const __deque_iterator<T, Ref, Ptr>&) {
  return random_access_iterator_tag();
}

template <class T, class Ref, class Ptr>                   // ��ȡ��������value type 
inline T* value_type(const __deque_iterator<T, Ref, Ptr>&) { return 0; }

template <class T, class Ref, class Ptr>                   // ��ȡ��������distance type 
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr>&) {
  return 0;
}

#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// See __deque_buf_size().  The only reason that the default value is 0
//  is as a workaround for bugs in the way that some compilers handle
//  constant expressions.
// Ĭ��ֵΪ 0 ��Ψһԭ����Ϊ�˽��ĳЩ���������������ʽʱ���ֵĴ���
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
  typedef simple_alloc<value_type, Alloc> data_allocator;   // date_allocator �з��� value_type 
  typedef simple_alloc<pointer, Alloc> map_allocator;       // map_allocator ���� pointer = value_type* 

  static size_type buffer_size() {                          // �õ�deque��һ���������Ĵ�С 
    return __deque_buf_size(BufSiz, sizeof(value_type));
  }
  static size_type initial_map_size() { return 8; }         // ��ʼ�� map���������ĵĴ�С��Ϊ8 

protected:                      // Data members
  iterator start;                                            // һ��deque������ iterator start��finish 
  iterator finish;

  map_pointer map;                                           // �Լ� map (ָ���������)��map_pointer = T** 
  size_type map_size;                                        // size_type = size_t 

public:                         // Basic accessors
  iterator begin() { return start; }                         // ���� start iterator
  iterator end() { return finish; }                          // ����finish iterator �����ͬ�� 
  
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

  reference operator[](size_type n) { return start[difference_type(n)]; } // �õ��� n ����������ָ��ֵ��deque[n] = start[n] = *(start+n)
  const_reference operator[](size_type n) const {
    return start[difference_type(n)];
  }

  reference front() { return *start; }                        // ����deque��ǰ���ֵ 
  reference back() {                                          // ����deque����ֵ 
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

  size_type size() const { return finish - start;; }          // �õ�deque�Ĵ�С��- �����ؼ�150 
  size_type max_size() const { return size_type(-1); }        // �õ�deque�����۴�С 
  bool empty() const { return finish == start; }              // deque�Ƿ�Ϊ�� 

public:                         // Constructor, destructor.
  deque()                                                     // ctor 
    : start(), finish(), map(0), map_size(0) 
  {
    create_map_and_nodes(0);
  }

  deque(const deque& x)                                       // �������캯������ x�� 
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(x.size());// x�Ĵ�С 359 
    __STL_TRY {
      uninitialized_copy(x.begin(), x.end(), start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

  deque(size_type n, const value_type& value)
    : start(), finish(), map(0), map_size(0)                  // ��n��value��ʼ��deque��size_type�� 
  {
    fill_initialize(n, value);
  }

  deque(int n, const value_type& value)                       // ��n��value��ʼ��deque��int��
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }
 
  deque(long n, const value_type& value)                      // ��n��value��ʼ��deque��long��
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }

  explicit deque(size_type n)                                 // ���ɽ�����ʽ�滻 value = value_type() 
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value_type());
  }

#ifdef __STL_MEMBER_TEMPLATES

  template <class InputIterator>
  deque(InputIterator first, InputIterator last)              // [first, last)����ʼ�� deque ��input iterator�� 
    : start(), finish(), map(0), map_size(0)
  {
    range_initialize(first, last, iterator_category(first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  deque(const value_type* first, const value_type* last)      // [first, last)����ʼ�� deque ��const value_type*�� 
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

  deque(const_iterator first, const_iterator last)           // [first, last)����ʼ�� deque 
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

  deque& operator= (const deque& x) {                          // deque�ĸ�ֵ����
  //  
    const size_type len = size();
    if (&x != this) {// x �����ڱ��� (deque)
      if (len >= x.size())
	  // deque.size() >= x.size()����x.begin()��x.end()���ֿ����� deque���ٰ�deque֮���ɾȥ 
        erase(copy(x.begin(), x.end(), start), finish);
      else {// deque.size < x.size() 
        const_iterator mid = x.begin() + difference_type(len);
        copy(x.begin(), mid, start); //�Ƚ�x.begin() �� x.begin() + len�Ĳ��ֿ�����deque
        insert(finish, mid, x.end()); //�ٽ����£�mid��x.end()���Ĳ��뵽x֮��
      }
    }
    return *this;// �������� deque 
  }        

  void swap(deque& x) {                                        // ����deque������һ��deque�н����� start��finish��map��map_size�� 
    __STD::swap(start, x.start);
    __STD::swap(finish, x.finish);
    __STD::swap(map, x.map);
    __STD::swap(map_size, x.map_size);
  }

public:                         // push_* and pop_*
  
  void push_back(const value_type& t) {                        // ��ֵΪt�Ľڵ��嵽deque 
    if (finish.cur != finish.last - 1) {
    	// finish���ڵ�buffer ����ʣ��Ľڵ���Բ��롣 
      construct(finish.cur, t);
      ++finish.cur;
    }
    else// finish���ڵ�buffer�Ѿ����� 
      push_back_aux(t);                                                                                                                                // push_back_aux() 
  }

  void push_front(const value_type& t) {                       // ��ֵΪt�Ľڵ� ǰ�嵽deque
    if (start.cur != start.first) {
    	// start�ĵ�ǰλ�ò�Ϊstart�� first λ�ã�start���ڵ�buffer������װ�� 
      construct(start.cur - 1, t);
      --start.cur;
    }
    else// start���ڵ�bufferװ������ 
      push_front_aux(t);                                                                                                                               // push_front_aux()
  }

  void pop_back() {                                            // ɾ�����һ���ڵ� 
    if (finish.cur != finish.first) {
      --finish.cur;
      destroy(finish.cur);
    }
    else// ������Ϊ finish ��ָ��bufferû��һ��Ԫ�� 
      pop_back_aux();                                                                                                                                  // pop_back_aux() 
  }

  void pop_front() {                                           // ɾ����ǰ��һ���ڵ� 
    if (start.cur != start.last - 1) {
      destroy(start.cur);
      ++start.cur;
    }
    else// ������Ϊ start ��ָ��buffer����һ��Ԫ��
      pop_front_aux();                                                                                                                                  // pop_front_aux() 
  }

public:                         // Insert

  iterator insert(iterator position, const value_type& x) {    // ��positionλ�ò���ֵΪx�ؽڵ㡣 
    if (position.cur == start.cur) {// ��position����startָ���cur����ǰ�� 
      push_front(x);
      return start;
    }
    else if (position.cur == finish.cur) { // ��position����finishָ���cur������ 
      push_back(x);
      iterator tmp = finish;
      --tmp;
      return tmp;
    }
    else {           // ��Ҫ���λ��Ϊdeque�м��λ�ã����� insert_aux()                                                                                // insert_aux() 
      return insert_aux(position, x);
    }
  }

  iterator insert(iterator position) { return insert(position, value_type()); }// insert()�����ذ汾 

  void insert(iterator pos, size_type n, const value_type& x); // ��posλ�ò��� n(size_type) ��ֵΪx�Ľڵ㡣 

  void insert(iterator pos, int n, const value_type& x) {       // ��posλ�ò��� n(int�������ǿ��ת��Ϊsize_type) ��ֵΪx�Ľڵ㡣 
    insert(pos, (size_type) n, x);
  }
  void insert(iterator pos, long n, const value_type& x) {     //��posλ�ò��� n(long�������ǿ��ת��Ϊsize_type) ��ֵΪx�Ľڵ㡣 
    insert(pos, (size_type) n, x);
  }

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last) {// ��[fisrt, last)���뵽posλ�ã�������Ϊinput iterator 
    insert(pos, first, last, iterator_category(first)); 
	// insert��һ�����غ��� 
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator pos, const value_type* first, const value_type* last);// ��[fisrt, last)���뵽posλ�ã�������Ϊ ��const value_type* 
  void insert(iterator pos, const_iterator first, const_iterator last);// ��[fisrt, last)���뵽posλ�ã�������Ϊ ��const_iterator = __deque_iterator<T, const T, const T*>

#endif /* __STL_MEMBER_TEMPLATES */   // insert end 

  void resize(size_type new_size, const value_type& x) {       // resize���� 
    const size_type len = size();
    if (new_size < len) // ��new_size С�� len ��ֻȡǰnew_size ��Ԫ�� 
      erase(start + new_size, finish);
    else // ������֮�����new_size - len��ֵΪx�Ľڵ� 
      insert(finish, new_size - len, x);
  }

  void resize(size_type new_size) { resize(new_size, value_type()); } // resize�����ذ汾 

public:                         // Erase
  iterator erase(iterator pos) {                                     // ɾ��λ��Ϊpos�Ľڵ㡣 
    iterator next = pos;
    ++next;// next ����һλ(pos�ĺ�һλ) 
    difference_type index = pos - start;
    // Ŀ��λ��pos��start�ľ��� ����index 
    if (index < (size() >> 1)) {
      // index ������ͷ�ڵ� 
      copy_backward(start, pos, next);
      // �ƶ������pos֮ǰ��Ԫ�ء�                                                                                                                           // ����[start,pos)�ƶ���nextλ�� �� 
      pop_front();
      // ��ǰ���Ԫ��ɾ��(pos��ָ�Ľڵ�) 
    }
    else {// ������β�ڵ� 
      copy(next, finish, pos);
      // �ƶ������֮��Ľڵ㣨Ԫ�أ� 
      pop_back();
      // ������Ԫ��ɾ���� 
    }
    return start + index;// ���صĵ�����ָ��pos�ĺ�һλԪ�� 
  }

  iterator erase(iterator first, iterator last);                   // ɾ��[first, last)֮���Ԫ�� 
  void clear();                                                      // ���deque(ɾ�����еĽڵ�) 

protected:                        // Internal construction/destruction�����õĹ��캯�������������� 

  void create_map_and_nodes(size_type num_elements);  // ���� map�ʹ���node�� 
  void destroy_map_and_nodes();                       // ���� map 
  void fill_initialize(size_type n, const value_type& value);                                                                                                                  // ������n��value��ʼ��deque 

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void range_initialize(InputIterator first, InputIterator last,                                                                                                                 // ������[first, last)��ʼ��  input iterator�汾 
                        input_iterator_tag);

  template <class ForwardIterator>
  void range_initialize(ForwardIterator first, ForwardIterator last, // forward iterator �汾 
                        forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

protected:                        // Internal push_* and pop_* ����push_back �� push_front �ĸ������� 

  void push_back_aux(const value_type& t);
  void push_front_aux(const value_type& t);
  void pop_back_aux();
  void pop_front_aux();

protected:                        // Internal insert functions

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last, // ��[first, last)���뵽posλ�ã����input_iterator_tag
              input_iterator_tag);

  template <class ForwardIterator>
  void insert(iterator pos, ForwardIterator first, ForwardIterator last,// ��[first, last)���뵽posλ�ã����forward_iterator_tag 
              forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */
                                                                        // insert_aux()���� 
  iterator insert_aux(iterator pos, const value_type& x);
  void insert_aux(iterator pos, size_type n, const value_type& x);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class ForwardIterator>// ���forward iterator 
  void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last,
                  size_type n);

#else /* __STL_MEMBER_TEMPLATES */
  
  void insert_aux(iterator pos,// ���const value_type* 
                  const value_type* first, const value_type* last,
                  size_type n);

  void insert_aux(iterator pos, const_iterator first, const_iterator last,
                  size_type n);// ���const_iterator 
 
#endif /* __STL_MEMBER_TEMPLATES */

iterator reserve_elements_at_front(size_type n) {          // ��ͷ������n���洢�Ľڵ� 
    size_type vacancies = start.cur - start.first; 
	// buffer��ʣ�¼������Բ���Ľڵ㡣 
    if (n > vacancies) 
    // ��� n ���� vacancies 
      new_elements_at_front(n - vacancies);// ����µ� node(����buffers) 
    return start - difference_type(n); // ���صĵ�������start��ǰ��n����startǰ�Ѿ���n������Ľڵ�elems�� 
  }

  iterator reserve_elements_at_back(size_type n) {         // ��β������n���洢�Ľڵ� 
    size_type vacancies = (finish.last - finish.cur) - 1;
    // finish����bufferʣ�¿��Բ���Ľڵ�  
    if (n > vacancies)// ��n���� vacancies 
      new_elements_at_back(n - vacancies);// ���������µ� node 
    return finish + difference_type(n);// ���ص�������finish������n���� 
  }
                                                           // ��ͷ��β�����µ�Ԫ�أ�δ��ֵ�����ֱ� reserve_elements_at_front �� reserve_elements_at_back ���á� 
  void new_elements_at_front(size_type new_elements);   
  void new_elements_at_back(size_type new_elements);
                                                           //�����������о���Ϊ��debug (����������__STL_UNWIND) 
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

  pointer allocate_node() { return data_allocator::allocate(buffer_size()); } // ����node���ڴ档 
  void deallocate_node(pointer n) {                                           // ����nodeָ�� n 
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
deque<T, Alloc, BufSize>::iterator       // ����ֵ deque<T,Alloc. BufSize>::iterator 
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {// ������ deque<T, Alloc, BufSize>::earse()  
  if (first == start && last == finish) { 
    // ǡ��Ϊȫdeque 
    clear();
    return finish;// ����finish 
  }
  else { // ����Ϊȫdeque 
    difference_type n = last - first;
	// ȷ�������Ԫ�ظ��� 
    difference_type elems_before = first - start;
    // first����start��Ԫ�ظ��� 
    if (elems_before < (size() - n) / 2) {
    // first֮ǰ��Ԫ�ظ������� 
      copy_backward(start, first, last);// �ƶ�[first��last)��startλ�� 
      // ����[first,last)�Ѿ�Ϊdeque��ǰ����Ԫ�أ�������������ֻ��Ҫ��ǰ���Ԫ��ɾ�����ɡ�
	  iterator new_start = start + n; 
      destroy(start, new_start);// ɾ��[start��new_start)֮���Ԫ�� 
      for (map_pointer cur = start.node; cur < new_start.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());// ���� map�е�node 
      start = new_start;// ����ָ��start 
    }
    else {// last֮���Ԫ�ظ��� 
      copy(last, finish, first);// [last,finish)�ƶ���firstλ��
	  //������ֻ��ɾ��deque�����Ԫ�ء� 
      iterator new_finish = finish - n;
      destroy(new_finish, finish);// ɾ����� 
      for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());// ����map�е�node 
      finish = new_finish;// ����ָ��finish 
    }
    return start + elems_before;// ���صĵ�����ָ��ԭ��firstλ�õ�Ԫ�ء� 
  }
}

template <class T, class Alloc, size_t BufSize>            // ���deque�е�����Ԫ�� 
void deque<T, Alloc, BufSize>::clear() {                                                                                                                         // deallocate��distory������������Ϊ�ڴ��������ݣ� 
  for (map_pointer node = start.node + 1; node < finish.node; ++node) {
    // ��start���ڵ�node��finish���ڵ�node 
	destroy(*node, *node + buffer_size()); // ����node���ڵ�buffer��Ҳ�����buffer�ϵ����нڵ㡣��
    data_allocator::deallocate(*node, buffer_size()); // ������ڴ档
  }

  if (start.node != finish.node) {// �����߲���ͬһ��node 
    destroy(start.cur, start.last);
    destroy(finish.first, finish.cur);
	// ��һ�� buffer
    data_allocator::deallocate(finish.first, buffer_size()); 
  }
  else
    destroy(start.cur, finish.cur);

  finish = start; // �������buffer �ص���ԭʼ��ʱ��finish = start��
}

// �������ֻ����deque�Ĺ��캯���в�ʹ�ã���������fill_initialized
// ��range_initialized����Ҳֻ��deque�Ĺ��캯�����á� 
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
  // num_elements Ϊ�µ�Ԫ�ظ���
  size_type num_nodes = num_elements / buffer_size() + 1;
  map_size = max(initial_map_size(), num_nodes + 2);
  map = map_allocator::allocate(map_size);
  // ����ȷ����Ҫ��node�����ٻ�ȡmap_size�������map_allocator::allocate���������ڴ� 
  map_pointer nstart = map + (map_size - num_nodes) / 2;
  map_pointer nfinish = nstart + num_nodes - 1;// ��1Ӧ������Ϊ nstartռ��һ��node��������ֻ��Ҫ num_nodes - 1���� 
  /*
   ȷ��nstart��nfinish  ����Ĵ�����м���
  1,����nstart = map+(map_size - num_nodes)/2����֤�� nstart - map>= 1����ô����֤��nstartǰ������һ����������
  2,����nfinish = nstart + num_nodes - 1�� ��֤�� nfinish<= map_size - 1��ͬ����ô����Ҳʹ��nfinish��������һ�������� 
  */ 
  map_pointer cur; 
  __STL_TRY {
    for (cur = nstart; cur <= nfinish; ++cur)
      *cur = allocate_node();// �����ڴ棨��nstart��nfinish֮���node�� 
  }
#     ifdef  __STL_USE_EXCEPTIONS 
  catch(...) {
    for (map_pointer n = nstart; n < cur; ++n)
      deallocate_node(*n);
    map_allocator::deallocate(map, map_size);
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */
// ����start��finish��first��last��cur                                    ����Ŀ��Բ鿴set_node()���� 
  start.set_node(nstart);// start �ƶ��� nstart 
  finish.set_node(nfinish);// finish �ƶ��� nfinish 
  start.cur = start.first;// ȷ��start��cur 
  finish.cur = finish.first + num_elements % buffer_size(); // ȷ��finish��cur   %�����࣡����������
}

// This is only used as a cleanup function in catch clauses.
// �������catch�Ӿ��е���������(try()...catch())
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes() {                  // map���������� 
  for (map_pointer cur = start.node; cur <= finish.node; ++cur)           // cur_node ��ָ��ÿһ��buffer ͷ����ָ�롣
    deallocate_node(*cur);
  map_allocator::deallocate(map, map_size);                               // map ��һ��Ϊ start.node��
}
  

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
                                               const value_type& value) {
  create_map_and_nodes(n);// �õ������node������һ������n�� 
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
    push_back(*first);      // ��� first��ָ��ֵ�� 
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
    start.cur = start.last - 1;    // ȷ��λ��
    construct(start.cur, t_copy);   // ��ֵ
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
  deallocate_node(start.first);  // �������node���ڵ�buffer
  start.set_node(start.node + 1);
  start.cur = start.first;
}      

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,    // insert������ʵ�֣�����input iterator 
                                      InputIterator first, InputIterator last,
                                      input_iterator_tag) {
  copy(first, last, inserter(*this, pos));// ��[first��last)�ƶ���
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,     // insert������ʵ�֣�����forward iterator 
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
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {// ����ͨ��insert_aux(). 
  difference_type index = pos - start; //����start��ֵ�� 
  value_type x_copy = x;
  if (index < size() / 2) {// ���������ǰ�벿�� 
    push_front(front());// ��ͷ������ڵ� 
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
  // �õ������λ�þ���start�ľ��롪��elems_before�� 
  size_type length = size();
  value_type x_copy = x; // xΪconst���ͣ��޷��޸ġ� 
  
  if (elems_before < length / 2) {
  // �����λ�ø�����ͷ�ڵ� 
    iterator new_start = reserve_elements_at_front(n);
    // 
    iterator old_start = start;
    // old_start Ϊ�ϵ�start 
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start; // start���¡� 
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
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) { //  �� front�����µ�Ԫ�أ�elems�� 
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();// ��Ҫ���ӵ�node�� (new_nodes)
  reserve_map_at_front(new_nodes);// ��map(��������)����new_nodes��node�� 
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(start.node - i) = allocate_node(); // �����ڴ���µ� node 
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {  // debug����ֹ���� 
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(start.node - j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {//��back�����µ�Ԫ�أ�elems���������new_elements_at_front()���ơ� 
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

// ���˸о���������ĳ��־���Ϊ��debug 
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start) {// deallocate before_start �� start֮��� node 
  for (map_pointer n = before_start.node; n < start.node; ++n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish) { // deallocate finish �� after_finish ֮���node 
  for (map_pointer n = after_finish.node; n > finish.node; --n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add,
                                              bool add_at_front) {
  size_type old_num_nodes = finish.node - start.node + 1;
  size_type new_num_nodes = old_num_nodes + nodes_to_add;

  map_pointer new_nstart;
  if (map_size > 2 * new_num_nodes) {   // ���map_size > 2��new_num_nodes����ô��ԭ�е�map���е����������ŵ�map�����м䣩
    new_nstart = map + (map_size - new_num_nodes) / 2 
                     + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < start.node)
      copy(start.node, finish.node + 1, new_nstart);
    else
      copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
  }
  else {                               // ����������һ���ڴ洴���µ�map��
    size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;

    map_pointer new_map = map_allocator::allocate(new_map_size);
    new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
    copy(start.node, finish.node + 1, new_nstart);
    map_allocator::deallocate(map, map_size);

    map = new_map;
    map_size = new_map_size;
  }

  start.set_node(new_nstart);// ȷ��start��������λ�� 
  finish.set_node(new_nstart + old_num_nodes - 1);// ȷ��finish��������λ��
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

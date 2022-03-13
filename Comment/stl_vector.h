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
 * Copyright (c) 1996
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


/*
1，当成员函数的 const 和 non-const 版本同时存在时，const object 只会（只能）调用 const 版本
   non-const object 只会（只能）调用 non-const 版本 
*/
#ifndef __SGI_STL_INTERNAL_VECTOR_H
#define __SGI_STL_INTERNAL_VECTOR_H

__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

template <class T, class Alloc = alloc>
class vector {
public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type* iterator;                    // iterator = value_type* = T* = pointer
  typedef const value_type* const_iterator;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
                           difference_type>  const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
protected:
  typedef simple_alloc<value_type, Alloc> data_allocator;
                                                             // 三根迭代器， 三者次序关系为 [start, ...., )finish, ...., )end_of_storage 
  iterator start;                                            //start指向当前存储头节点
  iterator finish;                                           //finish指向当前存储的尾节点的下一个节点  
  iterator end_of_storage;                                   //end_of_storage指向现分配存储大小的节点的下一个节点
  
  void insert_aux(iterator position, const T& x);            //                                                                                                                   //insert_aux？？ 
  
  void deallocate() {                                        //删除 vector 
    if (start) data_allocator::deallocate(start, end_of_storage - start);                                                                                                         // deallocate() ?? 
    // start 非空，表示vector非空。 
  }

  void fill_initialize(size_type n, const T& value) {         // 初始化vector，n 为n个元素，且finish 和 end_of_storage 指向同一个节点。 
    start = allocate_and_fill(n, value);
    finish = start + n;
    end_of_storage = finish;
  }
public:
  iterator begin() { return start; }                        // 返回start迭代器 
  const_iterator begin() const { return start; }            // 返回 start 常迭代器  const_itertor =  const value_type*
  iterator end() { return finish; }                         // 返回 finish 迭代器 
  const_iterator end() const { return finish; }             // 返回 finish 常迭代器  const_itertor =  const value_type*
  reverse_iterator rbegin() { return reverse_iterator(end()); } // 以下与上面类似 
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(end()); 
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(begin()); 
  }
  size_type size() const { return size_type(end() - begin()); } // 返回 vector 的元素多少，size_type() 为强制转换。 
  size_type max_size() const { return size_type(-1) / sizeof(T); } // vector理论上能存储的元素多少  （最大大小） 
  size_type capacity() const { return size_type(end_of_storage - begin()); } // vector 现能存储的总元素个数（容量） 
  bool empty() const { return begin() == end(); }               // 判断 vector 是否为空 
  reference operator[](size_type n) { return *(begin() + n); }  // 返回第 n 个元素的值  reference = value_type& 
  const_reference operator[](size_type n) const { return *(begin() + n); } //  返回第 n 个元素的值  const_reference = const value_type&

																		// 以下为构造函数 
  vector() : start(0), finish(0), end_of_storage(0) {}                  // 最原始的版本 
  vector(size_type n, const T& value) { fill_initialize(n, value); }    // 构造 size_type类型的 n 个为value的数值的 vector 
  vector(int n, const T& value) { fill_initialize(n, value); }          // 构造 int 型 n个value数值的 vector 
  vector(long n, const T& value) { fill_initialize(n, value); }         // 构造 long 型 n个 value 数值的 vector 
  explicit vector(size_type n) { fill_initialize(n, T()); }             // 无法隐式转换的构造函数
  																		// end ctor 

  vector(const vector<T, Alloc>& x) {                                   // 以构造好的vector x 来初始化本 vector                                                                       // allocate_and_copy ？？ 
    start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
    finish = start + (x.end() - x.begin());
    end_of_storage = finish;
  }
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>                                        // 构造函数 （input iterator） 
  vector(InputIterator first, InputIterator last) :                     // [first, last)
    start(0), finish(0), end_of_storage(0)
  {
    range_initialize(first, last, iterator_category(first));                                                                                                                          // range_initialize() 
  }
#else /* __STL_MEMBER_TEMPLATES */
  vector(const_iterator first, const_iterator last) {                   // 构造函数（const iterator） 
    size_type n = 0;
    distance(first, last, n);
    start = allocate_and_copy(n, first, last);
    finish = start + n;
    end_of_storage = finish;
  }
#endif /* __STL_MEMBER_TEMPLATES */
  ~vector() {                                                           // 析构函数 
    destroy(start, finish);
    deallocate();
  }
  vector<T, Alloc>& operator=(const vector<T, Alloc>& x);               // 赋值函数，将 x 赋值给本 vector 
   
  void reserve(size_type n) {                                           // 将 vector 的容量增长到 n （n应该大于capacity()）
  	// 记录 old_size，创建新的 vector并将老的 vector 的值赋值到新的vector上。返回头节点的迭代器.
	// 删除老的 vector
	// start = tmp; finish = start + old_size; end_of_storage = start + n; 
    if (capacity() < n) {
      const size_type old_size = size();
      iterator tmp = allocate_and_copy(n, start, finish);
      destroy(start, finish);
      deallocate();
      start = tmp;
      finish = tmp + old_size;
      end_of_storage = start + n;
    }
  	}
  
  reference front() { return *begin(); }                   // 得到指向头节点的值 
  const_reference front() const { return *begin(); }       //  针对 const object 的版本 
  reference back() { return *(end() - 1); }                // 返回最后一个结点的数值 
  const_reference back() const { return *(end() - 1); }    // 针对 const object 的版本 
  void push_back(const T& x) {                             // 在vector末尾加入值为 x 的节点 
    if (finish != end_of_storage) {
      construct(finish, x);
      ++finish;
    }
    else
      insert_aux(end(), x);
  }
  void swap(vector<T, Alloc>& x) {                        // 将本 vector 和 x 互换                                                                                                  //__STD::swap() 
	__STD::swap(start, x.start);                           // 由于一个 vector 由三根 iterator 构成，所以只要交换两个vector的iterator，就为交换了两个 vector 
    __STD::swap(finish, x.finish);
    __STD::swap(end_of_storage, x.end_of_storage);
  }
  
  iterator insert(iterator position, const T& x) {         // 在 position 位置插入值为 x 的节点 
    size_type n = position - begin();//距离头结点的距离 
    if (finish != end_of_storage && position == end()) {
	// 若 vector 没有满(finish != end_of_storage) 且 position 为 end()（=finish）则直接插在后面          
	// 本人认为可以调用push_back()函数，但是调用push_back，可能又要调用insert_aux，导致效率降低
      construct(finish, x);
      ++finish;
    }
    else
    // 否则调用 insert_aux()函数 
      insert_aux(position, x);                                                                                                                                                       // insert_aux() 
    return begin() + n;
  }
  
  iterator insert(iterator position) { return insert(position, T()); } // 为不带特定数值版本，可以看出这里插入的值为一个临时对象 T() 
  
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last) {  //迭代器为input iterator类型，将[first, last) 插到位置 position                                          //range_insert() 
    range_insert(position, first, last, iterator_category(first));
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator position,                                        // 迭代器为 const_iterator 版本，const_iterator = const value_type*                                            
              const_iterator first, const_iterator last);
#endif /* __STL_MEMBER_TEMPLATES */

  void insert (iterator pos, size_type n, const T& x);                  // 在pos位置插入n个值为x的节点 （n为size_type类型） 
  void insert (iterator pos, int n, const T& x) {                       // 在pos位置插入n个值为x的节点 （n为int类型）
    insert(pos, (size_type) n, x);// 强制转为size_type类型 
  }
  void insert (iterator pos, long n, const T& x) {                     // 在pos位置插入n个值为x的节点 （n为long类型）
    insert(pos, (size_type) n, x);// 强制转为size_type类型 
  }

  void pop_back() {                                                    // 删去尾节点（finish） 
    --finish;
    destroy(finish);              //能否使用语句 destroy(end());
  }
  
  iterator erase(iterator position) {                                 // 删去迭代器 pos 指向的节点，核心为 copy() 函数 
    if (position + 1 != end())
      copy(position + 1, finish, position);                                                                                                                                           // copy() 函数 
    --finish;
    destroy(finish);
    return position;
  }
  iterator erase(iterator first, iterator last) {                    // 删去 [first, last)之间的节点
  	// 将[last, finish) 拷贝到 first 位置
	// 接下来删除 finish 之后的节点
	// 最后改变 finish 所指的节点 
    iterator i = copy(last, finish, first);
    destroy(i, finish);
    finish = finish - (last - first);
    return first;
  }
  void resize(size_type new_size, const T& x) {                     // 对 vector 重新设置大小
    // 若 new_size < size() 则删除多的部分
	// 否则加 new_size - size() 个值为 x 的节点 
    if (new_size < size()) 
      erase(begin() + new_size, end());
    else
      insert(end(), new_size - size(), x);
  }
  void resize(size_type new_size) { resize(new_size, T()); } 
  void clear() { erase(begin(), end()); }                        // 清空vector 

protected:
  iterator allocate_and_fill(size_type n, const T& x) {           // 猜测应该是 构造大小为 n 值全为 x 的 vector                                                                       //allocate_and_fill() 
    iterator result = data_allocator::allocate(n);
    __STL_TRY {
      uninitialized_fill_n(result, n, x);                                                                                                                                             // uninitialized_fill_n() 
      return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class ForwardIterator>                              // 针对 forward iterator 
  iterator allocate_and_copy(size_type n,                                                                                                                                              // allocate_and_copy()
                             ForwardIterator first, ForwardIterator last) {
    iterator result = data_allocator::allocate(n);
    __STL_TRY {
      uninitialized_copy(first, last, result);
      return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
  }
#else /* __STL_MEMBER_TEMPLATES */
  iterator allocate_and_copy(size_type n,                       // 针对 const_iterator 版本 
                             const_iterator first, const_iterator last) {
    iterator result = data_allocator::allocate(n);
    __STL_TRY {
      uninitialized_copy(first, last, result);
      return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
  }
#endif /* __STL_MEMBER_TEMPLATES */


/*
InputIterator
	/ \
     |
forward_iterator
*/

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  // 这个函数和下面的函数相比更加基础。
  void range_initialize(InputIterator first, InputIterator last,  // [first, last) 之间的节点依次 push_back 到 vector， 针对 input iterator 版本 
                        input_iterator_tag) {
    for ( ; first != last; ++first)
      push_back(*first);
  }

  // This function is only called by the constructor.  We have to worry 
  //  about resource leaks, but not about maintaining invariants.
  // 此函数仅由构造函数调用。我们必须担心资源泄漏，而不是维护不变量。
  template <class ForwardIterator>
  void range_initialize(ForwardIterator first, ForwardIterator last,    // *****构造函数调用， 使用 [first, last) 初始化 vector。 （该函数和上面的函数实现方式不相同） 
                        forward_iterator_tag) {
    size_type n = 0;
    distance(first, last, n);                                                                                                                                                    // distance()
	//  猜测：n = first - last 
    start = allocate_and_copy(n, first, last);
    // 分配 vector 的内存 
    finish = start + n;
    // finish指向的迭代器 
    end_of_storage = finish;
  }

  template <class InputIterator>
  void range_insert(iterator pos,                                     //针对 intput iterator ，将 [first, last) 的节点插入到位置 pos  
                    InputIterator first, InputIterator last,
                    input_iterator_tag);

  template <class ForwardIterator>
  void range_insert(iterator pos,                                     //针对 forward iterator ，将 [first, last) 的节点插入到位置 pos
                    ForwardIterator first, ForwardIterator last,
                    forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */
};

template <class T, class Alloc>
inline bool operator==(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {   // 判断两个 vector 是否相等。 
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());                                                                                                                                         // equal() 
  // 相等的几个指标，1，size() 2， ?????                     
}

template <class T, class Alloc>
inline bool operator<(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {    // vector 比大小 
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());                                                                                                             // 比大小的条件？？ 
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class T, class Alloc>
inline void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) { // 猜测 x 与 y 两个vector交换 
  x.swap(y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {   // 赋值函数 
  if (&x != this) {// 需要 x 不为本 vector 
    if (x.size() > capacity()) {
		// x 的大小大于 vector 的容量 size() = begin() - end() = start - finish 
		// 创建新的 vector, 返回新的vector 头节点的迭代器 tmp 
		// 删除原来的 vector 
		// 析构并将前vector 的start设为 tmp 
		// end_of_storage = start + x.size() 
      iterator tmp = allocate_and_copy(x.end() - x.begin(),                                                                                                      //allocate_and_copy()
                                       x.begin(), x.end());
      destroy(start, finish);                                    																								// destroy() 
      deallocate(); 																																			 // deallocate() 
      start = tmp; 
      end_of_storage = start + (x.end() - x.begin());  
    }
    else if (size() >= x.size()) {
    	// 若 vector 的 size 大于 x.size()
		// 将[x.begin(), x.end()) 拷贝到begin()，返回迭代器 i 
		// 删除 vector 中 i 到 finish 之间的节点 
      iterator i = copy(x.begin(), x.end(), begin());
      destroy(i, finish);
    }
    else {
    	// size() < x.size() <= capacity()
		//  将 x 拷贝到本 vector 
		// ??
      copy(x.begin(), x.begin() + size(), start);                                                                                                                   // copy()                          
      uninitialized_copy(x.begin() + size(), x.end(), finish);                                                                                                      // unintialized_copy()
    }
    finish = start + x.size();
  }
  return *this;
}

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {           // insert_aux 还会被其他函数调用（push_back） 
  if (finish != end_of_storage) { // 还有备用空间 
     // 备用空间起始建构一个元素，并以vector最后一个元素的值设为其初值
    construct(finish, *(finish - 1)); 
    ++finish;  //调整水位 
    T x_copy = x;
    copy_backward(position, finish - 2, finish - 1);
    *position = x_copy;
  }
  else { // 无备用空间 
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1;
    // 以上分配原则：若原大小为0，则分配1；
	// 若原大小不为0，则分配原大小的两倍
	// 前半段用来放置原数据 ，后半段数据用来放置新数据。
	 
    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;                      //这里本人理解为初始化 
    __STL_TRY {
      // 将原vector的内容拷贝到新的 vector 
      new_finish = uninitialized_copy(start, position, new_start);
      construct(new_finish, x); // 为新元素设置初值 
      ++new_finish; // 调整水位 
      // 拷贝安插点之后的内容（因它也可能被 insert(p,x) 呼叫） 
      new_finish = uninitialized_copy(position, finish, new_finish);
    }

#       ifdef  __STL_USE_EXCEPTIONS 
    catch(...) {
      destroy(new_start, new_finish); 
      data_allocator::deallocate(new_start, len);
      throw;
    }
#       endif /* __STL_USE_EXCEPTIONS */
    destroy(begin(), end());
    deallocate();
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;
  }
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) { // 再position 位置插入n个值为x的元素。
  if (n != 0) {
    if (size_type(end_of_storage - finish) >= n) {
      T x_copy = x;
      const size_type elems_after = finish - position;
      iterator old_finish = finish;
      if (elems_after > n) {
        uninitialized_copy(finish - n, finish, finish);
        finish += n;
        copy_backward(position, old_finish - n, old_finish);
        fill(position, position + n, x_copy);
      }
      else {
        uninitialized_fill_n(finish, n - elems_after, x_copy);   // 从finish开始n个元素为x_copy
        finish += n - elems_after;
        uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        fill(position, old_finish, x_copy);
      }
    }
    else {
      const size_type old_size = size();        
      const size_type len = old_size + max(old_size, n);
      iterator new_start = data_allocator::allocate(len);
      iterator new_finish = new_start;
      __STL_TRY {
		                                                 // position 是原 Vector中的
        new_finish = uninitialized_copy(start, position, new_start);
        new_finish = uninitialized_fill_n(new_finish, n, x);
        new_finish = uninitialized_copy(position, finish, new_finish);
      }
#         ifdef  __STL_USE_EXCEPTIONS 
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class T, class Alloc> template <class InputIterator>
void vector<T, Alloc>::range_insert(iterator pos,          // input iterator版本，将[first, last) 插入到 pos 位置。 
                                    InputIterator first, InputIterator last,
                                    input_iterator_tag) {
  for ( ; first != last; ++first) {
    pos = insert(pos, *first);// 在pos插入值为 *first 的节点                                                                                                                           // 返回的迭代器指向哪？？ 
    ++pos;
  }
}

template <class T, class Alloc> template <class ForwardIterator>
void vector<T, Alloc>::range_insert(iterator position,
                                    ForwardIterator first,
                                    ForwardIterator last,
                                    forward_iterator_tag) {
  if (first != last) {
    size_type n = 0;
    distance(first, last, n);
    if (size_type(end_of_storage - finish) >= n) {
      const size_type elems_after = finish - position;
      iterator old_finish = finish;
      if (elems_after > n) {
        uninitialized_copy(finish - n, finish, finish);
        finish += n;
        copy_backward(position, old_finish - n, old_finish);
        copy(first, last, position);
      }
      else {
        ForwardIterator mid = first;
        advance(mid, elems_after);
        uninitialized_copy(mid, last, finish);
        finish += n - elems_after;
        uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        copy(first, mid, position);
      }
    }
    else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      iterator new_start = data_allocator::allocate(len);
      iterator new_finish = new_start;
      __STL_TRY {
        new_finish = uninitialized_copy(start, position, new_start);
        new_finish = uninitialized_copy(first, last, new_finish);
        new_finish = uninitialized_copy(position, finish, new_finish);
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, 
                              const_iterator first, 
                              const_iterator last) {
  if (first != last) {
    size_type n = 0;
    distance(first, last, n);
    if (size_type(end_of_storage - finish) >= n) {
      const size_type elems_after = finish - position;
      iterator old_finish = finish;
      if (elems_after > n) {
        uninitialized_copy(finish - n, finish, finish);
        finish += n;
        copy_backward(position, old_finish - n, old_finish);
        copy(first, last, position);
      }
      else {
        uninitialized_copy(first + elems_after, last, finish);
        finish += n - elems_after;
        uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        copy(first, first + elems_after, position);
      }
    }
    else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      iterator new_start = data_allocator::allocate(len);
      iterator new_finish = new_start;
      __STL_TRY {
        new_finish = uninitialized_copy(start, position, new_start);
        new_finish = uninitialized_copy(first, last, new_finish);
        new_finish = uninitialized_copy(position, finish, new_finish);
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE 

#endif /* __SGI_STL_INTERNAL_VECTOR_H */

// Local Variables:
// mode:C++
// End:

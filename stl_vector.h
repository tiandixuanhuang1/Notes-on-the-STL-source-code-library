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
1������Ա������ const �� non-const �汾ͬʱ����ʱ��const object ֻ�ᣨֻ�ܣ����� const �汾
   non-const object ֻ�ᣨֻ�ܣ����� non-const �汾 
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
                                                             // ������������ ���ߴ����ϵΪ [start, ...., )finish, ...., )end_of_storage 
  iterator start;                                            //startָ��ǰ�洢ͷ�ڵ�
  iterator finish;                                           //finishָ��ǰ�洢��β�ڵ����һ���ڵ�  
  iterator end_of_storage;                                   //end_of_storageָ���ַ���洢��С�Ľڵ����һ���ڵ�
  
  void insert_aux(iterator position, const T& x);            //                                                                                                                   //insert_aux���� 
  
  void deallocate() {                                        //ɾ�� vector 
    if (start) data_allocator::deallocate(start, end_of_storage - start);                                                                                                         // deallocate() ?? 
    // start �ǿգ���ʾvector�ǿա� 
  }

  void fill_initialize(size_type n, const T& value) {         // ��ʼ��vector��n Ϊn��Ԫ�أ���finish �� end_of_storage ָ��ͬһ���ڵ㡣 
    start = allocate_and_fill(n, value);
    finish = start + n;
    end_of_storage = finish;
  }
public:
  iterator begin() { return start; }                        // ����start������ 
  const_iterator begin() const { return start; }            // ���� start ��������  const_itertor =  const value_type*
  iterator end() { return finish; }                         // ���� finish ������ 
  const_iterator end() const { return finish; }             // ���� finish ��������  const_itertor =  const value_type*
  reverse_iterator rbegin() { return reverse_iterator(end()); } // �������������� 
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(end()); 
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(begin()); 
  }
  size_type size() const { return size_type(end() - begin()); } // ���� vector ��Ԫ�ض��٣�size_type() Ϊǿ��ת���� 
  size_type max_size() const { return size_type(-1) / sizeof(T); } // vector�������ܴ洢��Ԫ�ض���  ������С�� 
  size_type capacity() const { return size_type(end_of_storage - begin()); } // vector ���ܴ洢����Ԫ�ظ����������� 
  bool empty() const { return begin() == end(); }               // �ж� vector �Ƿ�Ϊ�� 
  reference operator[](size_type n) { return *(begin() + n); }  // ���ص� n ��Ԫ�ص�ֵ  reference = value_type& 
  const_reference operator[](size_type n) const { return *(begin() + n); } //  ���ص� n ��Ԫ�ص�ֵ  const_reference = const value_type&

																		// ����Ϊ���캯�� 
  vector() : start(0), finish(0), end_of_storage(0) {}                  // ��ԭʼ�İ汾 
  vector(size_type n, const T& value) { fill_initialize(n, value); }    // ���� size_type���͵� n ��Ϊvalue����ֵ�� vector 
  vector(int n, const T& value) { fill_initialize(n, value); }          // ���� int �� n��value��ֵ�� vector 
  vector(long n, const T& value) { fill_initialize(n, value); }         // ���� long �� n�� value ��ֵ�� vector 
  explicit vector(size_type n) { fill_initialize(n, T()); }             // �޷���ʽת���Ĺ��캯��
  																		// end ctor 

  vector(const vector<T, Alloc>& x) {                                   // �Թ���õ�vector x ����ʼ���� vector                                                                       // allocate_and_copy ���� 
    start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
    finish = start + (x.end() - x.begin());
    end_of_storage = finish;
  }
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>                                        // ���캯�� ��input iterator�� 
  vector(InputIterator first, InputIterator last) :                     // [first, last)
    start(0), finish(0), end_of_storage(0)
  {
    range_initialize(first, last, iterator_category(first));                                                                                                                          // range_initialize() 
  }
#else /* __STL_MEMBER_TEMPLATES */
  vector(const_iterator first, const_iterator last) {                   // ���캯����const iterator�� 
    size_type n = 0;
    distance(first, last, n);
    start = allocate_and_copy(n, first, last);
    finish = start + n;
    end_of_storage = finish;
  }
#endif /* __STL_MEMBER_TEMPLATES */
  ~vector() {                                                           // �������� 
    destroy(start, finish);
    deallocate();
  }
  vector<T, Alloc>& operator=(const vector<T, Alloc>& x);               // ��ֵ�������� x ��ֵ���� vector 
   
  void reserve(size_type n) {                                           // �� vector ������������ n ��nӦ�ô���capacity()��
  	// ��¼ old_size�������µ� vector�����ϵ� vector ��ֵ��ֵ���µ�vector�ϡ�����ͷ�ڵ�ĵ�����.
	// ɾ���ϵ� vector
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
  
  reference front() { return *begin(); }                   // �õ�ָ��ͷ�ڵ��ֵ 
  const_reference front() const { return *begin(); }       //  ��� const object �İ汾 
  reference back() { return *(end() - 1); }                // �������һ��������ֵ 
  const_reference back() const { return *(end() - 1); }    // ��� const object �İ汾 
  void push_back(const T& x) {                             // ��vectorĩβ����ֵΪ x �Ľڵ� 
    if (finish != end_of_storage) {
      construct(finish, x);
      ++finish;
    }
    else
      insert_aux(end(), x);
  }
  void swap(vector<T, Alloc>& x) {                        // ���� vector �� x ����                                                                                                  //__STD::swap() 
	__STD::swap(start, x.start);                           // ����һ�� vector ������ iterator ���ɣ�����ֻҪ��������vector��iterator����Ϊ���������� vector 
    __STD::swap(finish, x.finish);
    __STD::swap(end_of_storage, x.end_of_storage);
  }
  
  iterator insert(iterator position, const T& x) {         // �� position λ�ò���ֵΪ x �Ľڵ� 
    size_type n = position - begin();//����ͷ���ľ��� 
    if (finish != end_of_storage && position == end()) {
	// �� vector û����(finish != end_of_storage) �� position Ϊ end()��=finish����ֱ�Ӳ��ں���          
	// ������Ϊ���Ե���push_back()���������ǵ���push_back��������Ҫ����insert_aux������Ч�ʽ���
      construct(finish, x);
      ++finish;
    }
    else
    // ������� insert_aux()���� 
      insert_aux(position, x);                                                                                                                                                       // insert_aux() 
    return begin() + n;
  }
  
  iterator insert(iterator position) { return insert(position, T()); } // Ϊ�����ض���ֵ�汾�����Կ�����������ֵΪһ����ʱ���� T() 
  
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last) {  //������Ϊinput iterator���ͣ���[first, last) �嵽λ�� position                                          //range_insert() 
    range_insert(position, first, last, iterator_category(first));
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator position,                                        // ������Ϊ const_iterator �汾��const_iterator = const value_type*                                            
              const_iterator first, const_iterator last);
#endif /* __STL_MEMBER_TEMPLATES */

  void insert (iterator pos, size_type n, const T& x);                  // ��posλ�ò���n��ֵΪx�Ľڵ� ��nΪsize_type���ͣ� 
  void insert (iterator pos, int n, const T& x) {                       // ��posλ�ò���n��ֵΪx�Ľڵ� ��nΪint���ͣ�
    insert(pos, (size_type) n, x);// ǿ��תΪsize_type���� 
  }
  void insert (iterator pos, long n, const T& x) {                     // ��posλ�ò���n��ֵΪx�Ľڵ� ��nΪlong���ͣ�
    insert(pos, (size_type) n, x);// ǿ��תΪsize_type���� 
  }

  void pop_back() {                                                    // ɾȥβ�ڵ㣨finish�� 
    --finish;
    destroy(finish);              //�ܷ�ʹ����� destroy(end());
  }
  
  iterator erase(iterator position) {                                 // ɾȥ������ pos ָ��Ľڵ㣬����Ϊ copy() ���� 
    if (position + 1 != end())
      copy(position + 1, finish, position);                                                                                                                                           // copy() ���� 
    --finish;
    destroy(finish);
    return position;
  }
  iterator erase(iterator first, iterator last) {                    // ɾȥ [first, last)֮��Ľڵ�
  	// ��[last, finish) ������ first λ��
	// ������ɾ�� finish ֮��Ľڵ�
	// ���ı� finish ��ָ�Ľڵ� 
    iterator i = copy(last, finish, first);
    destroy(i, finish);
    finish = finish - (last - first);
    return first;
  }
  void resize(size_type new_size, const T& x) {                     // �� vector �������ô�С
    // �� new_size < size() ��ɾ����Ĳ���
	// ����� new_size - size() ��ֵΪ x �Ľڵ� 
    if (new_size < size()) 
      erase(begin() + new_size, end());
    else
      insert(end(), new_size - size(), x);
  }
  void resize(size_type new_size) { resize(new_size, T()); } 
  void clear() { erase(begin(), end()); }                        // ���vector 

protected:
  iterator allocate_and_fill(size_type n, const T& x) {           // �²�Ӧ���� �����СΪ n ֵȫΪ x �� vector                                                                       //allocate_and_fill() 
    iterator result = data_allocator::allocate(n);
    __STL_TRY {
      uninitialized_fill_n(result, n, x);                                                                                                                                             // uninitialized_fill_n() 
      return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class ForwardIterator>                              // ��� forward iterator 
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
  iterator allocate_and_copy(size_type n,                       // ��� const_iterator �汾 
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
  // �������������ĺ�����ȸ��ӻ�����
  void range_initialize(InputIterator first, InputIterator last,  // [first, last) ֮��Ľڵ����� push_back �� vector�� ��� input iterator �汾 
                        input_iterator_tag) {
    for ( ; first != last; ++first)
      push_back(*first);
  }

  // This function is only called by the constructor.  We have to worry 
  //  about resource leaks, but not about maintaining invariants.
  // �˺������ɹ��캯�����á����Ǳ��뵣����Դй©��������ά����������
  template <class ForwardIterator>
  void range_initialize(ForwardIterator first, ForwardIterator last,    // *****���캯�����ã� ʹ�� [first, last) ��ʼ�� vector�� ���ú���������ĺ���ʵ�ַ�ʽ����ͬ�� 
                        forward_iterator_tag) {
    size_type n = 0;
    distance(first, last, n);                                                                                                                                                    // distance()
	//  �²⣺n = first - last 
    start = allocate_and_copy(n, first, last);
    // ���� vector ���ڴ� 
    finish = start + n;
    // finishָ��ĵ����� 
    end_of_storage = finish;
  }

  template <class InputIterator>
  void range_insert(iterator pos,                                     //��� intput iterator ���� [first, last) �Ľڵ���뵽λ�� pos  
                    InputIterator first, InputIterator last,
                    input_iterator_tag);

  template <class ForwardIterator>
  void range_insert(iterator pos,                                     //��� forward iterator ���� [first, last) �Ľڵ���뵽λ�� pos
                    ForwardIterator first, ForwardIterator last,
                    forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */
};

template <class T, class Alloc>
inline bool operator==(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {   // �ж����� vector �Ƿ���ȡ� 
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());                                                                                                                                         // equal() 
  // ��ȵļ���ָ�꣬1��size() 2�� ?????                     
}

template <class T, class Alloc>
inline bool operator<(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {    // vector �ȴ�С 
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());                                                                                                             // �ȴ�С���������� 
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class T, class Alloc>
inline void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) { // �²� x �� y ����vector���� 
  x.swap(y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {   // ��ֵ���� 
  if (&x != this) {// ��Ҫ x ��Ϊ�� vector 
    if (x.size() > capacity()) {
		// x �Ĵ�С���� vector ������ size() = begin() - end() = start - finish 
		// �����µ� vector, �����µ�vector ͷ�ڵ�ĵ����� tmp 
		// ɾ��ԭ���� vector 
		// ��������ǰvector ��start��Ϊ tmp 
		// end_of_storage = start + x.size() 
      iterator tmp = allocate_and_copy(x.end() - x.begin(),                                                                                                      //allocate_and_copy()
                                       x.begin(), x.end());
      destroy(start, finish);                                    																								// destroy() 
      deallocate(); 																																			 // deallocate() 
      start = tmp; 
      end_of_storage = start + (x.end() - x.begin());  
    }
    else if (size() >= x.size()) {
    	// �� vector �� size ���� x.size()
		// ��[x.begin(), x.end()) ������begin()�����ص����� i 
		// ɾ�� vector �� i �� finish ֮��Ľڵ� 
      iterator i = copy(x.begin(), x.end(), begin());
      destroy(i, finish);
    }
    else {
    	// size() < x.size() <= capacity()
		//  �� x �������� vector 
		// ??
      copy(x.begin(), x.begin() + size(), start);                                                                                                                   // copy()                          
      uninitialized_copy(x.begin() + size(), x.end(), finish);                                                                                                      // unintialized_copy()
    }
    finish = start + x.size();
  }
  return *this;
}

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {           // insert_aux ���ᱻ�����������ã�push_back�� 
  if (finish != end_of_storage) { // ���б��ÿռ� 
     // ���ÿռ���ʼ����һ��Ԫ�أ�����vector���һ��Ԫ�ص�ֵ��Ϊ���ֵ
    construct(finish, *(finish - 1)); 
    ++finish;  //����ˮλ 
    T x_copy = x;
    copy_backward(position, finish - 2, finish - 1);
    *position = x_copy;
  }
  else { // �ޱ��ÿռ� 
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1;
    // ���Ϸ���ԭ����ԭ��СΪ0�������1��
	// ��ԭ��С��Ϊ0�������ԭ��С������
	// ǰ�����������ԭ���� �����������������������ݡ�
	 
    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;                      //���ﱾ�����Ϊ��ʼ�� 
    __STL_TRY {
      // ��ԭvector�����ݿ������µ� vector 
      new_finish = uninitialized_copy(start, position, new_start);
      construct(new_finish, x); // Ϊ��Ԫ�����ó�ֵ 
      ++new_finish; // ����ˮλ 
      // ���������֮������ݣ�����Ҳ���ܱ� insert(p,x) ���У� 
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
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) { // ��position λ�ò���n��ֵΪx��Ԫ�ء�
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
        uninitialized_fill_n(finish, n - elems_after, x_copy);   // ��finish��ʼn��Ԫ��Ϊx_copy
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
		                                                 // position ��ԭ Vector�е�
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
void vector<T, Alloc>::range_insert(iterator pos,          // input iterator�汾����[first, last) ���뵽 pos λ�á� 
                                    InputIterator first, InputIterator last,
                                    input_iterator_tag) {
  for ( ; first != last; ++first) {
    pos = insert(pos, *first);// ��pos����ֵΪ *first �Ľڵ�                                                                                                                           // ���صĵ�����ָ���ģ��� 
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

#pragma once
#include <vector>
#include <list>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include <functional>
#include <algorithm>
#include <numeric>

#include <exception>
#include <stdexcept>

namespace cpplinq
{
	template <typename T>
	struct IEnumerable : private std::vector<T>
	{
		template <typename Iter>
		IEnumerable(Iter begin, Iter end): std::vector<T>(begin, end) {};

		IEnumerable(std::vector<T>&& m): std::vector<T>(std::move(m)) {};

		IEnumerable(): std::vector<T>() {};

		IEnumerable(const std::vector<T>& vec): std::vector<T>(vec) {};

		/*=== FRIENDS ===*/

		
		template <typename ACont, typename _A, typename Cont, typename U>
		friend IEnumerable<U> LINQ(ACont);

		template <typename U>
		friend IEnumerable<U> LINQ(std::vector<U>&&);
		

		/*=== QUERY FUNCTIONS ===*/
		//		 SORTING

		// Because of complexity of implementing all the boilerplate required to implement 'ThenBy' faithfully, I opted for overloading 'OrderBy' and 'OrderByDesending' to allow for multiple V->K functions, which work just like applying ThenBy to the 'normal' OrderBy 

		template <typename F1, typename K1 = typename std::result_of<F1(T)>::type>
		IEnumerable<T>& OrderBy(F1 func1)
		{
			std::sort(this->begin(), this->end(), [&func1](T val1, T val2) {return func1(val1) < func1(val2); });
			return *this;
		}

		template <typename F1, typename F2, typename K1 = typename std::result_of<F1(T)>::type, typename K2 = typename std::result_of<F2(T)>::type>
		IEnumerable<T>& OrderBy(F1 func1, F2 func2)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2](T val1, T val2) -> bool
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);

				if (k1v1 < k1v2) { return true; }
				else if (k1v1 > k1v2) { return false; }
				else { return k2v1 < k2v2; }
			});
			return *this;
		}

		template <typename F1, typename F2, typename F3, typename K1 = typename std::result_of<F1(T)>::type, typename K2 = typename std::result_of<F2(T)>::type, typename K3 = typename std::result_of<F3(T)>::type>
		IEnumerable<T>& OrderBy(F1 func1, F2 func2, F3 func3)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2, &func3](T val1, T val2) -> bool
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);
				K3 k3v1 = func3(val1), k3v2 = func3(val2);

				if (k1v1 < k1v2) { return true; }
				else if (k1v1 > k1v2) { return false; }
				else if (k2v1 < k2v2) { return true; }
				else if (k2v1 > k2v2) { return false; }
				else { return k3v1 < k3v2; }
			});
			return *this;
		}



		template <typename F1, typename K1 = typename std::result_of<F1(T)>::type>
		IEnumerable<T>& OrderByDescending(F1 func1)
		{
			std::sort(this->begin(), this->end(), [&func1](T val1, T val2) {return func1(val1) > func1(val2); });
			return *this;
		}

		template <typename F1, typename F2, typename K1 = typename std::result_of<F1(T)>::type, typename K2 = typename std::result_of<F2(T)>::type>
		IEnumerable<T>& OrderByDescending(F1 func1, F2 func2)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2](T val1, T val2) -> bool
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);

				if (k1v1 > k1v2) { return true; }
				else if (k1v1 < k1v2) { return false; }
				else { return k2v1 > k2v2; }
			});
			return *this;
		}

		template <typename F1, typename F2, typename F3, typename K1 = typename std::result_of<F1(T)>::type, typename K2 = typename std::result_of<F2(T)>::type, typename K3 = typename std::result_of<F3(T)>::type>
		IEnumerable<T>& OrderByDescending(F1 func1, F2 func2, F3 func3)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2, &func3](T val1, T val2) -> bool
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);
				K3 k3v1 = func3(val1), k3v2 = func3(val2);

				if (k1v1 > k1v2) { return true; }
				else if (k1v1 < k1v2) { return false; }
				else if (k2v1 > k2v2) { return true; }
				else if (k2v1 < k2v2) { return false; }
				else { return k3v1 > k3v2; }
			});
			return *this;
		}



		IEnumerable<T>& Reverse()
		{
			std::reverse(this->begin(), this->end());
			return *this;
		}

		//		 SET OPERATIONS

		IEnumerable<T> Except(IEnumerable<T> other)
		{
			IEnumerable<T> ret;
			ret.reserve(this->size() + other->size() / 2); // RESERVE
			std::sort(this->begin(), this->end());
			std::sort(other.begin(), other.end());
			std::set_difference(this->begin(), this->end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T> Intersect(IEnumerable<T> other)
		{
			IEnumerable<T> ret;
			ret.reserve(this->size() + other->size() / 2); // RESERVE
			std::sort(this->begin(), this->end());
			std::sort(other.begin(), other.end());
			std::set_intersection(this->begin(), this->end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T> Union(IEnumerable<T> other)
		{
			IEnumerable<T> ret;
			ret.reserve(this->size() + other->size()); // RESERVE
			std::sort(this->begin(), this->end());
			std::sort(other.begin(), other.end());
			std::set_union(this->begin(), this->end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T>& Distinct()
		{
			std::unordered_set<T> vals;
			vals.reserve(this->size() / 2); // RESERVE
			std::for_each(this->begin(), this->end(), [&vals](T val) {if (vals.find(val) == vals.end()) { vals.insert(val); }});
			this->erase(std::copy(vals.begin(), vals.end(), this->begin()), this->end());
			return *this;
		}

		//		 FILTERING DATA

		// ordinary version 
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		IEnumerable<T>& Where(F func)
		{
			auto newend = std::copy_if(this->begin(), this->end(), this->begin(), func);
			this->erase(newend, this->end());
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T, int)>::type, bool>::value>::type, typename = void>
		IEnumerable<T>& Where(F func)
		{
			int n = 0;
			auto newend = std::copy_if(this->begin(), this->end(), this->begin(), [&n, &func](T val) {return func(val, n++); });
			this->erase(newend, this->end());
			return *this;
		}

		//		 QUANTIFIER OPERATIONS

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		bool All(F func) const
		{
			return std::all_of(this->begin(), this->end(), func);
		}

		bool Any() const
		{
			return this->size() > 0;
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		bool Any(F func) const
		{
			return std::any_of(this->begin(), this->end(), func);
		}

		bool Contains(T val) const
		{
			return std::find(this->begin(), this->end(), val) != this->end();
		}

		//		 PROJECTION OPERATORS

		// ordinary version
		template <typename F, typename R = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<!std::is_same<R, T>::value>::type>
		IEnumerable<R> Select(F func)
		{
			IEnumerable<R> ret;
			ret.reserve(this->size()); // RESERVE, UNCHANGABLE
			std::transform(this->begin(), this->end(), std::back_inserter(ret), func);
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename R = typename std::result_of<F(T, int)>::type, typename _A = typename std::enable_if<!std::is_same<R, T>::value>::type, typename = void>
		IEnumerable<R> Select(F func)
		{
			IEnumerable<R> ret;
			ret.reserve(this->size()); // RESERVE, UNCHANGABLE
			int n = 0;
			std::transform(this->begin(), this->end(), std::back_inserter(ret), [&n, &func](T val) {return func(val, n++); });
			return ret;
		}
		
		// ordinary version, R == T
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, T>::value>::type, typename = void, typename = void, typename = void>
		IEnumerable<T>& Select(F func)
		{
			std::transform(this->begin(), this->end(), this->begin(), func);
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element, R == T
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T, int)>::type, T>::value>::type, typename = void, typename = void, typename = void, typename = void>
		IEnumerable<T>& Select(F func)
		{
			int n = 0;
			std::transform(this->begin(), this->end(), this->begin(), [&n, &func](T val) {return func(val, n++); });
			return *this;
		}

		// ordinary version
		template <typename F, typename RC = typename std::result_of<F(T)>::type, typename R = typename RC::value_type>
		IEnumerable<R> SelectMany(F func)
		{
			IEnumerable<R> ret;
			IEnumerable<RC> inter;
			ret.reserve(this->size()); // RESERVING
			inter.reserve(this->size()); // RESERVING, unavoidable
			std::transform(this->begin(), this->end(), std::back_inserter(inter), func);
			std::for_each(inter.begin(), inter.end(), [&ret](RC& rc) {std::copy(rc.begin(), rc.end(), std::back_inserter(ret)); });
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename RC = typename std::result_of<F(T, int)>::type, typename R = typename RC::value_type, typename = void>
		IEnumerable<R> SelectMany(F func)
		{
			IEnumerable<R> ret;
			IEnumerable<RC> inter;
			ret.reserve(this->size()); // RESERVING
			inter.reserve(this->size()); // RESERVING, unavoidable
			int n = 0;
			std::transform(this->begin(), this->end(), std::back_inserter(inter), [&n, &func](T val) {return func(val, n++); });
			std::for_each(inter.begin(), inter.end(), [&ret](RC& rc) {std::copy(rc.begin(), rc.end(), std::back_inserter(ret)); });
			return ret;
		}

		// ordinary version with result selector
		template <typename F, typename P, typename RC = typename std::result_of<F(T)>::type, typename R = typename std::result_of<P(T, typename RC::value_type)>::type>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			IEnumerable<R> ret;
			std::unordered_map<T, RC> inter;
			ret.reserve(this->size()); // RESERVING
			inter.reserve(this->size()); // RESERVING, unavoidable
			std::for_each(this->begin(), this->end(), [&inter, &func](T val) {inter[val] = func(val); });
			std::for_each(inter.begin(), inter.end(), [&ret, &result_selector](std::pair<T, RC>& rc)
			{
				std::transform(rc.second.begin(), rc.second.end(), std::back_inserter(ret), [&rc, &result_selector](typename RC::value_type val) 
				{
					return result_selector(rc.first, val); 
				}); 
			});
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element with result selector
		template <typename F, typename P, typename RC = typename std::result_of<F(T, int)>::type, typename R = typename std::result_of<P(T, typename RC::value_type)>::type, typename = void>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			IEnumerable<R> ret;
			std::unordered_map<T, RC> inter;
			ret.reserve(this->size()); // RESERVING
			inter.reserve(this->size()); // RESERVING, unavoidable
			int n = 0;
			std::for_each(this->begin(), this->end(), [&inter, &func, &n](T val) {inter[val] = func(val, n++); });
			std::for_each(inter.begin(), inter.end(), [&ret, &result_selector](std::pair<T, RC>& rc)
			{
				std::transform(rc.second.begin(), rc.second.end(), std::back_inserter(ret), [&rc, &result_selector](typename RC::value_type val)
				{
					return result_selector(rc.first, val);
				});
			});
			return ret;
		}

		//		 PARTITIONING DATA

		IEnumerable<T>& Skip(int n)
		{
			this->erase(this->begin(), this->begin() + std::min(n, (int)this->size()));
			return *this;
		}
		
		IEnumerable<T>& Take(int n)
		{
			this->erase(this->begin() + std::min(n, (int)this->size()), this->end());
			return *this;
		}

		// ordinary version
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		IEnumerable<T>& TakeWhile(F func)
		{
			this->erase(std::find_if_not(this->begin(), this->end(), func), this->end());
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T, int)>::type, bool>::value>::type, typename = void>
		IEnumerable<T>& TakeWhile(F func)
		{
			int n = 0;
			this->erase(std::find_if_not(this->begin(), this->end(), [&func, &n](T t) {return func(t, n++); }), this->end());
			return *this;
		}

		// ordinary version
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		IEnumerable<T>& SkipWhile(F func)
		{
			this->erase(this->begin(), std::find_if_not(this->begin(), this->end(), func));
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T, int)>::type, bool>::value>::type, typename = void>
		IEnumerable<T>& SkipWhile(F func)
		{
			int n = 0;
			this->erase(this->begin(), std::find_if_not(this->begin(), this->end(), [&func, &n](T t) {return func(t, n++); }));
			return *this;
		}

		//		 JOIN OPERATORS

		template <typename FOuterKey, typename FInnerKey, typename FJoin, typename TInner, typename TOuter = T, typename TKey = typename std::result_of<FOuterKey(TOuter)>::type, typename TJoined = typename std::result_of<FJoin(TOuter, TInner)>::type, typename _AInnerKey = typename std::enable_if<std::is_same<typename std::result_of<FInnerKey(TInner)>::type, TKey>::value>::type>
		IEnumerable<TJoined> Join(const IEnumerable<TInner>& innerkeys, FOuterKey outerkeyselector, FInnerKey innerkeyselector, FJoin joiner)
		{
			IEnumerable<TJoined> ret; 
			std::unordered_map<TKey, TInner> innervals;
			ret.reserve(this->size()); // RESERVE
			innervals.reserve(this->size()); // RESERVE

			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, &innerkeyselector](TInner innerval) {innervals[innerkeyselector(innerval)] = innerval; });
			std::for_each(this->begin(), this->end(), [&innervals, &outerkeyselector, &joiner, &ret](TOuter outerval)
			{
				TKey key = outerkeyselector(outerval);

				if (innervals.count(key))
				{
					ret.push_back(joiner(outerval, innervals[key]));
				}
			});
			return ret;
		}

		template <typename FOuterKey, typename FInnerKey, typename FJoin, typename TInner, typename TOuter = T, typename TKey = typename std::result_of<FOuterKey(TOuter)>::type, typename TJoined = typename std::result_of<FJoin(TOuter, IEnumerable<TInner>)>::type, typename _AInnerKey = typename std::enable_if<std::is_same<typename std::result_of<FInnerKey(TInner)>::type, TKey>::value>::type>
		IEnumerable<TJoined> GroupJoin(const IEnumerable<TInner>& innerkeys, FOuterKey outerkeyselector, FInnerKey innerkeyselector, FJoin joiner)
		{
			IEnumerable<TJoined> ret;
			std::unordered_multimap<TKey, TInner> innervals;
			ret.reserve(this->size()); // RESERVE
			innervals.reserve(this->size()); // RESERVE

			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, &innerkeyselector](TInner innerval) {innervals.insert(std::make_pair(innerkeyselector(innerval), innerval)); });
			std::for_each(this->begin(), this->end(), [&innervals, &outerkeyselector, &joiner, &ret](TOuter outerval)
			{
				TKey key = outerkeyselector(outerval);
				
				if (innervals.count(key))
				{
					auto range = innervals.equal_range(key);
					IEnumerable<TInner> block;
					std::for_each(range.first, range.second, [&block](std::pair<TKey, TInner> p) {block.push_back(p.second); });
					ret.push_back(joiner(outerval, block));
				}
			});
			return ret;
		}

		//		 GROUPING DATA

		// ¯\_(ツ)_/¯ - neither Lookup<TKey, TElement> nor IGrouping<TKey, TElement> can be meaningfully recreated in C++, the closest thing to them is ToMap and ToUnsortedMap below

		//		 GENERATION OPERATORS

		IEnumerable<T>& DefaultIfEmpty()
		{
			if (this->Any()) { return *this; }
			else { this->push_back(T()); return *this; }
		}

		IEnumerable<T>& DefaultIfEmpty(T val)
		{
			if (this->Any()) { return *this; }
			else { this->push_back(val); return *this; }
		}

		static IEnumerable<T> Empty()
		{
			return IEnumerable<T>();
		}

		static IEnumerable<T> Range(T init, std::size_t count, T inc = T(1))
		{
			IEnumerable<T> ret;		

			for (std::size_t i = 0; i < count; i++)
			{
				ret.push_back(init);
				init += inc;
			}

			return ret;
		}

		static IEnumerable<T> Repeat(T val, std::size_t count)
		{
			IEnumerable<T> ret;
			for (std::size_t i = 0; i < count; i++)
			{
				ret.push_back(val);
			}
			return ret;
		}

		//		 EQUALITY OPERATORS	

		bool SequenceEqual(const IEnumerable<T>& other) const
		{
			return std::equal(this->begin(), this->end(), other->begin());
		}

		//		 ELEMENT OPERATORS

		T ElementAt(int index) const
		{
			if (index < this->size()) {return *this[index]; }
			else throw std::out_of_range("IEnumberable<T>::ElementAt(int index) | Index out of range.");
		}

		T ElementAtOrDefault(int index) const
		{
			if (index < this->size()) {return *this[index]; }
			else return T();
		}

		T First() const
		{
			if (this->Any()) {return this->front(); }
			else throw std::length_error("IEnumberable<T>::First() | IEnumerable<T> is empty.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T First(F func) const
		{
			auto v = std::find_if(this->begin(), this->end(), func);
			if (v == this->end())
			{
				throw std::length_error("IEnumberable<T>::First(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T FirstOrDefault() const
		{
			if (this->Any()) {return this->front(); }
			else return T();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T FirstOrDefault(F func) const
		{
			auto v = std::find_if(this->begin(), this->end(), func);
			if (v == this->end())
			{
				return T();
			}
			return *v;
		}

		T Last() const
		{
			if (this->Any()) {return this->back(); }
			else throw std::length_error("IEnumberable<T>::Last() | IEnumerable<T> is empty.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T Last(F func) const
		{
			auto v = std::find_if(this->rbegin(), this->rend(), func);
			if (v == this->rend())
			{
				throw std::length_error("IEnumberable<T>::Last(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T LastOrDefault() const
		{
			if (this->Any()) {return this->back(); }
			else return T();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T LastOrDefault(F func) const
		{
			auto v = std::find_if(this->rbegin(), this->rend(), func);
			if (v == this->rend())
			{
				return T();
			}
			return *v;
		}

		T Single() const
		{
			if (this->size() == 1) { return *this[0]; }
			else throw std::length_error("IEnumberable<T>::Single() | Size isn't exactly 1.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T Single(F func) const
		{
			T retval;
			int n = 0;

			std::for_each(this->begin(), this->end(), [&retval, &func, &n](T val) 
			{
				if (func(val) == true) 
				{ 
					if (n == 0) { n++; retval = val; } 
					else 
					{ 
						throw std::length_error("IEnumberable<T>::Single(F func) | More than 1 element matched the selection function."); 
					} 
				}
			});

			if (n == 1) { return retval; }
			throw std::length_error("IEnumberable<T>::Single(F func) | No elements matched the selection function.");
		}

		T SingleOrDefault() const
		{
			if (this->size() == 1) { return *this[0]; }
			else if (this->size() == 0) { return T(); }
			else throw std::length_error("IEnumberable<T>::SingleOrDefault() | Size of IEnumerable<T> is bigger than 1.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T SingleOrDefault(F func) const
		{
			T retval;
			int n = 0;

			std::for_each(this->begin(), this->end(), [&retval, &func, &n](T val) 
			{
				if (func(val) == true) 
				{ 
					if (n == 0) { n++; retval = val; } 
					else 
					{ 
						throw std::length_error("IEnumberable<T>::Single(F func) | More than one element matched the selection function.");
					} 
				}
			});

			if (n == 1) { return retval; }
			else return T();
		}

		//		CONVERTING DATA TYPES

		// AsEnumerable, AsQueryable, OfType and ToLookup methods can't be meaningfully implemented either because of the type system or because of redundancy

		template<typename R, typename _A = typename std::enable_if<std::is_convertible<T, R>::value>::type>
		IEnumerable<R> Cast()
		{
			IEnumerable<R> ret;
			ret.reserve(this->size()); // RESERVE
			std::transform(this->begin(), this->end(), std::back_inserter(ret), [](T val) {return (R)val; });
			return ret;
		}

		std::vector<T> ToVector()
		{
			return std::vector<T>(std::move(*this));
		}

		std::vector<T> ToVectorNotConsume() const
		{
			std::vector<T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::list<T> ToList() const
		{
			std::list<T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::forward_list<T> ToForwardList() const
		{
			std::forward_list<T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::set<T> ToSet() const
		{
			std::set<T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.insert(val); });
			return ret;
		}

		std::unordered_set<T> ToUnorderedSet() const
		{
			std::unordered_set<T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.insert(val); });
			return ret;
		}

		template <typename F, typename K = typename std::result_of<F(T)>::type>
		std::map<K, T> ToMap(F func) const
		{
			std::map<K, T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret, &func](T val) {ret[func(val)] = val; });
			return ret;
		}

		template <typename F, typename K = typename std::result_of<F(T)>::type>
		std::unordered_map<K, T> ToUnorderedMap(F func) const
		{
			std::unordered_map<K, T> ret;
			ret.reserve(this->size()); // RESERVE
			std::for_each(this->begin(), this->end(), [&ret, &func](T val) {ret[func(val)] = val; });
			return ret;
		}

		//		CONCATENATION OPERATORS

		IEnumerable<T>& Concat(const IEnumerable<T>& other)
		{
			std::copy(other.begin(), other.end(), std::back_inserter(*this));
			return *this;
		}

		//		AGGREGATION OPERATORS

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T, T)>::type, T>::value>::type>
		T Aggregate(F func) const
		{
			return std::accumulate(this->begin(), this->end(), T(), func);
		}

		template <typename F, typename V, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(V, T)>::type, V>::value>::type>
		V Aggregate(V val, F func) const
		{
			return std::accumulate(this->begin(), this->end(), val, func);
		}

		template <typename F, typename R, typename V, typename E = typename std::result_of<R(V)>, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(V, T)>::type, V>::value>::type>
		E Aggregate(V val, F func, R retfunc) const
		{
			return retfunc(std::accumulate(this->begin(), this->end(), val, func));
		}

		template <typename F, typename N = typename std::result_of<F(T)>, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Average(F func) const
		{
			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Average(F func) | IEnumerable<T> is empty."); }

			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val / this->size();
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Average() const
		{
			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Average() | IEnumerable<T> is empty."); }

			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val](T v) {val += v; });
			return val / this->size();
		}

		std::size_t Count() const noexcept
		{
			return this->size();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		std::size_t Count(F func) const
		{
			return std::accumulate(this->begin(), this->end(), 0, [&func](int a, T val) {if (func(val)) { return ++a; } else return a; });
		}

		template <typename F, typename N = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Max(F func) const
		{
			N val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::max(val, func(v)); }});
			return val;
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Max() const
		{
			T val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::max(val, v); }});
			return val;
		}

		template <typename F, typename N = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Min(F func) const
		{
			N val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::min(val, func(v)); }});
			return val;
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Min() const
		{
			T val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::min(val, v); }});
			return val;
		}

		template <typename F, typename N = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Sum(F func) const
		{
			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val;
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Sum() const
		{
			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val](T v) {val += v; });
			return val;
		}
	};

	//template <typename Cont, bool Const = std::is_const<Cont>::value, typename T = std::decay<decltype(*(std::declval<Cont>().begin()))>::type>
	template <typename Cont, bool Const, typename T>
	struct RefIEnumerable
	{
	private:
		RefIEnumerable(Cont& cont) : ref(cont) {};
		RefIEnumerable() = delete;		
	//	RefIEnumerable(const RefIEnumerable<Cont, Const, T>& other) = delete;
	//	RefIEnumerable(RefIEnumerable<Cont, Const, T>&& other) = delete;
	//	RefIEnumerable<Cont, Const, T>& operator=(const RefIEnumerable<Cont, Const, T>& other) = delete;
	//	RefIEnumerable<Cont, Const, T>& operator=(RefIEnumerable<Cont, Const, T>&& other) = delete;
	public:

		template <typename A, typename _A, typename C, bool B, typename U>
		friend RefIEnumerable<C, B, U> LINQ(A);

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T, T)>::type, T>::value>::type>
		T Aggregate(F func) const
		{
			return std::accumulate(ref.begin(), ref.end(), T(), func);
		}

		template <typename F, typename V, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(V, T)>::type, V>::value>::type>
		V Aggregate(V val, F func) const
		{
			return std::accumulate(ref.begin(), ref.end(), val, func);
		}

		template <typename F, typename R, typename V, typename E = typename std::result_of<R(V)>, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(V, T)>::type, V>::value>::type>
		E Aggregate(V val, F func, R retfunc) const
		{
			return retfunc(std::accumulate(ref.begin(), ref.end(), val, func));
		}

		template <typename F, typename N = typename std::result_of<F(T)>, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Average(F func) const
		{
			N val = N(0);
			std::for_each(ref.begin(), ref.end(), [&val, &func](T v) {val += func(v); });
			return val / this->Count();
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Average() const
		{
			T val = T(0);
			std::for_each(ref.begin(), ref.end(), [&val](T v) {val += v; });
			return val / this->Count();
		}

		std::size_t Count() const
		{
			return ref.end() - ref.begin();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		std::size_t Count(F func) const
		{
			return std::accumulate(ref.begin(), ref.end(), 0, [&func](int a, T val) {if (func(val)) { return ++a; } else return a; });
		}

		template <typename F, typename N = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Max(F func) const
		{
			N val;
			bool first = false;
			std::for_each(ref.begin(), ref.end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::max(val, func(v)); }});
			return val;
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Max() const
		{
			T val;
			bool first = false;
			std::for_each(ref.begin(), ref.end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::max(val, v); }});
			return val;
		}

		template <typename F, typename N = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Min(F func) const
		{
			N val;
			bool first = false;
			std::for_each(ref.begin(), ref.end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::min(val, func(v)); }});
			return val;
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Min() const
		{
			T val;
			bool first = false;
			std::for_each(ref.begin(), ref.end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::min(val, v); }});
			return val;
		}

		template <typename F, typename N = typename std::result_of<F(T)>::type, typename _A = typename std::enable_if<std::is_arithmetic<N>::value>::type>
		N Sum(F func) const
		{
			N val = N(0);
			std::for_each(ref.begin(), ref.end(), [&val, &func](T v) {val += func(v); });
			return val;
		}

		template <typename _A = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T Sum() const
		{
			T val = T(0);
			std::for_each(ref.begin(), ref.end(), [&val](T v) {val += v; });
			return val;
		}

		std::vector<T> ToVector() const
		{
			std::vector<T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::list<T> ToList() const
		{
			std::list<T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::forward_list<T> ToForwardList() const
		{
			std::forward_list<T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::set<T> ToSet() const
		{
			std::set<T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret](T val) {ret.insert(val); });
			return ret;
		}

		std::unordered_set<T> ToUnorderedSet() const
		{
			std::unordered_set<T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret](T val) {ret.insert(val); });
			return ret;
		}

		template <typename F, typename K = typename std::result_of<F(T)>::type>
		std::map<K, T> ToMap(F func) const
		{
			std::map<K, T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret, &func](T val) {ret[func(val)] = val; });
			return ret;
		}

		template <typename F, typename K = typename std::result_of<F(T)>::type>
		std::unordered_map<K, T> ToUnorderedMap(F func) const
		{
			std::unordered_map<K, T> ret;
			std::for_each(ref.begin(), ref.end(), [&ret, &func](T val) {ret[func(val)] = val; });
			return ret;
		}

		T ElementAt(int index) const
		{
			if (index < this->Count()) { return *(ref.begin() + index); }
			else throw std::out_of_range("RefIEnumberable<T>::ElementAt(int index) | Index out of range.");
		}

		T ElementAtOrDefault(int index) const
		{
			if (index < this->Count()) { return *(ref.begin() + index); }
			else return T();
		}

		T First() const
		{
			if (this->Any()) { return *ref.begin(); }
			else throw std::length_error("RefIEnumberable<T>::First() | The collection referred to by the RefIEnumerable<T> is empty.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T First(F func) const
		{
			auto v = std::find_if(ref.begin(), ref.end(), func);
			if (v == ref.end())
			{
				throw std::length_error("RefIEnumberable<T>::First(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T FirstOrDefault() const
		{
			if (this->Any()) { return *ref.begin(); }
			else return T();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T FirstOrDefault(F func) const
		{
			auto v = std::find_if(ref.begin(), ref.end(), func);
			if (v == ref.end())
			{
				return T();
			}
			return *v;
		}

		T Last() const
		{
			if (this->Any()) { return *(ref.rbegin()); }
			else throw std::length_error("RefIEnumberable<T>::Last() | The collection referred to by the RefIEnumerable<T> is empty.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T Last(F func) const
		{
			auto v = std::find_if(ref.rbegin(), ref.rend(), func);
			if (v == ref.rend())
			{
				throw std::length_error("RefIEnumberable<T>::Last(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T LastOrDefault() const
		{
			if (this->Any()) { return *(ref.rbegin()); }
			else return T();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T LastOrDefault(F func) const
		{
			auto v = std::find_if(ref.rbegin(), ref.rend(), func);
			if (v == ref.rend())
			{
				return T();
			}
			return *v;
		}

		T Single() const
		{
			if (this->Count() == 1) { return *(ref.begin()); }
			else throw std::length_error("RefIEnumberable<T>::Single() | Size isn't exactly 1.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T Single(F func) const
		{
			T retval;
			int n = 0;

			std::for_each(ref.begin(), ref.end(), [&retval, &func, &n](T val) 
			{
				if (func(val) == true) 
				{ 
					if (n == 0) { n++; retval = val; } 
					else 
					{ 
						throw std::length_error("RefIEnumberable<T>::Single(F func) | More than 1 element matched the selection function.");
					} 
				}
			});

			if (n == 1) { return retval; }
			throw std::length_error("RefIEnumberable<T>::Single(F func) | No elements matched the selection function.");
		}

		T SingleOrDefault() const
		{
			if (this->Count() == 1) { return *(ref.begin()); }
			else if (this->Count() == 0) { return T(); }
			else throw std::length_error("RefIEnumberable<T>::SingleOrDefault() | Size is bigger than 1.");
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		T SingleOrDefault(F func) const
		{
			T retval;
			int n = 0;

			std::for_each(ref.begin(), ref.end(), [&retval, &func, &n](T val) 
			{
				if (func(val) == true) 
				{ 
					if (n == 0) { n++; retval = val; }
					else 
					{ 
						throw std::length_error("RefIEnumberable<T>::SingleOrDefault(F func) | More than 1 element matched the selection function.");
					} 
				}
			});

			if (n == 1) { return retval; }
			else return T();
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		bool All(F func) const
		{
			return std::all_of(ref.begin(), ref.end(), func);
		}

		bool Any() const
		{
			return this->Count() > 0;
		}

		template <typename F, typename _A = typename std::enable_if<std::is_same<typename std::result_of<F(T)>::type, bool>::value>::type>
		bool Any(F func) const
		{
			return std::any_of(ref.begin(), ref.end(), func);
		}

		bool Contains(T val) const
		{
			return std::find(ref.begin(), ref.end(), val) != ref.end();
		}

		IEnumerable<T> Copy()
		{
			return IEnumerable<T>(ref.begin(), ref.end());
		}

	/*	template <typename _A1 = std::enable_if<std::is_same<Cont, std::vector<T>>::value>::type, typename _A2 = std::enable_if<!Const>::type>
		IEnumerable<T> Consume()
		{
			return IEnumerable<T>(std::move(ref));
		} */

	private:
		Cont& ref;
	};

	//typename ArgCont, typename IsRef = std::enable_if<std::is_lvalue_reference<ArgCount>::value>::type, typename Cont = std::decay<ArgCont>::type
	template <typename ACont, typename _A = typename std::enable_if<std::is_lvalue_reference<ACont>::value>::type, typename Cont = typename std::remove_reference<ACont>::type, bool Const = std::is_const<Cont>::value, typename T = typename std::decay<decltype(*(std::declval<Cont>().begin()))>::type>
	RefIEnumerable<Cont, Const, T> LINQ(ACont cont)
	{
		static_assert(std::is_same<decltype(*(std::declval<Cont>().begin())), decltype(*(std::declval<Cont>().end()))>::value, "Types of begin() and end() iterators must be the same.");
		return RefIEnumerable<Cont, Const, T>(cont);
	}
	
	template <typename ACont, typename _A = typename std::enable_if<!std::is_lvalue_reference<ACont>::value>::type, typename Cont = typename std::remove_reference<ACont>::type, typename T = typename std::decay<decltype(*(std::declval<Cont>().begin()))>::type>
	IEnumerable<T> LINQ(ACont cont)
	{
		static_assert(std::is_same<decltype(*(std::declval<Cont>().begin())), decltype(*(std::declval<Cont>().end()))>::value, "Types of begin() and end() iterators must be the same.");
		IEnumerable<T> ret;
		std::for_each(cont.begin(), cont.end(), [&ret](T val) {ret.push_back(val); });
		return ret;
	}
	
	template <typename T>
	IEnumerable<T> LINQ(std::vector<T>&& cont)
	{
		return IEnumerable<T>(std::forward<std::vector<T>&&>(cont));
	}
	
}
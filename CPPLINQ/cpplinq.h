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

namespace cpplinq
{
	template <typename T>
	struct IEnumerable : std::vector<T>
	{
		template <typename Iter>
		IEnumerable(Iter begin, Iter end): std::vector<T>(begin, end) {};

		IEnumerable(std::vector<T>&& m): std::vector<T>(std::move(m)) {};

		IEnumerable() : std::vector<T>() {};

		IEnumerable(const std::vector<T>& vec) : std::vector<T>(vec) {};

		/*=== QUERY FUNCTIONS ===*/
		//		 SORTING

		// Because of complexity of implementing all the boilerplate required to implement 'ThenBy' faithfully, I opted for overloading 'OrderBy' and 'OrderByDescending' to allow for multiple V->K functions, which work just like applying ThenBy to the 'normal' OrderBy 

		template <typename F1, typename K1 = std::result_of_t<F1(T)>>
		IEnumerable<T>& OrderBy(F1 func1)
		{
			std::sort(this->begin(), this->end(), [&func1](T val1, T val2) {return func1(val1) < func1(val2); });
			return *this;
		}

		template <typename F1, typename K1 = std::result_of_t<F1(T)>, typename F2, typename K2 = std::result_of_t<F2(T)>>
		IEnumerable<T>& OrderBy(F1 func1, F2 func2)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2](T val1, T val2)
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);

				if (k1v1 < k1k2) { return true; }
				else if (k1v1 > k1k2) { return false; }
				else { return k2v1 < k2v2; }
			});
			return *this;
		}

		template <typename F1, typename K1 = std::result_of_t<F1(T)>, typename F2, typename K2 = std::result_of_t<F2(T)>, typename F3, typename K3 = std::result_of_t<F3(T)>>
		IEnumerable<T>& OrderBy(F1 func1, F2 func2, F3 func3)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2, &func3](T val1, T val2)
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);
				K3 k3v1 = func3(val1), k3v2 = func3(val2);

				if (k1v1 < k1k2) { return true; }
				else if (k1v1 > k1k2) { return false; }
				else if (k2v1 < k2k2) { return true; }
				else if (k2v1 > k2k2) { return false; }
				else { return k3v1 < k3v2; }
			});
			return *this;
		}

		template <typename F1, typename K1 = std::result_of_t<F1(T)>>
		IEnumerable<T>& OrderByDescending(F1 func1)
		{
			std::sort(this->begin(), this->end(), [&func1](T val1, T val2) {return func1(val1) > func1(val2); });
			return *this;
		}

		template <typename F1, typename K1 = std::result_of_t<F1(T)>, typename F2, typename K2 = std::result_of_t<F2(T)>>
		IEnumerable<T>& OrderByDescending(F1 func1, F2 func2)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2](T val1, T val2)
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);

				if (k1v1 > k1k2) { return true; }
				else if (k1v1 < k1k2) { return false; }
				else { return k2v1 > k2v2; }
			});
			return *this;
		}

		template <typename F1, typename K1 = std::result_of_t<F1(T)>, typename F2, typename K2 = std::result_of_t<F2(T)>, typename F3, typename K3 = std::result_of_t<F3(T)>>
		IEnumerable<T>& OrderByDescending(F1 func1, F2 func2, F3 func3)
		{
			std::sort(this->begin(), this->end(), [&func1, &func2, &func3](T val1, T val2)
			{
				K1 k1v1 = func1(val1), k1v2 = func1(val2);
				K2 k2v1 = func2(val1), k2v2 = func2(val2);
				K3 k3v1 = func3(val1), k3v2 = func3(val2);

				if (k1v1 > k1k2) { return true; }
				else if (k1v1 < k1k2) { return false; }
				else if (k2v1 > k2k2) { return true; }
				else if (k2v1 < k2k2) { return false; }
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
			std::sort(this->begin()), this->end());
			std::sort(other.begin()), other.end());
			std::set_difference(this->begin(), this->end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T> Intersect(IEnumerable<T> other)
		{
			IEnumerable<T> ret;
			std::sort(this->begin()), this->end());
			std::sort(other.begin()), other.end());
			std::set_intersection(this->begin(), this->end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T> Union(IEnumerable<T> other)
		{
			IEnumerable<T> ret;
			std::sort(this->begin()), this->end());
			std::sort(other.begin()), other.end());
			std::set_union(this->begin(), this->end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T>& Distinct()
		{
			std::unordered_set<T> vals;
			std::for_each(this->begin(), this->end(), [&vals](T val) {if (vals.find(val) == vals.end()) { vals.insert(val); }});
			this->erase(std::copy(vals.begin(), vals.end(), this->begin()), this->end());
			return *this;
		}

		//		 FILTERING DATA

		// ordinary version 
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of<F(T)>::type, bool>::value>::type>
		IEnumerable<T>& Where(F func)
		{
			auto newend = std::copy_if(this->begin(), this->end(), this->begin(), func);
			this->erase(newend, this->end());
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of<F(T, int)>::type, bool>::value>::type, typename = void>
		IEnumerable<T>& Where(F func)
		{
			int n = 0;
			auto newend = std::copy_if(this->begin(), this->end(), this->begin(), [&n, &func](T val) {return func(val, n++); });
			this->erase(newend, this->end());
			return *this;
		}

		//		 QUANTIFIER OPERATIONS

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		bool All(F func)
		{
			return std::all_of(this->begin(), this->end(), func);
		}

		bool Any()
		{
			return this->size() > 0;
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		bool Any(F func)
		{
			return std::any_of(this->begin(), this->end(), func);
		}

		bool Contains(T val)
		{
			return std::find(this->begin(), this->end(), val) != this->end();
		}

		//		 PROJECTION OPERATORS

		// ordinary version
		template <typename F, typename R = std::result_of<F(T)>::type, typename _A = std::enable_if<!std::is_same<R, T>::value>::type>
		IEnumerable<R> Select(F func)
		{
			IEnumerable<R> ret;
			std::transform(this->begin(), this->end(), std::back_inserter(ret), func);
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename R = std::result_of<F(T, int)>::type, typename _A = std::enable_if<!std::is_same<R, T>::value>::type, typename = void>
		IEnumerable<R> Select(F func)
		{
			IEnumerable<R> ret;
			int n = 0;
			std::transform(this->begin(), this->end(), std::back_inserter(ret), [&n, &func](T val) {return func(val, n++); });
			return ret;
		}
		
		// ordinary version
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of<F(T)>::type, T>::value>::type, typename = void, typename = void, typename = void>
		IEnumerable<T>& Select(F func)
		{
			std::transform(this->begin(), this->end(), this->begin(), func);
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of<F(T, int)>::type, T>::value>::type, typename = void, typename = void, typename = void, typename = void>
		IEnumerable<T>& Select(F func)
		{
			int n = 0;
			std::transform(this->begin(), this->end(), this->begin(), [&n, &func](T val) {return func(val, n++); });
			return *this;
		}

		// ordinary version
		template <typename F, typename RC = typename std::result_of<F(T)>::type, typename R = RC::value_type>
		IEnumerable<R> SelectMany(F func)
		{
			IEnumerable<R> ret;
			IEnumerable<RC> inter;
			std::transform(this->begin(), this->end(), std::back_inserter(inter), func);
			std::for_each(inter.begin(), inter.end(), [&ret](RC& rc) {std::copy(rc.begin(), rc.end(), std::back_inserter(ret)); });
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename RC = typename std::result_of<F(T, int)>::type, typename R = RC::value_type, typename = void>
		IEnumerable<R> SelectMany(F func)
		{
			IEnumerable<R> ret;
			IEnumerable<RC> inter;
			int n = 0;
			std::transform(this->begin(), this->end(), std::back_inserter(inter), [&n, &func](T val) {return func(val, n++); });
			std::for_each(inter.begin(), inter.end(), [&ret](RC& rc) {std::copy(rc.begin(), rc.end(), std::back_inserter(ret)); });
			return ret;
		}

		// ordinary version with result selector
		template <typename F, typename P, typename RC = typename std::result_of<F(T)>::type, typename R = typename std::result_of<P(T, RC::value_type)>::type>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			IEnumerable<R> ret;
			unordered_map<T, RC> inter;
			std::for_each(this->begin(), this->end(), [&inter, &func](T val) {inter[val] = func(val)});
			std::for_each(inter.begin(), inter.end(), [&ret, &result_selector](std::pair<T, RC>& rc)
			{
				std::transform(rc.second.begin(), rc.second.end(), std::back_inserter(ret), [&rc](RC::value_type val) 
				{
					return result_selector(rc.first, val); 
				}); 
			});
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element with result selector
		template <typename F, typename P, typename RC = typename std::result_of<F(T, int)>::type, typename R = typename std::result_of<P(T, RC::value_type)>::type, typename = void>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			IEnumerable<R> ret;
			unordered_map<T, RC> inter;
			int n = 0;
			std::for_each(this->begin(), this->end(), [&inter, &func, &n](T val) {inter[val] = func(val, n++); });
			std::for_each(inter.begin(), inter.end(), [&ret, &result_selector](std::pair<T, RC>& rc)
			{
				std::transform(rc.second.begin(), rc.second.end(), std::back_inserter(ret), [&rc](RC::value_type val)
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
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		IEnumerable<T>& TakeWhile(F func)
		{
			this->erase(std::find_if_not(this->begin(), this->end(), func), this->end());
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T, int)>, bool>::value>::type, typename = void>
		IEnumerable<T>& TakeWhile(F func)
		{
			int n = 0;
			this->erase(std::find_if_not(this->begin(), this->end(), [&func, &n](T t) {return func(t, n++); }), this->end());
			return *this;
		}

		// ordinary version
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		IEnumerable<T>& SkipWhile(F func)
		{
			this->erase(this->begin(), std::find_if_not(this->begin(), this->end(), func));
			return *this;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T, int)>, bool>::value>::type, typename = void>
		IEnumerable<T>& SkipWhile(F func)
		{
			int n = 0;
			this->erase(this->begin(), std::find_if_not(this->begin(), this->end(), [&func, &n](T t) {return func(t, n++); }));
			return *this;
		}

		//		 JOIN OPERATORS

		template <typename FOuterKey, typename FInnerKey, typename FJoin, typename TInner, typename TOuter = T, typename TKey = std::result_of_t<FOuterKey(TOuter)>, typename TJoined = std::result_of_t<FJoin(TOuter, TInner)>, typename _AInnerKey = std::enable_if<std::is_same<std::result_of_t<FInnerKey(TInner)>, TKey>::value>::type>
		IEnumerable<TJoined> Join(IEnumerable<TInner> innerkeys, FOuterKey outerkeyselector, FInnerKey innerkeyselector, FJoin joiner)
		{
			IEnumerable<TJoined> ret; 
			std::unordered_map<TKey, TInner> innervals;

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

		template <typename FOuterKey, typename FInnerKey, typename FJoin, typename TInner, typename TOuter = T, typename TKey = std::result_of_t<FOuterKey(TOuter)>, typename TJoined = std::result_of_t<FJoin(TOuter, IEnumerable<TInner>)>, typename _AInnerKey = std::enable_if<std::is_same<std::result_of_t<FInnerKey(TInner)>, TKey>::value>::type>
		IEnumerable<TJoined> GroupJoin(IEnumerable<TInner> innerkeys, FOuterKey outerkeyselector, FInnerKey innerkeyselector, FJoin joiner)
		{
			IEnumerable<TJoined> ret;
			std::unordered_multimap<TKey, TInner> innervals;

			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, &innerkeyselector](TInner innerval) {innervals.insert(std::make_pair(innerkeyselector(innerval), innerval)); });
			std::for_each(this->begin(), this->end(), [&innervals, &outerkeyselector, &joiner, &ret](TOuter outerval)
			{
				TKey key = outerkeyselector(outerval);
				
				if (innervals.count(key))
				{
					auto range = innervals.equal_range(key);
					IEnumerable<TInner> block;
					std::for_each(range.first, range.second, [&block](auto p) {block.push_back(p.second); });
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

			for (int i = 0; i < count; i++)
			{
				ret.push_back(init);
				init += inc;
			}

			return ret;
		}

		static IEnumerable<T> Repeat(T val, std::size_t count)
		{
			IEnumerable<T> ret;
			for (int i = 0; i < count; i++)
			{
				ret.push_back(val);
			}
			return ret;
		}

		//		 EQUALITY OPERATORS	

		bool SequenceEqual(IEnumerable<T> other)
		{
			return std::equal(this->begin(), this->end(), other->begin());
		}

		//		 ELEMENT OPERATORS

		T ElementAt(int index)
		{
			if (index < this->size())) {return *this[index]; }
			else throw std::out_of_range("Index out of range.");
		}

		T ElementAtOrDefault(int index)
		{
			if (index < this->size())) {return *this[index]; }
			else return T();
		}

		T First()
		{
			if (this->Any())) {return this->front(); }
			else throw std::exception("empty sequence");
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		T First(F func)
		{
			auto v = std::find_if(this->begin(), this->end(), func);
			if (v == this->end())
			{
				throw std::exception("no element in sequence satisfies func");
			}
			return *v;
		}

		T FirstOrDefault()
		{
			if (this->Any())) {return this->front(); }
			else return T();
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		T FirstOrDefault(F func)
		{
			auto v = std::find_if(this->begin(), this->end(), func);
			if (v == this->end())
			{
				return T();
			}
			return *v;
		}

		T Last()
		{
			if (this->Any())) {return this->back(); }
			else throw std::exception("empty sequence");
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		T Last(F func)
		{
			auto v = std::find_if(this->rbegin(), this->rend(), func);
			if (v == this->rend())
			{
				throw std::exception("no element in sequence satisfies func");
			}
			return *v;
		}

		T LastOrDefault()
		{
			if (this->Any())) {return this->back(); }
			else return T();
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		T LastOrDefault(F func)
		{
			auto v = std::find_if(this->rbegin(), this->rend(), func);
			if (v == this->rend())
			{
				return T();
			}
			return *v;
		}

		T Single()
		{
			if (this->size() == 1) { return *this[0]; }
			else throw exception("Size of IEnumerable<T> wasn't exactly 1");
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		T Single(F func)
		{
			T retval;
			int n = 0;
			std::for_each(this->begin(), this->end(), [&retval, &func, &n](T val) {if (func(val) == true) { if (n == 0) { n++; retval = val; } else { throw exception("More than one element matched the selection function in Single(F func)"); } }});
			if (n == 1) { return retval; }
			throw exception("No elements matched the selection function in Select(F func)");
		}

		T SingleOrDefault()
		{
			if (this->size() == 1) { return *this[0]; }
			else if (this->size() == 0) { return T(); }
			else throw exception("Size of IEnumerable<T> wasn't exactly 1");
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		T SingleOrDefault(F func)
		{
			T retval;
			int n = 0;
			std::for_each(this->begin(), this->end(), [&retval, &func, &n](T val) {if (func(val) == true) { if (n == 0) { n++; retval = val; } else { throw exception("More than one element matched the selection function in Single(F func)"); } }});
			if (n == 1) { return retval; }
			else return T();
		}

		//		CONVERTING DATA TYPES

		// AsEnumerable, AsQueryable, OfType and ToLookup methods can't be meaningfully implemented either because of the type system or because of redundancy

		template<typename R, typename _A = std::enable_if<std::is_convertible<T, R>::value>::type>
		IEnumerable<R> Cast()
		{
			IEnumerable<R> ret;
			std::transform(this->begin(), this->end(), std::back_inserter(ret), [](T val) {return (R)val; });
			return ret;
		}

		std::vector<T> ToVector()
		{
			return std::vector<T>(std::move(*this));
		}

		std::list<T> ToList()
		{
			std::list<T> ret;
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::forward_list<T> ToForwardList()
		{
			std::forward_list<T> ret;
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.push_back(val); });
			return ret;
		}

		std::set<T> ToSet()
		{
			std::set<T> ret;
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.insert(val); });
			return ret;
		}

		std::unordered_set<T> ToUnorderedSet()
		{
			std::unordered_set<T> ret;
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret.insert(val); });
			return ret;
		}

		template <typename F, typename K = std::result_of_t<F(T)>>
		std::map<K, T> ToMap(F func)
		{
			std::map<K, T> ret;
			std::for_each(this->begin(), this->end(), [&ret, &func](T val) {ret[func(val)] = val; });
			return ret;
		}

		template <typename F, typename K = std::result_of_t<F(T)>>
		std::unordered_map<K, T> ToUnorderedMap()
		{
			std::unordered_map<K, T> ret;
			std::for_each(this->begin(), this->end(), [&ret](T val) {ret[func(val)] = val; });
			return ret;
		}

		//		CONCATENATION OPERATORS

		IEnumerable<T>& Concat(IEnumerable<T>& other)
		{
			std::copy(other.begin(), other.end(), std::back_inserter(*this));
			return *this;
		}

		//		AGGREGATION OPERATORS

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T, T)>, T>::value>::type>
		T Aggregate(F func)
		{
			return std::accumulate(this->begin(), this->end(), T(), func);
		}

		template <typename F, typename V, typename _A = std::enable_if<std::is_same<std::result_of_t<F(V, T)>, V>::value>::type>
		V Aggregate(V val, F func)
		{
			return std::accumulate(this->begin(), this->end(), val, func);
		}

		template <typename F, typename R, typename V, typename E = std::result_of_t<R(V)>, typename _A = std::enable_if<std::is_same<std::result_of_t<F(V, T)>, V>::value>::type>
		E Aggregate(V val, F func, R retfunc)
		{
			return retfunc(std::accumulate(this->begin(), this->end(), val, func));
		}

		template <typename F, typename N = std::result_of_t<F(T)>, typename _A = std::enable_if<std::is_arithmetic<N>::value>::type>
		N Average(F func)
		{
			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val / this->size();
		}

		template <typename _A = std::enable_if<std::is_arithmetic<T>::value>::type>
		T Average()
		{
			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += v; });
			return val / this->size();
		}

		std::size_t Count()
		{
			return this->size();
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		std::size_t Count(F func)
		{
			return std::accumulate(this->begin(), this->end(), 0, [&func](int a, T val) {if (func(val)) { return ++a; } else return a; });
		}

		template <typename F, typename N = std::result_of_t<F(T)>, typename _A = std::enable_if<std::is_arithmetic<N>::value>::type>
		N Max(F func)
		{
			N val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::max(val, func(v)); }});
			return val;
		}

		template <typename _A = std::enable_if<std::is_arithmetic<T>::value>::type>
		T Max()
		{
			T val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = v; } else { val = std::max(val, v); }});
			return val;
		}

		template <typename F, typename N = std::result_of_t<F(T)>, typename _A = std::enable_if<std::is_arithmetic<N>::value>::type>
		N Min(F func)
		{
			N val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::min(val, func(v)); }});
			return val;
		}

		template <typename _A = std::enable_if<std::is_arithmetic<T>::value>::type>
		T Min()
		{
			T val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = v; } else { val = std::min(val, v); }});
			return val;
		}

		template <typename F, typename N = std::result_of_t<F(T)>, typename _A = std::enable_if<std::is_arithmetic<N>::value>::type>
		N Sum(F func)
		{
			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val;
		}

		template <typename _A = std::enable_if<std::is_arithmetic<T>::value>::type>
		T Sum()
		{
			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += v; });
			return val;
		}
	};

	template <typename T>
	IEnumerable<T> LINQ(const std::vector<T>& vec)
	{
		return IEnumerable<T>(vec.begin(), vec.end());
	}

	template <typename T>
	IEnumerable<T> LINQ(std::vector<T>&& vec)
	{
		return IEnumerable<T>(std::move(vec));
	}
}
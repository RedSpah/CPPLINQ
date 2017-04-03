#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_map>

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


		// ordinary version
		template <typename F, typename R = std::result_of<F(T)>::type>
		IEnumerable<R> Select(F func)
		{
			IEnumerable<R> ret;
			std::transform(this->begin(), this->end(), std::back_inserter(ret), func);
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename R = std::result_of<F(T, int)>::type, typename = void>
		IEnumerable<R> Select(F func)
		{
			IEnumerable<R> ret;
			int n = 0;
			std::transform(this->begin(), this->end(), std::back_inserter(ret), [&n, &func](T val) {return func(val, n++); });
			return ret;
		}



		// ordinary version 
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of<F(T)>::type,bool>::value>::type>
		IEnumerable<T> Where(F func)
		{
			IEnumerable<T> ret;
			std::copy_if(this->begin(), this->end(), std::back_inserter(ret), func);
			return ret;
		}

		// func takes a second parameter, which is the zero-based index of the element
		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of<F(T, int)>::type, bool>::value>::type, typename = void>
		IEnumerable<T> Where(F func)
		{
			IEnumerable<T> ret;
			int n = 0;
			std::copy_if(this->begin(), this->end(), std::back_inserter(ret), [&n, &func](T val) {return func(val, n++); });
			return ret;
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
		template <typename F, typename P, typename RC = typename std::result_of<F(T)>::type, typename R = typename std::result_of<P(T, RC::value_type)>>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			IEnumerable<R> ret;
			unordered_map<T, RC> inter;
			std::for_each(this->begin(), this->end(), [&inter, &func](T val) {inter[val] = func(val)});
			std::for_each(inter.begin(), inter.end(), [&ret](std::pair<T, RC>& rc) 
			{
				std::transform(rc.second.begin(), rc.second.end(), std::back_inserter(ret), [&rc](RC::value_type val) 
				{
					return result_selector(rc.first, val); 
				}); 
			});
			return ret;
		}

		template <typename F, typename P, typename RC = typename std::result_of<F(T)>::type, typename R = typename std::result_of<P(T, RC::value_type)>, typename = void>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			IEnumerable<R> ret;
			unordered_map<T, RC> inter;
			int n = 0;
			std::for_each(this->begin(), this->end(), [&inter, &func, &n](T val) {inter[val] = func(val, n++); });
			std::for_each(inter.begin(), inter.end(), [&ret](std::pair<T, RC>& rc)
			{
				std::transform(rc.second.begin(), rc.second.end(), std::back_inserter(ret), [&rc](RC::value_type val)
				{
					return result_selector(rc.first, val);
				});
			});
			return ret;
		}



		IEnumerable<T> Take(int n)
		{
			IEnumerable<T> ret;
			std::copy_n(this->begin(), std::min(n, (int)this->size()), std::back_inserter(ret));
			return ret;
		}

		IEnumerable<T> Skip(int n)
		{
			IEnumerable<T> ret;		
			std::copy(this->begin() + std::min(n, (int)this->size()), this->end(), std::back_inserter(ret));
			return ret;
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		IEnumerable<T> TakeWhile(F func)
		{
			IEnumerable<T> ret;
			bool done = false;
			std::copy_if(this->begin(), this->end(), std::back_inserter(ret), [&done, &func](T t) {if (!done) { done = !func(t); } return !done; });
			return ret;
		}

		template <typename F, typename _A = std::enable_if<std::is_same<std::result_of_t<F(T)>, bool>::value>::type>
		IEnumerable<T> SkipWhile(F func)
		{
			IEnumerable<T> ret;
			bool done = false;
			std::copy_if(this->begin(), this->end(), std::back_inserter(ret), [&done, &func](T t) {if (!done) { done = !func(t); } return done; });
			return ret;
		}

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

		IEnumerable<T>& Concat(IEnumerable<T> other)
		{
			std::copy(other.begin(), other.end(), std::back_inserter(*this));
			return *this;
		}



		std::vector<T> ToVector()
		{
			return std::vector<T>(std::move(*this));
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

	//## IMPL


}
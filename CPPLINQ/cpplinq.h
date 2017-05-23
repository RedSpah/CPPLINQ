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
#include <type_traits>

#include <exception>
#include <stdexcept>

namespace cpplinq
{
	template <typename, bool, typename, bool>
	struct IEnumerable;

	enum class alloc_mode : unsigned char {no_alloc, auto_alloc};

	namespace res
	{
		template <std::size_t thousands>
		struct res_var { static constexpr float res = ((float)thousands / 1000); };

		using none = res_var<0>;	
		using half = res_var<500>;
		using full = res_var<1000>;
		using two = res_var<2000>;
		using three = res_var<3000>;
		using five = res_var<5000>;
		using ten = res_var<10000>;
	}

	namespace templ
	{
		template<class ...> using void_t = void;

		template <class = void, class = void>
		struct is_sortable : std::false_type {};

		template <class T>
		struct is_sortable<T, void_t<typename std::enable_if<std::is_convertible<decltype(std::declval<T>() < std::declval<T>()), bool>::value>::type, typename std::enable_if<std::is_convertible<decltype(std::declval<T>() > std::declval<T>()), bool>::value>::type>> : std::true_type {};
	
		template <class T>
		constexpr bool is_sortable_v = is_sortable<T>::value;

		template <class = void, class = void>
		struct is_comparable : std::false_type {};

		template <class T>
		struct is_comparable<T, void_t<typename std::enable_if<std::is_convertible<decltype(std::declval<T>() == std::declval<T>()), bool>::value>::type, typename std::enable_if<std::is_convertible<decltype(std::declval<T>() != std::declval<T>()), bool>::value>::type>> : std::true_type {};

		template <class T>
		constexpr bool is_comparable_v = is_comparable<T>::value;

		template <class = void, class = void>
		struct is_container : std::false_type {};

		template <typename T>
		struct is_container<T, void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {};

		template <class T>
		constexpr bool is_container_v = is_container<T>::value;

		template <typename T>
		using value_type_t = typename std::decay<decltype(*(std::declval<T>().begin()))>::type;

		template <typename F>
		using ret_type = typename std::result_of<F>::type;

		template <typename T>
		using dummy_iterator_t = typename std::vector<T>::iterator;

		//http://stackoverflow.com/questions/24233516/checking-correctness-of-function-call-expression
		template<typename F, typename... Args>
		struct is_valid_call {
		private:
			template<typename FF, typename... AA>
			static constexpr auto check(int) -> decltype(std::declval<FF>()(std::declval<AA>()...), std::true_type()) {return std::true_type();};

			template<typename FF, typename... AA>
			static constexpr std::false_type check(...) { return std::false_type(); };
		public:
			static constexpr bool value = decltype(check<F, Args...>(0))::value;
		};

		template <typename F, typename... A>
		constexpr bool is_defined_v = is_valid_call<F, A...>::value;

		template <typename = void, bool = false, typename...>
		struct sanitizer_impl {using type = void; };

		template <typename F, typename... A>
		struct sanitizer_impl<F, true, A...> { using type = typename std::result_of<F(A...)>::type; };

		// produces an intellisense error in visual studio, but compiles correctly
		template <typename F, typename... A>
		using sanitizer_t = typename sanitizer_impl<F, is_defined_v<F, A...>, A...>::type;

		template <typename = void, typename = void, bool = false, typename...>
		struct is_func_impl : std::false_type {};

		template <typename F, typename  R, typename... A>
		struct is_func_impl<F, R, true, A...> : std::integral_constant<bool, std::is_same<typename std::result_of<F(A...)>::type, R>::value> {};

		template <typename F, typename R, typename... A>
		constexpr auto is_func_v = is_func_impl<F, R, is_defined_v<F, A...>, A...>::value;

		template <typename F, typename A>
		constexpr bool is_counter_func_v = is_defined_v<F, A, std::size_t>;

		template <typename F, typename A>
		constexpr bool is_filter_v = is_func_v<F, bool, A> || is_func_v<F, bool, A, std::size_t>;

		template <typename F, typename A>
		using safe_result_of_t = typename std::conditional<is_counter_func_v<F, A>, sanitizer_t<F, A, int>, sanitizer_t<F, A>>::type;

		constexpr bool lol = false;



		template <bool B>
		struct pt_lambda
		{
			template <typename F, typename T, typename R = typename std::result_of<F(T)>::type>
			static inline R ret(F& func, T& val, int n) { ++n; return func(val); }
		};

		template <>
		struct pt_lambda<true>
		{
			template <typename F, typename T, typename R = typename std::result_of<F(T, int)>::type>
			static inline R ret(F& func, T& val, int n) { return func(val, n); }
		};
	}


	template <typename T, bool IsRef = false, typename Cont = std::vector<T>, bool IsContConst = false>
	struct IEnumerable : public std::vector<T>
	{
	private:
		using VectorIterType = decltype(std::declval<std::vector<T>>().begin());
		using ContIterType = decltype(std::declval<Cont>().begin());
		Cont& refcont__;

	public:
		template <typename RetIter = typename std::conditional<IsRef, ContIterType, VectorIterType>::type>
		inline RetIter begin() 
		{
			if (IsRef) { return refcont__.begin(); }
			else return std::vector<T>::begin();
		}

		template <typename RetIter = typename std::conditional<IsRef, ContIterType, VectorIterType>::type>
		inline RetIter end() 
		{
			if (IsRef) { return refcont__.end(); }
			else return std::vector<T>::end();
		}

		inline std::size_t size()
		{
			if (IsRef) { return std::distance(begin(), end()); }
			else return std::vector<T>::size();
		}


		template <typename Iter>
		IEnumerable(Iter begin, Iter end) : std::vector<T>(begin, end), refcont__(*this) {};

		IEnumerable(std::vector<T>&& m) : std::vector<T>(std::move(m)), refcont__(*this) {};

		IEnumerable() : std::vector<T>(), refcont__(*this) {};

		IEnumerable(const std::vector<T>& vec) : std::vector<T>(vec), refcont__(*this) {};

		IEnumerable(Cont& c) : refcont__(c) {};

		/*=== QUERY FUNCTIONS ===*/
		//		 SORTING

		// Because of complexity of implementing all the boilerplate required to implement 'ThenBy' faithfully, I opted for overloading 'OrderBy' and 'OrderByDesending' to allow for multiple V->K functions, which work just like applying ThenBy to the 'normal' OrderBy 

		template <typename F1, typename K1 = typename templ::ret_type<F1(T)>>
		IEnumerable<T, IsRef, Cont>& OrderBy(F1 func1)
		{
			static_assert(templ::is_sortable<K1>::value, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call OrderBy() on an IEnumerable created from a const referenced container, a Copy() is required first.");

			std::sort(begin(), end(), [&func1](T val1, T val2) {return func1(val1) < func1(val2); });
			return *this;
		}

		template <typename F1, typename F2, typename K1 = typename std::result_of<F1(T)>::type, typename K2 = typename std::result_of<F2(T)>::type>
		IEnumerable<T, IsRef, Cont>& OrderBy(F1 func1, F2 func2)
		{
			static_assert(templ::is_sortable<K1>::value, "Values of the return type of the first passed functor must define greater-than and less-than operators.");
			static_assert(templ::is_sortable<K2>::value, "Values of the return type of the second passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call OrderBy() on an IEnumerable created from a const referenced container, a Copy() is required first.");

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
		IEnumerable<T, IsRef, Cont>& OrderBy(F1 func1, F2 func2, F3 func3)
		{
			static_assert(templ::is_sortable<K1>::value, "Values of the return type of the first passed functor must define greater-than and less-than operators.");
			static_assert(templ::is_sortable<K2>::value, "Values of the return type of the second passed functor must define greater-than and less-than operators.");
			static_assert(templ::is_sortable<K3>::value, "Values of the return type of the third passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call OrderBy() on an IEnumerable created from a const referenced container, a Copy() is required first.");

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
		IEnumerable<T, IsRef, Cont>& OrderByDescending(F1 func1)
		{
			static_assert(templ::is_sortable<K1>::value, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call OrderByDescending() on an IEnumerable created from a const referenced container, a Copy() is required first.");

			std::sort(this->begin(), this->end(), [&func1](T val1, T val2) {return func1(val1) > func1(val2); });
			return *this;
		}

		template <typename F1, typename F2, typename K1 = typename std::result_of<F1(T)>::type, typename K2 = typename std::result_of<F2(T)>::type>
		IEnumerable<T, IsRef, Cont>& OrderByDescending(F1 func1, F2 func2)
		{
			static_assert(templ::is_sortable<K1>::value, "Values of the return type of the first passed functor must define greater-than and less-than operators.");
			static_assert(templ::is_sortable<K2>::value, "Values of the return type of the second passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call OrderByDescending() on an IEnumerable created from a const referenced container, a Copy() is required first.");

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
		IEnumerable<T, IsRef, Cont>& OrderByDescending(F1 func1, F2 func2, F3 func3)
		{
			static_assert(templ::is_sortable<K1>::value, "Values of the return type of the first passed functor must define greater-than and less-than operators.");
			static_assert(templ::is_sortable<K2>::value, "Values of the return type of the second passed functor must define greater-than and less-than operators.");
			static_assert(templ::is_sortable<K3>::value, "Values of the return type of the third passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call OrderByDescending() on an IEnumerable created from a const referenced container, a Copy() is required first.");


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



		IEnumerable<T, IsRef, Cont>& Reverse()
		{
			static_assert(!IsContConst, "Cannot call Reverse() on a IEnumerable created from a const referenced container, a Copy() is required first.");

			std::reverse(begin(), end());
			return *this;
		}

		//		 SET OPERATIONS

		template <bool B0, typename C, bool B1>
		IEnumerable<T> Except(IEnumerable<T, B0, C, B1> other)
		{
			static_assert(!IsContConst, "Cannot call Except() on an IEnumerable created from a const referenced container, a Copy() is required first.");
			static_assert(!B1, "Cannot use an IEnumerable created from a const referenced container as an argument in Except(), a Copy() is required first.");
			static_assert(templ::is_sortable<T>::value, "Contained values must define greater-than and less-than operators.");

			IEnumerable<T> ret;
			ret.reserve(size() + other.size() / 2); // RESERVE
			std::sort(begin(), end());
			std::sort(other.begin(), other.end());
			std::set_difference(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <bool B0, typename C, bool B1>
		IEnumerable<T> Intersect(IEnumerable<T, B0, C, B1> other)
		{
			static_assert(!IsContConst, "Cannot call Intersect() on an IEnumerable created from a const referenced container, a Copy() is required first.");
			static_assert(!B1, "Cannot use an IEnumerable created from a const referenced container as an argument in Intersect(), a Copy() is required first.");
			static_assert(!templ::is_sortable<T>::value, "Contained values must define greater-than and less-than operators.");

			IEnumerable<T> ret;
			ret.reserve(size() + other.size() / 2); // RESERVE
			std::sort(begin(), end());
			std::sort(other.begin(), other.end());
			std::set_intersection(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <bool B0, typename C, bool B1>
		IEnumerable<T> Union(IEnumerable<T, B0, C, B1> other)
		{
			static_assert(!IsContConst, "Cannot call Union() on an IEnumerable created from a const referenced container, a Copy() is required first.");
			static_assert(!B1, "Cannot use an IEnumerable created from a const referenced container as an argument in Union(), a Copy() is required first.");
			static_assert(!templ::is_sortable<T>::value, "Contained values must define greater-than and less-than operators.");

			IEnumerable<T> ret;
			ret.reserve(size() + other.size()); // RESERVE
			std::sort(begin(), end());
			std::sort(other.begin(), other.end());
			std::set_union(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <
			alloc_mode MemMode = alloc_mode::no_alloc, 
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type
		>
		RetVal Distinct()
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Distinct() on an IEnumerable created from a referenced container, either a Copy() or specifying cpplinq::mem::auto_alloc as the first template argument is required first.");
			static_assert(templ::is_comparable<T>::value, "Contained values must define equal and not-equal operators.");
			static_assert(templ::is_sortable<T>::value, "Contained values must define greater-than and less-than operators.");

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;			
				ret.reserve(size());
				std::copy(begin(), end(), std::back_inserter(ret));
				std::sort(ret.begin(), ret.end());
				ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
				return ret;
			}
			else
			{
				std::sort(begin(), end());
				erase(std::unique(begin(), end()), end());
				return *this;
			}		
		}

		//		 FILTERING DATA

		// god forgive me
		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type,
			typename F
		>
		RetVal Where(F func)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T) or bool(T, int), where T is the type of the contained values.");
			static_assert(!IsRef || AllowAlloc, "Cannot call Where() on an IEnumerable created from a referenced container, either a Copy() or specifying cpplinq::mem::auto_alloc as the first template argument is required first.");

			int n = 0;

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(size()); // RESERVE
				std::copy_if(begin(), end(), std::back_inserter(ret), [&n, &func](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); });
				return ret;
			}
			else
			{
				erase(std::copy_if(begin(), end(), begin(), [&n, &func](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); }), end());
				return *this;
			}
		}
		
		//		 QUANTIFIER OPERATIONS

		template <typename F>
		bool All(F func) const
		{
			static_assert(templ::is_filter_v<F, T> && !templ::is_counter_func_v<F, T>, "Passed functor must have the following signature: bool(T), where T is the type of the contained values.");

			return std::all_of(begin(), end(), func);
		}

		bool Any() const
		{
			return size() > 0;
		}

		template <typename F>
		bool Any(F func) const
		{
			static_assert(templ::is_filter_v<F, T> && !templ::is_counter_func_v<F, T>, "Passed functor must have the following signature: bool(T), where T is the type of the contained values.");

			return std::any_of(begin(), end(), func);
		}

		bool Contains(T val) const
		{
			return std::find(begin(), end(), val) != end();
		}

		//		 PROJECTION OPERATORS

		// ordinary version
		template <
			typename F,
			typename R = typename templ::safe_result_of_t<F, T>,		
			typename SFINAE_GUARD = typename std::enable_if<!std::is_same<R, T>::value || IsContConst>::type
		>
		IEnumerable<R> Select(F func)
		{
			static_assert(!std::is_same<R, void>::value, "Passed functor must have one of the following signatures: R(T), R(T, int), T(T), T(T, int), where R is any type, and T is the type of values stored in the container.");

			int n = 0;

			IEnumerable<R> ret;
			ret.reserve(size()); // RESERVE, UNCHANGABLE
			std::transform(begin(), end(), std::back_inserter(ret), [&n, &func](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); });
			return ret;			
		}
		
		template <
			typename F,
			typename R = typename templ::safe_result_of_t<F, T>,
			typename SFINAE_GUARD = typename std::enable_if<std::is_same<R, T>::value && !IsContConst>::type
		>
		IEnumerable<T>& Select(F func)
		{
			static_assert(!std::is_same<R, void>::value, "Passed functor must have one of the following signatures: R(T), R(T, int), T(T), T(T, int), where R is any type, and T is the type of values stored in the container.");

			int n = 0;

			std::transform(begin(), end(), begin(), [&n, &func](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); });
			return *this;
		}

		
		// ordinary version
		template <
			typename F,
			typename RC = typename templ::safe_result_of_t<F, T>,
			typename R = typename templ::value_type_t<RC>
		>
		IEnumerable<R> SelectMany(F func)
		{
			static_assert(!std::is_same<RC, void>::value, "Passed selection functor must have the following signature: RC(T) or RC(T, int), where RC is a container, and T is the type of the contained elements within this container.");
			static_assert(!templ::is_container<RC>::value, "Return type of the passed functor must implement begin() and end().");
			
			IEnumerable<R> ret;
			ret.reserve(size()); // RESERVING

			int n = 0;

			std::for_each(begin(), end(), [&n, &ret, &func](T val)
			{
				RC rc = templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++);
				std::copy(rc.begin(), rc.end(), std::back_inserter(ret));
			});

			return ret;
		}

		// result selector version
		template <
			typename F,
			typename P,
			typename RC = typename templ::safe_result_of_t<F, T>,
			typename RC_val = typename templ::value_type_t<RC>,
			typename R = typename templ::sanitizer_t<P, RC_val, T>
		>
		IEnumerable<R> SelectMany(F func, P result_selector)
		{
			static_assert(!std::is_same<RC, void>::value, "Passed selection functor must have the following signature: RC(T) or RC(T, int), where RC is a container, and T is the type of the contained elements within this container.");
			static_assert(templ::is_container<RC>::value, "Return type of the passed selection functor must implement begin() and end().");
			static_assert(!std::is_same<R, void>::value, "Passed result selector functor must have the following signature: R(RC_val, T), where R is any type, RC_val is the type of the contained elements of container returned by the selection functor, and T is the type of the contained elements within this container.");
	
			IEnumerable<R> ret;
			ret.reserve(size()); // RESERVING

			int n = 0;

			std::for_each(begin(), end(), [&n, &ret, &func, &result_selector](T val)
			{
				RC rc = templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++);
				std::transform(rc.begin(), rc.end(), std::back_inserter(ret), [&result_selector, &val](RC_val rcval) {return result_selector(rcval, val); });
			});

			return ret;
		}


		//		 PARTITIONING DATA

		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type
		>
		RetVal Skip(int n)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Skip() on an IEnumerable created from a referenced container, a Copy() or setting the first template argument to cpplinq::mem::auto_alloc is required first.");

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(std::max(size() - n, 0));
				std::copy(begin() + n, end(), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(begin(), begin() + std::min(n, (int)size()));
				return *this;
			}
		}

		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type
		>
		RetVal Take(int n)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Take() on an IEnumerable created from a referenced container, a Copy() or setting the first template argument to cpplinq::mem::auto_alloc is required first.");

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(std::min(size(), n));
				std::copy(begin(), begin() + std::min(n, (int)size()), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(begin(), begin() + std::min(n, (int)size()));
				return *this;
			}
		}

		// ordinary version
		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type, 
			typename F
		>
		RetVal TakeWhile(F func)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), bool(T, int), where T is the type of the contained values.");
			static_assert(!IsRef || AllowAlloc, "Cannot call TakeWhile() on an IEnumerable created from a referenced container, a Copy() is required first.");

			int n = 0;

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(size()); // RESERVE
				std::copy(begin(), std::find_if_not(begin(), end(), [&func, &n](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); }), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(std::find_if_not(begin(), end(), [&func, &n](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); }), end());
				return *this;
			}	
		}

		// ordinary version
		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type, 
			typename F
		>
		RetVal SkipWhile(F func)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), bool(T, int), where T is the type of the contained values.");
			static_assert(!IsRef || AllowAlloc, "Cannot call SkipWhile() on an IEnumerable created from a referenced container, a Copy() is required first.");

			int n = 0;

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(size()); // RESERVE
				std::copy(std::find_if_not(begin(), end(), [&func, &n](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); }), end(), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(begin(), std::find_if_not(begin(), end(), [&func, &n](T val) {return templ::pt_lambda<templ::is_counter_func_v<F, T>>::ret(func, val, n++); }));
				return *this;
			}		
		}


		//		 JOIN OPERATORS

		//TODO: something about join

		template <
			bool B0, class C, bool B1,
			typename FOuterKey,
			typename FInnerKey,
			typename FJoin,
			typename TInner,
			typename TKey = typename templ::sanitizer_t<FOuterKey, T>,
			typename TJoined = typename templ::sanitizer_t<FJoin, T, TInner>
		>
		IEnumerable<TJoined> Join(const IEnumerable<TInner, B0, C, B1>& innerkeys, FOuterKey outerkeyselector, FInnerKey innerkeyselector, FJoin joiner)
		{
			static_assert(!std::is_same<TKey, void>::value, "Passed outer key selector must have the following signature: TKey(T), where TKey is any type implementing operator== and operator!= and T is the type of the values contained within this container.");
			static_assert(!templ::is_comparable_v<TKey>, "The values returned by the outer key selector must implement operator== and operator!=.");
			static_assert(templ::is_func_v<FInnerKey, TKey, TInner>, "Passed inner key selector must have the following signature: TKey(TInner), where TKey is the type returned by the outer key selector and TInner is the type of the values contained within the innerkeys container.");
			static_assert(!std::is_same<TJoined, void>::value, "Passed joiner must have the following signature: TJoined(T, TInner), where TJoined is any type, T is the type of values contained within this container and TInner is the type of the values contained within the innerkeys container.");

			IEnumerable<TJoined> ret; 

			std::vector<std::pair<TKey, TInner>> innervals;
			innervals.reserve(size()); // RESERVE unchangable
			ret.reserve(size()); // RESERVE
			
			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, &innerkeyselector](TInner innerval) {innervals.push_back(std::make_pair(innerkeyselector(innerval), innerval)); });
			std::for_each(begin(), end(), [&innervals, &outerkeyselector, &joiner, &ret](T outerval)
			{
				TKey key = outerkeyselector(outerval);
				auto iter = std::find_if(innervals.begin(), innervals.end(), [&key](std::pair<TKey, TInner> p) {return key == p.first; });

				if (iter != innervals.end())
				{
					ret.push_back(joiner(outerval, *iter));
				}
			});
			return ret;
		}

		template <
			bool B0, class C, bool B1,
			typename FOuterKey, 
			typename FInnerKey, 
			typename FJoin, 
			typename TInner,
			typename TKey = typename templ::sanitizer_t<FOuterKey, T>,
			typename TJoined = typename templ::sanitizer_t<FJoin, T, templ::dummy_iterator_t<TInner>, templ::dummy_iterator_t<TInner>>
		>
		IEnumerable<TJoined> GroupJoin(const IEnumerable<TInner, B0, C, B1>& innerkeys, FOuterKey outerkeyselector, FInnerKey innerkeyselector, FJoin joiner)
		{
			static_assert(!std::is_same<TKey, void>::value, "Passed outer key selector must have the following signature: TKey(T), where TKey is any type implementing operator== and operator!= and T is the type of the values contained within this container.");
			static_assert(!templ::is_comparable_v<TKey>, "The values returned by the outer key selector must implement operator== and operator!=.");
			static_assert(templ::is_func_v<FInnerKey, TInner, TKey>, "Passed inner key selector must have the following signature: TKey(TInner), where TKey is the type returned by the outer key selector and TInner is the type of the values contained within the innerkeys container.");
			static_assert(!std::is_same<TJoined, void>::value, "Passed joiner must have the following signature: TJoined(T, Iter<TInner>, Iter<TInner>), where TJoined is any type, T is the type of values contained within this container and Iter<TInner> is the type of an iterator to a collection of objects of type of the the type of the values contained within the innerkeys container.");

			IEnumerable<TJoined> ret;

			std::vector<std::pair<TKey, TInner>> innervals;
			ret.reserve(size()); // RESERVE
			innervals.reserve(size()); // RESERVE

			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, &innerkeyselector](TInner innerval) {innervals.push_back(std::make_pair(innerkeyselector(innerval), innerval)); });
			std::for_each(begin(), end(), [&innervals, &outerkeyselector, &joiner, &ret](T outerval)
			{
				TKey key = outerkeyselector(outerval);
				std::vector<TInner> block;

				std::for_each(innervals.begin(), innervals.end(), [&key, &block](std::pair<TKey, TInner> p) {if (p.first == key) { block.push_back(p.second); }});
				if (block.size() != 0) 
				{ 
					ret.push_back(joiner(outerval, block.begin(), block.end())); 
				}
			});
			return ret;
		}

		//		 GROUPING DATA

		// ¯\_(ツ)_/¯ - neither Lookup<TKey, TElement> nor IGrouping<TKey, TElement> can be meaningfully recreated in C++, the closest thing to them is ToMap and ToUnsortedMap below

		//		 GENERATION OPERATORS

		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type
		>
		RetVal DefaultIfEmpty()
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call DefaultIfEmpty() on an IEnumerable created from a referenced container, a Copy() is required first.");

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(size());
				std::copy(begin(), end(), std::back_inserter(ret));
				if (Any()) { return ret; }
				else { ret.push_back(T()); return ret; }
			}
			else
			{
				if (Any()) { return *this; }
				else { push_back(T()); return *this; }
			}
		}

		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type
		>
		RetVal DefaultIfEmpty(T val)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call DefaultIfEmpty() on an IEnumerable created from a referenced container, a Copy() is required first.");

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(size());
				std::copy(begin(), end(), std::back_inserter(ret));
				if (Any()) { return ret; }
				else { ret.push_back(val); return ret; }
			}
			else
			{
				if (Any()) { return *this; }
				else { push_back(val); return *this; }
			}
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

		template <bool B0, typename C, bool B1>
		bool SequenceEqual(const IEnumerable<T, B0, C, B1>& other) const
		{
			static_assert(templ::is_comparable_v<T>, "Type of the contained values must implement operator== and operator!=.");

			return std::equal(begin(), end(), other.begin());
		}

		//		 ELEMENT OPERATORS

		T ElementAt(int index) const
		{
			if (index < this->size()) {return *this[index]; }
			else throw std::out_of_range("IEnumerable<T>::ElementAt(int index) | Index out of range.");
		}

		T ElementAtOrDefault(int index) const
		{
			if (index < this->size()) {return *this[index]; }
			else return T();
		}

		T First() const
		{
			if (this->Any()) {return this->front(); }
			else throw std::length_error("IEnumerable<T>::First() | IEnumerable<T> is empty.");
		}

		template <typename F>
		T First(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			auto v = std::find_if(this->begin(), this->end(), func);
			if (v == this->end())
			{
				throw std::length_error("IEnumerable<T>::First(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T FirstOrDefault() const
		{
			if (this->Any()) {return this->front(); }
			else return T();
		}

		template <typename F>
		T FirstOrDefault(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

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

		template <typename F>
		T Last(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

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

		template <typename F>
		T LastOrDefault(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

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

		template <typename F>
		T Single(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			T retval;
			int n = 0;

			std::for_each(this->begin(), this->end(), [&retval, &func, &n](T val) 
			{
				if (func(val)) 
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

		template <typename F>
		T SingleOrDefault(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

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

		template <
			alloc_mode MemMode = alloc_mode::no_alloc,
			bool AllowAlloc = (MemMode == alloc_mode::auto_alloc),
			typename RetVal = typename std::conditional<IsRef && AllowAlloc, IEnumerable<T>, IEnumerable<T>&>::type, 
			bool B0, typename C, bool B1
		>
		RetVal Concat(const IEnumerable<T, B0, C, B1>& other)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Concat() on an IEnumerable created from a referenced container, a Copy() is required first.");

			if (IsRef && AllowAlloc)
			{
				IEnumerable<T> ret;
				ret.reserve(size() + other.size());
				std::copy(begin(), end(), std::back_inserter(ret));
				std::copy(other.begin(), other.end(), std::back_inserter(ret));
				return ret;
			}
			else
			{
				std::copy(other.begin(), other.end(), std::back_inserter(*this));
				return *this;
			}
		}

		//		AGGREGATION OPERATORS

		template <typename F>
		T Aggregate(F func) 
		{
			static_assert(templ::is_func_v<F, T, T, T>, "Passed functor must have one of the following signatures: T(T, T), V(V, T), where V is any type.");

			return std::accumulate(begin(), end(), T(), func);
		}

		template <typename F, typename V>
		V Aggregate(V val, F func) const
		{
			static_assert(templ::is_func_v<F, V, V, T>, "Passed functor must have one of the following signatures: T(T, T), V(V, T), where V is any type.");

			return std::accumulate(this->begin(), this->end(), val, func);
		}

		template <
			typename F, 
			typename R, 
			typename V, 
			typename E = typename templ::sanitizer_t<R, V>
		>
		E Aggregate(V val, F func, R retfunc) const
		{
			static_assert(templ::is_func_v<F, V, V, T>, "Passed functor must have the following signature: V(V, T), where V is any type.");
			static_assert(!std::is_same<E, void>::value, "Passed return functor must have the following signature: E(V), where E is any type and V is the type returned by the accumulating functor.");

			return retfunc(std::accumulate(this->begin(), this->end(), val, func));
		}

		template <
			typename F, 
			typename N = typename templ::sanitizer_t<F, T>
		>
		N Average(F func) const
		{
			static_assert(!std::is_same<N, void>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Average(F func) | IEnumerable<T> is empty."); }

			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val / this->size();
		}

		T Average() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");
			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Average() | IEnumerable<T> is empty."); }

			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val](T v) {val += v; });
			return val / this->size();
		}

		std::size_t Count() const noexcept
		{
			return this->size();
		}

		template <typename F>
		std::size_t Count(F func) const
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have the following signature: bool(T).");

			return std::accumulate(this->begin(), this->end(), 0, [&func](int a, T val) {if (func(val)) { return ++a; } else return a; });
		}

		template <
			typename F,
			typename N = typename templ::sanitizer_t<F, T>
		>
		N Max(F func) const
		{
			static_assert(!std::is_same<N, void>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Max(F func) | IEnumerable<T> is empty."); }

			N val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::max(val, func(v)); }});
			return val;
		}

		T Max() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");
			
			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Max() | IEnumerable<T> is empty."); }

			T val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::max(val, v); }});
			return val;
		}

		template <
			typename F,
			typename N = typename templ::sanitizer_t<F, T>
		>		
		N Min(F func) const
		{
			static_assert(!std::is_same<N, void>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Min(F func) | IEnumerable<T> is empty."); }

			N val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first, &func](T v) {if (!first) { first = true; val = func(v); } else { val = std::min(val, func(v)); }});
			return val;
		}

		T Min() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");
			
			if (this->size() == 0) { throw std::logic_error("IEnumerable<T>::Min() | IEnumerable<T> is empty."); }

			T val;
			bool first = false;
			std::for_each(this->begin(), this->end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::min(val, v); }});
			return val;
		}

		template <
			typename F,
			typename N = typename templ::sanitizer_t<F, T>
		>			
		N Sum(F func) const
		{
			static_assert(!std::is_same<N, void>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val;
		}

		T Sum() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");

			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val](T v) {val += v; });
			return val;
		}

		IEnumerable<T> Copy()
		{
			IEnumerable<T> ret;
			ret.reserve(this->size());
			std::copy(this->begin(), this->end(), ret.begin());
			return ret;
		}

	};

	template <typename Cont, bool Const = std::is_const<Cont>::value, typename T = typename std::decay<decltype(*(std::declval<Cont>().begin()))>::type>
	IEnumerable<T, true, Cont, Const> LINQ(Cont& cont)
	{
		static_assert(std::is_same<decltype(*(std::declval<Cont>().begin())), decltype(*(std::declval<Cont>().end()))>::value, "Types of begin() and end() iterators must be the same.");
		return IEnumerable<T, true, Cont, Const>(cont);
	}


	template <typename Cont, typename T = typename std::decay<decltype(*(std::declval<Cont>().begin()))>::type, typename _A = typename std::enable_if<!std::is_lvalue_reference<Cont>::value>::type>
	IEnumerable<T> LINQ(Cont&& cont)
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
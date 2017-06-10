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
#include <utility>

#include <exception>
#include <stdexcept>

namespace cpplinq
{
	template <typename, bool, typename, bool>
	struct container;

	enum class alloc_mode : unsigned char {no_alloc, auto_alloc};

	namespace reserve
	{
		template <std::size_t thousands>
		struct res_var { static constexpr float val__ = ((float)thousands / 1000); constexpr float value() { return val__; } };

		using none = res_var<0>;
		using hundredth = res_var<10>;
		using twentieth = res_var<50>;
		using tenth = res_var<100>;
		using sixth = res_var<166>;
		using fifth = res_var<200>;
		using quarter = res_var<250>;
		using third = res_var<333>;
		using half = res_var<500>;
		using two_thirds = res_var<666>;
		using three_quarters = res_var<750>;
		using one = res_var<1000>;
		using one_and_a_half = res_var<1500>;
		using two = res_var<2000>;
		using two_and_a_half = res_var<2500>;
		using three = res_var<3000>;
		using five = res_var<5000>;
		using ten = res_var<10000>;
	}

	namespace templ
	{
		template<typename ...> using void_t = void;
			
		template <typename T> using value_type_t = typename std::decay<decltype(*(std::declval<T>().begin()))>::type;
		template <typename T> using vector_iterator_t = typename std::vector<T>::iterator;

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

		template <typename T1, typename T2> struct plus { template <typename R = decltype(std::declval<T1>() + std::declval<T2>())> constexpr R operator()(const T1& L_, const T2& R_) { return L_ + R_; } using type = decltype(std::declval<T1>() + std::declval<T2>());};
		template <typename T1, typename T2> struct mul { template <typename R = decltype(std::declval<T1>() * std::declval<T2>())> constexpr R operator()(const T1& L_, const T2& R_) { return L_ * R_; } using type = decltype(std::declval<T1>() * std::declval<T2>()); };

		template <typename T1, typename T2> constexpr bool is_addable_v = is_defined_v<plus<T1, T2>, T1, T2>;
		//template <typename T> constexpr bool is_addable_v = is_defined_v<std::plus<>, T, T>;
		template <typename T> constexpr bool is_multiplicative_v = is_defined_v<std::multiplies<>, T, T>;
		template <typename T> constexpr bool is_sortable_v = is_defined_v<std::less<>, T, T> || is_defined_v<std::greater<>, T, T>;
		template <typename T> constexpr bool is_comparable_v = is_defined_v<std::equal_to<>, T, T> || is_defined_v<std::not_equal_to<>, T, T>;

		template <class = void, class = void> struct is_container : std::false_type {};
		template <typename T> struct is_container<T, void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {};
		template <class T> constexpr bool is_container_v = is_container<T>::value;

		template <typename = void, bool = false, typename...> struct sanitizer_impl {using type = void; };
		template <typename F, typename... A> struct sanitizer_impl<F, true, A...> { using type = typename std::result_of<F(A...)>::type; };
		template <typename F, typename... A> using sanitizer_t = typename sanitizer_impl<F, is_defined_v<F, A...>, A...>::type;

		template <typename = void, typename = void, bool = false, typename...> struct is_func_impl : std::false_type {};
		template <typename F, typename R, typename... A> struct is_func_impl<F, R, true, A...> : std::integral_constant<bool, std::is_same<typename std::result_of<F(A...)>::type, R>::value> {};
		template <typename F, typename R, typename... A> constexpr auto is_func_v = is_func_impl<F, R, is_defined_v<F, A...>, A...>::value;

		template <typename F, typename A> constexpr bool is_counter_func_v = is_defined_v<F, A, std::size_t>;
		template <typename F, typename A> constexpr bool is_filter_v = is_func_v<F, bool, A> || is_func_v<F, bool, A, std::size_t>;

		template <typename F, typename A> using safe_result_of_t = typename std::conditional<is_counter_func_v<F, A>, sanitizer_t<F, A, int>, sanitizer_t<F, A>>::type;

		template <bool B>
		struct counter_func_lambda {template <typename F, typename T, typename R = typename std::result_of<F(T)>::type> static inline R ret(F&& func, T& val, int n) { ++n; return func(val); }};

		template <>
		struct counter_func_lambda<true> {template <typename F, typename T, typename R = typename std::result_of<F(T, int)>::type> static inline R ret(F&& func, T& val, int n) { return func(val, n); }};
	}


	template <typename T, bool IsRef = false, typename Cont = std::vector<T>, bool IsContConst = false>
	struct container : public std::vector<T>
	{
	private:
		
		Cont& refcont__;
		using self_rettype = container<T, IsRef, Cont>&;

	public:
		using VectorIterType = decltype(std::declval<std::vector<T>>().begin());     
		using ContIterType = decltype(std::declval<Cont>().begin());

		template <bool Workaround = true, typename SFINAE_GUARD = typename std::enable_if<Workaround && !IsRef>::type>
		inline VectorIterType begin() 
		{
			return std::vector<T>::begin();
		}

		template <bool Workaround = true, typename SFINAE_GUARD = typename std::enable_if<Workaround && IsRef>::type, typename = void>
		inline ContIterType begin() 
		{
			return refcont__.begin(); 
		}

		template <bool Workaround = true, typename SFINAE_GUARD = typename std::enable_if<Workaround && !IsRef>::type>
		inline VectorIterType end() 
		{
			return std::vector<T>::end();
		}

		template <bool Workaround = true, typename SFINAE_GUARD = typename std::enable_if<Workaround && IsRef>::type, typename = void>
		inline ContIterType end() 
		{
			return refcont__.end();
		}

		inline std::size_t size() 
		{
			if (IsRef) { return std::distance(begin(), end()); }
			else return std::vector<T>::size();
		}

		T& operator[](std::size_t n)
		{
			return *(begin() + n);
		}


		template <typename Iter>
		container(Iter begin, Iter end) : std::vector<T>(begin, end), refcont__(*this) {};

		container(std::vector<T>&& m) : std::vector<T>(std::move(m)), refcont__(*this) {};

		container() : std::vector<T>(), refcont__(*this) {};

		container(const std::vector<T>& vec) : std::vector<T>(vec), refcont__(*this) {};

		container(Cont& c) : refcont__(c) {};

		/*=== ALGORITHM FUNCTIONS ===*/
		//		 SORTING
	
		self_rettype sort()
		{
			static_assert(templ::is_sortable_v<T>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort() on an container created from a const referenced container, a Copy() is required first.");

			std::sort(begin(), end());
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T, T>>>
		self_rettype sort(F&& value_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F value_function) on an container created from a const referenced container, a copy() is required first.");

			std::sort(begin(), end(), [value_function = std::forward<F>(value_function)](T& val1, T& val2) {return value_function(val1) < value_function(val2); });
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T>>, typename = void>
		self_rettype sort(F&& comparator_function)
		{
			static_assert(!std::is_same<R, bool>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a copy() is required first.");

			std::sort(begin(), end(), std::forward<F>(comparator_function));
			return *this;
		}



		bool is_sorted()
		{
			static_assert(templ::is_sortable_v<T>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort() on an container created from a const referenced container, a Copy() is required first.");

			return std::is_sorted(begin(), end());
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T, T>>>
		bool is_sorted(F&& value_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>, and T is the type of the values contained within this container.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");
		
			return std::is_sorted(begin(), end(), [value_function = std::forward<F>(value_function)](T& val1, T& val2) {return value_function(val1) < value_function(val2); });
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T>>, typename = void>
		bool is_sorted(F&& comparator_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>, and T is the type of the values contained within this container.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			return std::is_sorted(begin(), end(), std::forward<F>(comparator_function));
		}



		self_rettype partial_sort(std::size_t N)
		{
			static_assert(templ::is_sortable_v<T>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort() on an container created from a const referenced container, a Copy() is required first.");

			std::partial_sort(begin(), std::min(begin() + N, end()), end());
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T, T>>>
		self_rettype partial_sort(std::size_t N, F&& value_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>, and T is the type of the values contained within this container.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			std::partial_sort(begin(), std::min(begin() + N, end()), end(), [value_function = std::forward<F>(value_function)](T& val1, T& val2) {return value_function(val1) < value_function(val2); });
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T>>, typename = void>
		self_rettype partial_sort(std::size_t N, F&& comparator_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>, and T is the type of the values contained within this container.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			std::partial_sort(begin(), std::min(begin() + N, end()), end(), std::forward<F>(comparator_function));
			return *this;
		}



		self_rettype stable_sort()
		{
			static_assert(templ::is_sortable_v<T>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort() on an container created from a const referenced container, a Copy() is required first.");

			std::stable_sort(begin(), end());
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T, T>>>
		self_rettype stable_sort(F&& value_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>, and T is the type of the values contained within this container.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			std::stable_sort(begin(), end(), [value_function = std::forward<F>(value_function)](T val1, T val2) {return value_function(val1) < value_function(val2); });
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F, T, T>, typename SFINAE_GUARD = typename std::enable_if<!templ::is_defined_v<F, T>>, typename = void>
		self_rettype stable_sort(F&& comparator_function)
		{
			static_assert(!std::is_void<R>::value, "Passed functor must have one of the following signatures: R(T), bool(T, T), where R is any type that defines operator< and operator>, and T is the type of the values contained within this container.");
			static_assert(templ::is_sortable_v<R>, "Values of the return type of the passed functor must define greater-than and less-than operators.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			std::stable_sort(begin(), end(), std::forward<F>(comparator_function));
			return *this;
		}



		/*=== PARTITIONING ===*/

		template <typename F>
		self_rettype partition(F&& partitioning_function)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), where T is the type of the values contained within this container.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			std::partition(begin(), end(), std::forward<F>(partitioning_function));
			return *this;
		}

		template <typename F>
		bool is_partitioned(F&& partitioning_function)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), where T is the type of the values contained within this container.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			return std::is_partitioned(begin(), end(), std::forward<F>(partitioning_function));
		}

		template <typename F>
		self_rettype stable_partition(F&& partitioning_function)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), where T is the type of the values contained within this container.");
			static_assert(!IsContConst, "Cannot call sort(F func) on an container created from a const referenced container, a Copy() is required first.");

			std::stable_partition(begin(), end(), std::forward<F>(partitioning_function));
			return *this;
		}

		/*=== NON-MODIFYING SEQUENCE OPERATIONS ===*/

		template <typename F>
		bool all_of(F&& filter_func) const
		{
			static_assert(templ::is_filter_v<F, T> && !templ::is_counter_func_v<F, T>, "Passed functor must have the following signature: bool(T), where T is the type of the contained values.");

			return std::all_of(begin(), end(), std::forward<F>(filter_func));
		}

		template <typename F>
		bool any_of(F&& filter_func) const
		{
			static_assert(templ::is_filter_v<F, T> && !templ::is_counter_func_v<F, T>, "Passed functor must have the following signature: bool(T), where T is the type of the contained values.");

			return std::any_of(begin(), end(), std::forward<F>(filter_func));
		}

		template <typename F>
		bool none_of(F&& filter_func) const
		{
			static_assert(templ::is_filter_v<F, T> && !templ::is_counter_func_v<F, T>, "Passed functor must have the following signature: bool(T), where T is the type of the contained values.");

			return std::none_of(begin(), end(), std::forward<F>(filter_func));
		}

		bool any() const
		{
			return size() > 0;
		}

		std::size_t count() const
		{
			return size();
		}

		std::size_t count(const T& val) const
		{
			return std::count(begin(), end(), val);
		}

		template <typename F>
		std::size_t count_if(F&& filter_func) const
		{
			static_assert(templ::is_filter_v<F, T> && !templ::is_counter_func_v<F, T>, "Passed functor must have the following signature: bool(T).");

			return std::count_if(begin(), end(), std::forward<F>(filter_func));
		}

		// TODO ADD FIND_IF ADJACENT_FIND

		template <typename F>
		T find_if(F&& selection_func)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have the following signature: bool(T).");

			auto v = std::find_if(begin(), end(), std::forward<F>(selection_func));

		}

		bool contains(const T& val) const
		{
			return std::find(begin(), end(), val) != end();
		}

		template <bool B0, typename C, bool B1>
		bool equal(const container<T, B0, C, B1>& other) const
		{
			static_assert(templ::is_comparable_v<T>, "Type of the contained values must implement operator== and operator!=.");

			return std::equal(begin(), end(), other.begin());
		}

		/*=== MODIFYING SEQUENCE OPERARIONS ===*/

		container<T> copy()
		{
			container<T> ret;
			ret.reserve(size());
			std::copy(begin(), end(), ret.begin());
			return ret;
		}

		template <typename F, std::size_t N>
		container<T> copy_if(F&& filter_func, reserve::res_var<N> reserve_amount = reserve::half())
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T) or bool(T, int), where T is the type of the contained values.");

			int n = 0;

			container<T> ret;
			ret.reserve(size() * reserve_amount.value()); // RESERVE
			std::copy_if(begin(), end(), std::back_inserter(ret), [&n, filter_func = std::forward<T>(filter_func)](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(filter_func), val, n++); });
			return ret;		
		}

		container<T> copy_n(std::size_t N)
		{
			container<T> ret;
			ret.reserve(std::min(size(), N));
			std::copy(begin(), std::min(begin() + N, end()), ret.begin());
			return ret;
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type, typename F, std::size_t N>
		RetVal where(F&& filter_func, reserve::res_var<N> reserve_amount = reserve::half())
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T) or bool(T, int), where T is the type of the contained values.");
			static_assert(!IsRef || AllowAlloc, "Cannot call Where() on an container created from a referenced container, either a Copy() or specifying cpplinq::mem::auto_alloc as the first template argument is required first.");

			int n = 0;

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(size() * reserve_amount.value()); // RESERVE
				std::copy_if(begin(), end(), std::back_inserter(ret), [&n, filter_func = std::forward<T>(filter_func)](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<T>(filter_func), val, n++); });
				return ret;
			}
			else
			{
				erase(std::copy_if(begin(), end(), begin(), [&n, filter_func = std::forward<T>(filter_func)](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<T>(filter_func), val, n++); }), end());
				return *this;
			}
		}

		container<T> move()
		{
			container<T> ret;
			ret.reserve(size());
			std::move(begin(), end(), ret.begin());
			return ret;
		}

		self_rettype fill(const T& val)
		{
			static_assert(!IsContConst, "Cannot call fill() on an container created from a const referenced container, a copy() is required first.");

			std::fill(begin(), end(), val);
			return *this;
		}

		self_rettype fill_n(std::size_t N, const T& val)
		{
			static_assert(!IsContConst, "Cannot call fill() on an container created from a const referenced container, a copy() is required first.");

			std::fill(begin(), std::min(begin() + N, end()), val);
			return *this;
		}

		template <typename F, typename R = typename templ::safe_result_of_t<F, T>, typename SFINAE_GUARD = typename std::enable_if<!std::is_same<R, T>::value || IsContConst>::type>
		container<R> transform(F&& transform_func)
		{
			static_assert(!std::is_same<R, void>::value, "Passed functor must have one of the following signatures: R(T), R(T, int), T(T), T(T, int), where R is any type, and T is the type of values stored in the container.");

			int n = 0;

			container<R> ret;
			ret.reserve(size());
			std::transform(begin(), end(), std::back_inserter(ret), [&n, transform_func = std::forward<F>(transform_func)](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(transform_func), val, n++); });
			return ret;
		}

		template <typename F, typename R = typename templ::safe_result_of_t<F, T>, typename SFINAE_GUARD = typename std::enable_if<std::is_same<R, T>::value && !IsContConst>::type>
		self_rettype transform(F&& transform_func)
		{
			static_assert(!std::is_same<R, void>::value, "Passed functor must have one of the following signatures: R(T), R(T, int), T(T), T(T, int), where R is any type, and T is the type of values stored in the container.");

			int n = 0;

			std::transform(begin(), end(), begin(), [&n, transform_func = std::forward<F>(transform_func)](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(transform_func), val, n++); });
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F>>
		self_rettype generate(F&& generator_func)
		{
			static_assert(std::is_same<R, T>::value, "Passed functor must have the following signature: T().");

			int n = 0;

			std::generate(begin(), end(), std::forward<F>(generator_func));
			return *this;
		}

		template <typename F, typename R = typename templ::sanitizer_t<F>>
		self_rettype generate_n(std::size_t N, F&& generator_func)
		{
			static_assert(std::is_same<R, T>::value, "Passed functor must have the following signature: T().");

			std::generate(begin(), std::min(begin() + N, end()), std::forward<F>(generator_func));
			return *this;
		}

		self_rettype replace(const T& oldval, const T& newval)
		{
			static_assert(!IsContConst, "Cannot call replace() on a container created from a const referenced container, use replace_copy() instead.");

			std::replace(begin(), end(), oldval, newval);
			return *this;
		}

		template <typename F>
		self_rettype replace_if(F&& filter_func, const T& newval)
		{
			static_assert(!IsContConst, "Cannot call replace_if() on a container created from a const referenced container, use replace_copy_if() instead.");
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), bool(T, int).");

			std::size_t n = 0;

			std::replace_if(begin(), end(), [filter_func = std::forward<F>(filter_func), &n](T& val) {templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(filter_func), val, n++); }, newval);
			return *this;
		}

		container<T> replace_copy(const T& oldval, const T& newval)
		{
			container<T> ret;
			ret.reserve(size());

			std::replace_copy(begin(), end(), ret.begin(), oldval, newval);
			return ret;
		}

		template <typename F>
		container<T> replace_copy_if(F&& filter_func, const T& newval)
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), bool(T, int).");
			
			std::size_t n = 0;

			container<T> ret;
			ret.reserve(size());
			std::replace_copy_if(begin(), end(), ret.begin(), [filter_func = std::forward<F>(filter_func), &n](T& val) {templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(filter_func), val, n++); }, newval);
			return ret;
		}

		template <typename C>
		self_rettype swap_ranges(container<T, true, C>& other)
		{
			std::swap_ranges(begin(), begin() + std::min(size(), other.size()), other.begin());
			return *this;
		}

		self_rettype reverse()
		{
			static_assert(!IsContConst, "Cannot call reverse() on a container created from a const referenced container, use reverse_copy() instead.");

			std::reverse(begin(), end());
			return *this;
		}

		container<T> reverse_copy()
		{

			container<T> ret;
			ret.reserve(size());

			std::reverse_copy(begin(), end());
			return *this;
		}

		self_rettype rotate(int n)
		{
			static_assert(!IsContConst, "Cannot call rotate() on a container created from a const referenced container, use rotate_copy() instead.");

			std::size_t real_rot = n % size();

			std::rotate(begin(), begin() + real_rot, end());
			return *this;
		}
		
		container<T> rotate_copy(int n)
		{
			std::size_t real_rot = n % size();

			container<T> ret;
			ret.reserve(size());

			std::rotate(begin(), begin() + real_rot, end(), ret.begin());
			return *this;
		}

		template <typename RNG>
		self_rettype shuffle(RNG&& RNG_source)
		{
			static_assert(!IsContConst, "Cannot call rotate() on a container created from a const referenced container, use rotate_copy() instead.");

			std::shuffle(begin(), end(), std::forward<RNG>(RNG_source));
			return *this;
		}

		self_rettype unique()
		{
			static_assert(!IsRef, "Cannot call unique() on a container created from a referenced container, use unique_copy() instead.");
			static_assert(templ::is_comparable_v<T>, "T must implement operator== and operator!=.");

			std::erase(std::unique(begin(), end()), end());
			return *this;
		}

		template <std::size_t N>
		container<T> unique_copy(reserve::res_var<N> reserve_amount = reserve::half())
		{
			static_assert(templ::is_comparable_v<T>, "T must implement operator== and operator!=.");

			container<T> ret;
			ret.reserve(size() * reserve_amount.value());
			std::unique_copy(begin(), end(), std::back_inserter(ret));
			return ret;
		}

		template <typename F, typename RC = typename templ::safe_result_of_t<F, T>, typename R = typename templ::value_type_t<RC>, std::size_t N>
		container<R> select_many(F&& select_func, reserve::res_var<N> reserve_amount = reserve::two())
		{
			static_assert(!std::is_same<RC, void>::value, "Passed selection functor must have the following signature: RC(T) or RC(T, int), where RC is a container, and T is the type of the contained elements within this container.");
			static_assert(!templ::is_container<RC>::value, "Return type of the passed functor must implement begin() and end().");

			container<R> ret;
			ret.reserve(size() * reserve_amount.value()); // RESERVING

			int n = 0;

			std::for_each(begin(), end(), [&n, &ret, select_func = std::forward<F>(select_func)](T val)
			{
				RC rc = templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(select_func), val, n++);
				std::copy(rc.begin(), rc.end(), std::back_inserter(ret));
			});

			return ret;
		}

		template <typename F, typename P, typename RC = typename templ::safe_result_of_t<F, T>, typename RC_val = typename templ::value_type_t<RC>, typename R = typename templ::sanitizer_t<P, RC_val, T>, std::size_t N>
		container<R> select_many(F&& select_func, P&& result_func, reserve::res_var<N> reserve_amount = reserve::two())
		{
			static_assert(!std::is_same<RC, void>::value, "Passed selection functor must have the following signature: RC(T) or RC(T, int), where RC is a container, and T is the type of the contained elements within this container.");
			static_assert(templ::is_container<RC>::value, "Return type of the passed selection functor must implement begin() and end().");
			static_assert(!std::is_same<R, void>::value, "Passed result selector functor must have the following signature: R(RC_val, T), where R is any type, RC_val is the type of the contained elements of container returned by the selection functor, and T is the type of the contained elements within this container.");

			container<R> ret;
			ret.reserve(size()); // RESERVING

			int n = 0;

			std::for_each(begin(), end(), [&n, &ret, select_func = std::forward<F>(select_func), result_func = std::forward<P>(result_func)](T val)
			{
				RC rc = templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(select_func), val, n++);
				std::transform(rc.begin(), rc.end(), std::back_inserter(ret), [result_func = std::forward<P>(result_func), &val](RC_val rcval) {return result_selector(rcval, val); });
			});

			return ret;
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type>
		RetVal skip(std::size_t n)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Skip() on an container created from a referenced container, a Copy() or setting the first template argument to cpplinq::mem::auto_alloc is required first.");

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(std::max(size() - n, 0));
				std::copy(begin() + n, end(), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(begin(), begin() + std::min(n, size()));
				return *this;
			}
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type>
		RetVal take(std::size_t n)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Take() on an container created from a referenced container, a Copy() or setting the first template argument to cpplinq::mem::auto_alloc is required first.");

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(std::min(size(), n));
				std::copy(begin(), begin() + std::min(n, size()), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(begin(), begin() + std::min(n, size()));
				return *this;
			}
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type, typename F, std::size_t N>
		RetVal take_while(F&& filter_func, reserve::res_var<N> reserve_amount = reserve::third())
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), bool(T, int), where T is the type of the contained values.");
			static_assert(!IsRef || AllowAlloc, "Cannot call TakeWhile() on an container created from a referenced container, a Copy() is required first.");

			int n = 0;

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(size() * reserve_amount.value()); // RESERVE
				std::copy(begin(), std::find_if_not(begin(), end(), [filter_func = std::forward<F>(filter_func), &n](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(fstd::forward<F>(filter_func), val, n++); }), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(std::find_if_not(begin(), end(), [filter_func = std::forward<F>(filter_func), &n](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(filter_func), val, n++); }), end());
				return *this;
			}
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type, typename F, std::size_t N>
		RetVal skip_while(F&& filter_func, reserve::res_var<N> reserve_amount = reserve::two_thirds())
		{
			static_assert(templ::is_filter_v<F, T>, "Passed functor must have one of the following signatures: bool(T), bool(T, int), where T is the type of the contained values.");
			static_assert(!IsRef || AllowAlloc, "Cannot call SkipWhile() on an container created from a referenced container, a Copy() is required first.");

			int n = 0;

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(size() * reserve_amount.value()); // RESERVE
				std::copy(std::find_if_not(begin(), end(), [filter_func = std::forward<F>(filter_func), &n](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(filter_func), val, n++); }), end(), std::back_inserter(ret));
				return ret;
			}
			else
			{
				erase(begin(), std::find_if_not(begin(), end(), [filter_func = std::forward<F>(filter_func), &n](T val) {return templ::counter_func_lambda<templ::is_counter_func_v<F, T>>::ret(std::forward<F>(filter_func), val, n++); }));
				return *this;
			}
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type, bool B0, typename C, bool B1>
		RetVal concat(const container<T, B0, C, B1>& other)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call Concat() on an container created from a referenced container, a Copy() is required first.");

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
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

		/*=== SET OPERATIONS ===*/
		// fuckit we're doing it live
		
		template <bool B, typename C, bool C1>
		container<T> merge(const container<T, B, C, C1>& other)
		{
			static_assert(templ::is_sortable_v<T>, "Contained values must define operator< and operator>.");
			container<T> ret;
			ret.reserve(size() + other.size());
			std::merge(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <bool B, typename C, bool C1, typename F>
		container<T> merge(const container<T, B, C, C1>& other, F&& comparator_func)
		{
			static_assert(!std::is_same<typename templ::sanitizer_t<F, T, T>, bool>::value, "Passed functor must have the following signature: bool(T, T).");

			container<T> ret;
			ret.reserve(size() + other.size());
			std::merge(begin(), end(), other.begin(), other.end(), std::back_inserter(ret), std::forward<F>(comparator_func));
			return ret;
		}
		


		template <bool B0, typename C, bool B1, std::size_t N1, std::size_t N2>
		container<T> set_difference(const container<T, B0, C, B1>& other, reserve::res_var<N1> reserve_amount1 = reserve::half(), reserve::res_var<N2> reserve_amount2 = reserve::half())
		{
			static_assert(templ::is_sortable_v<T>, "Contained values must define operator< and operator>.");

			container<T> ret;
			ret.reserve(size() * reserve_amount1.value() + other.size() * reserve_amount2.value()); // RESERVE
			std::set_difference(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <bool B0, typename C, bool B1, typename F, std::size_t N1, std::size_t N2>
		container<T> set_difference(const container<T, B0, C, B1>& other, F&& comparator_func, reserve::res_var<N1> reserve_amount1 = reserve::half(), reserve::res_var<N2> reserve_amount2 = reserve::half())
		{
			static_assert(!std::is_same<typename templ::sanitizer_t<F, T, T>, bool>::value, "Passed functor must have the following signature: bool(T, T).");

			container<T> ret;
			ret.reserve(size() * reserve_amount1.value() + other.size() * reserve_amount2.value()); // RESERVE
			std::set_difference(begin(), end(), other.begin(), other.end(), std::back_inserter(ret), std::forward<F>(comparator_func));
			return ret;
		}



		template <bool B0, typename C, bool B1, std::size_t N1, std::size_t N2>
		container<T> set_intersection(const container<T, B0, C, B1>& other, reserve::res_var<N1> reserve_amount1 = reserve::half(), reserve::res_var<N2> reserve_amount2 = reserve::half())
		{
			static_assert(templ::is_sortable_v<T>, "Contained values must define operator< and operator>.");

			container<T> ret;
			ret.reserve(size() * reserve_amount1.value() + other.size() * reserve_amount2.value()); // RESERVE
			std::set_difference(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <bool B0, typename C, bool B1, typename F, std::size_t N1, std::size_t N2>
		container<T> set_intersection(const container<T, B0, C, B1>& other, F&& comparator_func, reserve::res_var<N1> reserve_amount1 = reserve::half(), reserve::res_var<N2> reserve_amount2 = reserve::half())
		{
			static_assert(!std::is_same<typename templ::sanitizer_t<F, T, T>, bool>::value, "Passed functor must have the following signature: bool(T, T).");

			container<T> ret;
			ret.reserve(size() * reserve_amount1.value() + other.size() * reserve_amount2.value()); // RESERVE
			std::set_intersection(begin(), end(), other.begin(), other.end(), std::back_inserter(ret), std::forward<F>(comparator_func));
			return ret;
		}



		template <bool B0, typename C, bool B1, std::size_t N1, std::size_t N2>
		container<T> set_union(const container<T, B0, C, B1>& other, reserve::res_var<N1> reserve_amount1 = reserve::three_quarters(), reserve::res_var<N2> reserve_amount2 = reserve::three_quarters())
		{
			static_assert(templ::is_sortable_v<T>, "Contained values must define operator< and operator>.");

			container<T> ret;
			ret.reserve(size() * reserve_amount1.value() + other.size() * reserve_amount2.value()); // RESERVE
			std::set_union(begin(), end(), other.begin(), other.end(), std::back_inserter(ret));
			return ret;
		}

		template <bool B0, typename C, bool B1, typename F, std::size_t N1, std::size_t N2>
		container<T> set_union(const container<T, B0, C, B1>& other, F&& comparator_func, reserve::res_var<N1> reserve_amount1 = reserve::three_quarters(), reserve::res_var<N2> reserve_amount2 = reserve::three_quarters())
		{
			static_assert(!std::is_same<typename templ::sanitizer_t<F, T, T>, bool>::value, "Passed functor must have the following signature: bool(T, T).");

			container<T> ret;
			ret.reserve(size() * reserve_amount1.value() + other.size() * reserve_amount2.value()); // RESERVE
			std::set_union(begin(), end(), other.begin(), other.end(), std::back_inserter(ret), std::forward<F>(comparator_func));
			return ret;
		}
		
		/*=== MIN/MAX OPERATIONS ===*/

		template <typename F, typename N = typename templ::sanitizer_t<F, T>>
		N average(F&& number_func) const
		{
			static_assert(!std::is_void<N>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (this->size() == 0) { throw std::logic_error("container<T>::average(F func) | container<T> is empty."); }

			N val = N(0);
			std::for_each(begin(), end(), [&val, number_func = std::forward<F>(number_func)](T v) {val += number_func(v); });
			return val / size();
		}

		T average() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");
			if (this->size() == 0) { throw std::logic_error("container<T>::Average() | container<T> is empty."); }

			T val = T(0);
			std::for_each(begin(), end(), [&val](T v) {val += v; });
			return val / size();
		}

		template <typename F, typename N = typename templ::sanitizer_t<F, T>>
		N max(F&& number_func) const
		{
			static_assert(!std::is_void<N>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (size() == 0) { throw std::logic_error("container<T>::Max(F func) | container<T> is empty."); }

			N val;
			bool first = false;
			std::for_each(begin(), end(), [&val, &first, number_func = std::forward<F>(number_func)](T v) {if (!first) { first = true; val = number_func(v); } else { val = std::max(val, number_func(v)); }});
			return val;
		}

		T max() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");

			if (this->size() == 0) { throw std::logic_error("container<T>::Max() | container<T> is empty."); }

			T val;
			bool first = false;
			std::for_each(begin(), end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::max(val, v); }});
			return val;
		}

		template <typename F, typename N = typename templ::sanitizer_t<F, T>>
		N min(F&& number_func) const
		{
			static_assert(!std::is_void<N>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (size() == 0) { throw std::logic_error("container<T>::Min(F func) | container<T> is empty."); }

			N val;
			bool first = false;
			std::for_each(begin(), end(), [&val, &first, number_func = std::forward<F>(number_func)](T v) {if (!first) { first = true; val = number_func(v); } else { val = std::min(val, number_func(v)); }});
			return val;
		}

		T min() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");

			if (size() == 0) { throw std::logic_error("container<T>::Min() | container<T> is empty."); }

			T val;
			bool first = false;
			std::for_each(begin(), end(), [&val, &first](T v) {if (!first) { first = true; val = v; } else { val = std::min(val, v); }});
			return val;
		}

		template <typename F, typename N = typename templ::sanitizer_t<F, T>>
		std::pair<N, N> minmax(F&& number_func) const
		{
			static_assert(!std::is_void<N>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			if (size() == 0) { throw std::logic_error("container<T>::Min(F func) | container<T> is empty."); }

			N min, max;
			bool first = false;
			std::for_each(begin(), end(), [&min, &max, &first, number_func = std::forward<F>(number_func)](T v) {if (!first) { first = true; min = number_func(v); max = number_func(v); } else { auto v = number_func(v);  min = std::min(min, v);  max = std::max(max, v); }});
			return std::make_pair(min, max);
		}

		std::pair<T, T> minmax() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");

			if (size() == 0) { throw std::logic_error("container<T>::Min() | container<T> is empty."); }

			T min, max;
			bool first = false;
			std::for_each(begin(), end(), [&min, &max, &first](T v) {if (!first) { first = true; min = v; max = v; } else { min = std::min(min, v); max = std::max(max, v);}});
			return std::make_pair(min, max);
		}

		template <typename F, typename N = typename templ::sanitizer_t<F, T>>
		N sum(F func) const
		{
			static_assert(!std::is_void<N>::value, "Passed functor must have the following signature: N(T), where N is any arithmetic type.");
			static_assert(std::is_arithmetic<N>::value, "Passed functor must return an arithmetic type.");

			N val = N(0);
			std::for_each(this->begin(), this->end(), [&val, &func](T v) {val += func(v); });
			return val;
		}

		T sum() const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");

			T val = T(0);
			std::for_each(this->begin(), this->end(), [&val](T v) {val += v; });
			return val;
		}

		template <bool B, typename C, bool C1>
		bool is_permutation(const container<T, B, C, C1>& other)
		{
			static_assert(templ::is_comparable_v<T>, "T must implement operator== and operator!=.");

			return std::is_permutation(begin(), end(), other.begin(), other.end());
		}

		template <typename F, bool B, typename C, bool C1 >
		bool is_permutation(F&& comparator_func, const container<T, B, C, C1>& other)
		{
			static_assert(!std::is_same<typename templ::sanitizer_t<F, T, T>, bool>::value, "Passed functor must have the following signature: bool(T, T).");

			return std::is_permutation(begin(), end(), other.begin(), other.end(), std::forward<F>(comparator_func));
		}

		/*=== NUMERIC OPERATIONS ===*/

		template <typename F>
		T accumulate(F&& accumulating_func)
		{
			static_assert(!std::is_void<templ::sanitizer_t<F, T, T, T>>::value, "Passed functor must have one of the following signatures: T(T, T), V(V, T), where V is any complete type.");

			return std::accumulate(begin(), end(), T(), std::forward<F>(accumulating_func));
		}

		template <typename F, typename V>
		V accumulate(V initial_val, F&& accumulating_func) 
		{
			static_assert(!std::is_void<templ::sanitizer_t<F, V, V, T>>::value, "Passed functor must have one of the following signatures: T(T, T), V(V, T), where V is any complete type.");

			return std::accumulate(begin(), end(), initial_val, std::forward<F>(accumulating_func));
		}

		template <typename F, typename R, typename V, typename E = templ::sanitizer_t<R, V>>
		E accumulate(V initial_val, F&& accumulating_func, R&& return_func)
		{
			static_assert(!std::is_void<typename templ::sanitizer_t<F, V, V, T>>::value, "Passed functor must have the following signature: V(V, T), where V is any complete type.");
			static_assert(!std::is_void<E>::value, "Passed return functor must have the following signature: E(V), where E is any complete type and V is the type returned by the accumulating functor.");

			return return_func(std::accumulate(begin(), end(), initial_val, std::forward<F>(accumulating_func)));
		}
		
		template <bool B, typename C, bool C1, typename F1, typename F2, typename V, typename R = typename templ::sanitizer_t<F2, T, T>>
		V inner_product(const container<T, B, C, C1>& other, V starting_val, F1&& func1, F2&& func2)
		{
			static_assert(!std::is_void<R>::value, "Second passed functor must have the following signature: R(T, T), where R is any complete type.");
			static_assert(std::is_same<typename templ::sanitizer_t<F1, V, R>, V>::value, "First passed functor must have the following signature: V(V, R), where V is the type of the passed initial value, and R is the type returned by the second passed functor.");		

			return std::inner_product(begin(), begin() + std::min(size(), other.size()), other.begin(), starting_val, std::forward<F1>(func1), std::forward<F2>(func2));
		}	
		
		template <bool B, typename C, bool C1, typename V, typename R = typename templ::sanitizer_t<templ::mul<T, T>, T, T>>
		V inner_product(const container<T, B, C, C1>& other, V starting_val)
		{
			static_assert(!std::is_void<R>::value, "The contained type must define operator* with itself.");
			static_assert(!std::is_void<templ::sanitizer_t<templ::plus<V, R>, V, R>>::value, "The result of operator* of the contained type with itself must be addable to the passed starting value and produce a value of the same type as the starting value.");
			static_assert(!std::is_same<V, templ::sanitizer_t<templ::plus<V, R>, V, R>>::value, "The result of operator* of the contained type with itself must be addable to the passed starting value and produce a value of the same type as the starting value.");

			return std::inner_product(begin(), begin() + std::min(size(), other.size()), other.begin(), starting_val);
		}

		/*=== GENERATION OPERATORS ===*/

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type>
		RetVal default_if_empty()
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call DefaultIfEmpty() on an container created from a referenced container, a Copy() is required first.");

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(size());
				std::copy(begin(), end(), std::back_inserter(ret));
				if (any()) { return ret; }
				else { ret.push_back(T()); return ret; }
			}
			else
			{
				if (any()) { return *this; }
				else { push_back(T()); return *this; }
			}
		}

		template <alloc_mode MemMode = alloc_mode::no_alloc, bool AllowAlloc = (MemMode == alloc_mode::auto_alloc), typename RetVal = typename std::conditional<IsRef && AllowAlloc, container<T>, container<T>&>::type
		>
		RetVal default_if_empty(const T& val)
		{
			static_assert(!IsRef || AllowAlloc, "Cannot call DefaultIfEmpty() on an container created from a referenced container, a Copy() is required first.");

			if (IsRef && AllowAlloc)
			{
				container<T> ret;
				ret.reserve(size());
				std::copy(begin(), end(), std::back_inserter(ret));
				if (any()) { return ret; }
				else { ret.push_back(val); return ret; }
			}
			else
			{
				if (any()) { return *this; }
				else { push_back(val); return *this; }
			}
		}

		static container<T> empty()
		{
			return container<T>();
		}

		static container<T> range(const T& init, std::size_t count, const T& inc = T(1))
		{
			container<T> ret;

			T val = init;

			for (std::size_t i = 0; i < count; i++)
			{
				ret.push_back(val);
				val += inc;
			}

			return ret;
		}

		static container<T> repeat(const T& val, std::size_t count)
		{
			container<T> ret;
			for (std::size_t i = 0; i < count; i++)
			{
				ret.push_back(val);
			}
			return ret;
		}

		/*=== ELEMENT OPERATORS ===*/

		T at(std::size_t index) 
		{
			if (index < size()) { return *this[index]; }
			else throw std::out_of_range("container<T>::ElementAt(int index) | Index out of range.");
		}

		T at_or_default(std::size_t index) 
		{
			if (index < size()) { return *this[index]; }
			else return T();
		}

		T first() 
		{
			if (any()) { return *this[0]; }
			else throw std::length_error("container<T>::First() | container<T> is empty.");
		}

		template <typename F>
		T first(F&& selector_func)
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			auto v = std::find_if(begin(), end(), std::forward<F>(selector_func));
			if (v == end())
			{
				throw std::length_error("container<T>::first(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T first_or_default() 
		{
			if (any()) { return first(); }
			else return T();
		}

		template <typename F>
		T first_or_default(F&& selector_func)
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			auto v = std::find_if(begin(), end(), std::forward<F>(selector_func));
			if (v == end())
			{
				return T();
			}
			return *v;
		}

		T last() 
		{
			if (any()) { return *this[size()-1]; }
			else throw std::length_error("container<T>::last() | container<T> is empty.");
		}

		template <typename F>
		T last(F&& selector_func)
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			auto v = end();
			while (--v, !(selector_func(*(v)) || v == begin()));

			if (v == begin())
			{
				throw std::length_error("container<T>::last(F func) | No element satisfies the selection function.");
			}
			return *v;
		}

		T last_or_default() 
		{
			if (any()) { return *this[size() - 1]; }
			else return T();
		}

		template <typename F>
		T last_or_default(F&& selector_func)
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			auto v = end();
			while (--v, !(selector_func(*(v)) || v == begin()));

			if (v == begin())
			{
				return T();
			}
			return *v;
		}

		T single() 
		{
			if (size() == 1) { return first(); }
			else throw std::length_error("IEnumberable<T>::single() | Size isn't exactly 1.");
		}

		template <typename F>
		T single(F&& selector_func)
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			T retval;
			int n = 0;

			std::for_each(begin(), end(), [&retval, selector_func = std::forward<F>(selector_func), &n](T val)
			{
				if (selector_func(val))
				{
					if (n == 0) { n++; retval = val; }
					else
					{
						throw std::length_error("container<T>::single(F func) | More than 1 element matched the selection function.");
					}
				}
			});

			if (n == 1) { return retval; }
			throw std::length_error("IEnumberable<T>::Single(F func) | No elements matched the selection function.");
		}

		T single_or_default() 
		{
			if (size() == 1) { return first(); }
			else if (size() == 0) { return T(); }
			else throw std::length_error("container<T>::single_or_default() | Size of container<T> is bigger than 1.");
		}

		template <typename F>
		T single_or_default(F func) 
		{
			static_assert(templ::is_filter_v<F, T>, "The passed functor must have the following signature: bool(T).");

			T retval;
			int n = 0;

			std::for_each(begin(), end(), [&retval, &func, &n](T val)
			{
				if (func(val))
				{
					if (n == 0) { n++; retval = val; }
					else
					{
						throw std::length_error("container<T>::single_or_default(F func) | More than one element matched the selection function.");
					}
				}
			});

			if (n == 1) { return retval; }
			else return T();
		}

		/*=== JOIN OPERATORS ===*/

		template <bool B0, class C, bool B1, typename FOuterKey, typename FInnerKey, typename FJoin, typename TInner, typename TKey = typename templ::sanitizer_t<FOuterKey, T>, typename TJoined = typename templ::sanitizer_t<FJoin, T, TInner>, std::size_t N>
		container<TJoined> join(const container<TInner, B0, C, B1>& innerkeys, FOuterKey&& outerkeyselector, FInnerKey&& innerkeyselector, FJoin&& joiner, reserve::res_var<N> reserve_amount = reserve::one())
		{
			static_assert(!std::is_same<TKey, void>::value, "Passed outer key selector must have the following signature: TKey(T), where TKey is any type implementing operator== and operator!= and T is the type of the values contained within this container.");
			static_assert(!templ::is_comparable_v<TKey>, "The values returned by the outer key selector must implement operator== and operator!=.");
			static_assert(templ::is_func_v<FInnerKey, TKey, TInner>, "Passed inner key selector must have the following signature: TKey(TInner), where TKey is the type returned by the outer key selector and TInner is the type of the values contained within the innerkeys container.");
			static_assert(!std::is_same<TJoined, void>::value, "Passed joiner must have the following signature: TJoined(T, TInner), where TJoined is any type, T is the type of values contained within this container and TInner is the type of the values contained within the innerkeys container.");

			container<TJoined> ret; 

			std::vector<std::pair<TKey, TInner>> innervals;
			innervals.reserve(size()); // RESERVE unchangable
			ret.reserve(size() * reserve_amount.value()); // RESERVE
			
			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, innerkeyselector = std::forward<FInnerKey>(innerkeyselector)](TInner innerval) {innervals.push_back(std::make_pair(innerkeyselector(innerval), innerval)); });
			std::for_each(begin(), end(), [&innervals, outerkeyselector = std::forward<FOuterKey>(outerkeyselector), joiner = std::forward<FJoin>(joiner), &ret](T& outerval)
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

		template <bool B0, class C, bool B1, typename FOuterKey, typename FInnerKey, typename FJoin, typename TInner, typename TKey = typename templ::sanitizer_t<FOuterKey, T>, typename TJoined = typename templ::sanitizer_t<FJoin, T, templ::vector_iterator_t<TInner>, templ::vector_iterator_t<TInner>>, std::size_t N>
		container<TJoined> group_join(const container<TInner, B0, C, B1>& innerkeys, FOuterKey&& outerkeyselector, FInnerKey&& innerkeyselector, FJoin&& joiner, reserve::res_var<N> reserve_amount = reserve::one())
		{
			static_assert(!std::is_same<TKey, void>::value, "Passed outer key selector must have the following signature: TKey(T), where TKey is any type implementing operator== and operator!= and T is the type of the values contained within this container.");
			static_assert(!templ::is_comparable_v<TKey>, "The values returned by the outer key selector must implement operator== and operator!=.");
			static_assert(templ::is_func_v<FInnerKey, TInner, TKey>, "Passed inner key selector must have the following signature: TKey(TInner), where TKey is the type returned by the outer key selector and TInner is the type of the values contained within the innerkeys container.");
			static_assert(!std::is_same<TJoined, void>::value, "Passed joiner must have the following signature: TJoined(T, Iter<TInner>, Iter<TInner>), where TJoined is any type, T is the type of values contained within this container and Iter<TInner> is the type of an iterator to a collection of objects of type of the the type of the values contained within the innerkeys container.");

			container<TJoined> ret;

			std::vector<std::pair<TKey, TInner>> innervals;
			ret.reserve(size() * reserve_amount.value()); // RESERVE
			innervals.reserve(size()); // RESERVE, unchangable

			std::for_each(innerkeys.begin(), innerkeys.end(), [&innervals, innerkeyselector = std::forward<FInnerKey>(innerkeyselector)](TInner innerval) {innervals.push_back(std::make_pair(innerkeyselector(innerval), innerval)); });
			std::for_each(begin(), end(), [&innervals, outerkeyselector = std::forward<FOuterKey>(outerkeyselector), joiner = std::forward<FJoin>(joiner), &ret](T outerval)
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

		/*=== CONVERSION OPERATORS ===*/

		template<typename R>
		container<R> cast()
		{
			static_assert(std::is_convertible<T, R>::value, "T must be statically convertible to R.");

			container<R> ret;
			ret.reserve(size()); // RESERVE
			std::transform(begin(), end(), std::back_inserter(ret), [](const T& val) {return static_cast<R>(val); });
			return ret;
		}

		template <bool Workaround = true, typename SFINAE_GUARD = typename std::enable_if<Workaround && !IsRef>::type>
		std::vector<T> to_vector_move()
		{
			static_assert(!IsRef, "Cannot move from a referenced container, use to_vector() instead.");

			return std::vector<T>(std::move(*this));
		}

		std::vector<T> to_vector()
		{
			std::vector<T> ret;
			ret.reserve(size()); // RESERVE
			std::copy(begin(), end(), std::back_inserter(ret));
			return ret;
		}

		std::list<T> to_list()
		{
			std::list<T> ret;
			ret.reserve(size()); // RESERVE
			std::for_each(begin(), end(), std::back_inserter(ret));
			return ret;
		}

		std::forward_list<T> to_forward_list()
		{
			std::forward_list<T> ret;
			ret.reserve(size()); // RESERVE
			std::for_each(begin(), end(), std::back_inserter(ret));
			return ret;
		}

		std::set<T> to_set()
		{
			std::set<T> ret;
			ret.reserve(size()); // RESERVE
			std::for_each(begin(), end(), std::back_inserter(ret));
			return ret;
		}

		std::unordered_set<T> to_unordered_set()
		{
			std::unordered_set<T> ret;
			ret.reserve(size()); // RESERVE
			std::for_each(begin(), end(), std::back_inserter(ret));
			return ret;
		}

		template <typename F, typename K = typename templ::sanitizer_t<F, T>>
		std::map<K, T> to_map(F&& key_func)
		{
			static_assert(!std::is_void<K>::value, "Passed functor must have the following signature: K(T), where K is any type.");

			std::map<K, T> ret;
			ret.reserve(size()); // RESERVE
			std::for_each(begin(), end(), [&ret, key_func = std::forward<F>(key_func)](T val) {ret[key_func(val)] = val; });
			return ret;
		}

		template <typename F, typename K = typename templ::sanitizer_t<F, T>>
		std::unordered_map<K, T> to_unordered_map(F&& key_func)
		{
			static_assert(!std::is_void<K>::value, "Passed functor must have the following signature: K(T), where K is any type.");

			std::unordered_map<K, T> ret;
			ret.reserve(size()); // RESERVE
			std::for_each(begin(), end(), [&ret, key_func = std::forward<F>(key_func)](T val) {ret[key_func(val)] = val; });
			return ret;
		}
	};

	template <typename Cont, bool Const = std::is_const<Cont>::value, typename T = typename std::decay<decltype(*(std::declval<Cont>().begin()))>::type>
	container<T, true, Cont, Const> LINQ(Cont& cont)
	{
		static_assert(std::is_same<decltype(*(std::declval<Cont>().begin())), decltype(*(std::declval<Cont>().end()))>::value, "Types of begin() and end() iterators must be the same.");
		return container<T, true, Cont, Const>(cont);
	}

	template <typename Cont, typename T = typename templ::value_type_t<Cont>, typename _A = typename std::enable_if<!std::is_lvalue_reference<Cont>::value>::type>
	container<T> LINQ(Cont&& cont)
	{
		static_assert(std::is_same<decltype(*(std::declval<Cont>().begin())), decltype(*(std::declval<Cont>().end()))>::value, "Types of begin() and end() iterators must be the same.");
		container<T> ret;
		std::for_each(cont.begin(), cont.end(), [&ret](T val) {ret.push_back(val); });
		return ret;
	}
	
	template <typename T>
	container<T> LINQ(std::vector<T>&& cont)
	{
		return container<T>(std::forward<std::vector<T>&&>(cont));
	}
	
}
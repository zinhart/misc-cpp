#include <iostream>
#include <functional>
#include <utility>
#include <tuple>
#include <iostream>
#include <tuple>
#include <string>
namespace zinhart
{
  // helper function to print a tuple of any size
  template<class Tuple, std::uint64_t N>
	struct tuple_printer 
	{
	  static void print(const Tuple& t) 
	  {
	  	tuple_printer<Tuple, N-1>::print(t);
        std::cout << ", " << std::get<N-1>(t);
	  }
	};
  template<class Tuple>
	struct tuple_printer<Tuple, 1> 
	{
	  static void print(const Tuple& t) 
	  {
	  	std::cout << std::get<0>(t);
	  }
	};
  template<class... Args>
	void print(const std::tuple<Args...>& t) 
	{
	  std::cout << "(";
	  tuple_printer<decltype(t), sizeof...(Args)>::print(t);
	  std::cout << ")\n";
	}

  template <typename Return_Type>
	class functor {};

  // void specialization 
  template <typename... Params>
	class functor<void(Params...)> 
	{
	  private:
		std::function<void(Params...)> func_;
		std::tuple<Params...> old_params; // Contains the previous arguments.
	  public:
  		explicit functor(std::function<void(Params...)> func)
		  : func_(std::move(func)) {}
		template <typename... Args>
		  void operator() (Args&&... args) 
		{
		  auto new_params = std::make_tuple(std::forward<Args>(args)...);

		  // Check if new arguments.
		  if (new_params != old_params) 
		  {
	  		/*result =*/ func_(std::forward<Args>(args)...);
  			old_params = new_params;
			std::cout << "Function evaluated" << std::endl;
		  }
		  std::cout << "Void specialization" << std::endl;
//		  return result;
		  }
	};

  template <typename Return_Type, typename... Params>
	class functor<Return_Type(Params...)> 
	{
	  public:
		explicit functor(std::function<Return_Type(Params...)> func)
		  : func_(std::move(func)) {}
		void save(std::function<Return_Type(Params...)> func, Params &&... params)
		{
		  test_ = std::move(func);
		  test_params = std::make_tuple(params...);
		  print(test_params);
		}
		Return_Type execute()
		{
		  return called_stored(typename gens<sizeof...(Params)>::type());
		}

		template <typename... Args>
		Return_Type operator() (Args&&... args) 
		{
		  auto new_params = std::make_tuple(std::forward<Args>(args)...);

		  // Check if new arguments.
		  if (new_params != old_params) 
		  {
			  result = func_(std::forward<Args>(args)...);
			  old_params = new_params;
			  std::cout << "Function evaluated" << std::endl;
		  }
		  std::cout << "Returned result" << std::endl;
		  return result;
	  }

	  private:
		std::function<Return_Type(Params...)> func_;
		Return_Type result;
		std::tuple<Params...> old_params; // Contains the previous arguments.

		std::function<Return_Type(Params...)> test_;
		std::tuple<Params...> test_params;
		Return_Type result_two;

		// For iterating over the function args	
		template<int ...> struct seq {};
		template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};
		template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };
		template<int ...S>
		  Return_Type called_stored(seq<S...>)
		  {
			return test_(std::get<S>(test_params)...);
		  }
	};
};

int main() 
{
  using namespace zinhart;
  auto f = [] (int a, int b){ return a + b;};
  auto f1 = [](std::string s){ std::cout<<s<<"\n"; };


  // Specify function type as template parameter.
  // E.g. Return_Type(Param1Type, Param2Type, ..., ParamNType)

  functor<int(int, int)> ld(f);
  std::cout << ld(1, 2) << std::endl;
  std::cout << ld(1, 2) << std::endl;
  std::cout << ld(3, 4) << std::endl;


  ld.save(f, 6,7);
  std::cout << ld.execute() << std::endl;
	
  functor<void(std::string)> ld1(f1);	
  ld1("apples");
}

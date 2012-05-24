/*
 * Shows the use of the new 'run-time sanity checking' option available in OTL:
 *
 * You must provide your own OTL_ASSERT assertion check macro to ensure OTL
 * can do 'the right thing' when such a mistake is caught at run-time: after all,
 * very few applications are best served with the RTL assert() printf()+abort() 
 * behaviour.
 *
 * This code shows an example OTL_ASSERT macro which throws an instance of the
 * 'otl_assertion_failure' exception class (derived from 'otl_out_of_range_exception')
 * when a condition check fails.
 */
#include <iostream>
#include <string>
#include <stdio.h>

// like ex702, but now with STL enabled.
#define OTL_STL

// note: you can always override the exception class derivation by defining your own base class:
#define OTL_EXCEPTION_DERIVED_FROM		my_base_exc_class

class my_base_exc_class
{
public:
	my_base_exc_class() {}
	~my_base_exc_class() {}

	virtual const char *what() const
	{
		return "???";
	}
};


#define OTL_TRACE_LEVEL 0xFF
#define OTL_TRACE_STREAM  std::cerr

// enable run-time sanity checks in the OTL software;
// whether this option is ON or OFF only influences the compile-time inclusion of
// additional sanity, pre- and post-condition checks; the otl_out_of_range_exception class
// will always exist. This simplifies application code as you don't have to litter your
// code with conditional compilation #if..#endif checks.
#define OTL_SANITY_CHECKS_ENABLED  


// since you do not provide your own OTL_ASSERT, a simple default is defined by OTL:
// #ifndef OTL_ASSERT
// #define OTL_ASSERT(e)							\
//     if (!(e))									\
// 	  		throw_otl_out_of_range_exception(-1,	\
// 			"ASSERTION FAILED: '"					\
// 	  		#e										\
//   		"' at (" __FILE__ ":" __LINE__ ")")	
// #endif


// doesn't matter which DB type you pick for this example, really.

//#define OTL_ORA9I // Compile OTL 4.0/OCI9I
//#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
//#define OTL_ODBC // Compile OTL 4.0/MS genric ODBC
#include <otlv4.h> // include the OTL 4.0 header file


int main()
{
 int cnt = 0;
 bool expected = false;
 try
 {
	 std::cerr<<std::endl<< "Point 1 - code should show points 1..3, no gaps."<<std::endl;

	 int i = 0;
	 OTL_ASSERT(i < 1); // 0 < 1
	 cnt++;

	 std::cerr<<std::endl<< "Point 2"<<std::endl;

	 i = 1;
	 expected = true;
	 cnt++;
	 OTL_ASSERT(i < 1); // 1 < 1
	 cnt = 1000;
 }
 catch(my_base_exc_class& p)
 { 
	 std::cerr<<std::endl<< "Point 3: "<<(expected?"expected error":"**UNEXPECTED** error")<<std::endl;
	 // intercept OTL 'ex' exceptions
	 std::cerr<<p.what()<<std::endl; // print out error message

	 if (expected) cnt++; else cnt = 1000;
 }

 return (cnt == 3 ? 0 /* EXIT_SUCCESS */ : 1 /* EXIT_FAILURE */ );
}

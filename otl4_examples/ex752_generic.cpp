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

//#define OTL_STL

#define OTL_TRACE_LEVEL 0xFF
#define OTL_TRACE_STREAM  std::cerr

// enable run-time sanity checks in the OTL software;
// whether this option is ON or OFF only influences the compile-time inclusion of
// additional sanity, pre- and post-condition checks; the otl_out_of_range_exception class
// will always exist. This simplifies application code as you don't have to litter your
// code with conditional compilation #if..#endif checks.
#define OTL_SANITY_CHECKS_ENABLED  


// ------------------- portable ASSERT which throws exception ----------------

#if !defined(__func__)
// __func__ is part of C99, but not all compilers support this standard yet.
// GCC already had something like it before C99, and so does MSVC, though the latter
// is not 100% C99 yet, so we use the old style for that one every time.
#if defined(__STDC_VERSION__) && defined(__GNUC__)
# if __STDC_VERSION__ < 199901L && if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# endif
#endif
#if defined(_MSC_VER)
# if (_MSC_VER >= 1200)
#  define __func__ __FUNCTION__
# endif
#endif
#if !defined(__GNUC__) && !defined(_MSC_VER)
# define __func__ "<unknown>"
#endif
#endif

#define OTL_ASSERT(e)							\
    if (!(e)) otl_report_assertion_failed		\
    (__FILE__, __func__, __LINE__, #e)

// ------------------- END: portable ASSERT which throws exception ----------------


// doesn't matter which DB type you pick for this example, really.

//#define OTL_ORA9I // Compile OTL 4.0/OCI9I
//#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
//#define OTL_ODBC // Compile OTL 4.0/MS genric ODBC
#include <otlv4.h> // include the OTL 4.0 header file






class otl_assertion_failure: public otl_out_of_range_exception
{
protected:
	const char *srcfile;
	const char *funcname;
	int srcline;

public:
	otl_assertion_failure(const char *filename, const char *function, const int line, const char *expr)
		: srcfile(filename), funcname(function), srcline(line),
		  otl_out_of_range_exception(expr, -1)
	{}

	const char *srcpath() const
	{
		return srcfile ? srcfile : "<unknown>";
	}
	const char *function() const
	{
		return funcname ? funcname : "<unknown>";
	}
	int srclineno() const
	{
		return srcline;
	}
};

void otl_report_assertion_failed(const char *filename, const char *function, const int line, const char *expr)
{
	throw otl_assertion_failure(filename, function, line, expr);
}






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
 catch(otl_assertion_failure& p)
 { 
	 std::cerr<<std::endl<< "Point 3: "<<(expected?"expected error":"**UNEXPECTED** error")<<std::endl;
	// intercept OTL 'ex' exceptions
	 std::cerr<<p.srcpath()<<":"<<p.srclineno()
		 <<"("<<p.function()<<") assertion failed: "<<p.what()<<std::endl; // print out error message

	 if (expected) cnt++; else cnt = 1000;
 }

 return (cnt == 3 ? 0 /* EXIT_SUCCESS */ : 1 /* EXIT_FAILURE */ );
}

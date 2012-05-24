/*
 * Shows the use of otl_long_string access methods and what can go wrong.
 * (Off by one indexing mistake by application programmer. pre-build202 this
 * will lead to indeterministic behaviour -- it largely depends on your
 * OS and C++ run-time library heap manager, but heap corruption was a serious risk, given
 * such programming mistakes.
 *
 * Since build202, OTL includes run-time range checking, which will throw an exception
 * instead to allow the application to cope/abort in a deterministic fashion.
 */
#include <iostream>
#include <string>
#include <stdio.h>

#define OTL_STL  // also try to compile this example with this line commented out; see comments at the 'catch()'es

#define OTL_EXPLICIT_NAMESPACES

#define OTL_TRACE_LEVEL 0xFF
#define OTL_TRACE_STREAM  std::cerr

// enable run-time sanity checks in the OTL software;
// whether this option is ON or OFF only influences the compile-time inclusion of
// additional sanity, pre- and post-condition checks; the otl_out_of_range_exception class
// will always exist. This simplifies application code as you don't have to litter your
// code with conditional compilation #if..#endif checks.
#define OTL_SANITY_CHECKS_ENABLED  

// doesn't matter which DB type you pick for this example, really.

//#define OTL_ORA9I // Compile OTL 4.0/OCI9I
//#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
//#define OTL_ODBC // Compile OTL 4.0/MS genric ODBC
#include <otlv4.h> // include the OTL 4.0 header file


int main()
{
 otl_long_string s1(28);
 otl_long_string s2(50);

 std::cerr<<"'Point' numbers must be in order, without gaps in the numbers."<<std::endl<<std::endl;

 bool expected = false;
 try
 {
	 // pre-build 202 would now have allocated 28+1 bytes for s1
	 int i;
	 for (i = 0; i < 28; i++)
		 s1[i] = 'a';
	 s1.set_len(28-1); // MUST be <=N-1 in build202 as otl now will reserve space for a NUL sentinel, always

	 for (i = 0; i < 50; i++)
		 s2[i] = 'a';
	 s2.set_len(50-1);

	 std::cerr<<std::endl<< "Point 1"<<std::endl;

	 expected = true;
	 // this MUST throw an ex as it is a programming error, writing out of bounds:
	 s1[29]=0;

	 // And this is harmless *HERE* in pre202, due to the alloc+1 in the constructor back then
	 // note however that the copy constructor did not do that, so this is a _potential_ out of bounds
	 // write op in pre202 as well, depending on what happened before!
	 s1[28]=0;
 }
 catch(std::out_of_range& p)
 { 
	 std::cerr<<std::endl<< "Point 2-A: "<<(expected?"expected error":"**UNEXPECTED** error")<<std::endl;
	 // intercept OTL 'out_of_range' exceptions too!
	 std::cerr<<p.what()<<std::endl; // print out error message
 }
 // Expected warning: C4286: 'otl_out_of_range_exception &' : is caught by base class ('std::out_of_range &')
 // See also comment below.
 catch(otl_out_of_range_exception& p)
 { 
	 // this one comes after 'std::out_of_range' catch so should NOT be used by the compiler,
	 // as the otl_out_of_range_exception class is derived from STL:out_of_range when
	 // OTL_STL is #defined...
	 //
	 // remove the OTL_STL #define though, or override the inheritance by defining your own
	 // OTL_EXCEPTION_DERIVED_FROM (see also ex703) and this one should catch the (intended) fault
	 // above.
	 std::cerr<<std::endl<< "Point 2-B: "<<
#if !defined(OTL_STL)
		 "expected error"
#else
		 "**UNEXPECTED** error"
#endif
		 <<std::endl;
	 std::cerr<<"("<<p.failure_code()<<") "<<p.what()<<std::endl; // print out error message
 }

 expected = false;
 try
 {
	 int i;
	 for (i = 0; i < 28; i++)
		 s1[i] = 'a';
	 expected = true;
	 s1.set_len(28); // EXCEPTION, because MUST be <=N-1 in build202 as otl now will reserve space for a NUL sentinel, always
 }
 catch(otl_out_of_range_exception& p)
 { 
	 std::cerr<<std::endl<< "Point 3: "<<(expected?"expected error":"**UNEXPECTED** error")<<std::endl;
	// intercept OTL 'ex' exceptions
	std::cerr<<"("<<p.failure_code()<<") "<<p.what()<<std::endl; // print out error message
 }

 expected = false;
 try
 {
	// pre202 would alloc N bytes (not N+1 ;-) ) when copying in a string bigger than the original buffer size.
	s1 = s2;

	// should be okay: now buf_size ==50+1
    s1[28]=0;
    s1[29]=0;
	expected = true;
    s1[50]=0;	// this is out of bounds indexed access
 }
 catch(otl_out_of_range_exception& p)
 { 
	std::cerr<<std::endl<< "Point 4: "<<(expected?"expected error":"**UNEXPECTED** error")<<std::endl;
	// intercept OTL 'ex' exceptions
	std::cerr<<"("<<p.failure_code()<<") "<<p.what()<<std::endl; // print out error message
 }

 try
 {
	// pre202: same goes for this constructor: would allocate N; the size-arg constructor was the odd one out with N+1.
	otl_long_string s3(s2);

	std::cerr<<std::endl<<"Point 5"<<std::endl;
	std::cerr<<s3.c_str()<<std::endl;
	std::cerr<<"length: "<<s3.len()<<std::endl;

	std::cerr<<std::endl<<"Point 6"<<std::endl;
	otl_long_string s4("Hello", 6, 5);

	// pre202: on good OS's, this should have invoked a hardware exception as a NUL sentinel was written
	// to const storage space; build202 protects agsinst this:
	s4.null_terminate_string(5);
 }
 catch(otl_out_of_range_exception& p)
 { 
	 std::cerr<<std::endl<< "Point XXX: UNEXPECTED error"<<std::endl;
	// intercept OTL 'ex' exceptions
	std::cerr<<"("<<p.failure_code()<<") "<<p.what()<<std::endl; // print out error message
 }

 try
 {
	char buf[50] = "Hello world";

	// external buffer-based otl_long_string instances copy BY REFERENCE
	// while internal (allocated) buffer based ones copy BY VALUE
	otl_long_string s3((const void *)buf, 50, strlen(buf));
	otl_long_string s4;

	std::cerr<<std::endl<<"Point 7"<<std::endl;

	s4 = s3;
	std::cerr<<s4.c_str()<<std::endl;

	std::cerr<<"Expect a coredump if you're running on a _proper_ OS!"<<std::endl;
	// CAVEAT EMPTOR: otl_long_string does NOT check whether you are modifying external CONST storage
	// which CAN (on proper OS's anyway) lead to a hardware exception being thrown!
	//
	// That's a type of failure you won't catch with C++ exceptions. 
	// Don't pass START, don't receive $20K, go straight to jail.
	s3[5] = '!'; // Ayee!
	std::cerr<<s3.c_str()<<std::endl;
	std::cerr<<"length: "<<s3.len()<<std::endl;
 }
 catch(otl_out_of_range_exception& p)
 { 
	 std::cerr<<std::endl<< "Point XXX: UNEXPECTED error"<<std::endl; // you won't get an error as we don't check; only the OS might
	// intercept OTL 'ex' exceptions
	 std::cerr<<"("<<p.failure_code()<<") "<<p.what()<<std::endl; // print out error message
 }

 return 0;
}

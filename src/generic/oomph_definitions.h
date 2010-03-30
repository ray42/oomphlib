//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
//A header containing the definition of the Oomph run-time
//exception classes and our standard (info) output stream 
//(essentially a wrapper to cout but it can be customised,
// e.g. for mpi runs.

//Include guard to prevent multiple inclusions of the header
#ifndef OOMPH_DEFINITIONS_HEADER
#define OOMPH_DEFINITIONS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//Standard libray headers
#include<stdexcept>
#include<iostream>
#include<string>


namespace oomph
{

//Pre-processor magic for error reporting
//Macro that converts argument to string 
#define OOMPH_MAKE_STRING(x) #x
//Macro wrapper to MAKE_STRING, required because calling
//OOMPH_MAKE_STRING(__LINE__) directly returns __LINE__
//i.e. the conversion of __LINE__ into a number must be performed before
//its conversion into a string
#define OOMPH_TO_STRING(x) OOMPH_MAKE_STRING(x)
//Combine the FILE and LINE built-in macros into a string that can
//be used in erorr messages.
#define OOMPH_EXCEPTION_LOCATION __FILE__ ":" OOMPH_TO_STRING(__LINE__)


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


///=====================================================================
/// A class for handling oomph-lib run-time exceptions quietly.
//======================================================================
class OomphLibQuietException : public std::runtime_error
{ 
 public:
 
 ///\short Constructor 
 OomphLibQuietException();
 
 ///The destructor cannot throw an exception (C++ STL standard) 
 ~OomphLibQuietException() throw() {}
};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


///=====================================================================
/// A Base class for oomph-lib run-time exception (error and warning) 
/// handling.
///
/// The class can only be instantiated by the derived classes 
/// OomphLibError and OomphLibWarning. The (protected) constructor 
/// combines its string arguments into a standard format 
/// for uniform exception reports which are written to the specified 
/// output stream.
//======================================================================
class OomphLibException : public std::runtime_error
{ 
 protected:
 
 ///\short Constructor takes the error description, function name
 ///and a location string provided by the OOMPH_EXCEPTION_LOCATION
 ///macro and combines them into a standard header. The exception type
 ///will be the string "WARNING" or "ERROR" and the message is written to
 ///the exception_stream, with a specified output_width
 OomphLibException(const std::string &error_description,
                   const std::string &function_name,
                   const char *location,
                   const std::string &exception_type,
                   std::ostream &exception_stream,
                   const unsigned &output_width);
 
  ///The destructor cannot throw an exception (C++ STL standard) 
  ~OomphLibException() throw() {}
};

//====================================================================
/// An OomphLibError object which should be thrown when an run-time
/// error is encountered. The error stream and stream width can be 
/// specified. The default is cout with a width of 70 characters.
//====================================================================
class OomphLibError : public OomphLibException
{
 ///Output stream that is used to write the errors
 static std::ostream *Stream_pt;

 ///Width in characters of the output report
 static unsigned Output_width;

  public:

 ///\short Constructor requires the error description and the function
 ///in which the error occured and the location provided by the 
 ///OOMPH_EXCEPTION_LOCATION macro
 OomphLibError(const std::string &error_description,
               const std::string &function_name,
               const char *location) :
  OomphLibException(error_description,function_name,location,"ERROR",
                    *Stream_pt,Output_width) { }

 /// \short Static member function used to specify the error stream,
 /// which must be passed as a pointer because streams cannot be copied.
 static inline void set_stream_pt(std::ostream* const &stream_pt)
  {Stream_pt = stream_pt;}

 /// \short Static member function used to specify the width (in characters)
 /// of the error stream
 static inline void set_output_width(const unsigned &output_width)
  {Output_width = output_width;}

};

//====================================================================
/// An OomphLibWarning object which should be created as a temporary
/// object to issue a warning. The warning stream and stream width can be 
/// specified. The default is cout with a width of 70 characters.
//====================================================================
class OomphLibWarning : public OomphLibException
{
 ///Output stream that is used to write the errors
 static std::ostream *Stream_pt;

 ///Width of output
 static unsigned Output_width;

  public:
 
 ///\short Constructor requires the warning description and the function
 ///in which the warning occurred. 
 OomphLibWarning(const std::string &warning_description,
                 const std::string &function_name,
                 const char* location) :
                 OomphLibException(warning_description,function_name,
                                   location,
                                   "WARNING",*Stream_pt,Output_width) { }
                 
 /// \short Static member function used to specify the error stream,
 /// which must be passed as a pointer because streams cannot be copied.
 static inline void set_stream_pt(std::ostream* const &stream_pt)
  {Stream_pt = stream_pt;}

 /// \short Static member function used to specify the width (in characters)
 /// of the error stream
 static inline void set_output_width(const unsigned &output_width)
  {Output_width = output_width;}
};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//=====================================================================
/// A small nullstream class that throws away everything sent to it.
//=====================================================================
class Nullstream : public std::ostream 
{
public:
 ///Constructor sets the buffer sizes to zero, suppressing all output
 Nullstream(): std::ios(0), std::ostream(0) {} 
};




//========================================================================
/// Single (global) instantiation of the Nullstream
//========================================================================
extern Nullstream oomph_nullstream;




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//========================================================================
/// A base class that contains a single virtual member function: 
/// The () operator that may be used to modify the output in 
/// OomphOutput objects. The default implementation 
///=======================================================================
class OutputModifier
{

public:

 ///Empty constructor
 OutputModifier() {}

 ///Empty virtual destructor
 virtual ~OutputModifier() {}

 /// \short Function that will be called before output from an 
 /// OomphOutput object. It returns a bool (true in this default
 /// implementation) to indicate that output should be continued.
 virtual bool operator()(std::ostream &stream) 
  {return true;}

};




//========================================================================
/// Single global instatiation of the default output modifier.
//========================================================================
 extern OutputModifier default_output_modifier;




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////




//=======================================================================
/// This class is a wrapper to a stream and an output modifier that is
/// used to control the "info" output from OomphLib. Its instationiation
/// can be used like std::cout.
//=======================================================================
class OomphInfo
{

private:

 ///Pointer to the output stream -- defaults to std::cout
 std::ostream *Stream_pt;

 ///Pointer to the output modifier object -- defaults to no modification 
 OutputModifier* Output_modifier_pt;

public:

 ///\short Set default values for the output stream (cout) 
 ///and modifier (no modification)
 OomphInfo() : Stream_pt(&std::cout), 
               Output_modifier_pt(&default_output_modifier) {}

 ///\short Overload the << operator, writing output to the stream addressed by 
 ///Stream_pt and calling the function defined by the object addressed by
 ///Output_modifier_pt
 template<class _Tp>
 std::ostream &operator<<(_Tp argument)
  {
   //If the Output_modifer function returns true
   //echo the argument to the stream and return the (vanilla) stream
   if((*Output_modifier_pt)(*Stream_pt))
    {
     *Stream_pt << argument;
     return (*Stream_pt);
    }
   //Otherwise return the null stream (suppress all future output)
   return oomph_nullstream;
  }

 ///Access function for the stream pointer
 std::ostream* &stream_pt() {return Stream_pt;}

 ///Overload insertor to handle stream modifiers
 std::ostream &operator<<(std::ostream& (*f)(std::ostream &))
  {
   return f(*Stream_pt);
  } 
 
 ///Access function for the output modifier pointer
 OutputModifier* &output_modifier_pt() {return Output_modifier_pt;}

};


//========================================================================
/// Single (global) instantiation of the OomphInfo object -- this
/// is used throughout the library as a "replacement" for std::cout
//========================================================================
extern OomphInfo oomph_info;




}

#endif

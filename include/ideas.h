// At namespace level

  // The following definitions of bitmask types are enums, not ints,
  // as permitted (but not required) in the standard, in order to provide
  // better type safety in iostream calls.  A side effect is that
  // expressions involving them are no longer compile-time constants.
  enum _Ios_Fmtflags 
    { 
      _S_boolalpha 	= 1L << 0,
      _S_dec 		= 1L << 1,
      _S_fixed 		= 1L << 2,
      _S_hex 		= 1L << 3,
      _S_internal 	= 1L << 4,
      _S_left 		= 1L << 5,
      _S_oct 		= 1L << 6,
      _S_right 		= 1L << 7,
      _S_scientific 	= 1L << 8,
      _S_showbase 	= 1L << 9,
      _S_showpoint 	= 1L << 10,
      _S_showpos 	= 1L << 11,
      _S_skipws 	= 1L << 12,
      _S_unitbuf 	= 1L << 13,
      _S_uppercase 	= 1L << 14,
      _S_adjustfield 	= _S_left | _S_right | _S_internal,
      _S_basefield 	= _S_dec | _S_oct | _S_hex,
      _S_floatfield 	= _S_scientific | _S_fixed,
      _S_ios_fmtflags_end = 1L << 16,
      _S_ios_fmtflags_max = __INT_MAX__,
      _S_ios_fmtflags_min = ~__INT_MAX__
    };


  inline _GLIBCXX_CONSTEXPR _Ios_Fmtflags
  operator&(_Ios_Fmtflags __a, _Ios_Fmtflags __b)
  { return _Ios_Fmtflags(static_cast<int>(__a) & static_cast<int>(__b)); }

  inline _GLIBCXX_CONSTEXPR _Ios_Fmtflags
  operator|(_Ios_Fmtflags __a, _Ios_Fmtflags __b)
  { return _Ios_Fmtflags(static_cast<int>(__a) | static_cast<int>(__b)); }

  inline _GLIBCXX_CONSTEXPR _Ios_Fmtflags
  operator^(_Ios_Fmtflags __a, _Ios_Fmtflags __b)
  { return _Ios_Fmtflags(static_cast<int>(__a) ^ static_cast<int>(__b)); }

  inline _GLIBCXX_CONSTEXPR _Ios_Fmtflags
  operator~(_Ios_Fmtflags __a)
  { return _Ios_Fmtflags(~static_cast<int>(__a)); }

  inline const _Ios_Fmtflags&
  operator|=(_Ios_Fmtflags& __a, _Ios_Fmtflags __b)
  { return __a = __a | __b; }

  inline const _Ios_Fmtflags&
  operator&=(_Ios_Fmtflags& __a, _Ios_Fmtflags __b)
  { return __a = __a & __b; }

  inline const _Ios_Fmtflags&
  operator^=(_Ios_Fmtflags& __a, _Ios_Fmtflags __b)
  { return __a = __a ^ __b; }



  enum _Ios_Seekdir 
    { 
      _S_beg = 0,
      _S_cur = _GLIBCXX_STDIO_SEEK_CUR,
      _S_end = _GLIBCXX_STDIO_SEEK_END,
      _S_ios_seekdir_end = 1L << 16 
    };


// 27.4.2  Class ios_base
  /**
   *  @brief  The base of the I/O class hierarchy.
   *  @ingroup io
   *
   *  This class defines everything that can be defined about I/O that does
   *  not depend on the type of characters being input or output.  Most
   *  people will only see @c ios_base when they need to specify the full
   *  name of the various I/O flags (e.g., the openmodes).
  */
  class ios_base
  {

 //...

 // 27.4.2.1.2  Type ios_base::fmtflags
    /**
     *  @brief This is a bitmask type.
     *
     *  @c @a _Ios_Fmtflags is implementation-defined, but it is valid to
     *  perform bitwise operations on these values and expect the Right
     *  Thing to happen.  Defined objects of type fmtflags are:
     *  - boolalpha
     *  - dec
     *  - fixed
     *  - hex
     *  - internal
     *  - left
     *  - oct
     *  - right
     *  - scientific
     *  - showbase
     *  - showpoint
     *  - showpos
     *  - skipws
     *  - unitbuf
     *  - uppercase
     *  - adjustfield
     *  - basefield
     *  - floatfield
    */
    typedef _Ios_Fmtflags fmtflags; // fmtflags is synonym for _Ios_Fmtflags.

    /// Insert/extract @c bool in alphabetic rather than numeric format.
    static const fmtflags boolalpha =   _S_boolalpha;

    /// Converts integer input or generates integer output in decimal base.
    static const fmtflags dec =         _S_dec;

    /// Generate floating-point output in fixed-point notation.
    static const fmtflags fixed =       _S_fixed;

    /// Converts integer input or generates integer output in hexadecimal base.
    static const fmtflags hex =         _S_hex;

    /// Adds fill characters at a designated internal point in certain
    /// generated output, or identical to @c right if no such point is
    /// designated.
    static const fmtflags internal =    _S_internal;

    /// Adds fill characters on the right (final positions) of certain
    /// generated output.  (I.e., the thing you print is flush left.)
    static const fmtflags left =        _S_left;

    /// Converts integer input or generates integer output in octal base.
    static const fmtflags oct =         _S_oct;

    /// Adds fill characters on the left (initial positions) of certain
    /// generated output.  (I.e., the thing you print is flush right.)
    static const fmtflags right =       _S_right;

    /// Generates floating-point output in scientific notation.
    static const fmtflags scientific =  _S_scientific;

    /// Generates a prefix indicating the numeric base of generated integer
    /// output.
    static const fmtflags showbase =    _S_showbase;

    /// Generates a decimal-point character unconditionally in generated
    /// floating-point output.
    static const fmtflags showpoint =   _S_showpoint;

    /// Generates a + sign in non-negative generated numeric output.
    static const fmtflags showpos =     _S_showpos;

    /// Skips leading white space before certain input operations.
    static const fmtflags skipws =      _S_skipws;

    /// Flushes output after each output operation.
    static const fmtflags unitbuf =     _S_unitbuf;

    /// Replaces certain lowercase letters with their uppercase equivalents
    /// in generated output.
    static const fmtflags uppercase =   _S_uppercase;

    /// A mask of left|right|internal.  Useful for the 2-arg form of @c setf.
    static const fmtflags adjustfield = _S_adjustfield;

    /// A mask of dec|oct|hex.  Useful for the 2-arg form of @c setf.
    static const fmtflags basefield =   _S_basefield;

    /// A mask of scientific|fixed.  Useful for the 2-arg form of @c setf.
    static const fmtflags floatfield =  _S_floatfield;

    // 27.4.2.1.3  Type ios_base::iostate
    /**
     *  @brief This is a bitmask type.
     *
     *  @c @a _Ios_Iostate is implementation-defined, but it is valid to
     *  perform bitwise operations on these values and expect the Right
     *  Thing to happen.  Defined objects of type iostate are:
     *  - badbit
     *  - eofbit
     *  - failbit
     *  - goodbit
    */
    typedef _Ios_Iostate iostate;

    /// Indicates a loss of integrity in an input or output sequence (such
    /// as an irrecoverable read error from a file).
    static const iostate badbit =	_S_badbit;

    /// Indicates that an input operation reached the end of an input sequence.
    static const iostate eofbit =	_S_eofbit;

    /// Indicates that an input operation failed to read the expected
    /// characters, or that an output operation failed to generate the
    /// desired characters.
    static const iostate failbit =	_S_failbit;

    /// Indicates all is well.
    static const iostate goodbit =	_S_goodbit;

 //...

  protected:
    streamsize		_M_precision;
    streamsize		_M_width;
    fmtflags		_M_flags;
    iostate		_M_exception;
    iostate		_M_streambuf_state;


 public:
  //....


// [27.4.2.2] fmtflags state functions
    /**
     *  @brief  Access to format flags.
     *  @return  The format control flags for both input and output.
    */
    fmtflags
    flags() const
    { return _M_flags; }

    /**
     *  @brief  Setting new format flags all at once.
     *  @param  __fmtfl  The new flags to set.
     *  @return  The previous format control flags.
     *
     *  This function overwrites all the format flags with @a __fmtfl.
    */
    fmtflags
    flags(fmtflags __fmtfl)
    {
      fmtflags __old = _M_flags;
      _M_flags = __fmtfl;
      return __old;
    }

    /**
     *  @brief  Setting new format flags.
     *  @param  __fmtfl  Additional flags to set.
     *  @return  The previous format control flags.
     *
     *  This function sets additional flags in format control.  Flags that
     *  were previously set remain set.
    */
    fmtflags
    setf(fmtflags __fmtfl)
    {
      fmtflags __old = _M_flags;
      _M_flags |= __fmtfl;
      return __old;
    }

    /**
     *  @brief  Setting new format flags.
     *  @param  __fmtfl  Additional flags to set.
     *  @param  __mask  The flags mask for @a fmtfl.
     *  @return  The previous format control flags.
     *
     *  This function clears @a mask in the format flags, then sets
     *  @a fmtfl @c & @a mask.  An example mask is @c ios_base::adjustfield.
    */
    fmtflags
    setf(fmtflags __fmtfl, fmtflags __mask)
    {
      fmtflags __old = _M_flags;
      _M_flags &= ~__mask;
      _M_flags |= (__fmtfl & __mask);
      return __old;
    }

    /**
     *  @brief  Clearing format flags.
     *  @param  __mask  The flags to unset.
     *
     *  This function clears @a __mask in the format flags.
    */
    void
    unsetf(fmtflags __mask)
    { _M_flags &= ~__mask; }

    //.... more of ios_base class

}; // end ios_base class

// Examples of usage from grepping source

./ostream.tcc:96:      const ios_base::fmtflags __fmt = this->flags() & ios_base::basefield;
./ostream.tcc:110:      const ios_base::fmtflags __fmt = this->flags() & ios_base::basefield;
./istream.tcc:54:	  if (!__noskip && bool(__in.flags() & ios_base::skipws))
./ostream_insert.h:90-99 

 const streamsize __w = __out.width();
	      if (__w > __n)
		{
		  const bool __left = ((__out.flags()
					& __ios_base::adjustfield)
				       == __ios_base::left);
		  if (!__left)
		    __ostream_fill(__out, __w - __n);
		  if (__out.good())
		    __ostream_write(__out, __s, __n);
		  if (__left && __out.good())
		    __ostream_fill(__out, __w - __n);
		}


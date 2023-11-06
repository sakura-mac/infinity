// Copyright (C) 2020 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Warning! This file is autogenerated.
#ifndef BOOST_TEXT_DATA_TH_HPP
#define BOOST_TEXT_DATA_TH_HPP

#include <boost/text/string_view.hpp>


namespace boost { namespace text { namespace data { namespace th {

inline string_view standard_collation_tailoring()
{
    return string_view((char const *)
u8R"(  
[normalization on]
[alternate shifted]
[reorder Thai]
&[before 1]๚<ฯ
&๛<ๆ
&๎<<์
&[before 1]ะ<ํ
&า<<<ๅ
&าํ<<<ํา<<<ำ
&ๅํ<<<ํๅ
&ไ<ฺ
  )");
}


}}}}

#endif
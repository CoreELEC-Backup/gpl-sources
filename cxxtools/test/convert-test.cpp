/*
 * Copyright (C) 2010 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cxxtools/convert.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"
#include "cxxtools/log.h"
#include <limits>
#include <string.h>

log_define("cxxtools.test.convert")

namespace
{
    bool nearBy(double v1, double v2, double e = 1e-6)
    {
        double q = v1 / v2;
        return q > 1 - e && q < 1 + e;
    }
}

class ConvertTest : public cxxtools::unit::TestSuite
{
    public:
        ConvertTest()
        : cxxtools::unit::TestSuite("convert")
        {
            registerMethod("successTest", *this, &ConvertTest::successTest);
            registerMethod("failTest", *this, &ConvertTest::failTest);
            registerMethod("nanTest", *this, &ConvertTest::nanTest);
            registerMethod("infTest", *this, &ConvertTest::infTest);
            registerMethod("emptyTest", *this, &ConvertTest::emptyTest);
            registerMethod("floatTest", *this, &ConvertTest::floatTest);
        }

        void successTest()
        {
          std::string s(" -15 ");
          int n = 0;
          CXXTOOLS_UNIT_ASSERT_NOTHROW(n = cxxtools::convert<int>(s));
          CXXTOOLS_UNIT_ASSERT_EQUALS(n, -15);

          cxxtools::String S(L" -42 ");
          CXXTOOLS_UNIT_ASSERT_NOTHROW(n = cxxtools::convert<int>(S));
          CXXTOOLS_UNIT_ASSERT_EQUALS(n, -42);

        }

        void failTest()
        {
          std::string s(" -15 a");
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<int>(s), cxxtools::ConversionError);

          cxxtools::String S(L" -42 a");
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<int>(S), cxxtools::ConversionError);

        }

        void nanTest()
        {
          // test string to number

          // test nan

          double d = cxxtools::convert<double>(std::string("NaN"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          float f = cxxtools::convert<float>(std::string("NaN"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          d = cxxtools::convert<double>(cxxtools::String(L"NaN"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          f = cxxtools::convert<float>(cxxtools::String(L"NaN"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          // test quiet nan

          d = cxxtools::convert<double>(std::string("NaNQ"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          f = cxxtools::convert<float>(std::string("NaNQ"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          d = cxxtools::convert<double>(cxxtools::String(L"NaNQ"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          f = cxxtools::convert<float>(cxxtools::String(L"NaNQ"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          // test signaling nan

          d = cxxtools::convert<double>(std::string("NaNS"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          f = cxxtools::convert<float>(std::string("NaNS"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          d = cxxtools::convert<double>(cxxtools::String(L"NaNS"));
          CXXTOOLS_UNIT_ASSERT(d != d);

          f = cxxtools::convert<float>(cxxtools::String(L"NaNS"));
          CXXTOOLS_UNIT_ASSERT(f != f);

          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<float>(cxxtools::String(L"NaNF")), cxxtools::ConversionError);

          // test number to string

          std::string s = cxxtools::convert<std::string>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "nan");

          s = cxxtools::convert<std::string>(f);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "nan");

          cxxtools::String ss = cxxtools::convert<cxxtools::String>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(ss.narrow(), "nan");

          ss = cxxtools::convert<cxxtools::String>(f);
          CXXTOOLS_UNIT_ASSERT_EQUALS(ss.narrow(), "nan");

        }

        void infTest()
        {
          // test string to number

          double d = cxxtools::convert<double>(std::string("inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<double>::infinity());

          d = cxxtools::convert<double>(std::string("infinity"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<double>::infinity());

          float f = cxxtools::convert<float>(std::string("inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<float>::infinity());

          d = cxxtools::convert<double>(cxxtools::String(L"iNf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<double>::infinity());

          f = cxxtools::convert<float>(cxxtools::String(L"inF"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, std::numeric_limits<float>::infinity());

          // test number to string

          std::string s = cxxtools::convert<std::string>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "inf");

          s = cxxtools::convert<std::string>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(s.c_str(), "inf") == 0);

          cxxtools::String ss = cxxtools::convert<cxxtools::String>(d);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "inf") == 0);

          ss = cxxtools::convert<cxxtools::String>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "inf") == 0);

          // negative inf

          // string to number
          d = cxxtools::convert<double>(std::string("-inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<double>::infinity());

          f = cxxtools::convert<float>(std::string("-inf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<float>::infinity());

          d = cxxtools::convert<double>(cxxtools::String(L"-iNf"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<double>::infinity());

          f = cxxtools::convert<float>(cxxtools::String(L"-INF"));
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -std::numeric_limits<float>::infinity());

          // test number to string
          s = cxxtools::convert<std::string>(d);
          CXXTOOLS_UNIT_ASSERT_EQUALS(s, "-inf");

          s = cxxtools::convert<std::string>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(s.c_str(), "-inf") == 0);

          ss = cxxtools::convert<cxxtools::String>(d);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "-inf") == 0);

          ss = cxxtools::convert<cxxtools::String>(f);
          CXXTOOLS_UNIT_ASSERT(strcasecmp(ss.narrow().c_str(), "-inf") == 0);

        }

        void emptyTest()
        {
          std::string emptyString;
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<int>(std::string()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<int>(cxxtools::String()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<unsigned>(std::string()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<unsigned>(cxxtools::String()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<double>(std::string()), cxxtools::ConversionError);
          CXXTOOLS_UNIT_ASSERT_THROW(cxxtools::convert<double>(cxxtools::String()), cxxtools::ConversionError);
        }

        void t(double d)
        {
          std::string s = cxxtools::convert<std::string>(d);
          double r = cxxtools::convert<double>(s);
          if (r == 0)
          {
            CXXTOOLS_UNIT_ASSERT_EQUALS(d, 0);
          }
          else
          {
            double q = d / r;
            log_debug("d=" << d << " s=\"" << s << "\" r=" << r << " q=" << q);
            if (q < 0.999999999999 || q > 1.0000000000001)
            {
              CXXTOOLS_UNIT_ASSERT_EQUALS(d, r);
            }
          }
        }

        void floatTest()
        {
          double d;

          d = cxxtools::convert<double>("1.5");
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, 1.5);

          d = cxxtools::convert<double>(" -345.75 ");
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, -345.75);

          d = cxxtools::convert<double>("\n1e6\r");
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, 1e6);

          d = cxxtools::convert<double>("7.0e4");
          CXXTOOLS_UNIT_ASSERT_EQUALS(d, 7e4);

          d = cxxtools::convert<double>("-2e-3");
          CXXTOOLS_UNIT_ASSERT(nearBy(d, -2e-3));

          d = cxxtools::convert<double>("-8E-5");
          CXXTOOLS_UNIT_ASSERT(nearBy(d, -8e-5));

          d = cxxtools::convert<double>("-3.0e-12");
          CXXTOOLS_UNIT_ASSERT(nearBy(d, -3e-12));

          d = cxxtools::convert<double>("-8.5E-23");
          CXXTOOLS_UNIT_ASSERT(nearBy(d, -8.5e-23));

          t(3.141592653579893);
          t(0.314);
          t(0.0314);
          t(0.00123);
          t(123456789.55555555);
          t(0);
          t(1);
          t(1.4567e17);
          t(12345);
          t(1.4567e-17);
          t(0.2);
          t(12);
        }

};

cxxtools::unit::RegisterTest<ConvertTest> register_ConvertTest;

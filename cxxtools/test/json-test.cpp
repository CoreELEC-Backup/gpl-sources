/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/serializationinfo.h"
#include "cxxtools/jsonserializer.h"
#include "cxxtools/jsondeserializer.h"
#include "cxxtools/log.h"
#include "cxxtools/hdstream.h"
#include <limits>
#include <stdint.h>
#include <config.h>

log_define("cxxtools.test.json")

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
        bool boolValue;
        bool nullValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
        si.getMember("boolValue") >>= obj.boolValue;
        const cxxtools::SerializationInfo* p = si.findMember("nullValue");
        obj.nullValue = p != 0 && p->isNull();
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.addMember("boolValue") <<= obj.boolValue;
        si.addMember("nullValue");
        si.setTypeName("TestObject");
    }

    bool operator== (const TestObject& obj1, const TestObject& obj2)
    {
        return obj1.intValue == obj2.intValue
            && obj1.stringValue == obj2.stringValue
            && obj1.doubleValue == obj2.doubleValue
            && obj1.boolValue == obj2.boolValue
            && obj1.nullValue == obj2.nullValue;
    }

    struct TestObject2 : public TestObject
    {
        typedef std::set<unsigned> SetType;
        typedef std::map<unsigned, std::string> MapType;
        SetType setValue;
        MapType mapValue;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, TestObject2& obj)
    {
        si >>= static_cast<TestObject&>(obj);
        si.getMember("setValue") >>= obj.setValue;
        si.getMember("mapValue") >>= obj.mapValue;
    }

    void operator<<= (cxxtools::SerializationInfo& si, const TestObject2& obj)
    {
        si <<= static_cast<const TestObject&>(obj);
        si.addMember("setValue") <<= obj.setValue;
        si.addMember("mapValue") <<= obj.mapValue;
        si.setTypeName("TestObject2");
    }

    bool operator== (const TestObject2& obj1, const TestObject2& obj2)
    {
        return static_cast<const TestObject&>(obj1) == static_cast<const TestObject&>(obj2)
            && obj1.setValue == obj2.setValue
            && obj1.mapValue == obj2.mapValue;
    }
}

class JsonTest : public cxxtools::unit::TestSuite
{
    public:
        JsonTest()
            : cxxtools::unit::TestSuite("json")
        {
            registerMethod("testScalar", *this, &JsonTest::testScalar);
            registerMethod("testInt", *this, &JsonTest::testInt);
            registerMethod("testDouble", *this, &JsonTest::testDouble);
            registerMethod("testArray", *this, &JsonTest::testArray);
            registerMethod("testObject", *this, &JsonTest::testObject);
            registerMethod("testComplexObject", *this, &JsonTest::testComplexObject);
            registerMethod("testObjectVector", *this, &JsonTest::testObjectVector);
            registerMethod("testBinaryData", *this, &JsonTest::testBinaryData);
        }

        void testScalar()
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            int value = 5;
            serializer.serialize(value);
            serializer.finish();

            log_debug("scalar: " << data.str());

            int value2 = 0;
            deserializer.deserialize(value2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, value2);
        }

        template <typename IntT>
        void testIntValue(IntT value)
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            serializer.serialize(value);
            serializer.finish();

            log_debug("int: " << data.str());

            IntT result = 0;
            deserializer.deserialize(result);

            CXXTOOLS_UNIT_ASSERT_EQUALS(value, result);
        }

        void testInt()
        {
            testIntValue(30);
            testIntValue(300);
            testIntValue(100000);

            testIntValue(-30);
            testIntValue(-300);
            testIntValue(-100000);

            testIntValue(static_cast<int16_t>(std::numeric_limits<int8_t>::max()) + 1);
            testIntValue(static_cast<int32_t>(std::numeric_limits<int16_t>::max()) + 1);
            testIntValue(std::numeric_limits<int32_t>::max());
#ifdef INT64_IS_BASETYPE
            testIntValue(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1);
            testIntValue(std::numeric_limits<int64_t>::max());
#endif

            testIntValue(static_cast<int16_t>(std::numeric_limits<int8_t>::min()) - 1);
            testIntValue(static_cast<int32_t>(std::numeric_limits<int16_t>::min()) - 1);
            testIntValue(std::numeric_limits<int32_t>::min());
#ifdef INT64_IS_BASETYPE
            testIntValue(static_cast<int64_t>(std::numeric_limits<int32_t>::min()) - 1);
            testIntValue(std::numeric_limits<int64_t>::min());
#endif

            testIntValue(static_cast<uint16_t>(std::numeric_limits<uint8_t>::max()) + 1);
            testIntValue(static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()) + 1);
            testIntValue(std::numeric_limits<uint32_t>::max());
#ifdef INT64_IS_BASETYPE
            testIntValue(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) + 1);
            testIntValue(std::numeric_limits<uint64_t>::max());
#endif
        }

        void testDoubleValue(double value)
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            serializer.serialize(value);
            serializer.finish();

            double result = 0.0;
            deserializer.deserialize(result);

            log_debug("test double value " << value << " => " << result);

            if (value != value) // check for nan
                CXXTOOLS_UNIT_ASSERT(result != result);
            else if (value == std::numeric_limits<double>::infinity())
                CXXTOOLS_UNIT_ASSERT_EQUALS(result, std::numeric_limits<double>::infinity());
            else if (value == -std::numeric_limits<double>::infinity())
                CXXTOOLS_UNIT_ASSERT_EQUALS(result, -std::numeric_limits<double>::infinity());
            else
                CXXTOOLS_UNIT_ASSERT(value / result < 1.00001 && value / result > 0.99999);
        }

        void testDouble()
        {
            testDoubleValue(3.14159);
            testDoubleValue(-3.877e-12);
        }

        void testArray()
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            std::vector<int> intvector;
            intvector.push_back(4711);
            intvector.push_back(4712);
            intvector.push_back(-3);
            intvector.push_back(-257);

            serializer.serialize(intvector);
            serializer.finish();

            log_debug("intvector: " << data.str());

            std::vector<int> intvector2;
            deserializer.deserialize(intvector2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector.size(), intvector2.size());
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[0], intvector2[0]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[1], intvector2[1]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[2], intvector2[2]);
            CXXTOOLS_UNIT_ASSERT_EQUALS(intvector[3], intvector2[3]);
        }

        void testObject()
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            TestObject obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            obj.boolValue = true;
            obj.nullValue = true;
            serializer.serialize(obj);
            serializer.finish();

            TestObject obj2;
            deserializer.deserialize(obj2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.intValue, obj2.intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.stringValue, obj2.stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.doubleValue, obj2.doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.boolValue, obj2.boolValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj.nullValue, obj2.nullValue);
            CXXTOOLS_UNIT_ASSERT(obj == obj2);
        }

        void testComplexObject()
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            std::vector<TestObject2> v;
            TestObject2 obj;
            obj.intValue = 17;
            obj.stringValue = "foobar";
            obj.doubleValue = 3.125;
            obj.boolValue = false;
            obj.nullValue = true;
            obj.setValue.insert(17);
            obj.setValue.insert(23);
            obj.mapValue[45] = "fourtyfive";
            obj.mapValue[88] = "eightyeight";
            obj.mapValue[100] = "onehundred";
            v.push_back(obj);

            obj.setValue.insert(88);
            v.push_back(obj);

            serializer.serialize(v);
            serializer.finish();

            std::vector<TestObject2> v2;
            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT(v == v2);
        }

        void testObjectVector()
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            std::vector<TestObject> obj;
            obj.resize(2);
            obj[0].intValue = 17;
            obj[0].stringValue = "foobar";
            obj[0].doubleValue = 3.125;
            obj[0].boolValue = true;
            obj[0].nullValue = true;
            obj[1].intValue = 18;
            obj[1].stringValue = "hi there";
            obj[1].doubleValue = -17.25;
            obj[1].boolValue = false;
            obj[1].nullValue = true;

            serializer.serialize(obj);
            serializer.finish();

            std::vector<TestObject> obj2;
            deserializer.deserialize(obj2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(obj2.size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].intValue, obj2[0].intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].stringValue, obj2[0].stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].doubleValue, obj2[0].doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].boolValue, obj2[0].boolValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[0].nullValue, obj2[0].nullValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].intValue, obj2[1].intValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].stringValue, obj2[1].stringValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].doubleValue, obj2[1].doubleValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].boolValue, obj2[1].boolValue);
            CXXTOOLS_UNIT_ASSERT_EQUALS(obj[1].nullValue, obj2[1].nullValue);
            CXXTOOLS_UNIT_ASSERT(obj == obj2);
        }

        void testBinaryData()
        {
            std::stringstream data;
            cxxtools::JsonSerializer serializer(data);
            cxxtools::JsonDeserializer deserializer(data);

            std::string v;
            for (unsigned n = 0; n < 1024; ++n)
                v.push_back(static_cast<char>(n));

            serializer.serialize(v);
            serializer.finish();

            log_debug("v.data=" << cxxtools::hexDump(data.str()));

            std::string v2;
            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v2.size(), 1024);
            CXXTOOLS_UNIT_ASSERT(v == v2);

            data.str(std::string());
            deserializer.clear();

            for (unsigned n = 0; n < 0xffff; ++n)
                v.push_back(static_cast<char>(n));

            serializer.serialize(v);
            serializer.finish();

            log_debug("v.data=" << cxxtools::hexDump(data.str()));

            deserializer.deserialize(v2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(v2.size(), 0xffff + 1024);
            CXXTOOLS_UNIT_ASSERT(v == v2);

        }
};

cxxtools::unit::RegisterTest<JsonTest> register_JsonTest;

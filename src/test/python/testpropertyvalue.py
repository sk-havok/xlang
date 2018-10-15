import find_projection

import _pyrt
import unittest

class TestPropertyValue(unittest.TestCase):

    # TODO: CreateEmpty seems to be failing @ the C++/WinRT layer. 

    def test_create_uint8(self):
       o = _pyrt.PropertyValue.CreateUInt8(250)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 1)
       self.assertTrue(ipv.GetUInt8(), 250)

    def test_create_int16(self):
       o = _pyrt.PropertyValue.CreateInt16(-32000)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 2)
       self.assertTrue(ipv.GetInt16(), -32000)

    def test_create_uint16(self):
       o = _pyrt.PropertyValue.CreateUInt16(65000)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 3)
       self.assertTrue(ipv.GetUInt16(), 65000)

    def test_create_int32(self):
       o = _pyrt.PropertyValue.CreateInt32(-2147483640)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 4)
       self.assertTrue(ipv.GetInt32(), -2147483640)

    def test_create_uint32(self):
       o = _pyrt.PropertyValue.CreateUInt32(4294967290)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 5)
       self.assertTrue(ipv.GetUInt32(), 4294967290)

    def test_create_int64(self):
       o = _pyrt.PropertyValue.CreateInt64(-9223372036854775800)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 6)
       self.assertTrue(ipv.GetInt64(), -9223372036854775800)

    def test_create_uint64(self):
       o = _pyrt.PropertyValue.CreateUInt64(18446744073709551610)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 7)
       self.assertTrue(ipv.GetUInt64(), 18446744073709551610)

    def test_create_single(self):
        o = _pyrt.PropertyValue.CreateSingle(3.14)
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 8)
        self.assertAlmostEqual(ipv.GetSingle(), 3.14, 5)

    def test_create_double(self):
        o = _pyrt.PropertyValue.CreateDouble(3.14)
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 9)
        self.assertEqual(ipv.GetDouble(), 3.14)

    # TODO: CreateChar16

    def test_create_boolean(self):
       o = _pyrt.PropertyValue.CreateBoolean(True)
       ipv = _pyrt.IPropertyValue(o)
       self.assertEqual(ipv.get_Type(), 11)
       self.assertTrue(ipv.GetBoolean())

    def test_create_string(self):
        o = _pyrt.PropertyValue.CreateString("Ni!")
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 12)
        self.assertEqual(ipv.GetString(), "Ni!")

    # TODO: CreateInspectable

    def test_create_datetime(self):
        o = _pyrt.PropertyValue.CreateDateTime(_pyrt.DateTime(0))
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 14)
        self.assertEqual(ipv.GetDateTime().UniversalTime, 0)

    def test_create_TimeSpan(self):
        o = _pyrt.PropertyValue.CreateTimeSpan(_pyrt.TimeSpan(0))
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 15)
        self.assertEqual(ipv.GetTimeSpan().Duration, 0)        

    # TODO: CreateGuid

    def test_create_Point(self):
        o = _pyrt.PropertyValue.CreatePoint(_pyrt.Point(2, 4))
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 17)
        s = ipv.GetPoint()
        self.assertEqual(s.X, 2)
        self.assertEqual(s.Y, 4)

    def test_create_Size(self):
        o = _pyrt.PropertyValue.CreateSize(_pyrt.Size(2, 4))
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 18)
        s = ipv.GetSize()
        self.assertEqual(s.Width, 2)
        self.assertEqual(s.Height, 4)

    def test_create_Rect(self):
        o = _pyrt.PropertyValue.CreateRect(_pyrt.Rect(2, 4, 6, 8))
        ipv = _pyrt.IPropertyValue(o)
        self.assertEqual(ipv.get_Type(), 19)
        s = ipv.GetRect()
        self.assertEqual(s.X, 2)
        self.assertEqual(s.Y, 4)
        self.assertEqual(s.Width, 6)
        self.assertEqual(s.Height, 8)

if __name__ == '__main__':
    _pyrt.init_apartment()
    unittest.main()
    _pyrt.uninit_apartment()
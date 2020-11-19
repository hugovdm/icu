def TestProps():
  print _starts
  print _props
  print "1..2: a=1"
  SetProps(1, 2, {"a": 1})
  print _starts
  print _props
  print "5..6: b=2"
  SetProps(5, 6, {"b": 2})
  print _starts
  print _props
  print "2..8: a=3, c=3"
  SetProps(2, 8, {"a": 3, "c": 3})
  print _starts
  print _props
  SetProps(0, 0x10ffff, {"z": 9})
  print _starts
  print _props
  AddBoundary(3)
  AddBoundary(100)
  AddBoundary(7)
  AddBoundary(200)
  print _starts
  print _props

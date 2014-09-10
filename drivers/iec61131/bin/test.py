#!/usr/bin/env python
import getpass
import sys
import time
import telnetlib

rc = 0

tn = telnetlib.Telnet("localhost",2004)

tn.read_until("key:")
tn.write("123\n")
tn.read_until(">>")

# Test case #1
tn.write("inter 0.0.0.0 TH_Temp_Inside:=24.0;\n")
tn.read_until(">>")

time.sleep(1)

tn.write("ls\n")
resp = tn.read_until(">>")
lines = resp.splitlines()
for line in lines:
  if line.find("TH_Output") >= 0 :
    #print line
    if line.find("FALSE") >= 0 :
      rc = 1

# Test case #2
tn.write("inter 0.0.0.0 TH_Temp_Inside:=26.0;\n")
tn.read_until(">>")

time.sleep(1)

tn.write("ls\n")
resp = tn.read_until(">>")
lines = resp.splitlines()
for line in lines:
  if line.find("TH_Output") >= 0 :
    #print line
    if line.find("TRUE") >= 0 :
      rc = 1

tn.write("quit\n")
tn.read_all()

#print rc

sys.exit(rc)

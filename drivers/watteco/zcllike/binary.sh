# demande lecture binary application type 4o
./xxcode.x "11 00 000F 0100"
./xxcode.x "11 01 000F 0100 00 23 01010101"

# demande lecture binary polarity 1o
./xxcode.x "11 00 000F 0054"
./xxcode.x "11 01 000F 0054 00 10 01"

# demande lecture binary edge-select
./xxcode.x "11 00 000F 0400"
./xxcode.x "11 01 000F 0400 00 18 ff"

# demande lecture binary debounce
./xxcode.x "11 00 000F 0401"
./xxcode.x "11 01 000F 0401 00 21 ffff"

# demande lecture binary counter
./xxcode.x "11 00 000F 0402"
./xxcode.x "11 01 000F 0402 00 23 ffffffff"

# demande config binary/polarity 2o mint, 2o maxt, 1o minchg
./xxcode.x "11 06 000F 00 0054 10 0010 0100 01"
./xxcode.x "11 07 000F 00 00 0054"

# demande config binary/value 2o mint, 2o maxt, 1o minchg
./xxcode.x "11 06 000F 00 0055 10 0010 0100 01"
./xxcode.x "11 07 000F 00 00 0055"

# demande config binary/edge 2o mint, 2o maxt, 1o minchg
./xxcode.x "11 06 000F 00 0400 18 0010 0100 FF"
./xxcode.x "11 07 000F 00 00 0400"

# demande config binary/debounce 2o mint, 2o maxt, 2o minchg
./xxcode.x "11 06 000F 00 0401 21 0010 0100 FFFF"
./xxcode.x "11 07 000F 00 00 0401"

# demande config binary/counter 2o mint, 2o maxt, 4o minchg
./xxcode.x "11 06 000F 00 0402 23 0010 0100 01020304"
./xxcode.x "11 07 000F 00 00 0402"

# event report attr 0054
./xxcode.x "11 0A 000F 0054 10 01"

# event report attr 0055
./xxcode.x "11 0A 000F 0055 10 01"

# event report attr 0400
./xxcode.x "11 0A 000F 0400 18 ff"

# event report attr 0401
./xxcode.x "11 0A 000F 0401 21 ffff"

# event report attr 0402 (075BCD15 => 123456789)
./xxcode.x "11 0A 000F 0402 23 075BCD15"


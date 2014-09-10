# all commands on on-off
# demande lecture attr 0000 (current) de on-off
./xxcode.x "11 00 0006 0000"
./xxcode.x "11 01 0006 0000 00 10 00"
./xxcode.x "11 01 0006 0000 00 10 01"

# demande config on-off 2o mint, 2o maxt, 1o minchg
./xxcode.x "11 06 0006 00 0000 10 0010 0100 01"
./xxcode.x "11 07 0006 00 00 0000"

# demande lecture config on-off
./xxcode.x "11 08 0006 00 0000"
./xxcode.x "11 09 0006 00 00 0000 10 0010 0100 01"

# event report attr 0000 on/off
./xxcode.x "11 0A 0006 0000 10 07"

# command toggle sur on-off
./xxcode.x "11 50 0006 00"
./xxcode.x "11 50 0006 01"
./xxcode.x "11 50 0006 02"

# -----------------------------------------------------

# demande lecture attr 0000 (current) de temperature
./xxcode.x "11 00 0402 0000"
./xxcode.x "11 01 0402 0000 00 29 0010"

# demande lecture attr 0001 (min) de temperature
./xxcode.x "11 00 0402 0001"
./xxcode.x "11 01 0402 0001 00 29 0100"

# demande lecture attr 0002 (max) de temperature
./xxcode.x "11 00 0402 0002"
./xxcode.x "11 01 0402 0002 00 29 1000"

# demande config temperature 2o mint, 2o maxt, 2o minchg
./xxcode.x "11 06 0402 00 0000 29 001001000010"
./xxcode.x "11 07 0402 00 00 0000"

# demande lecture config temperature
./xxcode.x "11 08 0402 00 0000"
./xxcode.x "11 09 0402 00 00 0000 29 001001000001"


# -----------------------------------------------------

# demande config occupancy 2o mint, 2o maxt, 1o minchg
./xxcode.x "11 06 0406 00 0000 18 0010 0100 05"
./xxcode.x "11 07 0406 00 00 0000"

# event report attr 0000 occup 1o
./xxcode.x "11 0A 0406 0000 18 01"

# -----------------------------------------------------

# demande config analog-input 2o mint, 2o maxt, 4o minchg
./xxcode.x "11 06 000C 00 0055 39 0010 0100 00100000"
./xxcode.x "11 07 000C 00 00 0055"


# event report attr 0000 ainput 4o
./xxcode.x "11 0A 000C 0055 39 00100000"

# -----------------------------------------------------


# event report attr 0000 binput 1o
./xxcode.x "11 0A 000F 0055 10 FF"

# event report attr 0000 illu
./xxcode.x "11 0A 0400 0000 21 0700"

# -----------------------------------------------------

# demande lecture panid
./xxcode.x "11 00 0050 0001"
./xxcode.x "11 01 0050 0001 00 21 abcd"

# demande ecriture panid, no response
./xxcode.x "11 05 0050 0001 21 abcd"

# demande lecture wip
./xxcode.x "11 00 0050 0000"
./xxcode.x "11 01 0050 0000 00 41 10 abcdef00 abcdef00 abcdef00 abcdef00"

# demande ecriture wip, no response
./xxcode.x "11 05 0050 0000 41 10 abcdef00 abcdef00 abcdef00 abcdef00"

# demande cluster supported
./xxcode.x "11 00 0050 0002"
./xxcode.x "11 01 0050 0002 00 4C 000E 03 0402 0405 0400 03 0402 0405 0400"


# -----------------------------------------------------

# reset command on smart plug
./xxcode.x "11 50 0052 00"

# demande lecture attribut 0000 cluster metering 0052 param complex type
./xxcode.x "11 00 0052 0000"
./xxcode.x "11 01 0052 0000 00 41 0C AABBCC DDEEFF FFFF 1020 3040"

# event report attribut 0000 cluster metering 0052 param complex type
./xxcode.x "11 0A 0052 0000 41 0C AABBCC DDEEFF FFFF 1020 3040"

# demande lecture config metering
./xxcode.x "11 08 0052 00 0000"
./xxcode.x "11 09 0052 00 00 0000 41 0001 0100 0C AABBCC DDEEFF FFFF 1020 3040"

# demande ecriture config metering
./xxcode.x "11 06 0052 00 00 00 41 0001 0100 0C AABBCC DDEEFF FFFF 1020 3040"
./xxcode.x "11 07 0052 00 0000"

# demande lecture analog application type 4o
./xxcode.x "11 00 000C 0100"
./xxcode.x "11 01 000C 0100 00 23 aabbccdd"

# demande lecture binary application type 4o
./xxcode.x "11 00 000F 0100"
./xxcode.x "11 01 000F 0100 00 23 01010101"

# ------------------------ TIC
./xxcode.x "11 01 0053 0001 00 20 07"
./xxcode.x "11 01 0053 0000 00 41 04 30313200"

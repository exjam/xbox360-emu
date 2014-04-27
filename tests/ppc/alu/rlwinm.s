# pre: %r1 = 0x90003000
# pre: %r2 = 0xFFFFFFFF
# post: %r2 == 0x4000C000

rlwinm %r2, %r1, 2, 0, 0x1D


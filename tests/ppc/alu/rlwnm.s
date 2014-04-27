# pre: %r1 = 0x90003000
# pre: %r2 = 0x00000002
# pre: %r3 = 0xFFFFFFFF
# post: %r3 == 0x4000C000

rlwnm %r3, %r1, %r2, 0, 0x1D


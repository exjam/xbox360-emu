# pre: %r1 = 0x90003000
# pre: %r2 = 0x00000003
# post: %r2 == 0x4000C003

rlwimi %r2, %r1, 2, 0, 0x1D


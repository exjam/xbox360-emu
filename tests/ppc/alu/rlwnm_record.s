# pre: %r1 = 0xB0043000
# pre: %r2 = 0x00000002
# pre: %r3 = 0xFFFFFFFF
# post: %r3 == 0xC010C000
# post: %crf0 == 8

rlwnm. %r2, %r1, %r2, 0, 0x1D

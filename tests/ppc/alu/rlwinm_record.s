# pre: %r1 = 0xB0043000
# pre: %r2 = 0xFFFFFFFF
# post: %r2 == 0xC010C000
# post: %crf0 == 8

rlwinm. %r2, %r1, 2, 0, 0x1D

# pre: [0x1000] = 0x123456789ABCDEF0
# pre: %r1 = [0xFFF]
# pre: %r2 = 1
# post: %r1 == [0xFFF]
# post: %r2 == 0x123456789ABCDEF0

ldx %r2, %r1, %r2

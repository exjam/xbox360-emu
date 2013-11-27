# pre: [0x1000] = 0x123456789ABCDEF0
# pre: %r1 = [0x1000]
# post: %r1 == [0x1000]
# post: %r2 == 0x12

lbz %r2, 0(%r1)

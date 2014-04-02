# pre: [0x1000] = 0x123456789ABCDEF0
# pre: %r1 = [0xFFF]
# pre: %r2 = 1
# post: %r1 == [0x1000]
# post: %r2 == 0x123456789ABCDEF0

ldux %r2, %r1, %r2

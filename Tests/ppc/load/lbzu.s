# pre: [0x1000] = 0x123456789ABCDEF0
# pre: %r1 = [0x1000]
# post: %r1 == [0x1001]
# post: %r2 == 0x34

lbzu %r2, 1(%r1)

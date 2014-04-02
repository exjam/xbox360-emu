# pre: [0x1000] = 0x123456789ABCDEF0
# pre: %r1 = [0xFF0]
# post: %r1 == [0x1000]
# post: %r2 == 0x123456789ABCDEF0

ldu %r2, 0x10(%r1)

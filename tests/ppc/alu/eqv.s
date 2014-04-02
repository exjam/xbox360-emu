# pre: %r1 = 1
# pre: %r2 = 1
# post: %r3 == 0xffffffffffffffff

# pre: %r4 = 0
# pre: %r5 = 1
# post: %r6 == 0xfffffffffffffffe

# pre: %r7 = 1
# pre: %r8 = 0
# post: %r9 == 0xfffffffffffffffe

# pre: %r10 = 0
# pre: %r11 = 0
# post: %r12 == 0xffffffffffffffff

eqv %r3, %r1, %r2
eqv %r6, %r4, %r5
eqv %r9, %r7, %r8
eqv %r12, %r10, %r11

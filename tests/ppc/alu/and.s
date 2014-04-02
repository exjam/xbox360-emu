# pre: %r1 = 1
# pre: %r2 = 1
# post: %r3 == 1

# pre: %r4 = 0
# pre: %r5 = 1
# post: %r6 == 0

# pre: %r7 = 1
# pre: %r8 = 0
# post: %r9 == 0

# pre: %r10 = 0
# pre: %r11 = 0
# post: %r12 == 0

# post: %crf0 == 0

and %r3, %r1, %r2
and %r6, %r4, %r5
and %r9, %r7, %r8
and %r12, %r10, %r11

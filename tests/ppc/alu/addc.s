# pre: %r1 = -1
# pre: %r2 = 2

# post: %r3 == 1
# post: %xer[ca] == 1
# post: %crf0 == 0

addc %r3, %r1, %r2

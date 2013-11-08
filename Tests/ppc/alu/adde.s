# pre: %r1 = 1
# pre: %r2 = 2
# pre: %xer[ca] = 1

# post: %r3 == 4
# post: %crf0 == 0

adde %r3, %r1, %r2

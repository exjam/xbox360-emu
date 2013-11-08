# pre: %r1 = -1

# post: %r2 == 1
# post: %xer[ca] == 1
# post: %crf0 == 0

addic %r2, %r1, 2

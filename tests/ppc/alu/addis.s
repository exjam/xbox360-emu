# pre: %r1 = 1

# post: %r2 == 0x10001
# post: %crf0 == 0

addis %r2, %r1, 1

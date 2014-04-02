# pre: %r1 = 0x7fffffffffffffff
# pre: %r2 = 1

# post: %r3 == 0x8000000000000000
# post: %xer[ov] == 1
# post: %crf0 == 0

addo %r3, %r1, %r2

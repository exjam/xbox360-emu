# pre: %r1 = 0xf0f0
# pre: %r2 = 0x4444

# post: %r3 == 0xb0b0
# post: %crf0 == 2

andc. %r3, %r1, %r2

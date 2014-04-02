# pre: %r1 = 0xf0f0

# post: %r3 == 0x4040
# post: %crf0 == 2

andi. %r3, %r1, 0x4444

# pre: %r1 = 0xf0f00000

# post: %r3 == 0x40400000
# post: %crf0 == 2

andis. %r3, %r1, 0x4444

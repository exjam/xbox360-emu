# pre: %r1 = 0xf0f0
# pre: %r2 = 0x4444

# post: %r3 == 0x4040
# post: %crf0 == 2

and. %r3, %r1, %r2

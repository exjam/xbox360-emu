# pre: %r1 = 0x789A789B
# pre: %r2 = 0x30000003
# post: %r2 == 0xE269E263
# post: %crf0 == 8

rlwimi. %r2, %r1, 2, 0, 0x1A

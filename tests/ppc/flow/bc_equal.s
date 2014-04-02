# pre: %r1 = 1
# pre: %r2 = 1

# post: %r3 == 0

cmp 0, 0, %r1, %r2
bc 12, 2, 8
li %r3, 1
nop

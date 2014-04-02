# pre: %crb0 = 1
# pre: %crb1 = 1
# post: %crb2 == 0

# pre: %crb3 = 0
# pre: %crb4 = 1
# post: %crb5 == 1

# pre: %crb6 = 1
# pre: %crb7 = 0
# post: %crb8 == 1

# pre: %crb9 = 0
# pre: %crb10 = 0
# post: %crb11 == 1

crnand 2, 0, 1
crnand 5, 3, 4
crnand 8, 6, 7
crnand 11, 9, 10

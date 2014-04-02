# pre: %crb0 = 1
# pre: %crb1 = 1
# post: %crb2 == 1

# pre: %crb3 = 0
# pre: %crb4 = 1
# post: %crb5 == 1

# pre: %crb6 = 1
# pre: %crb7 = 0
# post: %crb8 == 1

# pre: %crb9 = 0
# pre: %crb10 = 0
# post: %crb11 == 0

cror 2, 0, 1
cror 5, 3, 4
cror 8, 6, 7
cror 11, 9, 10

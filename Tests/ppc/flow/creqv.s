# pre: %crb0 = 1
# pre: %crb1 = 1
# post: %crb2 == 1

# pre: %crb3 = 0
# pre: %crb4 = 1
# post: %crb5 == 0

# pre: %crb6 = 1
# pre: %crb7 = 0
# post: %crb8 == 0

# pre: %crb9 = 0
# pre: %crb10 = 0
# post: %crb11 == 1

creqv 2, 0, 1
creqv 5, 3, 4
creqv 8, 6, 7
creqv 11, 9, 10

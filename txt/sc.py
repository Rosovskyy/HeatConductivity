with open("matrix2.txt","w") as file:
	for i in range(10):
		for j in range(10):
			if ( j == 9):
				file.write(str(100)+"\n")
			elif (i == 0) or (j == 0)  or (i == 9):
				file.write(str(100)+"|")
			else:
				file.write(str(0)+"|")

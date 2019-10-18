with open("matrix2.txt","w") as file:
	for i in range(100):
		for j in range(100):
			if ( j == 99):
				file.write(str(100)+"\n")
			elif (i == 0) or (j == 0)  or (i == 99):
				file.write(str(100)+"|")
			else:
				file.write(str(0)+"|")

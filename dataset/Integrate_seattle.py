for i in range(1,688+1):

	content = ''

	with open(file="./SeattleData_{}".format(i), mode='r') as fb:
		content = fb.read()

	with open(file="./SeattleData_all", mode='a+') as fb:
		fb.write(content)
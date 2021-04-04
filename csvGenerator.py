#!/usr/bin/env python3

import os, sys, random

straight = [150,150,6,6,0,0,1,1,1,0,0,0,0,3,0,1]
accelerating = [110,110,6,6,0,0,1,1,1,1,0,0,0,3,0,2]
left = [80,90,4,8,1,0,0,1,1,0,17,0,35,16,0,36]
slowing = [45,50,6,6,1,1,1,1,1,0,0,1,0,3,1,2]
right = [75,85,5,7,1,0,1,1,0,0,20,0,155,18,0,153]


def main(args):
	n = int(args[0])
	instructions = args[1:]
	result = populate_directions(instructions)
	for i in range(n):
		partition = n / len(result)
		count = int (i / partition)
		for index in range(0, len(result[count])):
			error = round(result[count][index] + random.uniform(-0.05 * result[count][index], 0.05 * result[count][index]), 3)
			if index == len(result[count]) - 1:
				print(error)
			else:
				print(error, end = ", ")


def populate_directions(directions):
	to_return = []
	for i in range(len(directions)):
		if directions[i] == "straight":
			to_return.append(straight)
		elif directions[i] == "accelerating":
			to_return.append(accelerating)
		elif directions[i] == "left":
			to_return.append(left)
		elif directions[i] == "slowing":
			to_return.append(slowing)
		elif directions[i] == "right":
			to_return.append(right)
		else:
			sys.exit("You did not input a valid direction")
		
	return to_return

if __name__ == "__main__":
    main(sys.argv[1:])

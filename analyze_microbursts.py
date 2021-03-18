import os


if __name__ == "__main__":
	
	# create files with microbursts info
	os.system("awk '/^Microburst/' log.txt > microbursts.txt")
	os.system("awk '/^NOMicroburst/' log.txt > microburstsNO.txt")

	microburst_count = len(open('microbursts.txt').readlines())
	non_micro_count = len(open('microburstsNO.txt').readlines())

	print(microburst_count, non_micro_count)

	print("{}%\n".format((100.0)*microburst_count / (microburst_count+non_micro_count)))
	
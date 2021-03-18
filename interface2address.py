import os


if __name__ == "__main__":
	
	# create files with microbursts info
	os.system("awk '/^IF2IP/' log.txt > if2addr.txt")

	interfaces_count = len(open('if2addr.txt').readlines())

	mac_addresses = {}
	interfaces = {}
	
	with open('if2addr.txt', 'r') as input_file:
		lines = input_file.readlines()
		for line in lines:
			split_line = line.strip().split()
			interface = split_line[1]
			mac_address = split_line[2]

			if interface in interfaces:
				print("warning, interface found twice!")
				interfaces[interface] += 1
			else:
				interfaces[interface] = 1

			if mac_address in mac_addresses:
				print("warning, mac_address found twice!")
				mac_addresses[mac_address] += 1
			else:
				mac_addresses[mac_address] = 1

	print(interfaces_count, len(interfaces), len(mac_addresses))

	print(interfaces)
	print(mac_addresses)

	#print("{}%\n".format((100.0)*microburst_count / (microburst_count+non_micro_count)))
	
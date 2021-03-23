import os


if __name__ == "__main__":
	
	# create files with microbursts info
	os.system("awk '/^REPORT/' log.txt > pitcher_reports.txt")
	os.system("awk '/^INT_REPORT/' log.txt > int_reports.txt")
	os.system("awk '/^CONTROL/' log.txt > control.txt")

	microburst_count = len(open('control.txt').readlines())
	pitcher_report_count = len(open('pitcher_reports.txt').readlines())
	int_report_count = pitcher_report_count + len(open('int_reports.txt').readlines())

	microbursts = {}
	with open('control.txt', 'r') as input_file:
		lines = input_file.readlines()
		for line in lines:
			split_line = line.strip().split()
			
			swid = split_line[1]
			start_time = int(split_line[2])
			end_time = int(split_line[3])

			if swid in microbursts:
				assert(start_time not in microbursts[swid])
				microbursts[swid][start_time] = end_time
			else:
				microbursts[swid] = {}
				microbursts[swid][start_time] = end_time


	pitcher_reports = {}
	with open('pitcher_reports.txt', 'r') as input_file:
		lines = input_file.readlines()
		for line in lines:
			split_line = line.strip().split()
			report_time = int(split_line[1])
			
			swid1 = split_line[9]
			hop_latency1 = int(split_line[10])
			swid2 = split_line[11]
			hop_latency2 = int(split_line[12])
			swid3 = split_line[13]
			hop_latency3 = int(split_line[14])

			# Only consider reports that indicate presence of a microburst, otherwise not needed
			if hop_latency1 > 50000:
				if swid1 not in pitcher_reports:
					pitcher_reports[swid1] = []
					pitcher_reports[swid1].append(report_time)
				elif report_time not in pitcher_reports[swid1]:
					pitcher_reports[swid1].append(report_time)

			if hop_latency2 > 50000:
				if swid2 not in pitcher_reports:
					pitcher_reports[swid2] = []
					pitcher_reports[swid2].append(report_time)
				elif report_time not in pitcher_reports[swid2]:
					pitcher_reports[swid2].append(report_time)

			if hop_latency3 > 50000:
				if swid3 not in pitcher_reports:
					pitcher_reports[swid3] = []
					pitcher_reports[swid3].append(report_time)
				elif report_time not in pitcher_reports[swid3]:
					pitcher_reports[swid3].append(report_time)

	int_reports = {}
	with open('int_reports.txt', 'r') as input_file:
		lines = input_file.readlines()
		for line in lines:
			split_line = line.strip().split()
			report_time = int(split_line[1])
			
			swid1 = split_line[9]
			hop_latency1 = int(split_line[10])
			swid2 = split_line[11]
			hop_latency2 = int(split_line[12])
			swid3 = split_line[13]
			hop_latency3 = int(split_line[14])

			# Only consider reports that indicate presence of a microburst, otherwise not needed
			if hop_latency1 > 50000:
				if swid1 not in int_reports:
					int_reports[swid1] = []
					int_reports[swid1].append(report_time)
				elif report_time not in int_reports[swid1]:
					int_reports[swid1].append(report_time)

			if hop_latency2 > 50000:
				if swid2 not in int_reports:
					int_reports[swid2] = []
					int_reports[swid2].append(report_time)
				elif report_time not in int_reports[swid2]:
					int_reports[swid2].append(report_time)

			if hop_latency3 > 50000:
				if swid3 not in int_reports:
					int_reports[swid3] = []
					int_reports[swid3].append(report_time)
				elif report_time not in int_reports[swid3]:
					int_reports[swid3].append(report_time)

 
	# Compute number of microbursts that are actually detected
	n_microbusts_detected_by_pitcher = 0

	# the set of microbursts that haven't been detected by pitcher reports, used to see if INT may be more accurate
	undetected_microbursts = {}

	for swid in microbursts:
		if swid in pitcher_reports:
			# ordered detections
			detection_times = sorted(pitcher_reports[swid])
			index=0
			for microburst_start_time in sorted(microbursts[swid]):
				while (index < len(detection_times) and detection_times[index] < microburst_start_time):
					index += 1
				# If we ran out of reports, just add the microburst to the list of undetected ones
				if (index == len(detection_times) or detection_times[index] > microbursts[swid][microburst_start_time]):
					undetected_microbursts[swid][microburst_start_time] = microbursts[swid][microburst_start_time]
				else:
					n_microbusts_detected_by_pitcher += 1
		else:
			# if there is no report for the gived swid, then all associated microburst are unreported
			undetected_microbursts[swid] = microbursts[swid]

	
	n_microbusts_detected_by_int = 0

	for swid in undetected_microbursts:
		if swid in int_reports:
			# ordered detections
			detection_times = sorted(int_reports[swid])
			index=0
			for microburst_start_time in sorted(undetected_microbursts[swid]):
				while (index < len(detection_times) and detection_times[index] < microburst_start_time):
					index += 1
				# If we ran out of reports, just add the microburst to the list of undetected ones
				if (index < len(detection_times) and detection_times[index] <= microbursts[swid][microburst_start_time]):
					n_microbusts_detected_by_int += 1


	print(microburst_count, n_microbusts_detected_by_pitcher, n_microbusts_detected_by_int, pitcher_report_count, int_report_count)
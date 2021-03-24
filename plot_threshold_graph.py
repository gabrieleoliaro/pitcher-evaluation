import matplotlib.pyplot as plt
import numpy as np

# the data:
#(134, 48, 4, 22060, 69417)
#(134, 48, 4, 21568, 69417)
#(134, 48, 4, 16111, 69416)
#(134, 45, 7, 1977, 69410)
#(134, 29, 23, 525, 69409)
#(134, 27, 25, 272, 69408)
#(134, 5, 47, 200, 69408)
#(134, 2, 50, 194, 69408)

thresholds = np.array([1, 5, 10, 12, 14, 16, 18, 20])

n_microbursts = 134
n_detected_by_pitcher = np.array([48.0, 48.0, 48.0, 45.0, 29.0, 27.0, 5.0, 2.0])
n_detected_by_int = np.array([48+4, 48+4, 48+4, 45+7, 29+23, 27+25, 5+47, 2+50])
n_int_reports = np.array([69417.0, 69417.0, 69416.0, 69410.0, 69409.0, 69408.0, 69408.0, 69408.0])
n_pitcher_reports = np.array([22060, 21568, 16111, 1977, 525, 272, 200, 194])

detection_percentage = (n_detected_by_pitcher / n_detected_by_int)*100
reports_percentage = (n_pitcher_reports / n_int_reports)*100

print(detection_percentage)
print(reports_percentage)

plt.figure()
plt.plot(thresholds, detection_percentage)
plt.plot(thresholds, reports_percentage)

plt.title("Accuracy and redundancy of PITCHER (compared to INT) \n when detecting microbursts")
plt.xticks(thresholds)
plt.xlabel("Threshold for binning (powers of 2)")
plt.ylabel("Accuracy/Redundancy in percentage points")
plt.legend(("Accuracy", "Redundancy"))
plot_filepath = "threshold_plot.png"
plt.savefig(plot_filepath)
plt.show()
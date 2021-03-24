nservers=16
nspines=4
nleaves=9
spineToLeafCapacity=40
leafToServerCapacity=10
endtime=0.01
#load=(0.25 0.6)
load=0.25
#cdfFiles=("examples/rtt-variations/DCTCP_CDF.txt" "examples/rtt-variations/FB_HADOOP_CDF.txt")
cdffile="examples/rtt-variations/DCTCP_CDF.txt"
outputfile="log.txt"

numpoints=(128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144)




# Experiment 1. Setting the SINK_CACHE_SIZE to 10, repeating for some values of the threshold
export SINK_CACHE_SIZE=10
thresholds=(1 5 10 12 14 16 18 20)
#thresholds=(10)
for n in ${thresholds[@]}; do
	export HOP_LATENCY_THRESHOLD=${n}
	./waf --run "large-scale --randomSeed=233 --EndTime=${endtime} --load=${load} --cdfFileName=${cdffile} --AQM=TCN --transportProt=DcTcp --serverCount=${nservers} --spineCount=${nspines} --leafCount=${nleaves} --spineLeafCapacity=${spineToLeafCapacity} --leafServerCapacity=${leafToServerCapacity}" > ${outputfile} 2>&1
	python perform_analysis.py
done
#export SINK_CACHE_SIZE=10;export HOP_LATENCY_THRESHOLD=20;
#./waf --run "large-scale --randomSeed=233 --EndTime=0.001 --load=0.25 --cdfFileName=examples/rtt-variations/DCTCP_CDF.txt --AQM=TCN --transportProt=DcTcp --serverCount=16 --spineCount=4 --leafCount=9 --spineLeafCapacity=40 --leafServerCapacity=10" > log2.txt 2>&1
	
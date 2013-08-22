#0) cts map
#1) output prefix
#optional:
#2) scaletype: scale types: linear, dyadic
#2) scalemin: min scale (cwt) = 2^-x
#3) scalemax: max scale (cwt) = 2^y e.g. image dimension 
#4) scalenum: number of scales (cwt)
#9) methistsize: number of different classes used for the histogram (e.g. 1000)
#5) cclsizemin: component size min (ccl)
#6) cclsizemax: component size max (ccl)
#7) cclradmin: radius min (ccl)
#8) cclradmax: radius max (ccl)
#9) cclscalemin: scale min
#10) cclscalemax: scale max

load "Parameters.rb" 

if ARGV[0].to_s == "help" || ARGV[0] == nil || ARGV[0] == "h"
		system("head -14 " + $0 );
		exit;
	end
	
cts = ARGV[0]
out = ARGV[1]

p = Parameters.new
p.processInput(2, ARGV)

#calcolo trasformata wavelet
#-w wavelet type (family)
#-s scale type:number of scales:min:max
#type = linear or dyadic (exponential). Dyadic = exponente a 2
cmd = "./cwt2 -v -w log -s " + p.scaletype.to_s + ":" + p.scalenum.to_s + ":" + p.scalemin.to_s + ":" + p.scalemax.to_s + " -i " + cts.to_s + " -o " + out.to_s + ".wtf "
if Dir[out.to_s + ".wtf"].size() == 0
	puts cmd
	system(cmd)
end

#Maximum Entropy Thresholding
#-r resolution
cmd ="./met -v -n " + p.methistsize.to_s + "  -i " + out.to_s + ".wtf " + " -o " + out.to_s + ".met "
if Dir[out.to_s + ".met"].size() == 0
	puts cmd
	system cmd
end

#Connected components labelling 
cmd = "./ccl -v "
if p.cclsizemin != -1 or  p.cclsizemax != -1
	cmd = cmd + " -c " + p.cclsizemin.to_s + ":" + p.cclsizemax.to_s
end
if p.cclradmin != -1 or p.cclradmax != -1
	cmd = cmd + " -r " + p.cclradmin.to_s + ":" + p.cclradmax.to_s
end
if p.cclscalemin != -1 or p.cclscalemax != -1
        cmd = cmd + " -s " + p.cclscalemin.to_s + ":" + p.cclscalemax.to_s
end
cmd = cmd +  " -i " + out.to_s + ".met > " + out.to_s + ".list "
puts cmd
system cmd



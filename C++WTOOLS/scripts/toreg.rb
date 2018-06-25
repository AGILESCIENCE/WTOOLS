puts "global color=green dashlist=8 3 width=1 font=\"helvetica 10 normal\" select=1 highlite=1 dash=0 fixed=0 edit=1 move=1 delete=1 include=1 source=1"
puts "image"

File.open(ARGV[0]).each_line do | line |
	ll = line.split("[")
	if ll.size() == 2
		ll = line.split(" ")
		puts "circle(" + ll[1].to_s + "," + ll[2].to_s + ",5)"
	end

end
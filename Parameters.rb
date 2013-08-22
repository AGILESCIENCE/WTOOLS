class Parameters
	public
		def initialize() 
			@scaletype = "dyadic"
			@scalemin = 2
			@scalemax = 1024
			@scalenum = 50
			@cclsizemin = -1
			@cclsizemax = -1
			@cclradmin = -1
			@cclradmax = -1
			@cclscalemin = -1
			@cclscalemax = -1
			@methistsize = 1000
		end
		
		
		def cclscalemin
                        @cclscalemin
                end

		def cclscalemax
                        @cclscalemax
                end

		def scaletype
			@scaletype
		end
		
		def scalemin
			@scalemin
		end
		
		def scalemax
			@scalemax
		end
		
		def scalenum
			@scalenum
		end
		
		def cclsizemin
			@cclsizemin
		end
		
		def cclsizemax
			@cclsizemax
		end
		
		def cclradmin
			@cclradmin
		end
		
		def cclradmax
			@cclradmax
		end
		
		def methistsize
			@methistsize
		end
		
		def processInput(startindex, s)
			for i in startindex...s.size
				if s[i] == nil
					break;
				else
					processLine(s[i]);
				end
			end
			initparam();
		end
		
		def buildCommandLine()
				a = "scaletype=" + @scaletype.to_s + " "
				a = a + "scalemin=" + @scalemin.to_s + " "
				a = a + "scalemax=" + @scalemax.to_s + " "
				a = a + "scalenum=" + @scalenum.to_s + " "
				a = a + "cclsizemin=" + @cclsizemin.to_s + " "
				a = a + "cclsizemax=" + @cclsizemax.to_s + " "
				a = a + "cclradmin=" + @cclradmin.to_s + " "
				a = a + "cclradmax=" + @cclradmax.to_s + " "
				a = a + "cclscalemin=" + @cclscalemin.to_s + " "
				a = a + "cclscalemax=" + @cclscalemax.to_s + " "
				a = a + "methistsize=" + @methistsize.to_s + " "
				@buildCommandLine = a
		end
		

		def processLine(argv)
			keyw = argv.split("=")[0];
			value = argv.split("=")[1];
			puts keyw.to_s + " " + value.to_s
			case keyw
				when "scaletype"
					@scaletype = value;
				when "scalemin"
					@scalemin = value;
				when "scalemax"
					@scalemax = value;
				when "scalenum"
					@scalenum = value;
				when "cclsizemin"
					@cclsizemin = value;
				when "cclsizemax"
					@cclsizemax = value;
				when "cclradmin"
					@cclradmin = value;
				when "cclscalemin"
                    @cclscalemin = value;
				when "cclscalemax"
                    @cclscalemax = value;
				when "cclradmax"
					@cclradmax = value;
				when "methistsize"
					@methistsize = value;
				else
					puts "Keyword " + argv.to_s + " error."
					exit;
			end
			
			
		end
		
		def initparam()

			
		end
end


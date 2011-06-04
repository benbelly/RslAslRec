def inDir(destdir)
    cur=Dir.pwd
    Dir.chdir(destdir)
    yield
    Dir.chdir(cur)
end

def noInterps
    File.open("empty", "w") { |f| f.puts "0" }
end

def truthstring
    "\"#{File.read("truth").chomp}\""
end

def logfile(dir, alpha)
    "#{dir}/#{(dir.split "/")[-3]}_alpha#{alpha}.log"
end

def numlevels
    truthstring.split.length - 1
end

Command = "/home/bholm/epenthesis/levelbuilding"
Dirs = IO.readlines("TruthDirList")
Alphas = [ "1.25", "1.50", "1.75", "2.0", "2.5", "3.0" ]

CommandLineFile = "LevelBuildingCommands"

def commandLineToFile(dir,file)
    Alphas.each { |alpha|
        file.puts "#{Command} #{alpha} #{numlevels} #{truthstring} #{dir} > #{logfile(dir,alpha)} < #{dir}/empty"
    }
end

def commandLineInDir(dir)
    dir = dir.chomp
    noInterps # create interpretation file with 0
    File.open(CommandLineFile, "w") { |f| commandLineToFile(dir, f) }
end

Dirs.each { |dir|
    d = dir.chomp
    inDir(d) { commandLineInDir(d) }
}


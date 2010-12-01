val line = "251 1 400 200";
val tokens = String.tokens Char.isSpace line;

AslIO.prFeature (AslIO.lineToFace tokens);
AslIO.prFeature (AslIO.lineToDominant tokens);
AslIO.prFeature (AslIO.lineToWeak tokens);
AslIO.prFeature AslIO.Missing;

print "Files and dirs . . .\n";
app (fn f => print (f ^ "\n")) (AslIO.getDatFiles ".");
app (fn f => print (f ^ "\n")) (AslIO.getSubDirs ".");
print "Done.\n";

windres EOLConverter.rc -O coff -o build\EOLConverter.res
gcc EOLConverter.c build\EOLConverter.res -o EOLConverter.exe -Os -s -static

md "Generated Files"
midlrt attributes.idl /nomidl /winrt /winmd "Generated Files\attributes.winmd" /metadata_dir C:\git\xlang\src\test\cpp /reference windows.winmd /h "nul"
midlrt component.idl /nomidl /winrt /winmd "Generated Files\component.winmd" /metadata_dir C:\git\xlang\src\test\cpp /reference windows.winmd /reference "Generated Files\attributes.winmd" /h "nul"

C:\git\xlang\_build\Windows\x86\Debug\tool\cpp\cpp.exe -in "Generated Files\component.winmd" -ref windows.winmd "Generated Files\attributes.winmd" -out "compare\out" -component "compare\project" -verbose -prefix -base -overwrite

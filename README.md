# TheSearchForFashion
Master Thesis Work

The search engine is written in C++ and is called the back-end. It can be found as a Visual Studio project in the Project folder.

A web-based front-end is written in C#/ASP.net and can be found in the WebSite folder.

Some python scripts to prepare the cataloge of fashion items can be found in the Pre-process folder.


Get my current build of OpenCV here https://drive.google.com/file/d/0B2ajciMUvjkVSTJZU0l6QWVaNm8/view?usp=sharing.

To make it work on MS Visual Studio
Need to include ..\opencv\build\include in the projects Properties -> Configuration Properties -> C/C++ -> General -> Aditional Include Directories

Need to link ..\opencv\build\x64\vc14\lib in the projects Properties -> Configuration Properties -> Linker -> General -> Additional Library Directories

Need to link opencv_ts310d.lib and opencv_world310.lib in the projects Properties -> Configuration Properties -> Linker -> Input -> Additional Dependencies

If opencv_ts310d.dll is missing it can be found in ..\opencv\build\x64\vc14\bin

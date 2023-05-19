Due to my unwillingness of including another repository in my own, but also hating my previous dependency system,
I have therefore decided how I will manage dependencies for this project. They will not be included in the repo
directly, instead the dependencies must be downloaded and setup based on the platform the engine user wishes
to compile out for. A download batch file is provided to fetch the repo dependencies, then platform specific
build tools are also provided.

The geodesuka engine does not include its dependencies in the github repository, and if the user wishes
to compile the engine locally he/she/they must first download the dependencies. Open a terminal in the 
dep/ directory.

# Building For Linux

Step I:
	First, give permission to all three of these files. install.sh, download.bat, setup-linux.sh.
	chmod +x install.sh
	chmod +x download.bat
	chmod +x setup-linux.sh

Step II:
	The engine user must first install all the proper system dependencies on their local machine. This 
	step has been automated. All that one needs to do is run the command
	./install.sh
	
Step III:
	Download all the needed dependencies by cloning them from github. This can be done by running
	the download.bat script.
	./download.bat

Step IV:
	Finally generate all the build files for each of these projects by executing the script setup-linux.sh
	./setup-linux.sh

# Building For Windows

GO TO: ".\freetype\bld\win32\include\freetype\config\ftoption.h"
To disable FT_CONFIG_OPTION_USE_ZLIB, otherwise freetype will
conflict with freeimage. They both use ZLib, and will define
the same function twice.

Step I:
	Install the Vulkan SDK from the location
	https://vulkan.lunarg.com/sdk/home#windows
	
Step II:
	Download all the needed dependencies by cloning them from github. This can be done by running
	the download.bat script.
	download.bat

Step III:
	Finally generate all the build files for each of these projects by executing the batch file
	setup-win32.bat
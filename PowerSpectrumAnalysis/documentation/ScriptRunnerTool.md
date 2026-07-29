# Script Runner Tool Overview

## Error Codes

1. Failed to find current working directory.
2. Unrecognized argument.
3. Unrecognized command.
4. System call performed on unsupported operating system. 
5. Failed to execute command in new process.
6.
7. Failed to execute command.
8. Failed to initialize config.
9.
10. Failed to write to 'settings.txt'
11. Incompatable flags.
12. Failed to open file in output mode.

## Build Instructions

### Dependencies

- C++ 17+ standard library
- C++ 17+ compiler
- CMAKE ver 3.15+

The project is build by calling cmake functions from within the build directory. Ensure you have some output folder called `build` or something, it should be in the same directory as the `CMakeLists.txt` file (the base directory of this project).

Because the project requires a certain MATLAB script in the directory of the executable, the CMAKE process will attempt to copy a MATLAB script from the `src/Matlab Scripts` folder, and place it in the `build/bin` folder. The MATLAB script is called `cmd_MagSpectrumBandpass.m`, ensure it remains in the src/Matlab Scripts folder.

From within your build folder, use the following commands to configure the CMAKE project, and build the project with generated files. Again, it is important to run these commands from <i>within</i> your build folder, and in this order.

``` shell
cmake ..
cmake --build .
```

Provided the project is successfully built, there should be a new executable and MATLAB script in the `build/bin` folder. The project can be open by entering the bin directory, and just using

``` shell
./MEAScriptRunner
```

### Build Instructions Cont.

I have experienced some issues where CMAKE will refuse to build when there already exists an executable or MATLAB script in the bin directory. I'm not certain why this happens, but can be solved easily by just removing the files then building as usual.

> Example for Windows. Run these commands from within your build/bin folder.

``` shell
if (Test-Path -Path "MEAScriptRunner.exe") { Remove-Item -Path "MEAScriptRunner.exe" }
if (Test-Path -Path "cmd_MagSpectrumBandpass.m") { Remove-Item -Path "cmd_MagSpectrumBandpass.m" }
cd ..
cmake .. 
cmake --build .
```

### VSCode Automated Build Tasks

Here is a simple tasks.json file that can be used to atuomate the build tasks in VSCode. Create a file called `tasks.json` in your `.vscode` folder for this project, and press `Ctrl+Shift+B` (on Windows) to run the automated vuild tasks.

``` json
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "shell",
			"label": "SETUP: Delete old files",
			"command": "if (Test-Path -Path \"MEAScriptRunner.exe\") { Remove-Item -Path \"MEAScriptRunner.exe\" }; if (Test-Path -Path \"cmd_MagSpectrumBandpass.m\") { Remove-Item -Path \"cmd_MagSpectrumBandpass.m\" }",
			"options": {
				"cwd": "${workspaceFolder}/build/bin"
			},
			"group": "build",
			"problemMatcher": []
		},
		{
			"type": "shell",
			"label": "CMAKE: Configure",
			"dependsOn": "SETUP: Delete old files",
			"command": "cmake",
			"args": [
				".."
			],
			"options": {
				"cwd": "${workspaceFolder}/build"
			},
			"group": "build",
			"problemMatcher": []
		},
		{
			"type": "shell",
			"label": "CMAKE: Build",
			"dependsOn": "CMAKE: Configure",
			"command": "cmake",
			"args": [
				"--build",
				"."
			],
			"options": {
				"cwd": "${workspaceFolder}/build"
			},
			"group": "build",
			"problemMatcher": []
		},
		{
			"type": "shell",
			"label": "Run",
			"dependsOn": "CMAKE: Build",
			"command": "./MEAScriptRunner",
			"options": {
				"cwd": "${workspaceFolder}/build/bin"
			},
			"group": {
				"kind": "build",
				"isDefault": true
			},
			"problemMatcher": []
		}
	]
}
```
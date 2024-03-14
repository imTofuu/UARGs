## Download

Zip file can be downloaded from this repositories [releases](https://github.com/imTofuu/UARGs/releases) page.

## Install

### Arduino IDE

In your Arduino IDE, navigate to: 
<br>_Sketch > Include Library > Add .ZIP Library_ and select this library.

### VSCode

#### Linux:
```bash
unzip filepath -d ~/.arduino15/packages/{your board}/tools/{whatever is here}/{whatever is here}/libraries
```

You will need to go into your `.vscode/c_cpp_properties.json` file or `Ctrl` + `Shift` + `P` and search for `Edit Configurations: (JSON / UI)` and find `Include Path` and add:<br>
`/home/{username}/.arduino15/packages/{your board}/tools/{whatever is here}/{whatever is here}/libraries/**` to it.

Installation steps may vary slightly between different boards and / or versions of the library / Arduino.